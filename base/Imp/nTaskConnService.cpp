#include "base/nThread.h"
#include "base/Tidy.h"
#include "base/nTime.h"
#include "base/nBaseServer.h"

#include "base/Imp/nTaskConnService.h"
#include "base/Imp/nSocket.h"

namespace Tidy
{

	long nTaskConnService::usleep_time=50000L;
	DWORD nTaskConnPoolVerifyThread::maxTaskNumInOneThread;
	DWORD nTaskConnPoolOkayThread::maxTaskNumInOneThread;

	nTaskConnPoolVerifyThread::nTaskConnPoolVerifyThread():
		nThread("nTaskConnPoolVerifyThread"),
		_task_conn_num(0)
	{
		kdpfd = epoll_create(maxTaskNumInOneThread);
		assert(-1 != kdpfd);
		epfds.resize(maxTaskNumInOneThread);
	}

	nTaskConnPoolVerifyThread::~nTaskConnPoolVerifyThread()
	{
		TEMP_FAILURE_RETRY(::close(kdpfd));
	}

	void nTaskConnPoolVerifyThread::_add(nTaskConn *conn)
	{
		conn->addEpoll(kdpfd, ASSERT_CAST(__uint32_t, EPOLLIN | EPOLLERR | EPOLLPRI), (void *)conn);
		_task_conn_list.push_back(conn);
		++ _task_conn_num;
		if (_task_conn_num > epfds.size()) {
			epfds.resize(_task_conn_num + 16);
		}
	}

	void nTaskConnPoolVerifyThread::remove(nTaskConn *conn)
	{
		conn->delEpoll(kdpfd, ASSERT_CAST(__uint32_t, EPOLLIN | EPOLLERR | EPOLLPRI));
		_task_conn_list.remove(conn);
		assert_debug(_task_conn_num > 0, "[%u]", _task_conn_num);
		-- _task_conn_num;
	}

	void nTaskConnPoolVerifyThread::remove(nTaskConn_IT &it)
	{
		(*it)->delEpoll(kdpfd, ASSERT_CAST(__uint32_t, EPOLLIN | EPOLLERR | EPOLLPRI));
		it = _task_conn_list.erase(it);
		assert_debug(_task_conn_num > 0, "[%u]", _task_conn_num);
		-- _task_conn_num;
	}

	DWORD nTaskConnPoolVerifyThread::taskSize() const
	{
		return _task_conn_num + _size;
	}

	void nTaskConnPoolVerifyThread::run()
	{
		nTime currentTime;
		nTaskConn_IT it;

		while(!isFinal()){
			this->setRunning();
			currentTime.now();

			check_queue();
			if (!_task_conn_list.empty()){
				for(it = _task_conn_list.begin(); it != _task_conn_list.end();){
					nTaskConn *conn = *it;
					if (conn->checkVerifyTimeout(currentTime)){
						//超过指定时间验证还没有通过，需要回收连接
						baseServer->debug("连接验证超时");
						remove(it);
						baseServer->taskConnService->addToRecycle(conn);
					}
					else{
						++it;
					}
				}

				int retcode = epoll_wait(kdpfd, &epfds[0], _task_conn_num, 0);
				if (retcode > 0){
					for(int i = 0; i < retcode; i++){
						nTaskConn *conn = (nTaskConn *)epfds[i].data.ptr;
						//if (epfds[i].events & (EPOLLERR | EPOLLPRI))
						if (epfds[i].events & (EPOLLERR)){
							//套接口出现错误
							baseServer->error("套接口错误:%u,%u",epfds[i].events,conn->getSocket().getSocket());
							remove(conn);
							baseServer->taskConnService->addToRecycle(conn);
						}
						else if (epfds[i].events & EPOLLIN){
							switch(conn->verifyConn())
							{
								case nTaskConn::eVerifyState_Success:
									{
										//验证成功
										remove(conn);
										//再做唯一性验证
										if (baseServer->addTaskConnID(conn) == true){
											//唯一性验证成功，获取下一个状态
											baseServer->taskConnService->addToSync(conn);
										}
										else{
											//唯一性验证失败，回收连接任务
											conn->setVerifyState(nTaskConn::eVerifyState_Fail);
											baseServer->info("客户端唯一性验证失败[%u]", conn->castAsServerConn()->getID());
											baseServer->taskConnService->addToRecycle(conn);
										}
									}
									break;
								case nTaskConn::eVerifyState_None:
									//超时，下面会处理
									break;
								case nTaskConn::eVerifyState_Fail:
									//验证失败，回收任务
									baseServer->debug("客户端连接验证失败");
									remove(conn);
									baseServer->taskConnService->addToRecycle(conn);
									break;
								default:
									{
									}
							}
						}
					}
				}

				nThread::msleep(50);
			}
			else {
				nThread::msleep(100);
			}
		}

		//把所有等待验证队列中的连接加入到回收队列中，回收这些连接
		for(it = _task_conn_list.begin(); it != _task_conn_list.end();){
			nTaskConn *conn = *it;
			remove(it);
			baseServer->taskConnService->addToRecycle(conn);
		}
	}

	nTaskConnPoolSyncThread::nTaskConnPoolSyncThread():
		nThread("nTaskConnPoolSyncThread"),
		_task_conn_num(0)
	{
	}

	nTaskConnPoolSyncThread::~nTaskConnPoolSyncThread() 
	{
	}

	void nTaskConnPoolSyncThread::_add(nTaskConn *conn)
	{
		baseServer->addSyncTaskConn(conn);
		_task_conn_list.push_front(conn);
		++ _task_conn_num;
	}

	nTaskConn_IT nTaskConnPoolSyncThread::remove(nTaskConn_IT &iter)
	{
		assert_debug(_task_conn_num > 0, "[%u]", _task_conn_num);
		-- _task_conn_num;
		baseServer->removeSyncTaskConn(*iter);
		return _task_conn_list.erase(iter);
	}

	DWORD nTaskConnPoolSyncThread::taskSize() const
	{
		return _task_conn_num + _size;
	}

	void nTaskConnPoolSyncThread::run()
	{
		nTaskConn_IT it;

		while(!isFinal()){
			this->setRunning();
			check_queue();

			if (!_task_conn_list.empty()){
				for(it = _task_conn_list.begin(); it != _task_conn_list.end();){
					nTaskConn *conn = *it;
					switch(conn->waitSync())
					{
						case nTaskConn::eSyncState_Success:
							{
								it = remove(it);
								if (baseServer->taskConnService->addToOkay(conn) == false){
									baseServer->taskConnService->addToRecycle(conn);
								}
							}
							break;
						case nTaskConn::eSyncState_None:
							{
								it++;
							}
							break;
						case nTaskConn::eSyncState_Fail:
							{
								//等待其它线程同步验证失败，需要回收连接
								it = remove(it);
								baseServer->taskConnService->addToRecycle(conn);
							}
							break;
						default:
							{
							}
					}
				}

				nThread::msleep(50);
			}
			else{
				nThread::msleep(500);
			}
		}

		//把所有等待同步验证队列中的连接加入到回收队列中，回收这些连接
		for(it = _task_conn_list.begin(); it != _task_conn_list.end();){
			nTaskConn *conn = *it;
			it = remove(it);
			baseServer->taskConnService->addToRecycle(conn);
		}
	}

	nTaskConnPoolOkayThread::nTaskConnPoolOkayThread():
		nThread("nTaskConnPoolOkayThread"),
		_task_conn_num(0)
	{
		kdpfd = epoll_create(maxTaskNumInOneThread);
		assert(-1 != kdpfd);
		epfds.resize(maxTaskNumInOneThread);
	}

	nTaskConnPoolOkayThread::~nTaskConnPoolOkayThread()
	{
		TEMP_FAILURE_RETRY(::close(kdpfd));
	}

	void nTaskConnPoolOkayThread::_add(nTaskConn *conn)
	{
		conn->addEpoll(kdpfd, ASSERT_CAST(__uint32_t, EPOLLOUT | EPOLLERR | EPOLLPRI ), (void *)conn);
		_task_conn_list.push_back(conn);
		++ _task_conn_num;
		if (_task_conn_num > epfds.size()){
			epfds.resize(_task_conn_num + 16);
		}
		conn->listeningRecv(false);

		baseServer->addTaskConn(conn);
	}

	void nTaskConnPoolOkayThread::remove(nTaskConn_IT &it)
	{
		baseServer->removeTaskConn(*it);
		(*it)->delEpoll(kdpfd, ASSERT_CAST(__uint32_t, EPOLLOUT | EPOLLERR | EPOLLPRI ));
		it = _task_conn_list.erase(it);
		assert_debug(_task_conn_num > 0, "[%u]", _task_conn_num);
		-- _task_conn_num;
	}

	DWORD nTaskConnPoolOkayThread::taskSize() const
	{
		return _task_conn_num + _size;
	}

	void nTaskConnPoolOkayThread::run()
	{
		nTime currentTime;
		nTime _write_time;
		nTaskConn_IT it;
		nTaskConn *conn = NULL;

		epollfdContainer readConnContainer;
		int readEpollContext = epoll_create(256);
		assert(-1 != readEpollContext);
		epfds.resize(256);
		DWORD readConnNum = 0;
		bool check=false;
		// 现在先不采用ET模式
		__uint32_t readEvent = ASSERT_CAST(__uint32_t, EPOLLIN | EPOLLERR | EPOLLPRI);
		while(!isFinal()){
			this->setRunning();
			currentTime.now();

			if (check){
				check_queue();
				if (!_task_conn_list.empty()) {
					for(it = _task_conn_list.begin(); it != _task_conn_list.end();) {
						assert_debug(_task_conn_list.empty() == false, "[%u]", _task_conn_list.size());
						conn = *it;
						//检查测试信号指令
						conn->checkSignal(currentTime);

						if (conn->checkNeedClose() == true) {
							if (conn->isReadEventRegged() == true) {
								conn->delEpoll(readEpollContext, readEvent);
								assert_debug(readConnNum > 0, "连接数错误");
								readConnNum --;
							}
							conn->listeningSend();
							remove(it);

							baseServer->taskConnService->addToRecycle(conn);
						}
						else {
							if(conn->isReadEventRegged() == false) {
								conn->addEpoll(readEpollContext, readEvent, (void *)conn);
								conn->setReadEventRegged(); 
								readConnNum++;
								if (readConnNum > readConnContainer.size()) {
									readConnContainer.resize(readConnNum + 16);
								}
							}
							++it;
						}
					}
				}
				check=false;
			}
			nThread::msleep(2);

			if(readConnNum) {
				int retcode = epoll_wait(readEpollContext, &readConnContainer[0], readConnNum, 0);
				if (retcode > 0) {
					struct epoll_event *readEventList = &readConnContainer[0];
					for(int i = 0; i < retcode; ++ i) {
						struct epoll_event &eventData = readEventList[i];
						nTaskConn *conn = (nTaskConn *)eventData.data.ptr;
						//if (eventData.events & (EPOLLERR | EPOLLPRI))
						if (eventData.events & (EPOLLERR)) {
							//套接口出现错误
                            baseServer->error("[%u,%s]套接口异常错误:%u", conn->castAsServerConn()->getServerID(), conn->castAsServerConn()->getName(), eventData.events);
                            conn->dealNetError();
							check = true;
						}
						else {
							if (eventData.events & EPOLLIN) {
								//套接口准备好了读取操作
								if (!conn->listeningRecv(true)) {
									conn->castAsServerConn()->info("套接口读操作错误");
									conn->dealNetError();
									check=true;
								}
							}
						}
						eventData.events=0; 
					}
				}
			}
			else{
				nThread::msleep(20);
			}
			if(check) {
				continue;
			}
			if (currentTime.msec() - _write_time.msec() >= (unsigned long)(baseServer->taskConnService->usleep_time/1000)) {
				_write_time = currentTime;
				if (!_task_conn_list.empty()) {
					int retcode = epoll_wait(kdpfd, &epfds[0], _task_conn_num, 0);
					if (retcode > 0) {
						epoll_event *eventList = &epfds[0];
						for(int i = 0; i < retcode; i++) {
							struct epoll_event &eventData = eventList[i];
							nTaskConn *conn = (nTaskConn *)eventData.data.ptr;
							//if (eventData.events & (EPOLLERR | EPOLLPRI))
							if (eventData.events & (EPOLLERR)) {
								//套接口出现错误
								baseServer->info("[%u,%s]套接口异常错误:%u", conn->castAsServerConn()->getServerID(), conn->castAsServerConn()->getName(), eventData.events);
								conn->dealNetError();
							}
							else {
								if (eventData.events & EPOLLOUT) {
									//套接口准备好了写入操作
									if (!conn->listeningSend()) {
										baseServer->info("[%u,%s]: 套接口写操作错误", conn->castAsServerConn()->getServerID(), conn->castAsServerConn()->getName());
										conn->dealNetError();
									}
								}
							}
							eventData.events=0; 
						}
					}
				}
				check=true;
			}
		}

		//把所有任务队列中的连接加入到回收队列中，回收这些连接
		for(it = _task_conn_list.begin(); it != _task_conn_list.end();) {
			assert_debug(_task_conn_list.empty() == false, "[%u]", _task_conn_list.size());
			conn = *it;
			remove(it);
			baseServer->taskConnService->addToRecycle(conn);
		}
		TEMP_FAILURE_RETRY(::close(readEpollContext));
	}

	nTaskConnPoolRecycleThread::nTaskConnPoolRecycleThread():
		nThread("nTaskConnPoolRecycleThread"),
		_task_conn_num(0)
	{
	}

	nTaskConnPoolRecycleThread::~nTaskConnPoolRecycleThread() 
	{
	}

	void nTaskConnPoolRecycleThread::_add(nTaskConn *conn)
	{
		_task_conn_list.push_front(conn);
		++ _task_conn_num;
	}

	DWORD nTaskConnPoolRecycleThread::taskSize() const
	{
		return _task_conn_num + _size;
	}

	void nTaskConnPoolRecycleThread::run()
	{
		nTaskConn_IT it;

		do{
			while(isFinal() == false || _task_conn_list.empty() == false){
				this->setRunning();
				check_queue();

				if (!_task_conn_list.empty()) {
					for(it = _task_conn_list.begin(); it != _task_conn_list.end();) {
						nTaskConn *conn = *it;
						if(conn->recycleConn() == true){
							//回收处理完成可以释放相应的资源
							it = _task_conn_list.erase(it);
							assert_debug(_task_conn_num > 0, "[%u]", _task_conn_num);
							-- _task_conn_num;
							conn->getNextState();
							SAFE_DELETE(conn);
						}
						else{
							//回收超时，下次再处理
							++ it;
						}
					}
					nThread::msleep(50);
				}
				else{
					nThread::msleep(200);
				}
			}

			//退出之前重新检查等待加入的队列.防止这一部分被遗漏.
			check_queue();
		}while(_task_conn_list.empty() == false);

		assert_debug(_task_conn_list.empty() == true, "");
	}


	nTaskConnService::nTaskConnService(const int us)
	{
		setUsleepTime(us);
	}

	nTaskConnService::~nTaskConnService()
	{
		assert_debug(_verify_thread_manager.empty(), "");
		assert_debug(_sync_thread_manager.empty(), "");
		assert_debug(_main_thread_manager.empty(), "");
		assert_debug(_recycle_thread_manager.empty(), "");
	}

	const int nTaskConnService::getAllSize()
	{
		return _main_thread_manager.getTaskNum();
	}

	bool nTaskConnService::addToVerify(nTaskConn *conn)
	{
		nTaskConnPoolVerifyThread *pThread = _verify_thread_manager.getOne();
		if(pThread == NULL) {
			assert_fail("没有找到线程");
			baseServer->error("没有找到验证线程添加任务");
			return false;
		}
		// state_sync -> state_okay
		conn->getNextState();
		pThread->add(conn);
		return true;
	}

	void nTaskConnService::addToSync(nTaskConn *conn)
	{
		nTaskConnPoolSyncThread *pThread = _sync_thread_manager.getOne();
		if(pThread == NULL) {
			assert_fail("没有找到线程");
			baseServer->error("没有找到同步线程添加任务");
			return;
		}

		// state_sync -> state_okay
		conn->getNextState();
		pThread->add(conn);
	}

	bool nTaskConnService::addToOkay(nTaskConn *conn)
	{
		nTaskConnPoolOkayThread *pThread = _main_thread_manager.getOne();
		if (pThread == NULL) {
			assert_fail("没有找到线程");
			baseServer->fatal("没有找到主线程添加任务");
			return false;
		}
		// state_sync -> state_okay
		conn->getNextState();
		pThread->add(conn);
		return true;
	}

	void nTaskConnService::addToRecycle(nTaskConn *conn)
	{
		nTaskConnPoolRecycleThread *pThread = _recycle_thread_manager.getOne();
		if(pThread == NULL) {
			baseServer->error("没有找到回收线程添加任务");
			assert_fail("没有找到线程");
			return;
		}
		conn->resetState();
		pThread->add(conn);
	}

	bool nTaskConnService::initService()
	{
		//创建回收线程
		if(_recycle_thread_manager.init(Tidy::global["maxTaskRecycleThreadNum"], Tidy::global["maxTaskNumInOneRecycleThread"], "nTaskConnPoolRecycleThread", this) == false) {
			baseServer->debug("nTaskConnPoolRecycleThread线程启动失败");
			return false;
		}

		//创建初始化验证线程
		nTaskConnPoolVerifyThread::maxTaskNumInOneThread = Tidy::global["maxTaskNumInOneVerifyThread"];
		if(_verify_thread_manager.init(Tidy::global["maxTaskVerifyThreadNum"], Tidy::global["maxTaskNumInOneVerifyThread"], "nTaskConnPoolVerifyThread", this) == false) {
			baseServer->debug("nTaskConnPoolVerifyThread线程启动失败");
			return false;
		}

		//创建初始化同步线程
		if(_sync_thread_manager.init(Tidy::global["maxTaskSyncThreadNum"], Tidy::global["maxTaskNumInOneSyncThread"], "nTaskConnPoolSyncThread", this) == false) {
			baseServer->debug("nTaskConnPoolSyncThread线程启动失败");
			return false;
		}

		//创建初始主线程
		nTaskConnPoolOkayThread::maxTaskNumInOneThread = Tidy::global["maxTaskNumInOneMainThread"];
		if(_main_thread_manager.init(Tidy::global["maxTaskMainThreadNum"], Tidy::global["maxTaskNumInOneMainThread"], "nTaskConnPoolOkayThread", this) == false) {
			baseServer->debug("nTaskConnPoolOkayThread线程启动失败");
			return false;
		}

		return true;
	}

	void nTaskConnService::finalService()
	{
		_verify_thread_manager.final();
		_sync_thread_manager.final();
		_main_thread_manager.final();
		_recycle_thread_manager.final();
	}
};
