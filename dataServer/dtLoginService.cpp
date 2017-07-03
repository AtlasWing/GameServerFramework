#include "base/nServerConn.h"
#include "base/nDBConnection.h"
#include "base/nDBConnectionPool.h"

#include "game/nSsqls.h"

#include "protocol/loginCommon.h"

#include "dataServer/dtLoginService.h"
#include "dataServer/dtServer.h"
#include "dataServer/dtUserService.h"

dtLoginService::dtLoginService()
{
}

dtLoginService::~dtLoginService()
{
}

const DWORD dtLoginService::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const
{
    return copyStr(headerBuffer, "Login", bufferSize) - 1;
}

Tidy::nLogger* dtLoginService::getLogger() const
{
    return server->getLogger();
}

bool dtLoginService::initService()
{
    return true;
}

void dtLoginService::finalService()
{
}

bool dtLoginService::checkAccountEmpty(const char *accountName)
{
    Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_Admin);
    if(conn == NULL) {
        error("不能从数据库连接池获取连接句柄 %s",__PRETTY_FUNCTION__);
        return false;
    }
    SqlAccountAdmin account;
    account.accountName = accountName;
    Tidy::nDBQuery query = conn->getQuery(); 
    Tidy::nDBQueryResult result = query.exeSelect(account.table(), account.field_list(SqlAccountAdmin_accountName), account.equal_list(SqlAccountAdmin_accountName), 1);
    return result.empty();
}

std::string dtLoginService::getNewAccount() const
{
    Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_Admin);
    if(conn == NULL) {
        error("不能从数据库连接池获取连接句柄 %s",__PRETTY_FUNCTION__);
        return false;
    }
    SqlAccountAdmin accountTable;
    Tidy::nDBQuery query = conn->getQuery(); 
    Tidy::nDBQueryResult result = query.exeSelect(accountTable.table(), " count(*) as count ", " 1=1 ", 0);
    if(result.empty() == true){
        error("查询失败%s",__PRETTY_FUNCTION__);
        return false;
    }
    DWORD newCount = static_cast<DWORD>(result[0]["count"]) + 100000;
    std::string newAccountStr;
    std::stringstream ss;
    ss<<newCount;
    ss>>newAccountStr;
    return newAccountStr;
}

bool dtLoginService::canCreateRole(const char *accountName)
{
    Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
    if(conn == NULL) {
        error("不能从数据库连接池获取连接句柄 %s",__PRETTY_FUNCTION__);
        return false;
    }
    SqlCharBase charBase;
    charBase.accountName = accountName;
    Tidy::nDBQuery query = conn->getQuery(); 
    Tidy::nDBQueryResult result = query.exeSelect(charBase.table(), charBase.field_list(SqlCharBase_accountName), charBase.equal_list(SqlCharBase_accountName), 1);
    return result.empty();
}

DWORD dtLoginService::createNewCharID() const
{
    Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
    if(conn == NULL) {
        error("不能从数据库连接池获取连接句柄 %s",__PRETTY_FUNCTION__);
        return false;
    }
    SqlCharBase charBase;
    Tidy::nDBQuery query = conn->getQuery(); 
    Tidy::nDBQueryResult result = query.exeSelect(charBase.table(), " count(*) as count ", " 1=1 ", 0);
    return static_cast<DWORD>(result[0]["count"]) + 100000;
}

void dtLoginService::dealLoginCheckDebug(const char* accountName, protocol::login::UserLoginData *data)
{
    Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
    if(conn == NULL) {
        error("不能从数据库连接池获取连接句柄 %s",__PRETTY_FUNCTION__);
        return;
    }
    SqlCharBase charBase;
    charBase.accountName = accountName;
    Tidy::nDBQuery query = conn->getQuery(); 
    Tidy::nDBQueryResult result = query.exeSelect(charBase.table(), charBase.field_list(SqlCharBase_charID, SqlCharBase_sex, SqlCharBase_name), charBase.equal_list(SqlCharBase_accountName), 1);
    if(result.empty() == true){
        error("查询失败 %s",__PRETTY_FUNCTION__);
        return;
    }
    data->charID = static_cast<DWORD>(result[0][SqlCharBase::names[SqlCharBase_charID]]);
    copyStr(data->accountName, accountName, sizeof(data->accountName));
    const char *name = static_cast<const char*>(result[0][SqlCharBase::names[SqlCharBase_name]]);
    copyStr(data->name, name, sizeof(data->name));
    data->sex = static_cast<DWORD>(result[0][SqlCharBase::names[SqlCharBase_sex]]);;
    server->debug("查询帐号基础数据成功[%s,%u][%s,%u]", data->accountName, data->charID, data->name, data->sex);
}

