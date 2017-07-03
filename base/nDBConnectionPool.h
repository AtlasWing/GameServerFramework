#ifndef _N_DB_CONNECTION_POOL_H_
#define _N_DB_CONNECTION_POOL_H_

#include "base/Tidy.h"
#include "base/nSTL.h"
#include "base/nMisc.h"
#include "base/nRWLock.h"
#include "base/nDBConnection.h"
//#include "mysql++/cpool.h"

namespace Tidy
{
	class nDBUrl;
	class nDBConnection;

	class nAutoReleaseConnection{
		public:
			nAutoReleaseConnection();
			~nAutoReleaseConnection();

		public:
			void setConnection(nDBConnection *connection);
			Tidy::nDBQuery getQuery();
			nDBConnection& getConnection() const;

		private:
			nDBConnection *_connection;
	};
	class nDBConnectionPool///, mysqlpp::ConnectionPool
	{
		public:
			DISALLOW_COPY_AND_ASSIGN(nDBConnectionPool);
			nDBConnectionPool(const bool needLock);
			~nDBConnectionPool();

		public:
			bool createConnection(const nDBUrl &url, const nDBUrl *slaveUrl, nDBConnection *conn);
			nDBConnection* getConnection(const DWORD dbID) const;
			bool empty() const;
			WORD size() const;
			bool getAutoReleaseConnection(const DWORD dbID, nAutoReleaseConnection *autoReleaseConnection);

		private:
			typedef Tidy::vector<nDBConnection*> ConnectionList;
			Tidy::map<DWORD, ConnectionList> _conn_map;
			Tidy::list<nDBConnection *> _all_conn_list;
			bool _need_lock;
			Tidy::nRWLock _manager_lock;
	};
};
#endif
