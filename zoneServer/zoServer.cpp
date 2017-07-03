#include "protocol/forwardCommand.h"
#include "protocol/serverRunCommand.h"
#include "protocol/adminCommand.h"

#include "base/nDBUrlManager.h"
#include "base/nDBConnectionPool.h"
#include "base/nDBConnection.h"
#include "base/nServerConn.h"
#include "base/nXMLParser.h"
#include "base/nClientConn.h"
#include "base/nServerConnManager.h"
#include "base/nCommandCtl.h"
#include "base/nSocketPortData.h"

#include "game/nTableRegister.h"
#include "game/nMiscDataSerialize.h"

#include "zoneServer/zoServer.h"
#include "zoneServer/zoUserService.h"
#include "zoneServer/zoTaskConn.h"
#include "zoneServer/zoSsqls.h"
#include "zoneServer/zoLoginService.h"
#include "zoneServer/zoMessageService.h"
#include "zoneServer/zoScheduleService.h"
#include "zoneServer/zoLogicConn.h"
#include "zoneServer/zoGatewayConn.h"
#include "zoneServer/zoMonitorService.h"
#include "zoneServer/zoMailService.h"
#include "zoneServer/zoRelationUserService.h"
#include "zoneServer/zoConfigService.h"
#include "zoneServer/zoMapService.h"

zoServer* server = new zoServer();
namespace Tidy{
	struct argp_option serverArgOptions[] = {
		{0,				0,		0,			0,	0,												0}
	};
	char   serverDoc[] = "zoServer";
};

zoServer::zoServer() : 
    Game::nGameServer("zoneServer", "ZO", ZONESERVER, true),
    miscDataSerialize(NULL),
    loginService(NULL),
    userService(NULL),
    messageService(NULL),
    scheduleService(NULL),
    monitorService(NULL),
    relationUserService(NULL),
    mailService(NULL),
    configService(NULL),
    mapService(NULL),
    _DB_conn_pool(NULL),
    _user_task_conn_manager(NULL),
    _logic_ai_gateway_is_all_running(false),
    _is_start_complete(false),
	_is_first_connect_admin(true),
    _client_conn_id(0),
    _game_type(0)
{
}

zoServer::~zoServer()
{
}

bool zoServer::finalGameServer()
{
    saveCloseZoneTime();
    finalService();

    return true;
}

void zoServer::finalService()
{
    loginService->finalService();
    SAFE_DELETE(loginService);

    SAFE_DELETE(miscDataSerialize);
    SAFE_DELETE(_DB_conn_pool);
    SAFE_DELETE(relationUserService);

    if(scheduleService != NULL){
        scheduleService->finalService();
        SAFE_DELETE(scheduleService);
    }
    if(mailService != NULL){
        mailService->finalService();
        SAFE_DELETE(mailService);
    }
    if(userService != NULL){
        SAFE_DELETE(userService);
    }
    if(configService != NULL){
        configService->finalService();
        SAFE_DELETE(configService);
    }
    if(mapService != NULL){
        mapService->finalService();
        SAFE_DELETE(mapService);
    }
}

bool zoServer::dealServerStartComplete(const DWORD serverID, const DWORD serverType)
{
	if(isGameServer(serverType) == false){
		return true;
	}

    const protocol::ServerRun::ServerEntity* serverInfo = getServerEntityByID(serverID);
    if(serverInfo == NULL){
        return true;
    }

    sendAllZoneData(serverID, serverType);
    protocol::ServerRun::NotifyStartupCompleteCommand send;
    send.serverInfo = *serverInfo;
    sendCmdToTaskConnAll(&send, sizeof(send));

    addRunningServerCount(serverType);

    switch(serverType)
    {
        case DATASERVER:
            {
                if(relationUserService->initAllRelationUser() == false){
                    assert_fail("初始化relationUserService失败"); 
                    return false;
                }
            }
            break;
    }

    return true;
}

bool zoServer::loginToServer(Tidy::nServerConn* conn)
{
    switch(conn->getServerType())
    {
        case PLATFORMSERVER:
            {
            }
            break;
        case ADMINSERVER:
            {
            }
            break;
        case ZONESERVER:
            {
                return Game::nGameServer::loginToServer(conn);
            }
            break;
        default:
            {
                assert_fail("未识别的连接");
            }
    }

    return false;
}

bool zoServer::createConnManager()
{
    _user_task_conn_manager = new Tidy::nQueueServerConnManager();
    return Game::nGameServer::createConnManager();
}

void zoServer::destroyConnManager()
{
    SAFE_DELETE(_user_task_conn_manager);
    Game::nGameServer::destroyConnManager();
}

Tidy::nTaskConn* zoServer::createTaskConn(const int sock, const struct sockaddr *addr, const Tidy::SocketPortData* listenPortData) const
{
    if(listenPortData->taskConnType == USERCLIENT){
        return NULL;
    }
    assert_debug(listenPortData->taskConnType == ZONESERVER, "[%u]", listenPortData->taskConnType);
    return new zoTaskConn(sock, addr, listenPortData);
}

bool zoServer::addTaskConnID(const Tidy::nTaskConn* conn)
{
    Tidy::nServerConn *serverConn = conn->castAsServerConn();
    if(serverConn->getServerType() == USERCLIENT){
        return true;
    }
    return Game::nGameServer::addTaskConnID(conn);
}

bool zoServer::removeTaskConnID(const Tidy::nTaskConn* conn)
{
    Tidy::nServerConn *serverConn = conn->castAsServerConn();
    if(serverConn->getServerType() == USERCLIENT){
        return true;
    }
    return Game::nGameServer::removeTaskConnID(conn);
}

bool zoServer::addTaskConn(Tidy::nTaskConn* conn)
{
    Tidy::nServerConn *serverConn = conn->castAsServerConn();
    if(serverConn->getServerType() == USERCLIENT){
        return _user_task_conn_manager->addConn(serverConn);
    }
    return Game::nGameServer::addTaskConn(conn);
}

bool zoServer::removeTaskConn(Tidy::nTaskConn* conn)
{
    Tidy::nServerConn *serverConn = conn->castAsServerConn();
    if(serverConn->getServerType() == USERCLIENT){
        _user_task_conn_manager->removeConn(serverConn);
        return true;
    }
    return Game::nGameServer::removeTaskConn(conn);
}

Tidy::nBaseMessageService* zoServer::createMessageService()
{
    messageService = new zoMessageService();
    return createGameMessageService(messageService);
}

void zoServer::dealHupSig()
{
	Game::nGameServer::dealHupSig();
    reloadConfig();
    scheduleService->dealHupSig();
}

void zoServer::reloadConfig()
{
    loadServerList(getLastCloseTimeInSec());
    sendServerEvent(0, 0, protocol::ServerRun::eServerEvent_ReloadConfig, NULL, 0);
    debug("zoServer 重新加载配置完成");
}

void zoServer::dealServerEvent(const DWORD eventID, const char *eventParam, const WORD eventParamLen)
{
    switch(eventID){
        case protocol::ServerRun::eServerEvent_ZeroClocker:
            {
                debug("零点事件");
                setNeedAnalyze();
                doZeroClock(true, 1);
                clearNeedAnalyze();
            }
            break;
        case protocol::ServerRun::eServerEvent_SequenceEloReward:
            {
                debug("天梯榜奖励");
            }
            break;
        case protocol::ServerRun::eServerEvent_SequenceEloReset:
            {
                debug("天梯榜重置");
            }
            break;
    }
}

void zoServer::dealCtrlcSig()
{
    normalCloseZone();
}

void zoServer::dealConnectSuccess(Tidy::nServerConn* conn)
{
    messageService->dealConnectSuccess(conn);
    switch(conn->getServerType())
    {
        case ZONESERVER:
            {
            }
            break;
        case DATASERVER:
            {
            }
            break;
        case LOGICSERVER:
            {
                conn->newData<zoLogicConn>();
            }
            break;
        case GATEWAYSERVER:
            {
                conn->newData<zoGatewayConn>();
                monitorService->dealGatewayConnectSuccess(conn);
            }
            break;
        case USERCLIENT:
            {
            }
            break;
        default:
            {
                assert_fail("未识别服务器类型");
            }
    }

    dealServerStartComplete(conn->getID(), conn->getServerType());
}

void zoServer::dealConnectClose(Tidy::nServerConn* conn)
{
    if(checkCloseZone() == false)
    {
        const protocol::ServerRun::ServerEntity* serverEntity = getServerEntityByID(conn->getID());
        if(serverEntity != NULL) {
            const_cast<protocol::ServerRun::ServerEntity*>(serverEntity)->lastServerCloseTimeInSec = server->time().sec();
            const_cast<protocol::ServerRun::ServerEntity*>(serverEntity)->hasClosed = true;
        }
    }

    removeRunningServerCount(conn->getServerType());

    messageService->dealConnectClose(conn);

    switch(conn->getServerType())
    {
        case DATASERVER:
            {
                setCloseZone();
            }
            break;
        case LOGICSERVER:
            {
                userService->dealLogicClose(conn);
                conn->deleteData<zoLogicConn>();
            }
            break;
        case GATEWAYSERVER:
            {
                loginService->dealGatewayClose(conn);
                userService->dealGatewayClose(conn);
                conn->deleteData<zoGatewayConn>();
            }
            break;
        default:
            {
            }
    }
}

void zoServer::normalCloseZone()
{
    setCloseZone();
}

BYTE zoServer::syncTaskConn(Tidy::nTaskConn* conn, const void* packet, const DWORD len)
{
    Tidy::nServerConn *serverConn = conn->castAsServerConn();
    if(serverConn->getServerType() == TELNETCLIENT){
        return Tidy::nTaskConn::eSyncState_Success;
    }
    else if(serverConn->getServerType() == HTTPCLIENT){
        return Tidy::nTaskConn::eSyncState_Success;
    }
    else if(serverConn->getServerType() == USERCLIENT){
        return Tidy::nTaskConn::eSyncState_Success;
    }

    const protocol::NullProtoCommand *nullCommand = reinterpret_cast<const protocol::NullProtoCommand *>(packet);
    zoTaskConn* taskConn = static_cast<zoTaskConn*>(conn);

    if(checkCloseZone() == true){
        return Tidy::nTaskConn::eSyncState_Fail;
    }
    else if(serverConn->getServerID() == getServerID()){
        return Tidy::nTaskConn::eSyncState_Success;
    }
    else if(nullCommand != NULL) {
        if(nullCommand->cmd == protocol::ServerRun::ServerStartupComplete::CMD && nullCommand->param == protocol::ServerRun::ServerStartupComplete::PARAM){
            taskConn->setSyncState(zoTaskConn::eServerState_OK);
        }
        else if(baseMessageService->parseTaskTickCommand(taskConn, reinterpret_cast<const char*>(packet), len) == false){
            assert_fail("当期状态无法处理消息[%u,%u],[%u,%u]", serverConn->getServerID(), serverConn->getServerType(), nullCommand->cmd, nullCommand->param);
        }
    }
    if(taskConn->getSyncState() == zoTaskConn::eServerState_WaitOtherStart) {
        if(taskConn->checkDependStart() == true) {
            taskConn->setSyncState(zoTaskConn::eServerState_WaitOtherRet);
        }
    }
    else if(taskConn->getSyncState() == zoTaskConn::eServerState_WaitOtherRet) {
        if(taskConn->checkDependServerRespond() == true) {
            taskConn->notifyMeStart();
            taskConn->setSyncState(zoTaskConn::eServerState_WaitMeStart);
        }
    }
    else if(taskConn->getSyncState() == zoTaskConn::eServerState_OK) {
        sendZoneInfo(taskConn);
        return Tidy::nTaskConn::eSyncState_Success;
    }

    return Tidy::nTaskConn::eSyncState_None;
}

bool zoServer::verifyTaskConn(Tidy::nTaskConn* conn, const void* packet, const DWORD len)
{
    Tidy::nServerConn *serverConn = conn->castAsServerConn();
    if(serverConn->getServerType() == TELNETCLIENT){
        return Game::nGameServer::verifyTaskConn(conn, packet, len);
    }
    else if(serverConn->getServerType() == HTTPCLIENT){
        return Game::nGameServer::verifyTaskConn(conn, packet, len);
    }
    else if(serverConn->getServerType() == USERCLIENT){
    }
    return (packet != NULL) && verifyGameTaskConn(conn, packet, len);
}

bool zoServer::verifyGameTaskConn(Tidy::nTaskConn* conn, const void* packet, const DWORD len)
{
    if(checkCloseZone() == true){
        return false;
    }
    const protocol::NullProtoCommand *nullCommand = reinterpret_cast<const protocol::NullProtoCommand *>(packet);
    using namespace protocol::ServerRun;

    switch(nullCommand->cmd)
    {
        case protocol::base::ServerRunCommandBase::CMD:
            {
                switch(nullCommand->param)
                {
                    case ServerConnectCommand::PARAM:
                        {
                            const ServerConnectCommand* command = static_cast<const ServerConnectCommand*>(nullCommand);
                            Tidy::vector<const protocol::ServerRun::ServerEntity*> serverEntityList;
                            if(getServerEntityByType(command->serverType, &serverEntityList) == 0) {
                                debug("获取服务器类型错误");
                                return false;
                            }
                            const protocol::ServerRun::ServerEntity *entity = NULL;
                            for(Tidy::vector<const protocol::ServerRun::ServerEntity*>::iterator iter = serverEntityList.begin(); iter != serverEntityList.end(); ++ iter) {
                                const protocol::ServerRun::ServerEntity &serverInfo = **iter;
                                if(checkServerStarted(serverInfo.serverID) == true) {
                                    continue;
                                }
                                for(BYTE i = 0; i < command->num; ++ i) {
                                    if(strcmp(command->data[i], serverInfo.internalIP) == 0) {
                                        entity = &serverInfo;
                                    }
                                    else{
                                        for(DWORD j = 0; j < serverInfo.listenDataNum; ++ j){
                                            if(strcmp(serverInfo.listenDataList[j].ip, command->data[i]) == 0){
                                                entity = &serverInfo;
                                                break;
                                            }
                                        }
                                    }
                                    if(entity != NULL){
                                        break;
                                    }
                                }
                                if(entity != NULL) {
                                    break;
                                }
                            }
                            if(entity == NULL) {
                                error("客户端连接验证失败,未找到服务器信息");
                                return false;
                            }

							zoTaskConn* taskConn = static_cast<zoTaskConn*>(conn);
							taskConn->setServerEntity(entity);
                            taskConn->notifyMeInit();
                            return true;
                        }
                        break;
                    default:
                        {
                        }
                }
            }
            break;
        default:
            {
                assert_fail("未识别消息");
                return true;
            }
    }

    assert_fail("未识别消息");
    return false;
}

bool zoServer::initConfig()
{
	zoSsqls::init();

    // 为了避开区内的服务器的ID,不能冲突,否则有问题了
    _client_conn_id = Tidy::global["MIN_CLIENT_CONN_ID"];
    assert_debug(_client_conn_id > 0, "");
    return true;
}

bool zoServer::startGameServer()
{
    if(initConfig() == false){
        return false;
    }

    if(initService() == false){
        return false;
    }

    if(loadZoneConfig() == false) {
        return false;
    }

    if(messageService->startService() == false){
        return false;
    }

    checkAllPastClock();
    return true;
}

bool zoServer::initService()
{
	_DB_conn_pool = new Tidy::nDBConnectionPool(false);
	Tidy::nDBUrl url(eDBConnectionID_ZoneMain, static_cast<const char*>(Tidy::global["ZONE_DATA_MYSQL_SERVER"]));
	if(url.checkValid() == false) {
		assert_fail("数据库连接配置格式错误");
		return false;
	}

    Tidy::nDBUrl slaveUrl(eDBConnectionID_ZoneMain, Tidy::global["ZONE_DATA_MYSQL_SERVER_SLAVE"].getCString());
	if(_DB_conn_pool->createConnection(url, (slaveUrl.checkValid() ? &slaveUrl : NULL), NULL) == false) {  
		error("连接数据库失败 %s, %s", url.getHostIP().c_str(),url.getDatabase().c_str());
		return false;
	}

	Tidy::nDBUrl adminUrl(eDBConnectionID_Admin, static_cast<const char*>(Tidy::global["ADMIN_DATA_MYSQL_SERVER"]));
	if(adminUrl.checkValid() == false) {
		assert_fail("数据库连接配置格式错误");
		return false;
	}

	Tidy::nDBUrl adminSlaveUrl(eDBConnectionID_Admin, Tidy::global["ADMIN_DATA_MYSQL_SERVER"].getCString());
	if(_DB_conn_pool->createConnection(adminUrl, (adminSlaveUrl.checkValid() ? &adminSlaveUrl : NULL), NULL) == false) {  
		error("连接数据库失败 %s, %s", adminUrl.getHostIP().c_str(), adminUrl.getDatabase().c_str());
		return false;
	}

    std::vector<std::string> nameList;
    if(TableRegister::RegisterAllTable(nameList) == false){
        debug("配表读取错误");
        return false;
    }

    miscDataSerialize = new Game::nMiscDataSerialize(_DB_conn_pool);

    scheduleService = new zoScheduleService();
    if(scheduleService->initService() == false){
        assert_fail("scheduleService 初始化失败");
        SAFE_DELETE(scheduleService);
        return false;
    }

    if(miscDataSerialize->init() == false) {
        SAFE_DELETE(miscDataSerialize);
        return false;
    }

    loginService = new zoLoginService();
    if(loginService->initService() == false) {
        SAFE_DELETE(loginService);
        return false;
    }

    mailService = new zoMailService();
    if(mailService->initService() == false){
        assert_fail("mailService 初始化失败");
        SAFE_DELETE(mailService);
        return false;
    }

    userService = new zoUserService();
    relationUserService = new zoRelationUserService();
    if(relationUserService->init() == false){
        assert_fail("relationUserService 初始化失败");
        SAFE_DELETE(relationUserService);
        return false;
    }

    configService = new zoConfigService();
    if(configService->initService() == false){
        assert_fail("configService 初始化失败");
        SAFE_DELETE(configService);
        return false;
    }

    mapService = new zoMapService();
    if(mapService->initService() == false){
        assert_fail("mapService 初始化失败");
        SAFE_DELETE(mapService);
        return false;
    }

    return true;
}

bool zoServer::loadZoneConfig()
{
    Tidy::string data;
    DWORD lastServerCloseTimeInSec = time().sec();
    miscDataSerialize->load(protocol::serialize::eMiscDataSerializeType_ZoneCloseTime, 0, &lastServerCloseTimeInSec, &data);
    if(data.empty() == false) {
        lastServerCloseTimeInSec = *(reinterpret_cast<const DWORD*>(data.c_str()));
    }

    if(loadServerList(lastServerCloseTimeInSec) == false){
        assert_fail("加载服务器配置失败");
        return false;
    }

    const protocol::ServerRun::ServerEntity* serverInfo = NULL;
    serverInfo = getNextServerEntityByType(ZONESERVER, NULL);
    assert_debug(Tidy::nStringTool::compare(serverInfo->internalIP, static_cast<const char*>(Tidy::global["ZONE_SERVER_IP"])) == 0, "Super配置的IP不对");
    if(serverInfo == NULL) {
        assert_debug("没有配ZONESERVER", "");
        return false;
    }

    setServerInfo(*serverInfo);
    return true;
}

bool zoServer::loadServerList(const DWORD lastServerCloseTimeInSec)
{
    //TODO 检查新增的服务器的ID不能比原来的小
    assert_debug(Tidy::global["SERVER_LIST"].empty() == false, "没有配置 ServerList");
    std::string serverListStr = Tidy::global["SERVER_LIST"].getString();
    serverListStr = Tidy::nStringTool::replace(serverListStr, "\"", "");
    Tidy::vector<std::string> serverListVec;
    Tidy::nStringTool::splitValue(serverListStr, ';', serverListVec);
    Tidy::map<DWORD, DWORD> serverSizeByTypeMap;
    Tidy::vector<protocol::ServerRun::ServerEntity> serverEntityList;
	bool hasClosed = false;;
    for(DWORD i = 0; i < serverListVec.size(); ++ i){
        Tidy::nParamMap serverData;
        serverData.parseCmdLine(serverListVec[i].c_str());

		hasClosed = false;
		const protocol::ServerRun::ServerEntity *oldEntityData = getServerEntityByID(serverData["id"].getWord());
		if(oldEntityData != NULL){
			hasClosed = oldEntityData->hasClosed;
		}
		BYTE zipMethod = 0;
		if(serverData["zipMethod"].empty() == false){
			zipMethod = serverData["zipMethod"].getByte();
		}
		else if(serverData["needCompress"].empty() == false){
			// 兼容以前的配置方式
			zipMethod = serverData["needCompress"].getByte();
		}
        protocol::ServerRun::ServerEntity entity(serverData["id"].getWord(),  serverData["type"].getWord(), getServerTypeString(serverData["type"].getWord()), serverData["ip"].getCString(), serverData["port"].getWord(), zipMethod, (serverData["checkHeartTick"].getWord() > 0), serverData["heartTickTimerInMsec"].getDword(), (serverData["needQuickAck"].getWord() > 0), (serverData["hasUnixSocket"].getWord() > 0), lastServerCloseTimeInSec, serverSizeByTypeMap[serverData["type"].getWord()], hasClosed);
        if(serverData["port"].getWord() == 0){
            assert_fail("需要配置端口");
            return false;
        }
        serverSizeByTypeMap[entity.serverType] = serverSizeByTypeMap[entity.serverType] + 1;
        Tidy::vector< Tidy::vector<Tidy::nAny> > externalListenDataList;
        Tidy::nStringTool::splitValue(serverData["externListenConfig"].getString(), ',', ':', externalListenDataList);
        assert_debug(externalListenDataList.size() < protocol::ServerRun::MAX_SERVER_LISTEN_DATA_NUM, "");
        for(DWORD j = 0; j < externalListenDataList.size(); ++ j){
            const Tidy::vector<Tidy::nAny> &listenDataConfig = externalListenDataList[j];
            assert_debug(listenDataConfig.size() == 4, "[%u]", listenDataConfig.size());
            protocol::ServerRun::ServerListenData &listenData = entity.listenDataList[entity.listenDataNum];
            std::string taskConnTypeStr(listenDataConfig[0].getString());
            Tidy::nStringTool::trim(taskConnTypeStr);
            listenData.taskConnType = getServerTypeByString(taskConnTypeStr.c_str());
            if(listenData.taskConnType == MAX_SERVERTYPE){
                assert_fail("无效服务器类型[%u]", listenData.taskConnType);
                return false;
            }
            listenData.netType = listenDataConfig[1].getByte();
            assert_debug(listenData.netType < eNetType_MAX, "[%u]", listenData.netType);
            std::string url = listenDataConfig[2].getString();
            Tidy::nStringTool::trim(url);
            if(inet_addr(url.c_str()) != INADDR_NONE){
                // 使用的IP.如果使用域名,ip为空,不限制连接ip
                copyStr(listenData.ip, url.c_str(), sizeof(listenData.ip));
            }
            copyStr(listenData.url, url.c_str(), sizeof(listenData.url));
            listenData.port = listenDataConfig[3].getWord();
            if(listenData.port == 0){
                assert_fail("端口为0");
                return false;
            }
            ++ entity.listenDataNum;
        }
        serverEntityList.push_back(entity);
        if(entity.serverID > _client_conn_id){
            assert_debug(entity.serverID <= _client_conn_id, "不能和clientConnID冲突[%u,%u]", entity.serverID, _client_conn_id);
            return false;
        }
        assert_debug(entity.serverType != ZONESERVER || entity.serverID == Tidy::global["ZONE_SERVER_ID"].getDword(), "");
    }

    for(DWORD i = 0; i < serverEntityList.size(); ++ i){
        serverEntityList[i].serverNumInSameType = serverSizeByTypeMap[serverEntityList[i].serverType];
        addServerEntity(serverEntityList[i]);
    }

    return true;
}

bool zoServer::doServerTimetick()
{
    if(doGameTick() == false) {
        return false;
    }

	messageService->doTick();
	userService->doTick();
    _user_task_conn_manager->doTimeTick();
    scheduleService->doTick();
	monitorService->doTick();
    checkServerClose();
    if(server->relationUserService->doTick() == false){
        assert_fail("relationUserService tick fail");
    }

    return true;
}

void zoServer::outputServerDelayResult(zoUser *user, const DWORD delayTimeInMsec, const DWORD fromSvrType, const DWORD fromSvrID, const DWORD toSvrType, const DWORD toSvrID) {
    std::string resultStr = Tidy::nStringTool::format("延迟:%4ums %sID:%u->%sID:%u", delayTimeInMsec, getServerTypeString(fromSvrType), fromSvrID,getServerTypeString(toSvrType), toSvrID);
    server->debug(resultStr.c_str());
}

const bool zoServer::checkServerStarted(const DWORD serverID)
{
    return isServerConnIDAdded(serverID);
}

namespace
{
    struct GetAllServerEntityExec
    {
        Tidy::vector<protocol::ServerRun::ServerEntity>& _server_entity_vec;
        GetAllServerEntityExec(Tidy::vector<protocol::ServerRun::ServerEntity>& entityVec) : _server_entity_vec(entityVec)
        {
        }

        bool exec(Tidy::nServerConn* entity)
        {
            zoTaskConn* taskConn = static_cast<zoTaskConn*>(entity);
            _server_entity_vec.push_back(* (taskConn->getServerEntity() ));
            return true;
        }
    };
};

bool zoServer::getAllTaskConnEntity(Tidy::vector<protocol::ServerRun::ServerEntity>& taskConnEntityVec)
{
    GetAllServerEntityExec exec(taskConnEntityVec);
    return execEveryTaskConn(exec);
}

Tidy::nDBConnectionPool* zoServer::getDBConnPool() const
{
    return _DB_conn_pool;
}

bool zoServer::checkClientSyncResult(Tidy::nClientConn* conn, const void* packet, const DWORD len)
{
    const protocol::NullProtoCommand *nullCommand = reinterpret_cast<const protocol::NullProtoCommand *>(packet);

    Tidy::nServerConn* serverConn = conn->castAsServerConn();
    switch(serverConn->getServerType())
    {
        case PLATFORMSERVER:
            {
                using namespace protocol::platform;
                switch(nullCommand->param)
                {
                    default:
                        {
                        }
                }
            }
            break;
        case ADMINSERVER:
            {
                using namespace protocol::admin;
                switch(nullCommand->param)
                {
                    case A2S_ServerSyncOKCommand::PARAM:
                        {
                            const A2S_ServerSyncOKCommand *command = static_cast<const A2S_ServerSyncOKCommand*>(nullCommand);
                            setZoneInfo(command->zoneID, command->zoneName, command->gameType);
                            sendZoneInfo(NULL);
                            return true;
                        }
                        break;
                    default:
                        {
                        }
                }
            }
            break;
        case ZONESERVER:
            {
                return true;
            }
            break;
        default:
            {
                assert_fail("无法识别的连接类型");
            }
    }

    return false;
}

bool zoServer::saveCloseZoneTime()
{
    DWORD nowTimeInSec = time().sec();
    miscDataSerialize->save(protocol::serialize::eMiscDataSerializeType_ZoneCloseTime, 0, reinterpret_cast<const char*>(&nowTimeInSec), sizeof(DWORD));

    return true;
}

bool zoServer::closeGameServer()
{
    return true;
}

bool zoServer::sendServerEvent(const DWORD serverType, const DWORD serverID, const DWORD eventID, const char *eventParam, const WORD eventParamLen)
{
    BUFFER_PROTO_CMD(protocol::ServerRun::NotifyServerEventCommand, send);
    send->eventID = eventID;
    send->num = eventParamLen;
    if(send->num > 0){
        if(send->num > PROTO_CMD_CAPACITY(protocol::ServerRun::NotifyServerEventCommand)){
            assert_fail("空间不足");
            return false;
        }
        memcpy(send->eventParam, eventParam, send->num);
    }
    if(serverType != UNKNOWNSERVER){
        if(serverType != ZONESERVER){
            server->sendCmdToTaskConnByType(serverType, send, send->allSize());
        }
        else{
            server->messageService->parseServerCommand(NULL, send, send->allSize());
        }
    }
    else if(serverID != 0){
        server->sendCmdToTaskConnByID(serverID, send, send->allSize());
    }
    else{
        server->messageService->parseServerCommand(NULL, send, send->allSize());
        server->sendCmdToTaskConnAll(send, send->allSize());
    }

    return true;
}

bool zoServer::sendZoneInfo(Tidy::nServerConn *conn) const
{
    protocol::ServerRun::SetZoneInfoCommand send;
    send.zoneID = getZoneID();
    copyStr(send.zoneName, getZoneName(), sizeof(send.zoneName));
    if(conn != NULL){
        return conn->sendCmd(&send, sizeof(send));
    }
    return sendCmdToTaskConnAll(&send, sizeof(send));
}

bool zoServer::onReturnServerDelayTest(const Tidy::nServerConn *conn, const protocol::ServerRun::ServerDelayLinkedTestCommand *command)
{
    if(command->charID != 0) {
        // 是人发起的，要给输出回执
        Tidy::nTime curTime;
        DWORD delay = curTime.msec() - command->startTimeInMsec;
        std::string resultStr = Tidy::nStringTool::format("线路完成，总延迟 : %u ms", delay);
        zoUser *user = server->userService->getByID(command->charID);
        server->debug(resultStr.c_str());
        for(WORD i = 0; i<command->num; ++i) {
            outputServerDelayResult(user, command->infoList[i].delayTimeInMsec, command->infoList[i].fromServerType, command->infoList[i].fromServerID, command->infoList[i].toServerType, command->infoList[i].toServerID);
        }
        // 最后返回给zoneServer的时间也要输出
        outputServerDelayResult(user, curTime.msec() - command->sendTimeInMsec, conn->getServerType(), conn->getServerID(), server->getServerType(), server->getServerID());
    }
    return true;
}

void zoServer::addRunningServerCount(DWORD serverType)
{
    if(serverType >= MAX_SERVERTYPE) {
        assert_fail("服务器类型错误");
        return;
    }

    static DWORD logicServerEntityCount = getServerEntitySizeByServerType(LOGICSERVER);
    static DWORD gatewayServerEntityCount = getServerEntitySizeByServerType(GATEWAYSERVER);
    
    ++_running_server_count_data.count[serverType];
   
    if(_running_server_count_data.getRunningServerCountByType(LOGICSERVER) == logicServerEntityCount &&
            _running_server_count_data.getRunningServerCountByType(GATEWAYSERVER) == gatewayServerEntityCount) {
        dealAllServerStartComplete();
    }
}

void zoServer::removeRunningServerCount(DWORD serverType)
{
    if(serverType >= MAX_SERVERTYPE) {
        assert_fail("服务器类型错误");
        return;
    }
    -- _running_server_count_data.count[serverType];
}

void zoServer::dealAllServerStartComplete()
{
    bool isRestart = _is_start_complete;
    if(_is_start_complete == false){
        _is_start_complete = true;
        dealZoneStartComplete();
    }
    server->debug("Logic//Gateway服务器全部启动完成");
    sendServerEvent(0, 0, protocol::ServerRun::eServerEvent_AllGameServerStartComplete, reinterpret_cast<const char*>(&isRestart), sizeof(isRestart));
}

void zoServer::dealZoneStartComplete()
{
    loginService->openLogin();
}

void zoServer::clearAllZoneData()
{
    Game::nGameServer::clearAllZoneData();
    protocol::ServerRun::ClearZoneDataCommand send;
    sendCmdToTaskConnAll(&send, sizeof(send));
}

void zoServer::addAdjustServerTime(const SDWORD adjustTimeInSec)
{
    const std::string oldTimeStr = server->getServerTimeStr();
    Tidy::nBaseServer::addAdjustServerTime(adjustTimeInSec);
    const std::string currTimeStr = server->getServerTimeStr();
    char msg[1204] = {0};
    snprintf(msg, sizeof(msg), "服务器时间调整为 [%s] 周[%d]", currTimeStr.c_str(), server->time().getWDays());
    debug("服务器时间调整为 [%s], 原来时间 [%s], 变化 [%d]秒", currTimeStr.c_str(), oldTimeStr.c_str(), adjustTimeInSec);

    protocol::ServerRun::AdjustServerTimeCommand send;
    send.adjustTimeInSec = adjustTimeInSec;
    sendCmdToTaskConnAll(&send, sizeof(send));
}

DWORD zoServer::getClientConnID()
{
    ++ _client_conn_id;
    return _client_conn_id;
}

bool zoServer::setGlobalParam(const DWORD serverType, const std::string &key, const std::string &value)
{
    if(serverType == 0 || serverType == ZONESERVER){
        Tidy::global[key].setString(value);
    }

    BUFFER_PROTO_CMD(protocol::ServerRun::SetGlobalParamCommand, send);
    copyStr(send->key, key.c_str(), sizeof(send->key));
    send->valueLen = value.length();
    copyStr(send->value, value.c_str(), PROTO_CMD_CAPACITY(protocol::ServerRun::SetGlobalParamCommand));
    return server->sendCmdToTaskConnByType(serverType, send, send->allSize());
}

void zoServer::dealCore(int sig, siginfo_t *info, void *secret)
{
    normalCloseZone();

    Game::nGameServer::dealCore(sig, info, secret, false);
}

void zoServer::setZoneInfo(const DWORD &zoneID, const char* zoneName, const DWORD gameType)
{
    Game::nGameServer::setZoneInfo(zoneID, zoneName);
    _game_type = gameType;
}

DWORD zoServer::getServerTypeByString(const char *str) const
{
    DWORD num = sizeof(ServerTypeString)/sizeof(ServerTypeString[0]);
    for(DWORD i = 0; i < num; ++ i){
        if(strcmp(ServerTypeString[i].typeStr, str) == 0){
            return ServerTypeString[i].type;
        }
    }
    return MAX_SERVERTYPE;
}

Tidy::nBaseMonitorService* zoServer::createMonitorService()
{
    monitorService = new zoMonitorService();
    return monitorService;
}

bool zoServer::doZeroClock(const bool isNormalClocker, const DWORD ringTimes)
{
    userService->dealZeroClock(isNormalClocker, ringTimes);
    return true;
}

namespace {
    template<typename T>
        struct DoZeroClockExec {
            const bool _is_normal_clocker;
            DWORD _ring_times;
            DoZeroClockExec(const bool isNormalClocker, const DWORD ringTimes) : _is_normal_clocker(isNormalClocker), _ring_times(ringTimes) {
            }   
            bool exec(T* entity) {
                return entity->doZeroClock(_is_normal_clocker, 1); 
            }   
        };  
};

void zoServer::checkAllPastClock() 
{
    Tidy::nClocker zeroClock(ONE_DAY/1000, ONE_DAY/1000, getLastCloseTimeInSec());
    DWORD times = zeroClock.getContinueRingTimes(server->time());
    if(times > 0) {
        doZeroClock(false, times);
    }   
}
