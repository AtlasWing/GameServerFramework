#include "base/nServerConnManager.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	nServerConnManager::nServerConnManager()
	{
	}

	nServerConnManager::~nServerConnManager()
	{
	}

	namespace {
		struct SendCmdToAllExec {
			const void *_cmd;
			const int _cmd_len;
			SendCmdToAllExec(const void *cmd, const int cmdLen):_cmd(cmd),_cmd_len(cmdLen) {
			}
			bool exec(nServerConn *task) {
				task->sendCmd(_cmd, _cmd_len);
				return	true;
			}
		};
	};

	bool nServerConnManager::sendCmdToAll(const void *cmd, const int cmdLen)
	{
		SendCmdToAllExec exec(cmd, cmdLen);
		return execEveryConn(exec);
	}

	namespace {
		struct SendCmdToAllExceptOneExec {
			DWORD _except_id;
			const void *_cmd;
			const int _cmd_len;
			SendCmdToAllExceptOneExec(const DWORD exceptID, const void *cmd, const int cmdLen): _except_id(exceptID), _cmd(cmd),_cmd_len(cmdLen) {
			}
			bool exec(nServerConn *task) {
				if(task->getID() != _except_id) {
					task->sendCmd(_cmd, _cmd_len);
				}
				return	true;
			}
		};
	};
	bool nServerConnManager::sendCmdToAllExceptOne(const DWORD serverID, const void *cmd, const int cmdLen)
	{
		SendCmdToAllExceptOneExec exec(serverID, cmd, cmdLen);
		return execEveryConn(exec);
	}

	namespace {
		struct SendCmdToConnByTypeExceptOneExec {
			const void *_cmd;
			const int _cmd_len;
			const DWORD _conn_type;
			const DWORD _except;
			SendCmdToConnByTypeExceptOneExec(const void *cmd, const int cmdLen, const DWORD connType, const DWORD except) :_cmd(cmd),_cmd_len(cmdLen),_conn_type(connType),_except(except) {
			}
			bool exec(nServerConn *conn) {
				if(conn->getServerType() == _conn_type && conn->getID() != _except) {
					conn->sendCmd(_cmd, _cmd_len);
				}
				return	true;
			}
		};
	};

	bool nServerConnManager::sendCmdToConnByTypeExceptOne(const DWORD connType, const DWORD except, const void *cmd, const int cmdLen)
	{
		SendCmdToConnByTypeExceptOneExec exec(cmd, cmdLen,connType,except);
		return execEveryConn(exec);
	}

	namespace {
		struct SendCmdToTypeExec {
			const void *_cmd;
			const int _cmd_len;
			const DWORD _conn_type;
			SendCmdToTypeExec(const void *cmd, const int cmdLen, const DWORD connType)
				:_cmd(cmd),_cmd_len(cmdLen),_conn_type(connType) {
				}
			bool exec(nServerConn *conn) {
				if(conn->getServerType() == _conn_type) {
					conn->sendCmd(_cmd, _cmd_len);
				}
				return	true;
			}
		};
	};

	bool nServerConnManager::sendCmdToConnByType(const DWORD connType, const void *cmd, const int cmdLen)
	{
		SendCmdToTypeExec exec(cmd, cmdLen, connType);
		return execEveryConn(exec);
	}

	namespace {
		struct GetConnSizeExec {
			const DWORD _task_type;
			BYTE _size;
			GetConnSizeExec(const DWORD connType) : _task_type(connType), _size(0) {
			}
			bool exec(nServerConn *conn) {
				if(conn->getServerType() == _task_type) {
					_size ++;
				}
				return true;
			}
		};
	};

	DWORD nServerConnManager::getConnSizeByType(const DWORD connType)
	{
		GetConnSizeExec exec(connType);
		execEveryConn(exec);
		return exec._size;
	}

	bool nServerConnManager::addConnID(const DWORD id)
	{
		nScopeWRLock idLock(_conn_id_container_lock);
		if(_conn_id_contaner.find(id) != _conn_id_contaner.end()) {
			return false;
		}
		_conn_id_contaner.insert(id);
		return true;
	}

	bool nServerConnManager::removeConnID(const DWORD id)
	{
		nScopeWRLock idLock(_conn_id_container_lock);
		_conn_id_contaner.erase(id);
		return true;
	}

	bool nServerConnManager::checkConnIDAdded(const DWORD id)
	{
		nScopeRDLock lock(_conn_id_container_lock);
		return (_conn_id_contaner.find(id) != _conn_id_contaner.end());
	}

	nQueueServerConnManager::nQueueServerConnManager() 
	{
	}

	nQueueServerConnManager::~nQueueServerConnManager() 
	{
	}

	namespace {
		class DoTimeTickExec {
			public:
				DoTimeTickExec() {
				}
			public:
				bool exec(nServerConn *conn) {
					conn->doTimeTick();
					if(((nServerConnQueue*)conn)->checkRecycle() == true) {
						removeList.push_back(conn);
					}
					return true;
				}

			public:
				Tidy::vector<nServerConn*> removeList;
		};
	};

	bool nQueueServerConnManager::doTimeTick()
	{
        // 先删除连接,再处理新连接
		DoTimeTickExec exec;
		execEveryConn(exec);
		if(exec.removeList.empty() == false) {
			for(Tidy::vector<nServerConn*>::iterator iter=exec.removeList.begin(); iter != exec.removeList.end(); ++ iter) {
				baseServer->serverConnectClose(*iter);
				remove(*iter);
				(*iter)->setAddedToManager(false);
				((nServerConnQueue*)(*iter))->setRecycleState(nServerConnQueue::eConnRecycleState_WaitDelete);
			}
		}

		if(_wait_add_conn_container.empty() == false) {
			nScopeWRLock lock(_add_conn_lock);

			//double check
			if(_wait_add_conn_container.empty() == false) {
				for(Tidy::vector<nServerConn*>::iterator iter=_wait_add_conn_container.begin(); iter != _wait_add_conn_container.end(); ++ iter) {
					if(add(*iter) == false) {
						assert_fail("ID重复");
						continue;
					}
					else {
						baseServer->serverConnectSuccess(*iter);
					}
				}
				_wait_add_conn_container.clear();
			}
		}

		return true;
	}

	bool nQueueServerConnManager::addConn(nServerConn* conn)
	{
		nScopeWRLock addLock(_add_conn_lock);
		_wait_add_conn_container.push_back(conn);
		conn->setAddedToManager(true);
		static_cast<nServerConnQueue*>(conn)->setRecycleState(nServerConnQueue::eConnRecycleState_None);
		return true;
	}

	void nQueueServerConnManager::removeConn(nServerConn* conn)
	{
		if(baseServer->isStartupComplete() == false) {
			nScopeWRLock removeLock(_add_conn_lock);
			for(Tidy::vector<nServerConn*>::iterator iter = _wait_add_conn_container.begin(); iter != _wait_add_conn_container.end(); ++ iter) {
				if((*iter) == conn) {
					_wait_add_conn_container.erase(iter);
					(*iter)->setAddedToManager(false);
					static_cast<nServerConnQueue*>(*iter)->setRecycleState(nServerConnQueue::eConnRecycleState_WaitDelete);
					break;
				}
			}
		}
		else {
			// 从manager里删除肯定是要加入到recycle,会调用nTaskConnQueue::recycleConn,需要调用removeTaskConnID.原因见nTaskConnQueue::recycleConn的注释
			//static_cast<nServerConnQueue*>(conn)->setRecycleState(nServerConnQueue::eConnRecycleState_WaitRemoveFromManager);
		}
	}

	DWORD nQueueServerConnManager::getSize()
	{
		return size();
	}

	nServerConn *nQueueServerConnManager::getConnByID(const DWORD id)
	{
		return static_cast<nServerConn*>(getByID(id));
	}

	bool nQueueServerConnManager::sendCmdToConnByID(const WORD &serverID, const void *cmd, const int cmdLen)
	{
		nServerConn * conn = getByID(serverID);
		if (conn) {
			return conn->sendCmd(cmd, cmdLen);
		}
		else {
			//assert_fail("没有找到连接");
		}
		return	false;
	}

	nNotQueueServerConnManager::nNotQueueServerConnManager() 
	{
	}

	nNotQueueServerConnManager::~nNotQueueServerConnManager() 
	{
	}

	bool nNotQueueServerConnManager::doTimeTick()
	{
		return true;
	}

	bool nNotQueueServerConnManager::addConn(nServerConn* conn)
	{
		_manager_lock.wrlock();
		if(add(conn) == false) {
			_manager_lock.unlock();
			return false;
		}
		_manager_lock.unlock();
		conn->setAddedToManager(true);
		baseServer->serverConnectSuccess(conn);
		return true;
	}

	void nNotQueueServerConnManager::removeConn(nServerConn* conn)
	{
		conn->setAddedToManager(false);
		_manager_lock.wrlock();
		remove(conn);
		_manager_lock.unlock();
		baseServer->serverConnectClose(conn);
	}

	DWORD nNotQueueServerConnManager::getSize()
	{
		nScopeRDLock lock(_manager_lock);
		return size();
	}

	nServerConn *nNotQueueServerConnManager::getConnByID(const DWORD id)
	{
		nScopeRDLock lock(_manager_lock);
		return static_cast<nServerConn*>(getByID(id));
	}

	bool nNotQueueServerConnManager::sendCmdToConnByID(const WORD &serverID, const void *cmd, const int cmdLen)
	{
		nScopeRDLock lock(_manager_lock);
		nServerConn * conn = getByID(serverID);
		if (conn) {
			return conn->sendCmd(cmd, cmdLen);
		}
		else {
			assert_fail("没有找到连接");
		}
		return	false;
	}


	nCommonServerConnManager::nCommonServerConnManager() 
	{
	}

	nCommonServerConnManager::~nCommonServerConnManager() 
	{
	}

	bool nCommonServerConnManager::doTimeTick()
	{
		return true;
	}

	bool nCommonServerConnManager::addConn(nServerConn* conn)
	{
		return add(conn);
	}

	void nCommonServerConnManager::removeConn(nServerConn* conn)
	{
		remove(conn);
	}

	DWORD nCommonServerConnManager::getSize()
	{
		return size();
	}

	nServerConn *nCommonServerConnManager::getConnByID(const DWORD id)
	{
		return getByID(id);
	}

	bool nCommonServerConnManager::sendCmdToConnByID(const WORD &serverID, const void *cmd, const int cmdLen)
	{
		nServerConn * conn = getByID(serverID);
		if (conn == NULL) {
			assert_fail("没有找到连接[%u]", serverID);
			return false;
		}
		return conn->sendCmd(cmd, cmdLen);
	}
};
