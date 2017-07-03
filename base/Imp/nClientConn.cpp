#include "base/nClientConn.h"
#include "base/Tidy.h"
#include "base/nBaseServer.h"
#include "base/nBaseMessageService.h"
#include "base/nBaseMonitorService.h"

#include "base/Imp/nSocket.h"

namespace Tidy
{
	nClientConn::nClientConn(nServerConn* serverConn, const nConnectServerData &connectServerData) : 
		_state(eClientConnState_Close),
		_connect_server_data(connectServerData),
		_socket(NULL), 
		_is_read_event_regged(false),
		terminate(TM_no),
		_is_in_main_loop(false),
		_tick_timer(connectServerData.tickTimerInMsec, false, Tidy::randBetween(5 * ONE_SEC, TEN_SEC)),
		_is_in_heart_tick(false),
		_server_conn(serverConn)
	{
		lifeTime.now();
	}

	nClientConn::~nClientConn() 
	{
		final();
	}

	void nClientConn::final()
	{
		SAFE_DELETE(_socket);
		terminate = TM_no;
		_is_in_main_loop = false;
	}

	bool nClientConn::connectToServer()
	{
		if(connectSocket() == false) {
			return false;
		}
		if(baseServer->loginToServer(_server_conn) == false){
			SAFE_DELETE(_socket);
			return false;
		}
		return true;
	}

	bool nClientConn::connectSocket()
	{
		assert_debug(_socket == NULL, "");
		if(_connect_server_data.serverUnixSocketPath.empty() == true){
			_socket = nSocket::connectTcpServer(_connect_server_data.serverIP.c_str(), _connect_server_data.serverPort, _connect_server_data.commandCtl, _connect_server_data.zipMethod);
		}
		else{
			_socket = nSocket::connectUnixServer(_connect_server_data.serverUnixSocketPath.c_str(), _connect_server_data.commandCtl, _connect_server_data.zipMethod);
		}

		if(_socket == NULL){
			return false;
		}

		return true;
	}

	void nClientConn::checkConn()
	{
		if(_connect_server_data.needCheckHeartTick == false){
			return;
		}
		nTime &currentTime = baseServer->time();
		if(_tick_timer(currentTime)){
			if(checkHeartTick()){
				//测试信号在指定时间范围内没有返回
				_server_conn->error("套接口检查测试心跳失败,关闭连接");
				Terminate(nClientConn::TM_sock_error);
				clearTick();
			}
			else{
				if(baseServer->baseMessageService->sendClientTickCommand(this) == true){
					setInHeartTick();
				}
			}
		}
	}

	bool nClientConn::sendClientCmd(const void *pstrCmd, const int nCmdLen) const
	{
		if(_socket == NULL){
			return true;
		}
		bool buffer = (_state != eClientConnState_Close && _state != eClientConnState_Sync);
		baseServer->baseMonitorService->dealSendCommand(_server_conn, _socket->getCommandCtl(), pstrCmd, nCmdLen);
		return const_cast<nSocket*>(_socket)->sendCmd(pstrCmd, nCmdLen, buffer);
	}

	bool nClientConn::sendRawDataIM(const void *data, const DWORD len) const
	{
		if(_socket == NULL){
			return true;
		}
		return const_cast<nSocket*>(_socket)->sendRawData(data, len);
	}

	bool nClientConn::listeningRecv(bool needRecv)
	{
		int retcode = 0;
		if (needRecv) {
			retcode = _socket->recvToBuf_NoPoll();
		}
		if (-1 == retcode) {
			baseServer->error("%s", __PRETTY_FUNCTION__);
			return false;
		}

		BufferCmdQueue *bufferQueue = NULL;
		char pstrCmd[MAX_PROTO_CMD_LEN];
		do {
			char* cmdBuffer = pstrCmd;
			DWORD nCmdLen = _socket->recvToCmd_NoPoll(pstrCmd, sizeof(pstrCmd));
			if (nCmdLen == 0) {
				//这里只是从缓冲取数据包，所以不会出错，没有数据直接返回
				break;
			}
			else if(nCmdLen > sizeof(pstrCmd)) {
				if(bufferQueue == NULL) {
					bufferQueue = new BufferCmdQueue();
				}
				bufferQueue->wr_reserve(nCmdLen);
				nCmdLen = _socket->recvToCmd_NoPoll(bufferQueue->wr_buf(), bufferQueue->wr_size());
				cmdBuffer = bufferQueue->wr_buf();
			}
			if(baseServer->baseMessageService->parseClientTickCommand(this, cmdBuffer, nCmdLen) == false) {
				_tick_timer.next(baseServer->time());
				parseClientRawCmd(cmdBuffer, nCmdLen);
			}
		}while(true);

		SAFE_DELETE(bufferQueue);

		return true;
	}

	bool nClientConn::listeningSend()
	{
		if (_socket)
			return _socket->sync();
		else
			return false;
	}

	void nClientConn::getNextState()
	{
		eClientConnState old_state = getState();

		lifeTime.now();
		switch(old_state)
		{
			case eClientConnState_Close:
				setState(eClientConnState_Sync);
				break;
			case eClientConnState_Sync:
				*const_cast<bool*>(&_connect_server_data.needWaitConnected) = false;
				setState(eClientConnState_Ok);
				break;
			case eClientConnState_Ok:
				setState(eClientConnState_Recycle);
				break;
			case eClientConnState_Recycle:
				setState(eClientConnState_Close);
				final();
				break;
			default:
				{
				}
		}

		_server_conn->debug("client连接状态[%s,%u],[%s->%s]", _connect_server_data.serverIP.c_str(), _connect_server_data.serverPort, getStateString(old_state), getStateString(getState()));
	}

	void nClientConn::resetState()
	{
		eClientConnState old_state = getState();

		lifeTime.now();
		final();

		baseServer->debug("nClientConn[%s,%u,%s->%s]", _connect_server_data.serverIP.c_str(), _connect_server_data.serverPort, getStateString(old_state), getStateString(getState()));
		setState(eClientConnState_Recycle);
		if(_connect_server_data.needReconnect == false && _connect_server_data.needWaitConnected == true) {
			baseServer->terminate(eServerCloseReason_Normal);
		}
	}

	bool nClientConn::needReConn()
	{
		return _connect_server_data.needReconnect;
	}

	const WORD nClientConn::getPort() const
	{
		return _socket->getPort();
	}

	const char* nClientConn::getIP() const
	{
		return _socket->getIP();
	}

	bool nClientConn::isReadEventRegged()
	{
		return _is_read_event_regged;
	}

	bool nClientConn::setReadEventRegged(bool isReadEventRegged)
	{
		_is_read_event_regged = isReadEventRegged;
		return _is_read_event_regged;
	}

	nServerConn* nClientConn::castAsServerConn() const
	{
		return _server_conn;
	}

	void nClientConn::addEpoll(int kdpfd, __uint32_t events, void *ptr)
	{
		if (_socket)
			_socket->addEpoll(kdpfd, events, ptr);
	}

	void nClientConn::delEpoll(int kdpfd, __uint32_t events)
	{
		if (_socket)
			_socket->delEpoll(kdpfd, events);
	}

	bool nClientConn::checkStateTimeout(const eClientConnState state, const nTime &ct, const unsigned long timeout)
	{
		if (state == _state) {
			if((lifeTime.elapse(ct) >= timeout)) {
				lifeTime = ct;
				return true;
			}
		}
		return false;
	}

	int nClientConn::checkRebound()
	{
		int retcode = _socket->recvToBuf_NoPoll();
		if(retcode <= 0) {
			return retcode;
		}
		char pstrCmd[MAX_PROTO_CMD_LEN];
		while(true) {
			int nCmdLen = _socket->recvToCmd_NoPoll(pstrCmd, sizeof(pstrCmd));
			if(nCmdLen <= 0) {
				//这里只是从缓冲取数据包，所以不会出错，没有数据直接返回
				break;
			}

			if(baseServer->baseMessageService->parseClientTickCommand(this, pstrCmd, nCmdLen) == true) {
				continue;
			}

			if(baseServer->checkClientSyncResult(this, pstrCmd, nCmdLen) == true) {
				return 1;
			}
		}
		return 0;
	}

	bool nClientConn::checkHeartTick() const
	{
		return _is_in_heart_tick;
	}

	void nClientConn::setInHeartTick()
	{
		_is_in_heart_tick = true;
	}

	const char* nClientConn::getStateString(const eClientConnState state)
	{
		static const char* clientConnStateDescVec[eClientConnState_Max] = {"Close", "Sync", "OK", "Recycle"};
		return clientConnStateDescVec[state];
	}

	void nClientConn::setNeedClose()
	{
		Terminate(eTerminateMethod_NeedClose);
	}

	nClientConnNoQueue::nClientConnNoQueue(const nConnectServerData &connectServerData) : 
		nClientConn(static_cast<nServerConnNoQueue*>(this), connectServerData),
		nServerConnNoQueue(connectServerData.serverID, connectServerData.serverName.c_str(), connectServerData.serverType) 
	{
	}

	nClientConnNoQueue::~nClientConnNoQueue() 
	{
	}

	bool nClientConnNoQueue::sendCmd(const void *command, const DWORD len) const
	{
		return sendClientCmd(command, len);
	}

	bool nClientConnNoQueue::sendRawData(const void *data, const DWORD len) const
	{
		return sendRawDataIM(data, len);
	}

	bool nClientConnNoQueue::parseClientRawCmd(const void *packet, const DWORD len)
	{
		return parseRawCmd(packet, len);
	}

	bool nClientConnNoQueue::recycleConn()
	{
		return true;
	}

	nClientConnQueue::nClientConnQueue(const nConnectServerData &connectServerData) :
		nClientConn(static_cast<nServerConnQueue*>(this), connectServerData),
		nServerConnQueue(connectServerData.serverID, connectServerData.serverName.c_str(), connectServerData.serverType) 
	{
	}

	nClientConnQueue::~nClientConnQueue() 
	{
	}

	bool nClientConnQueue::sendCmd(const void *command, const DWORD len) const
	{
		return sendClientCmd(command, len);
	}

	bool nClientConnQueue::sendRawData(const void *data, const DWORD len) const
	{
		return sendRawDataIM(data, len);
	}

	bool nClientConnQueue::parseClientRawCmd(const void *packet, const DWORD len)
	{
		return parseRawCmd(packet, len);
	}

	bool nClientConnQueue::recycleConn()
	{
		switch(getRecycleState())
		{
			case nServerConnQueue::eConnRecycleState_None:
				{
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
					return true;
				}
				break;
			default:
				{
					assert_fail("无法处理的状态[%u]", getRecycleState());
				}
		}

		return false;
	}
};
