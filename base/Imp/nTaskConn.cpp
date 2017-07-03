#include "base/Tidy.h"
#include "base/nTaskConn.h"
#include "base/nBaseServer.h"
#include "base/nBaseMessageService.h"
#include "base/nSocketPortData.h"
#include "base/nBaseMonitorService.h"

#include "base/Imp/nSocket.h"
#include "base/Imp/nListenService.h"

namespace Tidy
{
	nTaskConn::nTaskConn(nServerConn* serverConn, const int sock,const struct sockaddr *addr,const SocketPortData* listenPortData) : 
		_buffered(false),
		_state(TaskConnState_NotUse),
		_is_read_event_regged(false),
		_start_time(),
		_need_check_heart_tick(listenPortData->needCheckHeartTick),
#ifdef _DEBUG
		_tick_timer(listenPortData->heartTickTimerInMsec),
#else
		_tick_timer(listenPortData->heartTickTimerInMsec, false, 10 * 1000L),
#endif
		_in_tick(false),
		_need_now_check_tick(false),
		_start_tick_time_in_usec(0),
		_ping_time_in_usec(0),
		_verify_state(eVerifyState_None),
		_recycle_time(1000),
		_server_conn(serverConn),
		_is_net_error(false),
		_socket(NULL)
		{
			_socket = new nSocket(sock, addr, listenPortData->commandCtl, listenPortData->zipMethod, listenPortData->needQuickAck);
		}

	nTaskConn::~nTaskConn()
	{
		SAFE_DELETE(_socket);
	}

	bool nTaskConn::sendTaskCmd(const void *nullCommand, const DWORD cmdLen) const
	{
		return const_cast<nSocket*>(_socket)->sendCmd(nullCommand, cmdLen, _buffered);
	}

	bool nTaskConn::sendRawDataIM(const void *data, const DWORD len) const
	{
		return const_cast<nSocket*>(_socket)->sendRawData(data, len);
	}

	void nTaskConn::addEpoll(int kdpfd, __uint32_t events, void *ptr)
	{
		_socket->addEpoll(kdpfd, events, ptr);
	}

	void nTaskConn::delEpoll(int kdpfd, __uint32_t events)
	{
		_socket->delEpoll(kdpfd, events);
	}

	bool nTaskConn::checkVerifyTimeout(const nTime &ct, const unsigned long interval) const
	{
		return (_start_time.elapse(ct) > interval);
	}

	bool nTaskConn::isReadEventRegged() const
	{
		return _is_read_event_regged;
	}

	bool nTaskConn::setReadEventRegged()
	{
		_is_read_event_regged = true;
		return _is_read_event_regged;
	}

	bool nTaskConn::checkNeedClose() const
	{
		return _socket->checkNeedClose();
	}

	void nTaskConn::dealNetError()
	{
		_is_net_error = true;
		setNeedClose();
	}

	void nTaskConn::setNeedClose()
	{
		_server_conn->info("主动关闭连接");
		_socket->setNeedClose();
	}

	const nTaskConn::eTaskConnState nTaskConn::getState() const
	{
		return _state;
	}

	void nTaskConn::setState(const eTaskConnState state)
	{
		this->_state = state;
	}

	const char *nTaskConn::getStateString(const eTaskConnState state) const
	{
		const static char* taskConnStateDescVec[TaskConnState_Max] = {"NotUse", "Verify", "Sync", "Ok", "Recycle"};
		assert_debug(state < TaskConnState_Max, "[%d]", state);
		return taskConnStateDescVec[state];
	}

	const WORD nTaskConn::getPort() const
	{
		return _socket->getPort();
	}

	const char *nTaskConn::getIP() const
	{
		return _socket->getIP();
	}

	void nTaskConn::setNeedNowCheckTick()
	{
		if(_need_now_check_tick == true){
			return;
		}
		if(_in_tick == true){
			return;
		}
		_need_now_check_tick = true;
	}

	bool nTaskConn::checkHeartTickInterval(const nTime &ct)
	{
		if(_need_now_check_tick == true && _in_tick == false){
			_need_now_check_tick = false;
			return true;
		}
		return _tick_timer(ct);
	}

	bool nTaskConn::checkTick() const
	{
		return _in_tick;
	}

	void nTaskConn::clearTick()
	{
		if(_in_tick == false){
			return;
		}
		// 来回需要除以2
		baseServer->time().now();
		assert_debug(_start_tick_time_in_usec > 0, "时间错误");
		_start_tick_time_in_usec = 0;
		_in_tick = false;
        /*
		_ping_time_in_usec = ASSERT_CAST(DWORD, (baseServer->time().usec() - _start_tick_time_in_usec) / 2);
        */
#ifdef _DEBUG
		_server_conn->debug("PING[%u]", _ping_time_in_usec);
#endif
	}

	void nTaskConn::setTick(const nTime &ct)
	{
		assert_debug(_in_tick == false, "已经在tick");
		assert_debug(_start_tick_time_in_usec == 0, "开始心跳时间为0");
		_start_tick_time_in_usec = ct.usec();
		_in_tick = true;
		_need_now_check_tick = false;
	}

	DWORD nTaskConn::getPingTimeInUsec() const
	{
		return _ping_time_in_usec;
	}

	bool nTaskConn::checkVerifyOK()
	{
		return _verify_state == eVerifyState_Success;
	}

	void nTaskConn::setVerifyState(const eVerifyState verifyState)
	{
		_verify_state = verifyState;
	}

	int nTaskConn::verifyConn()
	{
		if(baseServer->verifyTaskConn(this, NULL, 0) == true){
			_verify_state = eVerifyState_Success;
		}
		do{
			if(_verify_state != eVerifyState_None){
				break;
			}
			int retcode = _socket->recvToBuf_NoPoll();
			if(retcode < 0){
				_verify_state = eVerifyState_Fail;
				break;
			}
			else if(retcode == 0){
				assert_debug(_verify_state == eVerifyState_None, "状态错误");
				break;
			}
			char stackCommandBuffer[MAX_PROTO_CMD_LEN];
			void* command = stackCommandBuffer;
			BufferCmdQueue* heapBufferCommandBuffer = NULL;
			DWORD cmdLen = _socket->recvToCmd_NoPoll(stackCommandBuffer, sizeof(stackCommandBuffer));

			if(cmdLen > sizeof(stackCommandBuffer)) {
				heapBufferCommandBuffer = new BufferCmdQueue();
				heapBufferCommandBuffer->wr_reserve(cmdLen);
				command = heapBufferCommandBuffer->wr_buf();
				DWORD len = _socket->recvToCmd_NoPoll(heapBufferCommandBuffer->wr_buf(), heapBufferCommandBuffer->wr_size());
				assert_debug(heapBufferCommandBuffer->wr_size() >= cmdLen && len == cmdLen, "");
				cmdLen = len;
			}

			if(cmdLen <= 0) {
				//这里只是从缓冲取数据包，所以不会出错，没有数据直接返回
				assert_debug(_verify_state == eVerifyState_None, "状态错误");
				break;
			}


			if(baseServer->verifyTaskConn(this, command, cmdLen) == true) {
				_verify_state = eVerifyState_Success;
			}
			else{
				_verify_state = eVerifyState_Fail;
			}

			SAFE_DELETE(heapBufferCommandBuffer);
		}while(0);

		return _verify_state;
	}

	const nTaskConn::eSyncState nTaskConn::waitSync()
	{
		eSyncState result = static_cast<eSyncState>(baseServer->syncTaskConn(this, NULL, 0));
		do{
			if(result != eSyncState_None){
				break;
			}

			int retcode = _socket->checkIOForRead();
			if (-1 == retcode)
			{
				result = eSyncState_Fail;
				break;
			}
			else if(retcode > 0)
			{
				retcode = _socket->recvToBuf_NoPoll();
				if (-1 == retcode)
				{
					baseServer->error("%s", __PRETTY_FUNCTION__);
					result = eSyncState_Fail;
					break;
				}
				unsigned char stackCommandBuffer[MAX_PROTO_CMD_LEN];
				void* command = stackCommandBuffer;
				BufferCmdQueue* heapBufferCommandBuffer = NULL;
				DWORD cmdLen = 0;
				do{
					cmdLen = _socket->recvToCmd_NoPoll(stackCommandBuffer, sizeof(stackCommandBuffer));
					if (cmdLen > sizeof(stackCommandBuffer)) {
						heapBufferCommandBuffer = new BufferCmdQueue();
						heapBufferCommandBuffer->wr_reserve(cmdLen);
						command = heapBufferCommandBuffer->wr_buf();
						cmdLen = _socket->recvToCmd_NoPoll(heapBufferCommandBuffer->wr_buf(), heapBufferCommandBuffer->wr_size());
					}
					if (cmdLen > 0) {
						result = static_cast<eSyncState>(baseServer->syncTaskConn(this, command, cmdLen));
						if(result != eSyncState_None){
							break;
						}
					}
				}while(cmdLen > 0);
			}
		}while(0);

		return result;
	}

	bool nTaskConn::listeningRecv(bool needRecv)
	{
		int retcode = 0;

		if (needRecv) {
			retcode = _socket->recvToBuf_NoPoll();
		}
		//struct timeval tv_2;
		if (-1 == retcode)
		{
			return false;
		}
		else
		{
			BufferCmdQueue *bufferQueue = NULL;
			do
			{
				char stackCommandBuffer[MAX_PROTO_CMD_LEN];
				char* cmdBuffer = stackCommandBuffer;
				DWORD cmdLen = _socket->recvToCmd_NoPoll(stackCommandBuffer, sizeof(stackCommandBuffer));
				if(cmdLen > sizeof(stackCommandBuffer))
				{
					if(bufferQueue == NULL)
					{
						bufferQueue = new BufferCmdQueue();
					}
					bufferQueue->wr_reserve(cmdLen);
					cmdLen = _socket->recvToCmd_NoPoll(bufferQueue->wr_buf(), bufferQueue->wr_size());
					cmdBuffer = bufferQueue->wr_buf();
				}
				if (cmdLen == 0)
				{
					//这里只是从缓冲取数据包，所以不会出错，没有数据直接返回
					break;
				}

				if(baseServer->baseMessageService->parseTaskTickCommand(this, cmdBuffer, cmdLen) == false)
				{
					_tick_timer.next(baseServer->time());
					parseTaskRawCmd(cmdBuffer, cmdLen);
				}
			}
			while(true);
			SAFE_DELETE(bufferQueue);
		}

		return true;
	}

	bool nTaskConn::listeningSend()
	{
		return _socket->sync();
	}

	void nTaskConn::getNextState()
	{
		eTaskConnState old_state = getState();

		switch(old_state)
		{
			case TaskConnState_NotUse:
				setState(TaskConnState_Verify);
				break;
			case TaskConnState_Verify:
				setState(TaskConnState_Sync);
				break;
			case TaskConnState_Sync:
				_buffered = true;
				setState(TaskConnState_Okay);
				break;
			case TaskConnState_Okay:
				setState(TaskConnState_Recycle);
				break;
			case TaskConnState_Recycle:
				setState(TaskConnState_NotUse);
				break;
			default:
				{
					assert_fail("未知状态");
				}
		}

		_server_conn->debug("Task连接状态[%s],[%s->%s]", getIP(), getStateString(old_state), getStateString(getState()));
	}

	void nTaskConn::resetState()
	{
		eTaskConnState oldState = getState();
		assert_debug(oldState != TaskConnState_Recycle && oldState != TaskConnState_NotUse, "%u", oldState);
		setState(TaskConnState_Recycle);
		_server_conn->debug("Task状态重置[%s->%s]", getStateString(oldState), getStateString(getState()));
	}

	void nTaskConn::checkSignal(const nTime &ct)
	{
		if(needCheckTick() == false){
			return;
		}
		if(checkHeartTickInterval(ct) == false){
			return;
		}

		if (checkTick()){
			//测试信号在指定时间范围内没有返回
			_server_conn->error("套接口检查心跳失败 [%llu]", (baseServer->time().usec() - _start_tick_time_in_usec));
			_in_tick = false;
			_start_tick_time_in_usec = 0;
			setNeedClose();
		}
		else{
			//发送测试信号
			if(baseServer->baseMessageService->sendTaskTickCommand(this, ct) == true){
				setTick(ct);
			}
		}
	}

	nSocket& nTaskConn::getSocket()
	{
		return *_socket;
	}

	nServerConn* nTaskConn::castAsServerConn() const
	{
		return _server_conn;
	}

	bool nTaskConnNoQueue::parseTaskRawCmd(const void *packet, const DWORD len)
	{
		return parseRawCmd(packet, len);
	}

	bool nTaskConnNoQueue::sendCmd(const void *nullCommand, const DWORD cmdLen) const
	{
		return sendTaskCmd(nullCommand, cmdLen);
	}

	bool nTaskConnNoQueue::sendRawData(const void *data, const DWORD len) const
	{
		return sendRawDataIM(data, len);
	}

	nTaskConnNoQueue::nTaskConnNoQueue(const int sock, const struct sockaddr *addr, const SocketPortData* listenPortData):
		nTaskConn(static_cast<nServerConnNoQueue*>(this), sock, addr, listenPortData)
	{
	}

	nTaskConnNoQueue::~nTaskConnNoQueue()
	{
	}

	bool nTaskConnNoQueue::recycleConn()
	{
		if(checkVerifyOK()) {
			baseServer->removeTaskConnID(this);
		}
		return true;
	}

	nTaskConnQueue::nTaskConnQueue( const int sock, const struct sockaddr *addr, const SocketPortData* listenPortData) : nTaskConn((nServerConnQueue*)this, sock, addr, listenPortData)
	{
	}

	nTaskConnQueue::~nTaskConnQueue()
	{
		assert_debug(isAddedToManager() == false, "");
	}

	bool nTaskConnQueue::parseTaskRawCmd(const void *packet, const DWORD len)
	{
		return parseRawCmd(packet, len);
	}

	bool nTaskConnQueue::sendCmd(const void *nullCommand, const DWORD cmdLen) const
	{
		return sendTaskCmd(nullCommand, cmdLen);
	}

	bool nTaskConnQueue::sendRawData(const void *data, const DWORD len) const
	{
		return sendRawDataIM(data, len);
	}

	bool nTaskConnQueue::recycleConn()
	{
		switch(getRecycleState())
		{
			case nServerConnQueue::eConnRecycleState_None:
				{
					// 对于有缓冲队列的连接,在这一步已经在recycle状态,首先执行removeTaskConnID,会允许新的连接验证通过,但是由于在tick中才会实际加到管理器,而且是先执行remove，再执行条件,不会导致冲突.如果不在这一步执行,导致verify成功,但是addTaskConnID失败,会导致verify中设置的一些逻辑无法回退
					if(_server_conn->getID() != 0){
						baseServer->removeTaskConnID(this);
					}
					if(isAddedToManager() == false){
						return true;
					}
					setRecycleState(nServerConnQueue::eConnRecycleState_WaitRemoveFromManager);
					return false;
				}
				break;
			case nServerConnQueue::eConnRecycleState_WaitRemoveFromManager:
				{
					return false;
				}
				break;
			case nServerConnQueue::eConnRecycleState_WaitDelete:
				{
					if(_is_net_error == false && _socket->getSendBufferSize() > 0){
#ifdef _DEBUG
						baseServer->debug("发送了队列遗留数据 [%u]", _socket->getSendBufferSize());
#endif
						if(listeningSend() == false){
							_is_net_error = true;
						}
						else if(_socket->getSendBufferSize() > 0){
							// 没有发生网络错误,数据没有发送完,就继续发送
							return false;
						}
					}
					if(_recycle_time > 0){
						_recycle_time -= 100;
						return false;
					}
					return true;
				}
				break;
			default:
				{
					assert_fail("无法处理的状态[%u]", getRecycleState());
				}
		}

		return true;
	}
};
