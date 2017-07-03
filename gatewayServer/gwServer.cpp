#include "protocol/loginCommand.h"
#include "protocol/serializeCommand.h"

#include "base/nServerConnManager.h"
#include "base/nSocketPortData.h"
#include "base/nCommandCtl.h"
#include "base/nServerConn.h"
#include "base/nDBConnectionPool.h"
#include "base/nDBConnection.h"

#include "game/nSsqls.h"
#include "game/nTableRegister.h"
#include "game/nMiscDataSerialize.h"

#include "gatewayServer/gwServer.h"
#include "gatewayServer/gwLoginService.h"
#include "gatewayServer/gwUserService.h"
#include "gatewayServer/gwMessageService.h"
#include "gatewayServer/gwTelnetService.h"
#include "gatewayServer/gwConfigService.h"
#include "gatewayServer/gwLogicConn.h"
#include "gatewayServer/gwFilterService.h"

gwServer* server = new gwServer();
namespace Tidy{
    struct argp_option serverArgOptions[] = {
        {0,             0,      0,          0,  0,                                              0}
    };
    char   serverDoc[] = "gwServer";
};
gwServer::gwServer() :
    Game::nGameServer("gatewayServer", "GA", GATEWAYSERVER, false),
    telnetService(NULL),
    userService(NULL),
    loginService(NULL),
    filterService(NULL)
{
}

gwServer::~gwServer()
{
}

bool gwServer::closeGameServer()
{
    userService->dealServerClose();
    return true;
}

bool gwServer::finalGameServer()
{
    finalService();
    return true;
}

void gwServer::finalService()
{
    SAFE_DELETE(userService);
    if(loginService != NULL) {
        loginService->finalService();
        SAFE_DELETE(loginService);
    }
    if(telnetService != NULL) {
        telnetService->finalService();
        SAFE_DELETE(telnetService);
    }
    if(filterService != NULL) {
        if(Tidy::global["OPEN_NAME_FILTER"].getDword() > 0){
            filterService->finalService();
        }
    }
}

bool gwServer::dealServerStartComplete(const DWORD serverID, const DWORD serverType)
{
    switch(serverType)
    {
        case DATASERVER:
            {
                if(createServerClientConn(DATASERVER, false, false, 60 * 1000L) == false) {
                    return false;
                }
            }
            break;
        case LOGICSERVER:
            { 
                Tidy::nServerConn *oldLogicConn = server->getClientConnByID(serverID);
                if(oldLogicConn != NULL){
                    debug("旧Logic连接没有关闭[%u]", serverID);
                    Tidy::nClientConnNoQueue *logicClientConn = CLASS_CAST(Tidy::nClientConnNoQueue, oldLogicConn);
                    logicClientConn->setNeedClose();
                }
                if(createServerClientConn(serverID, LOGICSERVER, false, false, 60 * 1000L) == false){
                    return false;
                }
            }
            break;
        default:
            {
            }
    }

    return true;
}

Tidy::nTaskConn* gwServer::createTaskConn(const int sock, const struct sockaddr *addr, const struct Tidy::SocketPortData* listenPortData) const
{
    if(listenPortData->taskConnType == GATEWAYSERVER && listenPortData->path.empty() == true){
    }
    else{
        return Game::nGameServer::createTaskConn(sock, addr, listenPortData);
    }
}

Tidy::nClientConn* gwServer::createClientConn(const protocol::ServerRun::ServerEntity* serverInfo, const bool needReconnect, const DWORD reconnectTimer, const bool waitConnected) const
{
    if(serverInfo->serverType == LOGICSERVER) {
        std::string unixSocketPath;
        if(serverInfo->hasUnixSocket == true && checkLocalIP(serverInfo->internalIP) == true){
            unixSocketPath = getUnixSocketPath(serverInfo->serverID, serverInfo->internalIP, serverInfo->internalPort);
        }
        //TODO 使用配置
        DWORD tickTimerInMsec = ONE_MIN;
        Tidy::nConnectServerData connectServerData(serverInfo->serverID, serverInfo->serverType, serverInfo->pstrName, serverInfo->internalIP, serverInfo->internalPort, unixSocketPath.c_str(), serverInfo->zipMethod, needReconnect, reconnectTimer, waitConnected, serverInfo->checkHeartTick, tickTimerInMsec, &Tidy::baseCommandCtl);
        return new gwLogicConn(connectServerData);
    }

    return Game::nGameServer::createClientConn(serverInfo, needReconnect, reconnectTimer, waitConnected);
}

void gwServer::dealConnectSuccess(Tidy::nServerConn* conn)
{
    loginService->dealConnectSuccess(conn);
    messageService->dealConnectSuccess(conn);
}

void gwServer::dealConnectClose(Tidy::nServerConn* conn)
{
    userService->dealConnectClose(conn);
    loginService->dealConnectClose(conn);

    switch(conn->getServerType())
    {
        case LOGICSERVER:
            {
                gwLogicConn* logicConn = static_cast<gwLogicConn*>(conn);
                logicConn->dealClose();
            }
            break;
        case DATASERVER:
            {
            }
            break;
        default:
            {
            }
    }
    messageService->dealConnectClose(conn);
}

Tidy::nBaseMessageService* gwServer::createMessageService()
{
    messageService = new gwMessageService();
    return createGameMessageService(messageService);
}

bool gwServer::initService()
{
    std::vector<std::string> nameList;
    nameList.push_back("BuffTable");
    nameList.push_back("SkillGradeTable");
    nameList.push_back("SkillStudyTable");
    if(TableRegister::RegisterAllTable(nameList) == false){
        debug("配表读取错误");
        return false;
    }
    userService = new gwUserService();

    configService = new gwConfigService();
    if(configService->initService() == false){
        assert_fail("configService 初始化失败.");
        SAFE_DELETE(configService);
        return false;
    }

    loginService = new gwLoginService();
    if(loginService->initService() == false){
        assert_fail("loginService 初始化失败.");
        SAFE_DELETE(loginService);
        return false;
    }

    telnetService = new gwTelnetService();
    if(telnetService->initService() == false){
        assert_fail("telnetService 初始化失败.");
        SAFE_DELETE(telnetService);
        return false;
    }
    filterService = new gwFilterService();
    if(Tidy::global["OPEN_NAME_FILTER"].getDword() > 0){ 
        if(filterService->initService() == false){
            assert_fail("filterService 初始化失败.");
            SAFE_DELETE(filterService);
            return false;
        }   
    }
    return true;
}

bool gwServer::startGameServer()
{
    if(initService() == false){
        return false;
    }

    if(messageService->startService() == false){
        return false;
    }

    return true;
}

bool gwServer::doServerTimetick()
{
    if(doGameTick() == false){
        return false;
    }

    if(loginService->doTick() == false){
        return false;
    }

	if(messageService->doTick() == false){
		return false;
	}

	if(userService->doTick() == false){
		return false;
	}

    return true;
}

bool gwServer::dealCompleteStartup()
{
    char logName[128];
    snprintf(logName, sizeof(logName), "GA %u", getID());
    getLogger()->setName(logName);
    return true;
}

void gwServer::dealServerEvent(const DWORD eventID, const char *eventParam, const WORD eventParamLen)
{
}

gwUserTaskConn* gwServer::getUserClientConnByID(const DWORD sessionID)
{
    return NULL;
}

BYTE gwServer::syncTaskConn(Tidy::nTaskConn* conn, const void* packet, const DWORD len)
{
    Tidy::nServerConn *serverConn = conn->castAsServerConn();
    if(serverConn->getServerType() == USERCLIENT){
        return Tidy::nTaskConn::eSyncState_Success;
    }
    return Game::nGameServer::syncTaskConn(conn, packet, len);
}

bool gwServer::verifyTaskConn(Tidy::nTaskConn* conn, const void* packet, const DWORD len)
{
    if(conn->castAsServerConn()->getServerType() == USERCLIENT){
        return packet != NULL && loginService->verifyUserClientConn(conn, reinterpret_cast<const protocol::NullProtoCommand*>(packet), len);
    }
    return Game::nGameServer::verifyTaskConn(conn, packet, len);
}

void gwServer::dealAjustTimeChange()
{
    if(userService != NULL){
        //userService->setNeedSyncTime();
    }
}
