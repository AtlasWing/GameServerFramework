#include "base/Imp/nClientConnService.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	nCheckClientConnectThread::nCheckClientConnectThread():
		nThread("nCheckClientConnectThread"),
		_client_task_num(0)
	{
	}

	nCheckClientConnectThread::~nCheckClientConnectThread()
	{
		assert_debug(_client_task_num == 0, "[%u]", _client_task_num);
	}

	void nCheckClientConnectThread::_add(nClientConn *task)
	{
		_client_task_list.push_back(task);
		++ _client_task_num;
	}

	nClientConn_IT nCheckClientConnectThread::remove(nClientConn_IT &iter)
	{
		-- _client_task_num;
		return _client_task_list.erase(iter);
	}

	DWORD nCheckClientConnectThread::taskSize()const
	{
		return _client_task_num + _size;
	}

	void nCheckClientConnectThread::run()
	{
		nTime ct;
		while(isFinal() == false){
			this->setRunning();
			ct.now();
			check_queue();
			if(_client_task_list.empty() == false){
				nClientConn *task = NULL;
				for(nClientConn_IT it = _client_task_list.begin(); it != _client_task_list.end();){
					task = *it;
					switch(task->getState())
					{
						case nClientConn::eClientConnState_Close:
							{
								if (task->checkStateTimeout(nClientConn::eClientConnState_Close, ct, 500)){
									if(task->connectToServer()){
										baseServer->clientConnService->addToCheckwait(task);
										setRunning();
									}
									else if(!task->needReConn()){
										baseServer->debug("不需要断线重连");
										task->resetState();
										++ it;
										continue;
									}
								}
							}
							break;
						case nClientConn::eClientConnState_Sync:
							break;
						case nClientConn::eClientConnState_Ok:
							{
								//已经在连接状态，发送网络测试信号
								//task->checkConn();
							}
							break;
						case nClientConn::eClientConnState_Recycle:
							{
								//if (task->checkStateTimeout(nClientConn::eClientConnState_Recycle, ct, 1000)){
									if(task->recycleConn() == true){
										if(!task->needReConn()){
											baseServer->debug("回收后不需要断线重连");
											nClientConn_IT temp = it;
											++ it;
											remove(temp);
											baseServer->dealClientConnRecycle(task);
											SAFE_DELETE(task);
											continue;
										}
										else{
											task->getNextState();
										}
									}
								//}
							}
							break;
						default:
							{
							}
					}
					++ it;
				}

				nThread::msleep(200);
			}
			else{
				nThread::msleep(600);
			}
		}


		for(nClientConn_IT iter = _client_task_list.begin(); iter != _client_task_list.end(); ++ iter){
			(*iter)->resetState();
		}
		while(_client_task_list.empty() == false){
			nThread::msleep(50);
			for(nClientConn_IT iter = _client_task_list.begin(); iter != _client_task_list.end();){
				nClientConn *task = *iter;
				if(task->recycleConn() == true){
					iter = remove(iter);
					SAFE_DELETE(task);
				}
				else{
					++ iter;
				}
			}
		}
	}


	void nCheckClientWaitThread::_add(nClientConn *task)
	{
		task->addEpoll(kdpfd, EPOLLIN | EPOLLERR | EPOLLPRI, (void *)task);
		_client_task_list.push_back(task);
		++ _client_task_num;
		if (_client_task_num > epfds.size()){
			epfds.resize(_client_task_num + 16);
		}
	}

	void nCheckClientWaitThread::remove(nClientConn *task)
	{
		task->delEpoll(kdpfd, EPOLLIN | EPOLLERR | EPOLLPRI);
		_client_task_list.remove(task);
		-- _client_task_num;
	}

	void nCheckClientWaitThread::remove(nClientConn_IT &it)
	{
		(*it)->delEpoll(kdpfd, EPOLLIN | EPOLLERR | EPOLLPRI);
		_client_task_list.erase(it);
		-- _client_task_num;
	}

	nCheckClientWaitThread::nCheckClientWaitThread():
		nThread("nCheckClientWaitThread"),
		_client_task_num(0)
	{
		kdpfd = epoll_create(8);
		assert(-1 != kdpfd);
		epfds.resize(8);
	}

	nCheckClientWaitThread::~nCheckClientWaitThread()
	{
		assert_debug(_client_task_num == 0, "[%u]", _client_task_num);
		TEMP_FAILURE_RETRY(::close(kdpfd));
	}

	DWORD nCheckClientWaitThread::taskSize()const
	{
		return _client_task_num + _size;
	}

	void nCheckClientWaitThread::run()
	{
		nClientConn_IT it, next;

		while(!isFinal()){
			this->setRunning();
			//nThread::sleep(1);
			check_queue();
			if (!_client_task_list.empty()){
				int retcode = epoll_wait(kdpfd, &epfds[0], _client_task_num, 0);
				if (retcode > 0){
					struct epoll_event *eventList = &epfds[0];
					for(int i = 0; i < retcode; i++){
						struct epoll_event &eventData = eventList[i];
						nClientConn *task = (nClientConn *)eventData.data.ptr;
						if (eventData.events & (EPOLLERR | EPOLLPRI)){
							//套接口出现错误
							remove(task);
							task->resetState();
							task->setState(nClientConn::eClientConnState_Recycle);
						}
						else if (eventData.events & EPOLLIN){
							switch(task->checkRebound())
							{
								case 1:
									//验证成功，获取下一个状态
									remove(task);
									if (!baseServer->clientConnService->addToOkay(task))
										task->resetState();
									break;
								case 0:
									//超时，下面会处理
									break;
								case -1:
									//验证失败，回收任务
									remove(task);
									task->resetState();
									break;
								default:
									{
									}
							}
						}
						eventData.events = 0;
					}
				}

				//如果在同步状态下对方不返回也得判断超时,不然bug很严重
				nTime ct;
				for(it = _client_task_list.begin(), next = it, next++; it != _client_task_list.end(); it = next, next++){
					nClientConn *task = *it;
					if(task->needReConn() == false){
						continue;
					}
					if (task->checkStateTimeout(nClientConn::eClientConnState_Sync, ct, 4000)){
						remove(it);
						task->resetState();
					}
				}

				nThread::msleep(100);
			}
			else{
				nThread::msleep(1000);
			}
		}

		//把所有等待验证队列中的连接加入到回收队列中，回收这些连接
		for(it = _client_task_list.begin(), next = it, next++; it != _client_task_list.end(); it = next, next++){
			nClientConn *task = *it;
			remove(it);
			task->resetState();
		}
	}

	nClientRunThread::nClientRunThread():
		nThread("nClientRunThread"),
		_client_task_num(0)
	{
		kdpfd = epoll_create(16);
		assert(-1 != kdpfd);
		epfds.resize(16);
	}

	nClientRunThread::~nClientRunThread()
	{
		TEMP_FAILURE_RETRY(::close(kdpfd));
		assert_debug(_client_task_num == 0, "[%u]", _client_task_num);
	}

	DWORD nClientRunThread::taskSize()const
	{
		return _client_task_num + _size;
	}

	void nClientRunThread::_add(nClientConn *task)
	{
		task->addEpoll(kdpfd, EPOLLOUT | EPOLLERR | EPOLLPRI, (void *)task);
		_client_task_list.push_back(task);
		++ _client_task_num;
		if (_client_task_num > epfds.size()){
			epfds.resize(_client_task_num + 16);
		}

		baseServer->addClientConn(task);
	}

	void nClientRunThread::remove(nClientConn_IT &it)
	{
		baseServer->removeClientConn(*it);
		(*it)->delEpoll(kdpfd, EPOLLOUT | EPOLLERR | EPOLLPRI);
		_client_task_list.erase(it);
		assert_debug(_client_task_num > 0, "[%u]", _client_task_num);
		-- _client_task_num;
	}

	void nClientRunThread::remove(nClientConn *task)
	{
		baseServer->removeClientConn(task);
		task->delEpoll(kdpfd, EPOLLOUT | EPOLLERR | EPOLLPRI);
		_client_task_list.remove(task);
		assert_debug(_client_task_num > 0, "[%u]", _client_task_num);
		-- _client_task_num;
	}

	void nClientRunThread::run()
	{
		nClientConn_IT it, next;
		nTime currentTime;
		nTime _write_time;

		epollfdContainer readConnContainer;
		int readEpollContext = epoll_create(256);
		assert(-1 != readEpollContext);
		epfds.resize(256);
		DWORD readConnNum = 0;
		bool check=false;
		__uint32_t readEvent = ASSERT_CAST(__uint32_t, EPOLLIN | EPOLLERR | EPOLLPRI);
		while(!isFinal()){
			currentTime.now();
			this->setRunning();
			if (check){
				check_queue();
				if (!_client_task_list.empty()){
					for(it = _client_task_list.begin(), next = it, next++; it != _client_task_list.end(); it = next, next++){
						nClientConn *task = *it;

						if (task->isTerminate()){
							if (task->isReadEventRegged()){
								task->delEpoll(readEpollContext, readEvent);
								readConnNum --;
								task->setReadEventRegged(false); 
							}
							remove(it);
							// state_okay -> state_recycle
							task->getNextState();
						}
						else{
							if (task->checkFirstMainLoop()){
								//如果是第一次加入处理，需要预先处理缓冲中的数据
								task->listeningRecv(false);
							}
							if(!task->isReadEventRegged()){
								task->addEpoll(readEpollContext, readEvent, (void *)task);
								task->setReadEventRegged(true); 
								readConnNum++;
								if (readConnNum > readConnContainer.size()){
									readConnContainer.resize(readConnNum + 16);
								}
							}
							task->checkConn();
						}
					}
				}
				check=false;
			}
			nThread::msleep(2);

			if(readConnNum){
				int retcode = epoll_wait(readEpollContext, &readConnContainer[0], readConnNum, 0);
				if (retcode > 0){
					struct epoll_event *readEventList = &readConnContainer[0];
					for(int i = 0; i < retcode; i++){
						struct epoll_event &eventData = readEventList[i];
						nClientConn *task = (nClientConn *)eventData.data.ptr;
						if (eventData.events & (EPOLLERR | EPOLLPRI)){
							//套接口出现错误
							baseServer->debug("[%s]: 套接口异常错误", __PRETTY_FUNCTION__);
							task->Terminate(nClientConn::TM_sock_error);
							check=true;
						}
						else if (eventData.events & EPOLLIN){
							//套接口准备好了读取操作
							if (!task->listeningRecv(true)){
								task->castAsServerConn()->info("套接口读操作错误");
								task->Terminate(nClientConn::TM_sock_error);
								check=true;
							}
						}
						eventData.events=0; 
					}
				}
			}
			if(check){
				continue;
			}

			if (currentTime.msec() >= (_write_time.msec() + (unsigned long)(baseServer->clientConnService->usleep_time/1000))){
				_write_time = currentTime;
				if (!_client_task_list.empty()){
					int retcode = epoll_wait(kdpfd, &epfds[0], _client_task_num, 0);
					if (retcode > 0){
						struct epoll_event *writeEventList = &epfds[0];
						for(int i = 0; i < retcode; ++i){
							struct epoll_event &eventData = writeEventList[i];
							nClientConn *task = (nClientConn *)eventData.data.ptr;
							if (eventData.events & (EPOLLERR | EPOLLPRI)){
								//套接口出现错误
								baseServer->debug("[%s]: 套接口异常错误", __PRETTY_FUNCTION__);
								task->Terminate(nClientConn::TM_sock_error);
							}
							else if (eventData.events & EPOLLOUT){
								//套接口准备好了写入操作
								if (!task->listeningSend()){
									baseServer->debug("[%s]: 套接口写操作错误", __PRETTY_FUNCTION__);
									task->Terminate(nClientConn::TM_sock_error);
								}
							}
							eventData.events=0; 
						}
					}
				}
				check=true;
			}

			//nThread::usleep(baseServer->clientConnService->usleep_time);
		}

		//把所有任务队列中的连接加入到回收队列中，回收这些连接
		for(it = _client_task_list.begin(), next = it, next++; it != _client_task_list.end(); it = next, next++){
			nClientConn *task = *it;
			remove(it);
			// state_okay -> state_recycle
			task->getNextState();
		}
	}

	nClientConnService::nClientConnService(const DWORD us)
	{
		setUsleepTime(us);
	} 

	nClientConnService::~nClientConnService()
	{
	}

	bool nClientConnService::addToConnect(nClientConn *task)
	{
		nCheckClientConnectThread *pThread = checkconnectThread.getOne();
		if (pThread == NULL){
			assert_fail("不能得到一个空闲线程");
			return false;
		}

		pThread->add(task);
		return true;
	}

	bool nClientConnService::initService()
	{
		assert_debug(Tidy::global["maxClientConnectThreadNum"].empty() == false, "");
		assert_debug(Tidy::global["maxClientNumInOneConnectThread"].empty() == false, "");
		if (!checkconnectThread.init(Tidy::global["maxClientConnectThreadNum"].getDword(), Tidy::global["maxClientNumInOneConnectThread"].getDword(), "checkconnectThread",this)){
			return false;
		}
		assert_debug(Tidy::global["maxClientWaitThreadNum"].empty() == false, "");
		assert_debug(Tidy::global["maxClientNumInOneWaitThread"].empty() == false, "");
		if (!checkwaitThread.init(Tidy::global["maxClientWaitThreadNum"].getDword(), Tidy::global["maxClientNumInOneWaitThread"].getDword(), "checkwaitThread",this)){
			return false;
		}
		assert_debug(Tidy::global["maxClientRunThreadNum"].empty() == false, "");
		assert_debug(Tidy::global["maxClientNumInOneRunThread"].empty() == false, "");
		if (!_all_task_thread_manager.init(Tidy::global["maxClientRunThreadNum"].getDword(), Tidy::global["maxClientNumInOneRunThread"].getDword(), "_all_task_thread_manager",this)){
			return false;
		}

		return true;
	}

	void nClientConnService::finalService()
	{
		checkwaitThread.final();
		_all_task_thread_manager.final();
		checkconnectThread.final();
	}

	DWORD nClientConnService::getTotalSize()
	{
		return _all_task_thread_manager.getTaskNum();
	}

	void nClientConnService::addToCheckwait(nClientConn *task)
	{
		nCheckClientWaitThread *pThread = checkwaitThread.getOne();
		if (pThread){
			pThread->add(task);
			task->getNextState();
		}
	}

	bool nClientConnService::addToOkay(nClientConn *task)
	{
		nClientRunThread *pThread = _all_task_thread_manager.getOne(); 
		if (pThread == NULL){
			baseServer->fatal("%s: 不能得到一个空闲线程", __PRETTY_FUNCTION__);
			assert_fail("无法得到线程");
			return false;
		}

		pThread->add(task);
		task->getNextState();
		return true;
	}
};
