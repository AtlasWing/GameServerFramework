#include "protocol/serverRunCommand.h"
#include "protocol/forwardCommand.h"
#include "protocol/adminCommon.h"

#include "base/Tidy.h"
#include "base/nConfigFile.h"
#include "base/nBaseTickService.h"
#include "base/nCommandCtl.h"
#include "base/nSocketPortData.h"
#include "base/nClientConn.h"
#include "base/nTaskConn.h"
#include "base/nDebugger.h"
#include "base/nDBConnection.h"

#include "game/nGameServer.h"
#include "game/nGameMessageService.h"
#include "game/nGameTickService.h"

namespace Game
{
	nGameServer* gameServer = NULL;

	nGameServer::nGameServer(const char* serverName, const char* logPrefixName, const DWORD serverType, const bool queueCmd) : 
		Tidy::nBaseServer(serverName, logPrefixName, serverType, 1, queueCmd), 
		gameMessageService(NULL),
		zonePort(0),
		_zone_client(NULL), 
		_client_loop_conn(NULL),
		_task_loop_conn(NULL),
		_task_conn_manager(NULL),
		_client_conn_manager(NULL),
		_temp_task_conn_manager(NULL),
		_queue_cmd(queueCmd),
		_close_zone(false),
		_check_can_start(false),
        _temp_task_conn_id(0),
        _server_info(NULL),
        _need_load_map(false),
        _tick_time_in_msec(20),
		_is_in_profile(false)
	{
		gameServer = this;
	}

	nGameServer::~nGameServer()
	{
		gameServer = NULL;
		for(Tidy::map<DWORD, protocol::admin::ZoneData*>::iterator iter = _zone_data_map.begin(); iter != _zone_data_map.end(); ++ iter){
			SAFE_DELETE(iter->second);
		}
		_zone_data_map.clear();
	}

	Tidy::nTaskConn* nGameServer::createTaskConn(const int sock, const struct sockaddr *addr, const Tidy::SocketPortData* listenPortData) const
	{
		Tidy::nTaskConn* conn = NULL;
		if(_queue_cmd == true) {
			conn = new Tidy::nTaskConnQueue(sock, addr, listenPortData);
		}
		else {
			conn = new Tidy::nTaskConnNoQueue(sock, addr, listenPortData);
		}

        if(listenPortData->taskConnType == TELNETCLIENT) {
            Tidy::nServerConn *serverConn = conn->castAsServerConn();
            DWORD tempTaskConnID = getNextTempTaskConnID();
            serverConn->setServerConnInfo(tempTaskConnID, TELNETCLIENT, getServerTypeString(TELNETCLIENT));
        }
        else if(listenPortData->taskConnType == HTTPCLIENT) {
            Tidy::nServerConn *serverConn = conn->castAsServerConn();
            DWORD tempTaskConnID = getNextTempTaskConnID();
            serverConn->setServerConnInfo(tempTaskConnID, HTTPCLIENT, getServerTypeString(HTTPCLIENT));
        }
        else{
            assert_debug(listenPortData->taskConnType == getServerType(), "%u", listenPortData->taskConnType);
        }

		return conn;
	}

	Tidy::nClientConn* nGameServer::createClientConn(const protocol::ServerRun::ServerEntity* serverInfo, const bool needReconnect, const DWORD reconnectTimer, const bool waitConnected) const
	{
		std::string unixSocketPath;
		// 注意:对于loop连接,一定要用UNIX DOMAIN SOCKET.因为对于GatewayServer,如果用IP和端口,就无法区分loop连接和客户端连接了.
		if((serverInfo->serverType == getServerType() && serverInfo->serverID == getServerID()) || (serverInfo->hasUnixSocket == true && checkLocalIP(serverInfo->internalIP) == true)){
			unixSocketPath = getUnixSocketPath(serverInfo->serverID, serverInfo->internalIP, serverInfo->internalPort);
		}
		DWORD tickTimerInMsec = ONE_MIN;
#if (defined(_DEBUG_NYS)) || (defined(_DEBUG_YOULONG))
		tickTimerInMsec = 10 * ONE_MIN;
#endif
		Tidy::nConnectServerData connectServerData(serverInfo->serverID, serverInfo->serverType, serverInfo->pstrName, serverInfo->internalIP, serverInfo->internalPort, unixSocketPath.c_str(), serverInfo->zipMethod, needReconnect, reconnectTimer, waitConnected, serverInfo->checkHeartTick, tickTimerInMsec, &Tidy::baseCommandCtl);
		if(_queue_cmd == true){
			return new Tidy::nClientConnQueue(connectServerData);
		}
		else{
			return new Tidy::nClientConnNoQueue(connectServerData);
		}

		return NULL;
	}

	bool nGameServer::addTaskConnID(const Tidy::nTaskConn* conn)
	{
		Tidy::nServerConn *serverConn = conn->castAsServerConn();
#ifdef _DEBUG_NYS
		debug("addTaskConnID = [%u]", conn->castAsServerConn()->getID());
#endif
		if(isTempTaskConn(serverConn->getServerType()) == true) {
			return true;
		}
		else if(serverConn->getServerType() == getServerType() && serverConn->getServerID() == getServerID()){
			return true;
		}
		else {
			assert_debug(isGameServer(serverConn->getServerType()) || serverConn->getServerType() == USERCLIENT, "新服务器类型");
			return _task_conn_manager->addConnID(serverConn->getID());
		}
		return true;
	}

	bool nGameServer::removeTaskConnID(const Tidy::nTaskConn* conn)
	{
#ifdef _DEBUG_NYS
		debug("removeTaskConnID = [%u]", conn->castAsServerConn()->getID());
#endif
		Tidy::nServerConn *serverConn = conn->castAsServerConn();
		if(isTempTaskConn(serverConn->getServerType()) == true) {
			return true;
		}
		else if(serverConn->getServerType() == getServerType() && serverConn->getServerID() == getServerID()){
			return true;
		}
		else {
			assert_debug(isGameServer(serverConn->getServerType()) || serverConn->getServerType() == USERCLIENT, "新服务器类型");
			_task_conn_manager->removeConnID(serverConn->getID());
		}
		return true;
	}

	bool nGameServer::addTaskConn(Tidy::nTaskConn* conn)
	{
		Tidy::nServerConn *serverConn = conn->castAsServerConn();
		if(isTempTaskConn(serverConn->getServerType()) == true){
			return _temp_task_conn_manager->addConn(serverConn);
		}
		else if(serverConn->getServerType() == getServerType() && serverConn->getServerID() == getServerID()){
			assert_debug(_task_loop_conn == NULL, "不可能有值");
			_task_loop_conn = serverConn;
			debug("回环连接task建立");
		}
		else{
			assert_debug(isGameServer(conn->castAsServerConn()->getServerType()) || conn->castAsServerConn()->getServerType() == USERCLIENT, "新服务器类型");
			return _task_conn_manager->addConn(conn->castAsServerConn());
		}
		return true;
	}

	bool nGameServer::removeTaskConn(Tidy::nTaskConn* conn)
	{
		Tidy::nServerConn *serverConn = conn->castAsServerConn();
		if(isTempTaskConn(serverConn->getServerType()) == true){
			_temp_task_conn_manager->removeConn(serverConn);
		}
		else if(serverConn->getServerType() == getServerType() && serverConn->getServerID() == getServerID()){
			assert_debug(_task_loop_conn != NULL, "有值");
			_task_loop_conn = NULL;
			debug("回环连接task关闭");
		}
		else{
			assert_debug(isGameServer(serverConn->getServerType()) || serverConn->getServerType() == USERCLIENT, "新服务器类型");
			_task_conn_manager->removeConn(serverConn);
		}
		return true;
	}

	bool nGameServer::addSyncTaskConn(Tidy::nTaskConn* conn)
	{
		Tidy::nScopeWRLock lock(_sync_task_conn_lock);
		_sync_task_conn_container.push_back(conn);
		return true;
	}

	bool nGameServer::removeSyncTaskConn(Tidy::nTaskConn* conn)
	{
		Tidy::nScopeWRLock lock(_sync_task_conn_lock);
		for(Tidy::vector<Tidy::nTaskConn*>::iterator iter = _sync_task_conn_container.begin(); iter != _sync_task_conn_container.end(); ++ iter)
		{
			if((*iter) == conn)
			{
				_sync_task_conn_container.erase(iter);
				return true;
			}
		}
		return false;
	}

	Tidy::nTaskConn* nGameServer::getSyncTaskConn(const DWORD taskConnID)
	{
		Tidy::nScopeRDLock lock(_sync_task_conn_lock);
		for(Tidy::vector<Tidy::nTaskConn*>::iterator iter = _sync_task_conn_container.begin(); iter != _sync_task_conn_container.end(); ++ iter)
		{
			if((*iter)->castAsServerConn()->getServerID() == taskConnID)
			{
				return (*iter);
			}
		}

		return NULL;
	}

	bool nGameServer::addClientConn(Tidy::nClientConn* conn)
	{
		Tidy::nServerConn* serverConn = conn->castAsServerConn();
		if(_client_conn_manager->addConnID(serverConn->getID()) == false){
			return false;
		}
		else if(serverConn->getServerType() == getServerType() && serverConn->getServerID() == getServerID()){
			assert_debug(_client_loop_conn == NULL, "不可能有值");
			_client_loop_conn = serverConn;
			debug("回环连接client建立");
		}
		else if(_client_conn_manager->addConn(serverConn) == false){
			return false;
		}
		return true;
	}

	bool nGameServer::removeClientConn(Tidy::nClientConn* conn)
	{
		Tidy::nServerConn* serverConn = conn->castAsServerConn();
		if(serverConn == _client_loop_conn){
			_client_loop_conn = NULL;
			debug("回环连接client关闭");
		}
		else{
			_client_conn_manager->removeConnID(serverConn->getID());
			_client_conn_manager->removeConn(serverConn);
		}
		return true;
	}

	bool nGameServer::initServer()
	{
		if(loadGameConfig() == false){
			assert_fail("加载游戏配置失败");
			return false;
		}

		if(initLogger() == false){
			return false;
		}

		if(initDaemon() == false){
			return false;
		}

		protocol::ServerRun::ServerEntity zoneServerEntity(Tidy::global["ZONE_SERVER_ID"].getWord(), ZONESERVER, "zoneServer", Tidy::global["ZONE_SERVER_IP"].getCString(), Tidy::global["ZONE_SERVER_PORT"].getWord(), false, false, 0, false, false, 0, 0, false);
		addServerEntity(zoneServerEntity);

		setInDebugMode(Tidy::global["NEED_IN_DEBUG_MODE"].getDword() > 0);
		setNeedCore(Tidy::global["NEED_CREATE_CORE"].getDword() > 0);
		return true;
	}

	bool nGameServer::loadGameConfig()
	{
		const Tidy::nAny &configDirParam = Tidy::global["config"];
		std::string configDir("./config");
		if(configDirParam.empty() == false){
			configDir = configDirParam.getString();
			if(configDir.at(configDir.length() - 1) == '/'){
				configDir.erase(configDir.length() - 1);
			}
		}

		// 这个map每执行一次会创建一次新的,有内存泄露,但是为了线程安全,这个是值得的
		Tidy::nParamMap *globalConfigMap = new Tidy::nParamMap();
		*globalConfigMap = Tidy::global;
		std::string zoneConfigFile(configDir);
		zoneConfigFile.append("/ZoneConfig.xml");
		Tidy::nConfigFile zoneConf(zoneConfigFile.c_str());
		if(zoneConf.parse(getServerName(), globalConfigMap) == false) {
			return false;
		}

		std::string gameConfigFile("./config/GameConfig.xml");
		Tidy::nConfigFile gameConf(gameConfigFile.c_str());
		if(gameConf.parse(getServerName(), globalConfigMap) == false){
			return false;
		}

		Tidy::nConfigFile funcConf("./config/CommonFuncConfig.xml");
		if(funcConf.parse(getServerName(), globalConfigMap) == false){
			return false;
		}
		// 再调用一次zoneConf.parse是为了用ZoneConfig.xml的配置覆盖commonFuncConfig,GameConfig的配置
		if(zoneConf.parse(getServerName(), globalConfigMap) == false) {
			return false;
		}
        
		Tidy::global.swap(*globalConfigMap);

        assert_debug(Tidy::global["TICK_TIME_IN_MSEC"].getDword() > 0, "");
        setTickTime(Tidy::global["TICK_TIME_IN_MSEC"].getDword());

        Tidy::nLogger *logger = getLogger();
		logger->setLogLevel(Tidy::global["LOG_LEVEL"].getCString());

		if(Tidy::global["SERVER_LOG_DIR"].empty() == true){
			Tidy::global.set("SERVER_LOG_DIR", Tidy::global["SERVER_RUN_DATA_DIR"].getString());
		}
		return true;
	}

    bool nGameServer::startServer()
    {
        TEST_TIME_INIT();
        TEST_TIME_BEGIN();
        if(connectZone() == false) {
            return false;
        }
        TEST_TIME_END(debug, "服务器启动过程connectZone");

        if(startGameServer() == false) {
            return false;
        }

        return true;
    }

	bool nGameServer::completeStartup()
	{
		while(_check_can_start == false && getServerType() != ZONESERVER && isTerminate() == false){
			Tidy::nThread::msleep(100);
			if(_zone_client == NULL){
				return false;
			}
		}

		if(_zone_client != NULL){
			waitClientConnSucc();

			protocol::ServerRun::ServerStartupComplete send;
			if(_zone_client->sendCmd(&send, sizeof(send)) == false){
				return false;
			}
		}

		createServerClientConn(_server_info, false, false, 0);

		info("[%u, %s] 启动...完成", this->getServerID(), this->getServerName());

		// 在这里检查性能分析配置,避免启动过程对性能分析数据的影响,并且可以获取serverID 暂时关闭性能分析
		//baseMonitorService->setNeedProfile(Tidy::global["OPEN_PROFILE"].getDword() > 0, Tidy::global["SERVER_LOG_DIR"].getCString());
		return true;
	}

	bool nGameServer::dealCompleteStartup()
	{
		return true;
	}

	void nGameServer::dealHupSig()
	{
		info("接收到init信号!");
		loadGameConfig();
		//baseMonitorService->setNeedProfile(Tidy::global["OPEN_PROFILE"].getDword() > 0, Tidy::global["SERVER_LOG_DIR"].getCString());
	}

	void nGameServer::dealDBClosed(Tidy::nDBConnection *connection)
	{
		info("数据库连接关闭 %s", connection->getUrl()->getUrl().c_str());
	}

	void nGameServer::dealAssertFail(const bool needCore, const char *funcName, const char *fileName, const int line, const char *expr, const char * desc)
	{
		Tidy::nBaseServer::dealAssertFail(needCore, funcName, fileName, line, expr, desc);
	}

	bool nGameServer::createConnManager()
	{
		if(_queue_cmd == true) {
			_task_conn_manager = new Tidy::nQueueServerConnManager();
			_client_conn_manager = new Tidy::nQueueServerConnManager();
			_temp_task_conn_manager = new Tidy::nQueueServerConnManager();
		}
		else {
			_task_conn_manager = new Tidy::nNotQueueServerConnManager();
			_client_conn_manager = new Tidy::nNotQueueServerConnManager();
			_temp_task_conn_manager = new Tidy::nNotQueueServerConnManager();
		}

		return true;
	}

	bool nGameServer::connectZone()
	{
		if(getServerType() == ZONESERVER) {
			return true;
		}

		if(createServerClientConn(ZONESERVER, true, false, 300000) == false) {
			return false;
		}
       
		return true;
	}

	bool nGameServer::disconnectZone()
	{
		//走正常的nClientConn回收的流程
		assert_debug(_zone_client == NULL, "");
		return true;
	}

	void nGameServer::destroyConnManager()
	{
		SAFE_DELETE(_task_conn_manager);
		SAFE_DELETE(_client_conn_manager);
		SAFE_DELETE(_temp_task_conn_manager);
	}

	bool nGameServer::finalServer()
	{
		finalGameServer();

		disconnectZone();
		if(_unix_socket_file.empty() == false){
			unlink(_unix_socket_file.c_str());
		}

		return true;
	}

	bool nGameServer::sendCmdToZone(const protocol::NullProtoCommand *cmd, const DWORD len) const
	{
		if(_zone_client == NULL){
			return false;
		}
		return _zone_client->sendCmd(cmd, len);
	}

	bool nGameServer::sendCmdToLoop(const protocol::NullProtoCommand *cmd, const DWORD len) const
	{
		if(_client_loop_conn == NULL){
			return false;
		}
		return _client_loop_conn->sendCmd(cmd, len);
	}

	void nGameServer::setServerInfo(const protocol::ServerRun::ServerEntity &serverInfo)
	{
		Tidy::nBaseServer::setServerInfo(serverInfo.serverID);
        _server_info = &serverInfo;
        _process_name = getProcessName();
        _process_name.append(" -id=");
        _process_name.append(Tidy::nAny(getID()).getString());
        if(serverInfo.internalPort != 0){
            Tidy::nListenData listenData(getServerType(), eNetType_None, serverInfo.internalIP, serverInfo.internalPort);
            if(bindListen(listenData, serverInfo) == false){
                assert_fail("");
                return;
            }
        }
        for(DWORD i = 0; i < serverInfo.listenDataNum; ++ i){
            Tidy::nListenData listenData(serverInfo.listenDataList[i].taskConnType, serverInfo.listenDataList[i].netType, serverInfo.listenDataList[i].ip, serverInfo.listenDataList[i].port);
            if(serverInfo.listenDataList[i].taskConnType != USERCLIENT){
                if(bindListen(listenData, serverInfo) == false){
                    assert_fail("绑定监听失败");
                    return;
                }
            }
        }
		_unix_socket_file = getUnixSocketPath(getID(), serverInfo.internalIP, serverInfo.internalPort);
		if(_unix_socket_file.empty() == false){
			unlink(_unix_socket_file.c_str());
			if(addUnixServer(getServerType(), _unix_socket_file.c_str(), serverInfo.zipMethod, serverInfo.checkHeartTick, serverInfo.heartTickTimerInMsec, &Tidy::baseCommandCtl) == true){
				const_cast<protocol::ServerRun::ServerEntity*>(&serverInfo)->hasUnixSocket = true;
			}
			else{
				assert_fail("创建UnixSocket失败");
			}
		}

		// 设置进程名字,方便调试和监控
		setProcessName(_process_name.c_str());
	}

	bool nGameServer::addServerEntity(const protocol::ServerRun::ServerEntity &entity)
	{
		Tidy::nScopeWRLock lock(_server_list_lock);
		ServerEntitySetIterator it = _server_entity_list.find(entity);
		if(it != _server_entity_list.end()) {
			assert_debug(it->serverType == entity.serverType && it->serverID == entity.serverID, "");
			//因为没有改变Key值,所以可以直接赋值
			*const_cast<protocol::ServerRun::ServerEntity*>(&(*it)) = entity;
		}
		else {
			_server_entity_list.insert(entity);
		}
		return true;
	}

	const protocol::ServerRun::ServerEntity *nGameServer::getServerEntityByID(const DWORD serverID)
	{
		const protocol::ServerRun::ServerEntity *ret = NULL;
		ServerEntitySetIterator it;
		Tidy::nScopeRDLock lock(_server_list_lock);
		for(it = _server_entity_list.begin(); it != _server_entity_list.end(); ++it) {
			if (serverID == it->serverID) {
				ret = &(*it);
				break;
			}
		}
		return ret;
	}

	const DWORD nGameServer::getServerEntityByType(const DWORD serverType, Tidy::vector<const protocol::ServerRun::ServerEntity*> *entityList)
	{
		Tidy::nScopeRDLock lock(_server_list_lock);
		for(ServerEntitySetIterator iter = _server_entity_list.begin(); iter != _server_entity_list.end(); ++iter) {
			if (serverType == iter->serverType) {
				entityList->push_back(&(*iter));
			}
		}
		return entityList->size();
	}

	const DWORD	nGameServer::getServerEntitySizeByServerType(const DWORD serverType)
	{
		DWORD	count=0;
		ServerEntitySetIterator it;
		Tidy::nScopeRDLock lock(_server_list_lock);
		for(it = _server_entity_list.begin(); it != _server_entity_list.end(); it++) {
			if (serverType == it->serverType)	count++;
		}
		return count;	
	}

	const protocol::ServerRun::ServerEntity *nGameServer::getNextServerEntityByType(const DWORD serverType, const protocol::ServerRun::ServerEntity **prev)
	{
		const protocol::ServerRun::ServerEntity *ret = NULL;
		bool found = false;
		ServerEntitySetIterator it;
		Tidy::nScopeRDLock lock(_server_list_lock);
		for(it = _server_entity_list.begin(); it != _server_entity_list.end(); ++it) {
			if(serverType == it->serverType) {
				if(NULL == prev || (*prev) == NULL || found) {
					ret = &(*it);
					break;
				}
				else if (found == false && (*prev)->serverID == it->serverID) {
					found = true;
				}
			}
		}
		return ret;
	}

	void nGameServer::getDependServerEntityList(const DWORD serverType, Tidy::vector<const protocol::ServerRun::ServerEntity*> *dependServerList)
	{
		ServerEntitySetIterator it;
		Tidy::nScopeRDLock lock(_server_list_lock);
		for(it = _server_entity_list.begin(); it != _server_entity_list.end(); it++) {
			//注意,不要在这里把logicServer排除出去,因为会导致NotifyOtherWaitStartCommand的流程少了logicServer,从而在连接logicServer时找不到GatewayServer的ServerEntity.
			if (serverType > it->serverType && it->serverType != ZONESERVER) {
				dependServerList->push_back(&(*it));
			}
		}
	}

	DWORD nGameServer::getServerSizeInSameType() const
	{
		return _server_info->serverNumInSameType;
	}

	DWORD nGameServer::getRankInSameType() const
	{
		return _server_info->rankInSameType;
	}

	void nGameServer::addZoneData(const DWORD num, const protocol::admin::ZoneData *data)
	{
		for(DWORD i = 0; i < num; ++ i){
			assert_debug(_zone_data_map.find(data[i].zoneID) == _zone_data_map.end(), "重复");
			_zone_data_map[data[i].zoneID] = new protocol::admin::ZoneData(data[i]);
			dealZoneStart(_zone_data_map[data[i].zoneID]);
		}
	}

	void nGameServer::removeZoneData(const DWORD zoneID)
	{
		ZoneDataMap::iterator iter = _zone_data_map.find(zoneID);
		if(iter != _zone_data_map.end()){
			dealZoneClose(iter->second);
			delete(iter->second);
			_zone_data_map.erase(iter);
		}
	}

	void nGameServer::clearAllZoneData()
	{
		for(ZoneDataMap::iterator iter = _zone_data_map.begin(); iter != _zone_data_map.end(); ++ iter){
			dealZoneClose(iter->second);
			delete(iter->second);
		}
		_zone_data_map.clear();
	}

	void nGameServer::sendAllZoneData(const DWORD serverConnID, const BYTE serverType) const
	{
		if(isGameServer(serverType) == false){
			return;
		}

		BUFFER_PROTO_CMD(protocol::ServerRun::AddZoneServerCommand, send);
		if((PROTO_CMD_CAPACITY(protocol::ServerRun::AddZoneServerCommand)/sizeof(send->data[0])) < _zone_data_map.size()){
			assert_fail("越界");
			return;
		}
		send->num = 0;
		for(ZoneDataMap::const_iterator iter = _zone_data_map.begin(); iter != _zone_data_map.end(); ++ iter){
			send->data[send->num] = *(iter->second);
			++ send->num;
		}
		if(send->num > 0){
			sendCmdToTaskConnByID(serverConnID, send, send->allSize());
		}
	}

	bool nGameServer::checkZoneOpen(const DWORD zoneID) const
	{
		return _zone_data_map.find(zoneID) != _zone_data_map.end();
	}

	bool nGameServer::startServerDelayLinkedTest(DWORD charID, DWORD testID) const
	{
		if(getServerType() != ZONESERVER) {
			assert_fail("只有zoneServer可以发起该测试！");
			return false;
		}
		Tidy::nTime curTime;
		protocol::ServerRun::ServerDelayLinkedTestCommand send;
		send.startTimeInMsec = curTime.msec();
		send.charID = charID;
		send.testID = testID;
		onServerDelayLinkedTest(NULL, &send);
		return true;
	}

	bool nGameServer::onServerDelayLinkedTest(const Tidy::nServerConn *conn, const protocol::ServerRun::ServerDelayLinkedTestCommand *command) const
	{
		DWORD index = 0;
		while(true) {
			if(closeSequenceData[index] == getServerType()) {
				break;
			}
			++index;
			if(index >= MAX_SERVERTYPE) {
				assert_fail("无法识别当前服务器的类型");
				return false;
			}
		} 
		// 详细延迟计算
		Tidy::nTime curTime;
		BUFFER_PROTO_CMD(protocol::ServerRun::ServerDelayLinkedTestCommand, send);
		*send = *command;
		if(conn != NULL) { // 若不是首条，则需要计算其延迟，并在infoList中加入新条目
			protocol::ServerRun::ServerDelayTestInfo &info = send->infoList[send->num];
			info.fromServerID = conn->getServerID();
			info.fromServerType = conn->getServerType();
			info.toServerID = getServerID();
			info.toServerType = getServerType();
			info.delayTimeInMsec = curTime.msec() - command->sendTimeInMsec; // 计算从该消息发出到收到的延迟
			++ send->num;
		}
        if(PROTO_CMD_CAPACITY(protocol::ServerRun::ServerDelayLinkedTestCommand)/sizeof(send->infoList[0]) < send->num){
            assert_fail("消息包长度不够");
            return false;
        }
		send->sendTimeInMsec = curTime.msec();
		if(index == 0 || getTaskConnSizeByType(closeSequenceData[index - 1] == 0)) {
			// 到最下层了，返回给Zone
			return sendCmdToZone(send, send->allSize());
		}
		else {
			// 继续向下层发送
			return sendCmdToTaskConnByType(closeSequenceData[index - 1], send, send->allSize());
		}
		assert_fail("unresolved process");
		return false;
	}

	bool nGameServer::doGameTick()
	{
		Tidy::nBaseServer::doBaseTick();

		if(_queue_cmd == true) {
			if(_task_loop_conn != NULL && _task_loop_conn->doTimeTick() == false){
				assert_fail("taskLoopConn 失败");
			}
			if(_client_loop_conn != NULL && _client_loop_conn->doTimeTick() == false){
				assert_fail("clientLoopConn 失败");
			}
			if(_task_conn_manager->doTimeTick() == false) {
				assert_fail("连接管理器失败");
			}
			if(_client_conn_manager->doTimeTick() == false) {
				assert_fail("连接管理器失败");
			}
			if(_temp_task_conn_manager->doTimeTick() == false) {
				assert_fail("连接管理器失败");
			}
		}

		return true;
	}

	void nGameServer::setZoneInfo(const DWORD &zoneID, const char* zoneName)
	{
		this->_zone_id = zoneID;
		copyStr(_zone_name, zoneName, sizeof(_zone_name));
	}

	const char* nGameServer::getZoneName() const
	{
		return _zone_name;
	}

	const DWORD nGameServer::getZoneID() const
	{
		return _zone_id;
	}

	bool nGameServer::verifyTaskConn(Tidy::nTaskConn* conn, const void* packet, const DWORD len)
	{
		if(conn->castAsServerConn()->getServerType() == TELNETCLIENT) {
			return verifyTelnetTaskConn(conn, packet, len);
		}
		if(conn->castAsServerConn()->getServerType() == HTTPCLIENT) {
			return verifyHttpTaskConn(conn, packet, len);
		}

		return packet != NULL && verifyGameTaskConn(conn, packet, len);
	}

	bool nGameServer::verifyGameTaskConn(Tidy::nTaskConn* conn, const void* packet, int len)
	{
		using namespace protocol::ServerRun;

		const protocol::NullProtoCommand * nullCommand = reinterpret_cast<const protocol::NullProtoCommand *>(packet);
		switch(nullCommand->cmd)
		{
			case protocol::base::ServerRunCommandBase::CMD:
				{
					switch(nullCommand->param)
					{
						case ServerConnectCommand::PARAM:
                            {
                                const ServerConnectCommand* command = static_cast<const ServerConnectCommand*>(nullCommand);
                                /*
                                if(command->gameProtoVersion != protocol::ServerRun::gameProtoVersion){
                                    assert_fail("协议号错误");
                                    return false;
                                }
                                */
                                const protocol::ServerRun::ServerEntity *entity = getServerEntityByID(command->serverID);
                                if(entity == NULL) {
                                    debug("连接验证失败,未找到服务器信息");
                                    assert_fail("没有找到服务器信息");
                                    return false;
                                }
                                if(command->serverType != entity->serverType){
                                    debug("连接验证失败,服务器类型不匹配");
                                    assert_fail("服务器类型和ID不匹配");
                                    return false;
                                }
                                conn->castAsServerConn()->setServerConnInfo(command->serverID, command->serverType, getServerTypeString(command->serverType));
                                return true;
							}
							break;
						default:
							{
								assert_fail("unresolved command");
							}
					}
				}
				break;
			default:
				{
					assert_fail("无法处理的消息");
				}
		}

		assert_fail("无法处理的消息");
		return false;
	}

	bool nGameServer::verifyTelnetTaskConn(Tidy::nTaskConn* conn, const void* nullCommand, const DWORD len)
	{
		static DWORD telnetClientConnID = 0;
		++ telnetClientConnID;
		conn->castAsServerConn()->setServerConnInfo(telnetClientConnID, TELNETCLIENT, getServerTypeString(TELNETCLIENT));

		char serverInfo[128];
		const DWORD size = snprintf(serverInfo, sizeof(serverInfo), "服务器信息 :id=[%u], type=[%u]\n", getServerID(), getServerType());
		conn->castAsServerConn()->sendCmd(serverInfo, size+1);

		return true;
	}

	bool nGameServer::verifyHttpTaskConn(Tidy::nTaskConn* conn, const void* nullCommand, const DWORD len)
	{
		return true;
	}

	bool nGameServer::isServerConnIDAdded(const DWORD serverID) const
	{
		return _task_conn_manager->checkConnIDAdded(serverID) == true;
	}

	bool nGameServer::checkClientSyncResult(Tidy::nClientConn* conn, const void* packet, const DWORD len)
	{
		const protocol::NullProtoCommand *nullCommand = reinterpret_cast<const protocol::NullProtoCommand *>(packet);

		using namespace protocol::ServerRun;
		switch(nullCommand->cmd)
		{
			case protocol::base::ServerRunCommandBase::CMD:
				{
					switch(nullCommand->param)
					{
						case ServerSyncOKCommand::PARAM:
							{
								return true;
							}
							break;
						case ServerNotifyMeInitCommand::PARAM:
                            {
                                const ServerNotifyMeInitCommand *command = static_cast<const ServerNotifyMeInitCommand*>(nullCommand);
                                addServerEntity(command->serverInfo);
                                const protocol::ServerRun::ServerEntity *serverInfo = getServerEntityByID(command->serverInfo.serverID);
                                setServerInfo(*serverInfo);
                                if(command->adjustTime != 0){
                                    setAdjustServerTime(command->adjustTime);
                                }
								return true;
							}
							break;
						default:
							{
								assert_fail("unresolved command");
							}
					}
				}
			default:
				{
					assert_fail("unresolved command");
				}
		}
		return false;
	}

	BYTE nGameServer::syncTaskConn(Tidy::nTaskConn* conn, const void* packet, const DWORD len)
	{
		if(isTempTaskConn(conn->castAsServerConn()->getServerType()) == true) {
			return Tidy::nTaskConn::eSyncState_Success;
		}

		protocol::ServerRun::ServerSyncOKCommand send;
		conn->sendTaskCmd(&send, sizeof(send));
		return Tidy::nTaskConn::eSyncState_Success;
	}

    void nGameServer::dealNotifyMeStart()
    {
    }

	bool nGameServer::loginToServer(Tidy::nServerConn* conn)
	{
		BUFFER_PROTO_CMD(protocol::ServerRun::ServerConnectCommand, send);
        send->serverCodeVersion = serverCodeVersion;;
		send->baseImpCodeVersion = baseImpCodeVersion;;
		send->serverID = Tidy::baseServer->getServerID();
		send->serverType = Tidy::baseServer->getServerType();
		const Tidy::ServerRunInfo& runInfo = getRunInfo();
		send->num = runInfo.localIPNum;
		memcpy(send->data, &runInfo.localIPList[0], send->num * MAX_IP_LENGTH);
		if((PROTO_CMD_CAPACITY(protocol::ServerRun::ServerConnectCommand)/sizeof(send->data[0])) < send->num){
			assert_fail("越界");
			return false; 
		}
		return conn->sendCmd(send, send->size());
	}

	void nGameServer::serverConnectSuccess(Tidy::nServerConn* conn)
	{
		Tidy::set<Tidy::nServerConn*>::iterator iter = _dealed_success_connect_set.find(conn);
		if(iter != _dealed_success_connect_set.end()) {
			_dealed_success_connect_set.erase(iter);
			return;
		}
		if(conn->getServerType() == ZONESERVER) {
			_zone_client = conn;
		}
		if(_server_info->hasClosed == true){
			if(isTaskConn(conn) == true){
				if(getServerEntityByID(conn->getServerID()) != NULL){
					dealServerStartComplete(conn->getServerID(), conn->getServerType());
				}
			}
		}
		dealConnectSuccess(conn);
		info("[%u, %s]连接...成功", conn->getID(), conn->getName());
	}

	void nGameServer::serverConnectClose(Tidy::nServerConn* conn)
	{
		if(conn->getServerType() == ZONESERVER && conn == _zone_client){
			_zone_client = NULL;
		}
		dealConnectClose(conn);
		_dealed_success_connect_set.erase(conn);
		info("[%u, %s]连接...关闭", conn->getID(), conn->getName());
	}

	bool nGameServer::createSchedule(const DWORD scheduleServerType, const DWORD scheduleServerID, const DWORD eventID, const char *eventParam, const WORD eventParamLen, const SWORD month, const SWORD monthDay, const SWORD weekDay, const BYTE hour, const BYTE minute, const BYTE sec)
	{
		BUFFER_PROTO_CMD(protocol::ServerRun::AddScheduleEventCommand, send);
		send->scheduleServerType = scheduleServerType;
		send->scheduleServerID = scheduleServerID;
		send->month = month;
		send->monthDay = monthDay;
		send->weekDay = weekDay;
		send->hour = hour;
		send->minute = minute;
		send->sec = sec;
		send->eventID = eventID;
		send->num = eventParamLen;
		if(eventParam != NULL){
			memcpy(send->eventParam, eventParam, eventParamLen);
		}
		if((PROTO_CMD_CAPACITY(protocol::ServerRun::AddScheduleEventCommand)/sizeof(send->eventParam[0])) < send->num){
			assert_fail("越界");
			return false;
		}
		return sendCmdToZone(send, send->allSize());
	}

	bool nGameServer::createServerClientConn(const DWORD serverID, const DWORD serverType, const bool waitConnected, const bool needReconnect, const DWORD reconnectTimer)
	{
		const protocol::ServerRun::ServerEntity *serverEntity = getServerEntityByID(serverID);
		if(serverEntity == NULL){
			error("不能找到数据服务器相关信息,不能连接数据服务器");
			return false;
		}
		assert_debug(serverEntity->serverType == serverType, "[%u, %u]", serverEntity->serverType, serverType);
		return createServerClientConn(serverEntity, waitConnected, needReconnect, reconnectTimer);
	}

	bool nGameServer::createServerClientConn(const DWORD serverType, const bool waitConnected, const bool needReconnect, const DWORD reconnectTimer)
	{
		const protocol::ServerRun::ServerEntity *serverEntity = NULL;
		serverEntity = getNextServerEntityByType(serverType, &serverEntity);
		if(serverEntity == NULL){
			error("不能找到数据服务器相关信息,不能连接数据服务器");
			return false;
		}

		while(serverEntity){
			if(createServerClientConn(serverEntity, waitConnected, needReconnect, reconnectTimer) == false){
				return false;
			}
			serverEntity = getNextServerEntityByType(serverType, &serverEntity);
		}

		return true;
	}

	bool nGameServer::createServerClientConn(const protocol::ServerRun::ServerEntity *serverEntity, const bool waitConnected, const bool needReconnect, const DWORD reconnectTimer)
	{
		Tidy::nClientConn* clientConn = createClientConn(serverEntity, needReconnect, reconnectTimer, waitConnected);
		if(clientConn == NULL){
			return false;
		}

		if(addClientConnToPool(clientConn) == false){
			SAFE_DELETE(clientConn);
			return false;
		}

		if(waitConnected == true){
			if(waitForClientConnByID(serverEntity) == false){
				return false;
			}
			if(_queue_cmd == true) {
				serverConnectSuccess(clientConn->castAsServerConn());
				_dealed_success_connect_set.insert(clientConn->castAsServerConn());
			}
		}

		return true;
	}

	bool nGameServer::waitForClientConnByID(const protocol::ServerRun::ServerEntity *serverEntity)
	{
		while(_client_conn_manager->checkConnIDAdded(serverEntity->serverID) == false) {
			if(isTerminate() == true || (_zone_client == NULL && serverEntity->serverType != ZONESERVER)) {
				return false;
			}
			//setRunning();
			wait(50);
		}

		return true;
	}

	bool nGameServer::waitForClientConnByType(const WORD serverType)
	{
		Tidy::vector<const protocol::ServerRun::ServerEntity*> entityList;
		getServerEntityByType(serverType, &entityList);
		for(DWORD i = 0; i < entityList.size(); ++ i){
			waitForClientConnByID(entityList[i]);
		}

		return true;
	}

	Tidy::nServerConn* nGameServer::getZoneConn() const
	{
		assert_debug(_zone_client != NULL, "");
		return _zone_client;
	}

	Tidy::nServerConn* nGameServer::getLoopConn() const
	{
		assert_debug(_client_loop_conn != NULL, "");
		return _client_loop_conn;
	}

	Tidy::nServerConn* nGameServer::getServerConnByID(const DWORD connID)
	{
		Tidy::nServerConn *conn = NULL;
		conn = _task_conn_manager->getConnByID(connID);
		if(conn == NULL)
		{
			conn = _client_conn_manager->getConnByID(connID);
		}
		return conn;
	}

	Tidy::nServerConn* nGameServer::getTaskConnByID(const DWORD connID)
	{
		return _task_conn_manager->getConnByID(connID);
	}

	Tidy::nServerConn* nGameServer::getClientConnByID(const DWORD connID)
	{
		return _client_conn_manager->getConnByID(connID);
	}

	const DWORD nGameServer::getTaskConnSizeByType(const DWORD serverType) const
	{
		return _task_conn_manager->getConnSizeByType(serverType);
	}

	const DWORD nGameServer::getTaskConnSize() const
	{
		return _task_conn_manager->getSize();
	}

	const DWORD nGameServer::getClientConnSizeByType(const DWORD serverType) const
	{
		return _client_conn_manager->getConnSizeByType(serverType);
	}

	namespace{
		struct SelectConnByTypeExec{
			Tidy::vector<Tidy::nServerConn*> _conn_vec;
			const DWORD _server_conn_type;
			SelectConnByTypeExec(const DWORD serverType) : _server_conn_type(serverType){
			}
			bool exec(Tidy::nServerConn *conn){
				if(conn->getServerType() == _server_conn_type){
					_conn_vec.push_back(conn);
				}
				return true;
			}
		};
	};
	Tidy::nServerConn* nGameServer::selectTaskConnByType(const DWORD serverType)
	{
		SelectConnByTypeExec exec(serverType);
		_task_conn_manager->execAll(exec);
		if(exec._conn_vec.empty() == true){
			return NULL;
		}
		return exec._conn_vec[Tidy::randBetween(static_cast<unsigned int>(0), exec._conn_vec.size() - 1)];
	}

	Tidy::nServerConn* nGameServer::selectConnByType(const DWORD serverType)
	{
		if(getServerType() == LOGICSERVER){
			return _task_loop_conn;
		}
		SelectConnByTypeExec exec(serverType);
		_task_conn_manager->execAll(exec);
		if(exec._conn_vec.empty() == true){
            _client_conn_manager->execAll(exec);
		}
        if(exec._conn_vec.empty() == true){
            return NULL;
        }
        return exec._conn_vec[Tidy::randBetween(static_cast<unsigned int>(0), exec._conn_vec.size() - 1)];
    }

	bool nGameServer::sendCmdToClientConnByID(const DWORD serverID, const void* cmd, const unsigned int len)const
	{
		if(_client_conn_manager == NULL){
			return true;
		}
		return _client_conn_manager->sendCmdToConnByID(serverID, cmd, len);
	}

	bool nGameServer::sendCmdToClientConnByType(const DWORD serverType, const void* cmd, const unsigned int len)const
	{
		return _client_conn_manager->sendCmdToConnByType(serverType, cmd, len);
	}

	bool nGameServer::sendCmdToClientConnByTypeExceptOne(const DWORD serverType, const DWORD exceptServerID, const void* cmd, const unsigned int len)const
	{
		if(_client_conn_manager == NULL){
			return true;
		}
		return _client_conn_manager->sendCmdToConnByTypeExceptOne(serverType, exceptServerID, cmd, len);
	}

	bool nGameServer::sendCmdToTaskConnByID(const DWORD serverID, const void* cmd, const unsigned int len)const
	{
		if(_task_conn_manager == NULL){
			return true;
		}
		return _task_conn_manager->sendCmdToConnByID(serverID, cmd, len);
	}

	bool nGameServer::sendCmdToTaskConnByType(const DWORD serverType, const void* cmd, const unsigned int len)const
	{
		if(_task_conn_manager == NULL){
			return true;
		}
		return _task_conn_manager->sendCmdToConnByType(serverType, cmd, len);
	}

	bool nGameServer::sendCmdToTaskConnAll(const void* cmd, const unsigned int len)const
	{
		if(_task_conn_manager == NULL){
			return true;
		}
		return _task_conn_manager->sendCmdToAll(cmd, len);
	}

	bool nGameServer::sendCmdToTaskConnAllExceptOne(const DWORD serverID, const void* cmd, const unsigned int len) const
	{
		if(_task_conn_manager == NULL){
			return true;
		}
		return _task_conn_manager->sendCmdToAllExceptOne(serverID, cmd, len);
	}

	bool nGameServer::sendCmdToTaskConnByTypeExceptOne(const DWORD serverType, const DWORD exceptServerID, const void* cmd, const unsigned int len)const
	{
		if(_task_conn_manager == NULL){
			return true;
		}
		return _task_conn_manager->sendCmdToConnByTypeExceptOne(serverType, exceptServerID, cmd, len);
	}

	const DWORD nGameServer::getLastCloseTimeInSec() const
	{
		return _server_info->lastServerCloseTimeInSec;
	}

	bool nGameServer::isZoneConnected() const
	{
		return _zone_client != NULL || getServerType() == ZONESERVER;
	}

	void nGameServer::setCloseZone()
	{
		_close_zone = true;
	}

	const bool nGameServer::checkCloseZone() const
	{
		return _close_zone == true;
	}

	void nGameServer::checkServerClose()
	{
		if(checkCloseZone() == false || isTerminate() == true)
		{
			return;
		}

		static DWORD index = 0;
		if(index == 0 || getTaskConnSizeByType(closeSequenceData[index - 1]) == 0)
		{
			if(closeSequenceData[index] == ZONESERVER)
			{
				closeGameServer();
				terminate(Tidy::eServerCloseReason_Normal);
			}
			else
			{
				protocol::ServerRun::CloseServerCommand send;
				sendCmdToTaskConnByType(closeSequenceData[index], &send, sizeof(send));
				++ index;
			}
		}
	}

	std::string nGameServer::getUnixSocketPath(const DWORD id, const char *ip, const WORD port) const
	{
		char unixSocketPath[256];
		snprintf(unixSocketPath, sizeof(unixSocketPath), "/tmp/server_%s_%u.sock", ip, port);
		return unixSocketPath;
	}

	void nGameServer::waitClientConnSucc()
	{
		if(getServerType() == ZONESERVER){
			return;
		}
		for(ServerEntitySetIterator iter = _server_entity_list.begin(); iter != _server_entity_list.end(); ++ iter){
			// 等待依赖的服务器连接建立
			if((*iter).serverType < gameServer->getServerType()){
				if(waitForClientConnByID(&(*iter)) == false){
					break;
				}
			}
		}
	}

	bool nGameServer::isTaskConn(Tidy::nServerConn *conn) const
	{
		return isTaskConn(conn->getServerType());
	}

	bool nGameServer::isClientConn(Tidy::nServerConn *conn) const
	{
		return isClientConn(conn->getServerType());
	}

	bool nGameServer::isTaskConn(const DWORD serverType) const
	{
		return getServerType() < serverType;
	}

	bool nGameServer::isClientConn(const DWORD serverType) const
	{
		return getServerType() > serverType;
	}

	void nGameServer::requestCloseZone() const
	{
		this->info("请求关闭");
		protocol::ServerRun::RequestCloseZoneCommand send;
		gameServer->sendCmdToZone(&send, sizeof(send));
	}

	void nGameServer::raiseHupSig()
	{
		setNeedCheckHup();
	}

	void nGameServer::dealZoneStart(const protocol::admin::ZoneData *zoneData)
	{
	}

	void nGameServer::dealZoneClose(const protocol::admin::ZoneData *zoneData)
	{
	}

	Tidy::nServerConn* nGameServer::getTempTaskConnByID(const DWORD connID)
	{
		return _temp_task_conn_manager->getByID(connID);
	}

	Tidy::nServerConn* nGameServer::getHttpTaskConnByID(const DWORD connID)
	{
		return getTempTaskConnByID(connID);
	}

	void nGameServer::parseServerEvent(const DWORD eventID, const char *eventParam, const WORD eventParamLen)
	{
		if(eventID == protocol::ServerRun::eServerEvent_ReloadConfig){
			setNeedCheckHup();
			debug("Server 重新加载配置完成");
		}
        else if(eventID == protocol::ServerRun::eServerEvent_Analyze){
            setNeedAnalyze();
        }
		dealServerEvent(eventID, eventParam, eventParamLen);
	}

    DWORD nGameServer::getNextTempTaskConnID() const
    {
        ++ _temp_task_conn_id;
        if(_temp_task_conn_id == 0){
            _temp_task_conn_id = 1;
        }
        return _temp_task_conn_id;
    }

    bool nGameServer::bindListen(const Tidy::nListenData &listenData, const protocol::ServerRun::ServerEntity &serverInfo)
    {
        bool result = false;
        switch(listenData.connType){
            case TELNETCLIENT:
                {
                    result = addTcpServer(listenData, Tidy::eZipMethod_None, false, 0, false, &Tidy::telnetCommandCtl);
                    _process_name.append(" -telPort=");
                    _process_name.append(Tidy::nAny(listenData.bindPort).getString());
                };
                break;
            case HTTPCLIENT:
                {
                    result = addTcpServer(listenData, Tidy::eZipMethod_None, false, 0, false, &Tidy::httpCommandCtl);
                    _process_name.append(" -httpPort=");
                    _process_name.append(Tidy::nAny(listenData.bindPort).getString());
                }
                break;
            default:
                {
                    result = addTcpServer(listenData, serverInfo.zipMethod, serverInfo.checkHeartTick, serverInfo.heartTickTimerInMsec, serverInfo.needQuickAck, &Tidy::baseCommandCtl);                
                }
        }
        return result;
    }

    const protocol::ServerRun::ServerEntity& nGameServer::getServerInfo() const
    {
        return *_server_info;
    }
    bool nGameServer::getNeedLoadMap() const
    {
        return _need_load_map;
    }
    const char* nGameServer::getInternalIP() const
    {
        return _server_info->internalIP;
    }
    WORD nGameServer::getInternalPort() const
    {
        return _server_info->internalPort;
    }

    Tidy::nBaseTickService* nGameServer::createTickService()
    {
        nGameTickService *tickService = new nGameTickService();
        return tickService;
    }

    void nGameServer::setTickTime(const DWORD tickTimeInMsec)
    {
        _tick_time_in_msec = tickTimeInMsec;
    }

    DWORD nGameServer::getTickTimeInMsec() const
    {
        return _tick_time_in_msec;
    }

	void nGameServer::dealClientConnRecycle(Tidy::nClientConn *conn)
	{
		if(_queue_cmd == true) {
			Tidy::nClientConnQueue *clientConn = CLASS_CAST(Tidy::nClientConnQueue, conn);
			if(clientConn->isAddedToManager() == false){
				// 针对在nGameServer::createServerClientConn处waitConnected == true设置的
				if(clientConn->getServerType() == ZONESERVER){
					terminate(Tidy::eServerCloseReason_Normal);
					_zone_client = NULL;
				}
			}
		}
	}

};

int main(int argc, char **argv)
{
	return Tidy::baseServer->main(argc, argv);
}
