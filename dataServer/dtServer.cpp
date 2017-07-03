#include "base/nDBConnectionPool.h"
#include "base/nDBUrlManager.h"
#include "base/nDBConnection.h"
#include "base/nServerConn.h"

#include "game/nSsqls.h"
#include "game/nTableManager.h"
#include "game/nTableRegister.h"

#include "protocol/serverInfo.h"

#include "dataServer/dtServer.h"
#include "dataServer/dtUserService.h"
#include "dataServer/dtMessageService.h"
#include "dataServer/dtLoginService.h"
#include "dataServer/dtSerializeService.h"
#include "dataServer/dtMailService.h"

dtServer* server = new dtServer();
namespace Tidy{
    struct argp_option serverArgOptions[] = {
        {0, 0, 0, 0, 0, 0}
    };
    char   serverDoc[] = "dtServer";
};

dtServer::dtServer() :
    Game::nGameServer("dataServer", "DT", DATASERVER, true),
    userService(NULL),
    messageService(NULL),
    serializeService(NULL),
    _db_conn_pool(NULL)
{
}

dtServer::~dtServer()
{
}

bool dtServer::closeGameServer()
{
    return true;
}

bool dtServer::finalGameServer()
{
    finalService();
    return true;
}

bool dtServer::finalService()
{
    loginService->finalService();
    SAFE_DELETE(loginService);
    SAFE_DELETE(userService);

    if(serializeService != NULL){
        serializeService->finalService();
        SAFE_DELETE(serializeService);
    }

    if(mailService != NULL){
        mailService->finalService();
        SAFE_DELETE(mailService);
    }

    // 必须放在最后,避免其他service的final里有数据库操作
    SAFE_DELETE(_db_conn_pool);
    return true;
}

bool dtServer::dealServerStartComplete(const DWORD serverID, const DWORD serverType)
{
    switch(serverType)
    {
        case GATEWAYSERVER:
            {
            }
            break;
        default:
            {
            }
    }
    return true;
}

Tidy::nBaseMessageService* dtServer::createMessageService()
{
    messageService = new dtMessageService();
    return createGameMessageService(messageService);
}

void dtServer::dealServerEvent(const DWORD eventID, const char *eventParam, const WORD eventParamLen)
{
}

void dtServer::dealConnectSuccess(Tidy::nServerConn* conn)
{
    messageService->dealConnectSuccess(conn);
    switch(conn->getServerType())
    {
        case GATEWAYSERVER:
            {
            }
            break;
        default:
            {
            }
    }
}

void dtServer::dealConnectClose(Tidy::nServerConn* conn)
{
    messageService->dealConnectClose(conn);
    userService->dealConnectClose(conn);
    switch(conn->getServerType())
    {
        case ZONESERVER:
            {
                setCloseZone();
            }
            break;
        case GATEWAYSERVER:
            {
            }
            break;
        default:
            {
            }
    }
}

bool dtServer::initConfig()
{
    return true;
}

bool dtServer::initService()
{
    loginService = new dtLoginService();
    if(loginService->initService() == false){
        assert_fail("");
        SAFE_DELETE(loginService);
         return false;
    }

    userService = new dtUserService();
    if(userService->initService() == false){
        SAFE_DELETE(userService);
        return false;
    }

    serializeService = new dtSerializeService();
    if(serializeService->initService() == false){
        SAFE_DELETE(serializeService);
        return false;
    }

    mailService = new dtMailService();
    if(mailService->initService() == false){
        SAFE_DELETE(mailService);
        return false;
    }

    std::vector<std::string> nameList;
    nameList.push_back("BuffTable");
    nameList.push_back("SkillGradeTable");
    nameList.push_back("FilterBaseTable");
    if(TableRegister::RegisterAllTable(nameList) == false){
        server->debug("加载配置失败");
        return false;
    }
    return true;
}

bool dtServer::startGameServer()
{
    _db_conn_pool = new Tidy::nDBConnectionPool(false);
    if(createDBConnection(_db_conn_pool, "ZONE_DATA_MYSQL_SERVER", eDBConnectionID_ZoneMain) == false){
        return false;
    }
    if(createDBConnection(_db_conn_pool, "ADMIN_DATA_MYSQL_SERVER", eDBConnectionID_Admin) == false){
        return false;
    }
    if(createDBConnection(_db_conn_pool, "FRIEND_DATA_MYSQL_SERVER", eDBConnectionID_Friend) == false){
        return false;
    }
    if(initConfig() == false){
        return false;
    }
    if(initService() == false){
        return false;
    }
    return true;
}

bool dtServer::doServerTimetick()
{
    if(doGameTick() == false)
    {
        return false;
    }
    userService->doTick();
    serializeService->doTick();
    checkServerClose();
    return true;
}

Tidy::nDBConnectionPool* dtServer::getDBConnPool() const
{
    return _db_conn_pool;
}

bool dtServer::createDBConnection(Tidy::nDBConnectionPool *dbConnPool, const char *configName, const DWORD connectionID)
{
    Tidy::nDBUrl url(connectionID, static_cast<const char*>(Tidy::global[configName]));
    if(Tidy::global[configName].empty() == true){
        if(dbConnPool->createConnection(url, NULL, dbConnPool->getConnection(eDBConnectionID_ZoneMain)) == false){
            error("连接数据库失败 %s, %s", url.getHostIP().c_str(),url.getDatabase().c_str());
            return false;
        }
    }
    else{
        if(url.checkValid() == false){
            assert_fail("数据库URL格式错误");
            return false;
        }
        std::string slaveUrlConfigName(configName);
        slaveUrlConfigName += "_slave";
        Tidy::nDBUrl slaveUrl(connectionID, (Tidy::global[slaveUrlConfigName]).getCString());
        if(dbConnPool->createConnection(url, (slaveUrl.checkValid() ? &slaveUrl : NULL), NULL) == false){
            error("连接数据库失败 %s, %s", url.getHostIP().c_str(), url.getDatabase().c_str());
            return false;
        }
    }

    std::string versionConfigStr(configName);
    versionConfigStr.append("_VERSION");
    if(checkDBVersion(connectionID, static_cast<DWORD>(Tidy::global[versionConfigStr.c_str()])) == false){
        assert_fail("版本号验证失败,更新代码或数据库");
        return false;
    }
    return true;
}

bool dtServer::checkDBVersion(const DWORD connectionID, const DWORD versionNum)
{
    if(versionNum == 0){
        return true;
    }
    Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(connectionID);
    if(conn == NULL){
        return false;
    }

    Tidy::nDBQuery query = conn->getQuery();
    SqlDataBaseVersion dataBaseVersion;
    dataBaseVersion.connectionID = connectionID;
    Tidy::nDBQueryResult recordset = query.exeSelect(dataBaseVersion.table(),
            dataBaseVersion.field_list(SqlDataBaseVersion_versionNum
                ),
            dataBaseVersion.equal_list(SqlDataBaseVersion_connectionID), 1);

    if(recordset.empty() == true){
        assert_fail("版本号验证失败,更新代码或数据库");
        return false;
    }
    if(versionNum != static_cast<DWORD>(recordset[0][SqlDataBaseVersion::names[SqlDataBaseVersion_versionNum]])){
        debug("数据库版本号错误[%u, %u]", versionNum, static_cast<DWORD>(recordset[0][SqlDataBaseVersion::names[SqlDataBaseVersion_versionNum]]));
        assert_fail("版本号验证失败,更新代码或数据库");
        return false;
    }

    return true;
}

void dtServer::dealDBClosed(Tidy::nDBConnection *connection)
{
    switch(connection->getConnectionID()){
        case eDBConnectionID_ZoneMain:
            {
                requestCloseZone();
            }
            break;
        default:
            {
            }
    }
}

void dtServer::dealZoneStart(const protocol::admin::ZoneData *zoneData)
{
    userService->dealZoneStart(zoneData);
}

void dtServer::dealZoneClose(const protocol::admin::ZoneData *zoneData)
{
    userService->dealZoneClose(zoneData);
}

void dtServer::dealCore(int sig, siginfo_t *info, void *secret)
{
    userService->dealCore();
    requestCloseZone();

    Game::nGameServer::dealCore(sig, info, secret, false);
}
