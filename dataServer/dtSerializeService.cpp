#include "protocol/serializeCommand.h"
#include "protocol/Proto/UserData.pb.h"

#include "base/nStringTool.h"
#include "base/nDBConnection.h"
#include "base/nDBConnectionPool.h"
#include "base/nAsyncSqlThread.h"

#include "game/nSsqls.h"

#include "dataServer/dtSerializeService.h"
#include "dataServer/dtServer.h"
#include "dataServer/dtUserService.h"
#include "dataServer/dtMessageService.h"
#include "dataServer/dtUser.h"

bool AsyncSqlCallback::getResult() const
{
    return _result;
}

bool AsyncSqlCallback::exec(bool result)
{
    _result = result;
    return server->serializeService->addAsyncSqlCallback(this);
}

dtSerializeService::dtSerializeService() :
    Game::nMiscDataSerialize(server->getDBConnPool()),
    _sql_thread(NULL),
    _hundred_msec_timer(HUNDRED_MSEC)
{
}

dtSerializeService::~dtSerializeService()
{
    assert_debug(_sql_thread == NULL, "");
}

bool dtSerializeService::initService()
{
    if(Game::nMiscDataSerialize::init() == false) {
        return false;
    }

    _sql_thread = new Tidy::nAsyncSqlThread(Tidy::global["SQL_MEMORY_MAPPED_FILE"].getString(), (Tidy::global["SQL_MEMORY_MAPPED_BUFFER_SIZE"].getDword() * 1024 * 1024));
    if(server->createDBConnection(&_sql_thread->getDBConnPool(), "ZONE_DATA_MYSQL_SERVER", eDBConnectionID_ZoneMain) == false){
        assert_fail("创建数据库连接失败");
        return false;
    }
    if(server->createDBConnection(&_sql_thread->getDBConnPool(), "FRIEND_DATA_MYSQL_SERVER", eDBConnectionID_Friend) == false){
        assert_fail("创建好友数据库连接失败");
        return false;
    }

    if(_sql_thread->init() == false){
        SAFE_DELETE(_sql_thread);
        return false;
    }
    if(_sql_thread->start() == false){
        _sql_thread->final();
        SAFE_DELETE(_sql_thread);
        return false;
    }

    return true;
}

bool dtSerializeService::finalService()
{
    _sql_thread->stop();
    _sql_thread->join();
    _sql_thread->final();
    SAFE_DELETE(_sql_thread);
    return true;
}

const DWORD dtSerializeService::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const
{
    return Tidy::nStringTool::copyStr(headerBuffer, "Seria", bufferSize);
}

Tidy::nLogger* dtSerializeService::getLogger() const
{
    return server->getLogger();
}

bool dtSerializeService::saveUserData(const DWORD charID, const char *data, const DWORD num)
{
    if(num == 0) {
        assert_fail("存档数据为0");
        return false;
    }
    protocol::user::UserSaveData saveData;
    if(saveData.ParseFromArray(data, num) == false){
        assert_fail("无法解析");
        return false;
    }
    dtUser *user = server->userService->getUserByID(charID);
    if(user != NULL) {
        user->updateUserData(saveData);
    }
    else {
        assert_fail("非在线玩家的存档");
    }

    return true;
}

bool dtSerializeService::saveUserRelationData(const DWORD charID, const char *data, const DWORD num)
{
    if(num == 0) {
        assert_fail("存档数据为0");
        return false;
    }
    protocol::user::UserRelationSaveData saveData;
    if(saveData.ParseFromArray(data, num) == false){
        assert_fail("无法解析");
        return false;
    }
    SqlRelationBase relationBase;
    Tidy::vector<bool> relationSaveDataFieldList(SqlRelationBase_NULL, false);
    relationBase.charID = charID;
    if(saveData.has_clubdata() == true){
        relationBase.clubDataBinary = saveData.clubdata();
        relationSaveDataFieldList[SqlRelationBase_clubDataBinary] = true;
    }
    Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
    if(conn == NULL){
        assert_fail("不能从数据库连接池获取连接句柄");
        return false;
    }
    Tidy::nDBQuery query = conn->getQuery();
    std::string sql = query.getUpdateSql(relationBase, relationSaveDataFieldList, relationBase.equal_list(SqlRelationBase_charID), 1);
    return exeAsyncSql(eDBConnectionID_ZoneMain, sql, NULL);
}

bool dtSerializeService::exeAsyncSql(const DWORD dbConnID, const std::string &sql, AsyncSqlCallback *callback)
{
    _sql_thread->addSql(dbConnID, sql, callback);
    return true;
}

bool dtSerializeService::addAsyncSqlCallback(AsyncSqlCallback *callback)
{
    Tidy::nMutex_scope_lock lock(_async_callback_result_vec_mutex);
    _async_callback_result_vec.push_back(callback);
    return true;
}

bool dtSerializeService::doTick()
{
    if(_hundred_msec_timer(server->time()) == true){
        if(_async_callback_result_vec.empty() == false){
            Tidy::vector<AsyncSqlCallback*> callbackVec;
            _async_callback_result_vec_mutex.lock();
            callbackVec.swap(_async_callback_result_vec);
            _async_callback_result_vec_mutex.unlock();
            for(DWORD i = 0; i < callbackVec.size(); ++ i){
                callbackVec[i]->dealExecResult(callbackVec[i]->getResult());
                // 要求必须在堆上分配的
                SAFE_DELETE(callbackVec[i]);
            }
        }
    }
    return true;
}

std::string dtSerializeService::getLastLoginIP(const DWORD charID, const char *name)
{
	SqlCharBase sqlCharBase;

	Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
	if(conn == NULL){
		error("不能从数据库连接池获取连接句柄 %s",__PRETTY_FUNCTION__);
		return "";
	}

	char whereCondition[128];
	snprintf(whereCondition, sizeof(whereCondition), " %s=%u or %s='%s' ", SqlCharBase::names[SqlCharBase_charID], charID, SqlCharBase::names[SqlCharBase_name], name);
	Tidy::nDBQuery query = conn->getQuery();
	Tidy::nDBQueryResult result = query.exeSelect(sqlCharBase.table(), sqlCharBase.field_list(SqlCharBase_lastLoginIP), whereCondition, 1);
    if(result.empty() == true){
        return "";
    }
    return static_cast<const char *>(result[0][SqlCharBase::names[SqlCharBase_lastLoginIP]]);
}

std::string dtSerializeService::getAccountName(const char *name)
{
	SqlCharBase sqlCharBase;

	Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
	if(conn == NULL){
		error("不能从数据库连接池获取连接句柄 %s",__PRETTY_FUNCTION__);
		return "";
	}

	char whereCondition[128];
	snprintf(whereCondition, sizeof(whereCondition), "%s='%s' ", SqlCharBase::names[SqlCharBase_name], name);
	Tidy::nDBQuery query = conn->getQuery();
	Tidy::nDBQueryResult result = query.exeSelect(sqlCharBase.table(), sqlCharBase.field_list(SqlCharBase_accountName), whereCondition, 1);
    if(result.empty() == true){
        return "";
    }
    return static_cast<const char *>(result[0][SqlCharBase::names[SqlCharBase_accountName]]);
}

std::string dtSerializeService::getName(const char *accountName)
{
	SqlCharBase sqlCharBase;

	Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
	if(conn == NULL){
		error("不能从数据库连接池获取连接句柄 %s",__PRETTY_FUNCTION__);
		return "";
	}

	char whereCondition[128];
	snprintf(whereCondition, sizeof(whereCondition), " %s='%s' ", SqlCharBase::names[SqlCharBase_accountName], accountName);
	Tidy::nDBQuery query = conn->getQuery();
	Tidy::nDBQueryResult result = query.exeSelect(sqlCharBase.table(), sqlCharBase.field_list(SqlCharBase_name), whereCondition, 1);
    if(result.empty() == true){
        return "";
    }
    return static_cast<const char *>(result[0][SqlCharBase::names[SqlCharBase_name]]);
}

bool dtSerializeService::dealDeleteAllRobot()
{
    server->debug("目前还没有好友数据库");
    return true;
}

bool dtSerializeService::saveMiscData(const DWORD miscDataType, const DWORD dataKey, const char* data, const DWORD num)
{
    static Tidy::nDBQuery query(NULL);
	static SqlMiscDataSave saveData;
	saveData.dataType = miscDataType;
	saveData.dataKey = dataKey;
	saveData.saveTimeInSec = server->time().sec();
	saveData.allBinary.assign(data, num);
	static std::vector<bool> vb(SqlMiscDataSave_NULL, true);

    std::string sql = query.getReplaceSql(saveData, vb);
    return exeAsyncSql(eDBConnectionID_ZoneMain, sql, NULL);
}
