#include "base/nDBConnection.h"

#include "base/Tidy.h"

#include "base/nDBUrlManager.h"
#include "base/nDBConnectionPool.h"
#include "base/nBaseServer.h"
#include "base/nDebugger.h"

namespace Tidy
{
	using namespace mysqlpp;

	nDBQueryResult::nDBQueryResult(const StoreQueryResult & res):
		StoreQueryResult(res)
	{}

	const bool nDBQueryResult::empty() const
	{
		return num_rows() == 0;
	}

	const DWORD nDBQueryResult::size() const
	{
		return StoreQueryResult::size();
	}


	nDBQuery::nDBQuery(const nDBQuery &query):
		Query(query),
		_connection(query._connection),
		_need_unlock_table(false)
	{
	}

	nDBQuery::nDBQuery(const Query &query, nDBConnection *connection):
		Query(query),
		_connection(connection),
		_need_unlock_table(false)
	{
	}

	nDBQuery::nDBQuery( nDBConnection *conn):
		Query(conn,false),
		_connection(conn),
		_need_unlock_table(false)
	{
	}

	nDBQuery::~nDBQuery()
	{
		if(_need_unlock_table == true){
			exeUnlockTable();
		}
	}

	bool nDBQuery::exeQuery(const char* str,const int len)
	{
		SQL_EXECUTE_CHECKER(str)
			if(_connection->checkConnected() == false){
				baseServer->debug("数据库连接关闭, 无法执行[%s]", str);
				return false;
			}
#ifdef _DEBUG
		baseServer->debug("[%s]", str);
#endif
		if(Query::execute( str, len) == false) {
			baseServer->error("[%s, %s]", str, Query::error());
			baseServer->error("[%s]",Query::info().c_str());
			eCheckDBClosedResult checkResult = checkDBClosed();
			if(checkResult == eCheckDBClosedResult_ReconnectSucc){
				return exeQuery(str, len);
			}
			return false;
		}
		return true;
	}

	bool nDBQuery::exeQuery( const std::string& str )
	{
		return exeQuery(str.c_str(), str.length());
	}

	nDBQueryResult nDBQuery::exeSelect()
	{
#ifdef _DEBUG
		baseServer->debug("[%s]",this->str().c_str());
#endif
		nDBQueryResult res = Query::store();
		if( errnum() != 0 ) {
			baseServer->error("[%s] [%s]",Query::error(), this->str().c_str());
			baseServer->error("[%s]",Query::info().c_str());
			eCheckDBClosedResult checkResult = checkDBClosed();
			if(checkResult == eCheckDBClosedResult_ReconnectSucc){
				return this->exeSelect();
			}
		}
		return res;
	}

	DWORD nDBQuery::exeInsert()
	{
		if(_connection->checkConnected() == false){
			baseServer->debug("数据库连接关闭, 无法执行[%s]", this->str().c_str());
			return false;
		}
		if(exeQuery(this->str()) == false){
			return (DWORD)-1;
		}
		return insert_id();
	}

	DWORD nDBQuery::exeDelete()
	{
		if(_connection->checkConnected() == false){
			baseServer->debug("数据库连接关闭, 无法执行[%s]", this->str().c_str());
			return false;
		}
		if(exeQuery(this->str()) == false)
		{
			return (DWORD)-1;
		}
		return affected_rows();
	}

	DWORD nDBQuery::exeUpdate()
	{
		if(_connection->checkConnected() == false){
			baseServer->debug("数据库连接关闭, 无法执行[%s]", this->str().c_str());
			return false;
		}
		if(exeQuery(this->str()) == false)
		{
			return (DWORD)-1;
		}
		return affected_rows();
	}

	const char* nDBQuery::getSql()
	{
		return this->str().c_str();
	}

	eCheckDBClosedResult nDBQuery::checkDBClosed() 
	{
		if(_connection->checkDisconnected() == true){
			return eCheckDBClosedResult_Closed;
		}
		int errorCode = errnum();
		baseServer->debug("错误值[%d] sql=[%s]", errorCode, getSql());
		//连接断开
		if(errorCode == 2006 || errorCode == 2055 || errorCode == 2013) {
			if(_connection->ping() == true){
				baseServer->fatal("重新连接 ping 成功");
				return eCheckDBClosedResult_ReconnectSucc;
			}
			if(_connection->connectSlave() == true){
				baseServer->fatal("连接从数据库成功");
				return eCheckDBClosedResult_ReconnectSucc;
			}
			baseServer->fatal("连接从数据库失败");
			_connection->setConnState(eDBConnState_Disconnected);
			baseServer->dealDBClosed(_connection);
			return eCheckDBClosedResult_Closed;
		}
		//字段值重复，入库失败 
		else if(errorCode == 1062) {
			baseServer->debug("字段值重复，入库失败");
		}
		else {
#ifdef _DEBUG
			assert_fail("数据库错误[%u, %s]", errorCode, error());
#endif
		}
		return eCheckDBClosedResult_OtherErr;
	}

	nDBConnection::nDBConnection(const nDBUrl &url, const nDBUrl *slaveUrl, nDBConnectionPool *pool):
		Connection(false),
		_url(url),
		_slave_url(NULL),
		_pool(pool),
		_conn_state(eDBConnState_None),
		_used(false)
	{
		if(slaveUrl != NULL){
			_slave_url = new nDBUrl(*slaveUrl);
		}
	}

	nDBConnection::~nDBConnection()
	{
		SAFE_DELETE(_slave_url);
	}

	bool nDBConnection::connect(const nDBUrl &url)
	{
		this->set_option( new SetCharsetNameOption("utf8"));
		/***
		  读超时时间,底层会尝试一次,重试两次,所以实际超时时间是*3,暂定10秒,也就是实际30秒.可以使用以下代码测试
		  query << "select sleep(15)";
		  query.exeSelect();
		 ***/
		this->set_option( new ReadTimeoutOption(10));
		this->set_option( new WriteTimeoutOption(15));
		this->set_option( new ConnectTimeoutOption(10));
		this->set_option( new ReconnectOption(true));

		if(Connection::connect( url.getDatabase().c_str(), url.getHostIP().c_str(), url.getUser().c_str(), url.getPasswd().c_str(), url.getPort() )== false) {
			baseServer->error("[%s]",error());
			return false;
		}
		setConnState(eDBConnState_Connected);
		nDBQuery query = getQuery();
		if(query.exeQuery("set wait_timeout = 1073741824") == false) {
			return false;
		}

		baseServer->info("连接数据库 [%s:%u/%s] 成功...", url.getHostIP().c_str(), url.getPort(), url.getDatabase().c_str());
		return true;
	}

	bool nDBConnection::reConnect()
	{
		baseServer->warn("数据库超时重新连接 [%s:%u/%s]",_url.getHostIP().c_str(),_url.getPort(),_url.getDatabase().c_str());
		disconnect();
		return connectMaster();
	}

	bool nDBConnection::connectMaster()
	{
		return connect(_url);
	}

	bool nDBConnection::connectSlave()
	{
		if(_slave_url == NULL){
			return false;
		}
		return connect(*_slave_url);
	}

	nDBQuery nDBConnection::getQuery()
	{
		return nDBQuery(Connection::query(), this);
	}

	const nDBUrl* nDBConnection::getUrl() const
	{
		return &_url;
	}

	const nDBUrl* nDBConnection::getSlaveUrl() const
	{
		return _slave_url;
	}

	void nDBConnection::setConnState(const eDBConnState state)
	{
		_conn_state = state;
	}

	bool nDBConnection::checkConnected() const
	{
		return _conn_state == eDBConnState_Connected;
	}

	bool nDBConnection::checkDisconnected() const
	{
		return _conn_state == eDBConnState_Disconnected;
	}

	DWORD nDBConnection::getConnectionID() const
	{
		return _url.getID();
	}

	bool nDBConnection::setUsed(bool used)
	{
		assert_debug(_used != used, "");
		_used = used;
		return true;
	}

	bool nDBConnection::isUsed() const
	{
		return _used;
	}

	bool nDBQuery::exeLockTable(const char *tableName, const eDBTableLockLevel lockLevel)
	{
		char sqlBuffer[1024];
		DWORD len = 0;
		if(lockLevel == eDBTableLockLevel_Read){
			len = snprintf(sqlBuffer, sizeof(sqlBuffer), "LOCK TABLES %s READ", tableName) + 1;
		}
		else if(lockLevel == eDBTableLockLevel_Write){
			len = snprintf(sqlBuffer, sizeof(sqlBuffer), "LOCK TABLES %s WRITE", tableName) + 1;
		}
		else{
			assert_fail("无法处理的lockLevel[%u]", lockLevel);
		}
		if(exeQuery(sqlBuffer, len) == false){
			assert_fail("锁表失败[%u]", tableName);
			return false;
		}
		_need_unlock_table = true;
		return true;
	}

	bool nDBQuery::exeUnlockTable()
	{
		char sqlBuffer[] = "UNLOCK TABLES";
		_need_unlock_table = false;
		return exeQuery(sqlBuffer, sizeof(sqlBuffer));
	}

};
