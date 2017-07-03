#include "base/nDBConnectionPool.h"
#include "base/nDBConnection.h"

namespace Tidy
{
	nAutoReleaseConnection::nAutoReleaseConnection() : _connection(NULL)
	{
	}

	nAutoReleaseConnection::~nAutoReleaseConnection()
	{
		_connection->setUsed(false);
	}

	void nAutoReleaseConnection::setConnection(nDBConnection *connection)
	{
		_connection = connection;
		_connection->setUsed(true);
	}

	nDBQuery nAutoReleaseConnection::getQuery()
	{
		return _connection->getQuery();
	}

	nDBConnection& nAutoReleaseConnection::getConnection() const
	{
		assert_debug(_connection != NULL, "");
		return *_connection;
	}

	nDBConnectionPool::nDBConnectionPool(const bool needLock):
		_need_lock(needLock)
	{
	}

	nDBConnectionPool::~nDBConnectionPool()
	{
		_conn_map.clear();
		for(Tidy::list<nDBConnection*>::iterator iter = _all_conn_list.begin(); iter != _all_conn_list.end(); ++ iter){
			(*iter)->disconnect();
			SAFE_DELETE(*iter);
		}
		_all_conn_list.clear();
	}

	bool nDBConnectionPool::createConnection(const nDBUrl &url, const nDBUrl *slaveUrl, nDBConnection *conn)
	{
		if(_conn_map.find(url.getID()) != _conn_map.end()){
			assert_fail("连接重复");
			return false;
		}
		if(conn == NULL){
			conn = new nDBConnection(url, slaveUrl, this);
			if(conn->connectMaster() == false){
				SAFE_DELETE(conn);
				return false;
			}
			_all_conn_list.push_back(conn);
		}
		_conn_map[url.getID()].push_back(conn);

		return true;
	}

	bool nDBConnectionPool::getAutoReleaseConnection(const DWORD dbID, nAutoReleaseConnection *autoReleaseConnection) 
	{
		assert_debug(_need_lock == true, "");
		Tidy::map<DWORD, ConnectionList>::iterator iter = _conn_map.find(dbID);
		if(iter == _conn_map.end()) {
			assert_fail("没有连接");
			return false;
		}
		ConnectionList &connectionList = iter->second;
		if(connectionList.empty() == true){
			assert_fail("没有连接");
			return false;
		}
		Tidy::nScopeWRLock lock(_manager_lock);
		for(DWORD i= 0; i < connectionList.size(); ++ i){
			if(connectionList[i]->isUsed() == false){
				autoReleaseConnection->setConnection(connectionList[i]);
				return true;
			}
		}

		nDBConnection *orgConnection = connectionList[0];
		nDBConnection *conn = new nDBConnection(*orgConnection->getUrl(), orgConnection->getSlaveUrl(), this);
		if(conn->connectMaster() == false){
			assert_fail("连接失败");
			SAFE_DELETE(conn);
			return false;
		}
		autoReleaseConnection->setConnection(conn);
		connectionList.push_back(conn);
		_all_conn_list.push_back(conn);
		return true;
	}

	nDBConnection* nDBConnectionPool::getConnection(const DWORD dbID) const
	{
		assert_debug(_need_lock == false, "");
		Tidy::map<DWORD, ConnectionList>::const_iterator iter = _conn_map.find(dbID);
		if(iter == _conn_map.end()) {
			assert_fail("没有连接");
			return NULL;
		}
		return iter->second[0];
#if 0
		if( _conn->ping() == true)
			return _conn;
		else
		{
			if(_conn->reConnect() == false)
				return NULL;
			else
				return _conn;
		}
#endif
	}
};

