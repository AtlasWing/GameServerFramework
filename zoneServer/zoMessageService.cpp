#include "protocol/forwardCommand.h"
#include "protocol/loginCommand.h"
#include "protocol/platformCommon.h"

#include "base/nServerConn.h"
#include "base/nServerConnManager.h"
#include "base/nMailClient.h"
#include "base/nDBConnectionPool.h"

#include "game/nSqlData.h"

#include "zoneServer/zoMessageService.h"
#include "zoneServer/zoServer.h"
#include "zoneServer/zoUser.h"
#include "zoneServer/zoLoginService.h"
#include "zoneServer/zoLoginSession.h"
#include "zoneServer/zoMonitorService.h"
#include "zoneServer/zoUserService.h"
#include "zoneServer/zoGatewayConn.h"

zoMessageService::zoMessageService() :
    _data_server_conn(NULL),
    _master_admin_server_data(NULL), 
    _slave_admin_server_data(NULL),
    _admin_server_conn(NULL)
{
}

zoMessageService::~zoMessageService()
{
}

bool zoMessageService::init()
{
    if(Game::nGameMessageService<zoMessageService>::init() == false){
        return false;
    }

    return true;
}

void zoMessageService::final()
{
}

void zoMessageService::dealConnectSuccess(Tidy::nServerConn* conn)
{
    switch(conn->getServerType())
    {
        case ZONESERVER:
            {
            }
            break;
        case DATASERVER:
            {
                _data_server_conn = conn;
            }
            break;
        case LOGICSERVER:
            {
            }
            break;
        case GATEWAYSERVER:
            {
            }
            break;
        case USERCLIENT:
            {
            }
            break;
        case ADMINSERVER:
            {
                if(hasAdminServerConn() == true){
                    if(checkIsMasterAdminServerConn(conn) == true && hasSlaveAdminServerConn() == true){
                        // master连接代替slave连接
                        disconnectSlaveAdminServerConn();
                    }   
                }   
                _admin_server_conn = conn;
            }
            break;
        default:
            {
                assert_fail("未识别服务器类型");
            }
    }
}

void zoMessageService::dealConnectClose(Tidy::nServerConn* conn)
{
    switch(conn->getServerType())
    {
        case DATASERVER:
            {
                _data_server_conn = NULL;
            }
            break;
        case LOGICSERVER:
            {
            }
            break;
        case GATEWAYSERVER:
            {
            }
            break;
        case ADMINSERVER:
            {
                if(_admin_server_conn != conn){
                    return;
                }
                const bool isMasterAdminConn = checkIsMasterAdminServerConn(_admin_server_conn);
                if(isMasterAdminConn == true && server->checkCloseZone() == false && _slave_admin_server_data != NULL){
                    // 只连接一次,不重连
                    if(server->createServerClientConn(_slave_admin_server_data->serverID, ADMINSERVER, false, false, 0) == false) {
                        assert_fail("创建连接失败");
                    }
                }
                _admin_server_conn = NULL;
            }
            break;
        default:
            {
            }
    }
}

const DWORD zoMessageService::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const
{
    return Tidy::nStringTool::copyStr(headerBuffer, "[Message]", bufferSize);
}

Tidy::nLogger* zoMessageService::getLogger() const
{
    return server->getLogger();
}

bool zoMessageService::sendCmdToData(const protocol::NullProtoCommand* cmd, const unsigned int len) const
{
    return _data_server_conn != NULL && _data_server_conn->sendCmd(cmd, len);
}

bool zoMessageService::sendCmdToLogicByID(DWORD serverID, const protocol::NullProtoCommand* cmd, const unsigned int len) const
{
    return server->sendCmdToTaskConnByID(serverID, cmd, len);
}

bool zoMessageService::sendCmdToLogic(zoUser *user, const protocol::NullProtoCommand* cmd, const unsigned int len) const
{
    return user->getLogicConn()->sendCmd(cmd, len);
}

bool zoMessageService::sendCmdToLogicAll(const protocol::NullProtoCommand* cmd, const unsigned int len) const
{
    return server->sendCmdToTaskConnByType(LOGICSERVER, cmd, len);
}

bool zoMessageService::sendCmdToLogicAllExceptOne(const DWORD serverID, const protocol::NullProtoCommand* cmd, const unsigned int len) const
{
    return server->sendCmdToTaskConnByTypeExceptOne(LOGICSERVER, serverID, cmd, len);
}

bool zoMessageService::sendCmdToLogic(const zoUser *user, const protocol::NullProtoCommand *cmd, const DWORD len) const
{
    const Tidy::nServerConn *logicConn = user->getLogicConn();
    if(logicConn == NULL){
        return false;
    }
    return logicConn->sendCmd(cmd, len);
}

bool zoMessageService::sendCmdToGateway(const zoUser *user, const protocol::NullProtoCommand* cmd, const DWORD len) const
{
    const Tidy::nServerConn *gatewayConn = user->getGatewayConn();
    if(gatewayConn == NULL){
        return false;
    }
    return gatewayConn->sendCmd(cmd, len);
}

bool zoMessageService::doTick()
{
	return true;
}

bool zoMessageService::startService()
{
    server->debug("messageService初始化!");
    return true;
}

bool zoMessageService::connectAdminServer()
{
    if(server->getServerEntitySizeByServerType(ADMINSERVER) > 0) {
        return true;
    }

    std::string serverListStr = static_cast<const char*>(Tidy::global["ADMIN_SERVER_LIST"]);
    serverListStr = Tidy::nStringTool::replace(serverListStr, "\"", "");
    Tidy::vector<std::string> serverListVec; 
    Tidy::nStringTool::splitValue(serverListStr, ';', serverListVec);
    Tidy::map<DWORD, DWORD> serverSizeByTypeMap;
    Tidy::nParamMap serverData;
    assert_debug(serverListVec.size() > 0 && serverListVec.size() <= 2, "%u", serverListVec.size());
    for(DWORD i = 0; i < serverListVec.size(); ++ i){
        serverData.clear(); 
        serverData.parseCmdLine(serverListVec[i].c_str());
        assert_debug(serverData["ip"].empty() == false, "没有IP");
        assert_debug(serverData["port"].empty() == false, "没有端口");
        const char *ipStr = serverData["ip"].getCString();
        WORD port =  serverData["port"].getWord();

        protocol::ServerRun::ServerEntity entity;
        entity.serverID = server->getClientConnID();
        entity.serverType = ADMINSERVER;
        copyStr(entity.pstrName, "验证服务器", sizeof(entity.pstrName));
        copyStr(entity.internalIP, ipStr, sizeof(entity.internalIP));
        entity.internalPort = port;
        entity.hasUnixSocket = false;
        entity.checkHeartTick = true;
        entity.heartTickTimerInMsec = 15000;
        server->addServerEntity(entity);

        if(_master_admin_server_data == NULL){
            _master_admin_server_data = server->getServerEntityByID(entity.serverID);
        }
        else{
            _slave_admin_server_data = server->getServerEntityByID(entity.serverID);
        }
    }

    assert_debug(_master_admin_server_data != NULL, "");
    if(server->createServerClientConn(_master_admin_server_data->serverID, ADMINSERVER, false, true, 20 * 1000L) == false) {
        return false;
    }

    return true;
}

bool zoMessageService::hasSlaveAdminServerConn() const
{
    return checkIsMasterAdminServerConn(_admin_server_conn) == false;
}

bool zoMessageService::checkIsMasterAdminServerConn(Tidy::nServerConn *conn) const
{   
    return conn->getServerID() == _master_admin_server_data->serverID;
}

void zoMessageService::disconnectSlaveAdminServerConn()
{
    assert_debug(hasSlaveAdminServerConn() == true, "");
    Tidy::nClientConn *clientConn = dynamic_cast<Tidy::nClientConn*>(_admin_server_conn);
    clientConn->Terminate(Tidy::nClientConn::eTerminateMethod_NeedClose);
}

bool zoMessageService::sendCmdToAdminServer(const protocol::base::AdminCommandBase* command, const DWORD len) const
{
    if(_admin_server_conn == NULL)
    {
        return false;
    }
    return _admin_server_conn->sendCmd(command, len);
}

const bool zoMessageService::hasAdminServerConn() const
{
    return _admin_server_conn != NULL;
}
