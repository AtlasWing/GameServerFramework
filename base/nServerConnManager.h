#ifndef _N_TASK_CONN_MANAGER_H_
#define _N_TASK_CONN_MANAGER_H_

#include "base/nManager.h"
#include "base/nObj.h"
#include "base/nServerConn.h"
#include "base/nSTL.h"
#include "base/nRWLock.h"

namespace Tidy
{
	class nServerConnManager : public nManager<true, nIndexByID< CommonGetIDPolicy<nServerConn> > >
	{
		public:
			nServerConnManager();
			virtual ~nServerConnManager();

		public:
			virtual bool doTimeTick() = 0;

		public:
			bool sendCmdToAll(const void *cmd, const int cmdLen);
			bool sendCmdToAllExceptOne(const DWORD serverID, const void *cmd, const int cmdLen);
			bool sendCmdToConnByTypeExceptOne(const DWORD connType, const DWORD exceptServerID, const void *cmd, const int cmdLen);
			bool sendCmdToConnByType(const DWORD connType, const void *cmd, const int cmdLen);
			DWORD getConnSizeByType(const DWORD connType);

		public:
			bool addConnID(const DWORD id);
			bool removeConnID(const DWORD id);
			bool checkConnIDAdded(const DWORD id);

		public:
			virtual bool addConn(nServerConn* conn) = 0;
			virtual void removeConn(nServerConn* conn) = 0;
			virtual DWORD getSize() = 0;
			virtual nServerConn *getConnByID(const DWORD id) = 0;
			virtual bool sendCmdToConnByID(const WORD &ServerID, const void *cmd, const int cmdLen) = 0;

		public:
			template<typename ExecT>
				bool execConnByType(ExecT &exec, const DWORD serverType);

		public:
			template<typename ExecT>
				bool execEveryConn(ExecT &exec)
				{
					return execAll(exec);
				}

		private:
			Tidy::hash_set<DWORD> _conn_id_contaner;
			nRWLock _conn_id_container_lock;
	};

	namespace
	{
		template<typename ExecT>
			struct GetConnExec
			{
				ExecT& _exec;
				DWORD _server_type;
				GetConnExec(ExecT& exec, const DWORD serverType) : _exec(exec),  _server_type(serverType)
				{
				}
				bool exec(nServerConn* conn)
				{
					if(conn->getServerType() == _server_type)
					{
						return _exec.exec(conn);
					}
					return true;
				}
			};
	};

	template<typename ExecT>
		bool nServerConnManager::execConnByType(ExecT &exec, const DWORD serverType)
		{
			GetConnExec<ExecT> getExec(exec, serverType);
			return execAll(getExec);
		}

	class nQueueServerConnManager : public nServerConnManager
	{
		public:
			nQueueServerConnManager();
			virtual ~nQueueServerConnManager();

		public:
			virtual bool doTimeTick();

		private:
			virtual bool addConn(nServerConn* conn);
			virtual void removeConn(nServerConn* conn);
			virtual DWORD getSize();
			virtual nServerConn *getConnByID(const DWORD id);
			virtual bool sendCmdToConnByID(const WORD &ServerID, const void *cmd, const int cmdLen);

		private:
			Tidy::vector<nServerConn*> _wait_add_conn_container;
			nRWLock _add_conn_lock;
	};

	class nNotQueueServerConnManager : public nServerConnManager
	{
		public:
			nNotQueueServerConnManager();
			virtual ~nNotQueueServerConnManager();

		private:
			virtual bool doTimeTick();

		private:
			virtual bool addConn(nServerConn* conn);
			virtual void removeConn(nServerConn* conn);
			virtual DWORD getSize();
			virtual nServerConn *getConnByID(const DWORD id);
			virtual bool sendCmdToConnByID(const WORD &ServerID, const void *cmd, const int cmdLen);

		private:
			nRWLock _manager_lock;
	};

	class nCommonServerConnManager : public nServerConnManager
	{
		public:
			nCommonServerConnManager();
			virtual ~nCommonServerConnManager();

		private:
			virtual bool doTimeTick();

		private:
			virtual bool addConn(nServerConn* conn);
			virtual void removeConn(nServerConn* conn);
			virtual DWORD getSize();
			virtual nServerConn *getConnByID(const DWORD id);
			virtual bool sendCmdToConnByID(const WORD &ServerID, const void *cmd, const int cmdLen);
	};

};
#endif
