#ifndef _N_GAME_SERVER_H_
#define _N_GAME_SERVER_H_

#include "protocol/commonType.h"
#include "protocol/serverRunCommon.h"
#include "protocol/serverRunCommand.h"

#include "base/nBaseServer.h"
#include "base/nRWLock.h"
#include "base/nTimer.h"
#include "base/nObj.h"
#include "base/nServerConn.h"
#include "base/nServerConnManager.h"

namespace protocol {
	struct NullProtoCommand;
	namespace ServerRun {
		struct ServerEntity;
	};
	namespace admin{
		struct ZoneData;
	};
	namespace base{
		struct ServerRunCommandBase;
	};
};

namespace Game
{
	class nBaseGameMessageService;

	struct ServerEntityCompare {
		bool operator()(const protocol::ServerRun::ServerEntity& data_0, const protocol::ServerRun::ServerEntity& data_1) const{
			return data_0.serverType < data_1.serverType || (data_0.serverType == data_1.serverType && data_0.serverID < data_1.serverID);
		}
	};


	class nGameServer : public Tidy::nBaseServer
	{
		protected:
			nGameServer(const char* serverName, const char* logPrefixName, const DWORD serverType, const bool queueCmd);
			virtual ~nGameServer();

		public:
			virtual bool addTaskConnID(const Tidy::nTaskConn* conn);
			virtual bool removeTaskConnID(const Tidy::nTaskConn* conn);
			virtual bool addTaskConn(Tidy::nTaskConn* conn);
			virtual bool removeTaskConn(Tidy::nTaskConn* conn);
			virtual bool addSyncTaskConn(Tidy::nTaskConn* conn);
			virtual bool removeSyncTaskConn(Tidy::nTaskConn* conn);
			virtual Tidy::nTaskConn* getSyncTaskConn(const DWORD taskConnID);
			virtual bool addClientConn(Tidy::nClientConn* conn);
			virtual bool removeClientConn(Tidy::nClientConn* conn);
			virtual void serverConnectSuccess(Tidy::nServerConn* conn);
			virtual void serverConnectClose(Tidy::nServerConn* conn);
			virtual bool createSchedule(const DWORD scheduleServerType, const DWORD scheduleServerID, const DWORD eventID, const char *eventParam, const WORD eventParamLen, const SWORD month, const SWORD monthDay, const SWORD weekDay, const BYTE hour, const BYTE minute, const BYTE sec);

		public:
			virtual bool verifyTaskConn(Tidy::nTaskConn* conn, const void* packet, const DWORD nCmdLen);
			virtual bool checkClientSyncResult(Tidy::nClientConn* conn, const void* packet, const DWORD len);
			virtual BYTE syncTaskConn(Tidy::nTaskConn* conn, const void* packet = NULL, const DWORD len=0);
            virtual void dealNotifyMeStart();
			virtual bool dealServerStartComplete(const DWORD serverID, const DWORD serverType) = 0;
			virtual bool doServerTimetick() = 0;
			virtual bool loginToServer(Tidy::nServerConn* conn);
			virtual void dealDBClosed(Tidy::nDBConnection *connection);
			virtual void dealAssertFail(const bool needCore, const char *funcName, const char *fileName, const int line, const char *expr, const char * desc);

		public:
			bool sendCmdToZone(const protocol::NullProtoCommand *cmd, const DWORD len) const;
			bool sendCmdToLoop(const protocol::NullProtoCommand *cmd, const DWORD len) const;
			void setServerInfo(const  protocol::ServerRun::ServerEntity &entity);
			bool addServerEntity(const  protocol::ServerRun::ServerEntity &entity);
			void addZoneData(const DWORD num, const protocol::admin::ZoneData *data);
			void removeZoneData(const DWORD zoneID);
			bool checkZoneOpen(const DWORD zoneID) const;
			bool startServerDelayLinkedTest(DWORD charID, DWORD testID) const;

		public:
			void setZoneInfo(const DWORD &zoneID, const char* zoneName);
			const bool checkCloseZone() const;
			void requestCloseZone() const;
			void raiseHupSig();

		public:
			const protocol::ServerRun::ServerEntity *getServerEntityByID(const DWORD ServerID);
			const protocol::ServerRun::ServerEntity *getNextServerEntityByType(const DWORD ServerType, const protocol::ServerRun::ServerEntity **prev);
			const DWORD	getServerEntitySizeByServerType(const DWORD ServerType);
			const DWORD getServerEntityByType(const DWORD serverType, Tidy::vector<const protocol::ServerRun::ServerEntity*> *entityList);
			void getDependServerEntityList(const DWORD serverType, Tidy::vector<const protocol::ServerRun::ServerEntity*> *dependServerList);
			DWORD getServerSizeInSameType() const;
			DWORD getRankInSameType() const;
			template<typename ExecT>
                bool execAllServerEntity(ExecT &exec);
            const char* getInternalIP() const;
            WORD getInternalPort() const;
            const protocol::ServerRun::ServerEntity& getServerInfo() const;
            bool getNeedLoadMap() const;

        public:
            DWORD getTickTimeInMsec() const;
            void setTickTime(const DWORD tickTimeInMsec);


		public:
			Tidy::nServerConn* getZoneConn() const;
			Tidy::nServerConn* getLoopConn() const;
			Tidy::nServerConn* getServerConnByID(const DWORD connID);
			Tidy::nServerConn* getTaskConnByID(const DWORD connID);
			Tidy::nServerConn* getClientConnByID(const DWORD connID);
            Tidy::nServerConn* selectTaskConnByType(const DWORD serverType);
            Tidy::nServerConn* selectConnByType(const DWORD serverType);
			const DWORD getTaskConnSize() const;
			const DWORD getTaskConnSizeByType(const DWORD serverType) const;
			const DWORD getClientConnSizeByType(const DWORD serverType) const;
			bool isClientConn(Tidy::nServerConn *conn) const;
			bool isClientConn(const DWORD serverType) const;
			bool isTaskConn(Tidy::nServerConn *conn) const;
			bool isTaskConn(const DWORD serverType) const;
			Tidy::nServerConn* getTempTaskConnByID(const DWORD connID);
			Tidy::nServerConn* getHttpTaskConnByID(const DWORD connID);
			bool createServerClientConn(const DWORD serverType, const bool waitConnected, const bool needReconnect, const DWORD reconnectTimer);
			bool createServerClientConn(const DWORD serverID, const DWORD serverType, const bool waitConnected, const bool needReconnect, const DWORD reconnectTimer);

		public:
			const char* getZoneName() const;
			const DWORD getZoneID() const;
			const DWORD getLastCloseTimeInSec() const;

		public:
			bool sendCmdToClientConnByID(const DWORD serverID, const void* cmd, const unsigned int len)const;

		public:
			bool sendCmdToClientConnByType(const DWORD serverType, const void* cmd, const unsigned int len)const;
			bool sendCmdToClientConnByTypeExceptOne(const DWORD serverType, const DWORD exceptServerID, const void* cmd, const unsigned int len)const;
			bool sendCmdToTaskConnAll(const void* cmd, const unsigned int len) const;
			bool sendCmdToTaskConnAllExceptOne(const DWORD serverID, const void* cmd, const unsigned int len) const;
			bool sendCmdToTaskConnByID(const DWORD serverID, const void* cmd, const unsigned int len)const;
			bool sendCmdToTaskConnByType(const DWORD serverType, const void* cmd, const unsigned int len)const;
			bool sendCmdToTaskConnByTypeExceptOne(const DWORD serverType, const DWORD exceptServerID, const void* cmd, const unsigned int len)const;

		public:
			template<typename ExecT>
				bool execTaskConnByType(ExecT& exec, const DWORD serverType);

		protected:
			template<typename ExecT>
				bool execEveryTaskConn(ExecT& exec);
			template<typename MessageServiceT>
				MessageServiceT* createGameMessageService(MessageServiceT *messageService);

		protected:
			bool parseServerRunCommand(Tidy::nServerConn* serverConn, const protocol::base::ServerRunCommandBase *command, const unsigned int nCmdLen);

		protected:
			virtual Tidy::nClientConn* createClientConn(const protocol::ServerRun::ServerEntity* serverInfo, const bool needReconnect, const DWORD reconnectTimer, const bool waitConnected) const;
			virtual Tidy::nTaskConn* createTaskConn(const int sock, const struct sockaddr *addr, const Tidy::SocketPortData* listenPortData) const;

		protected:
			bool doGameTick();
			bool isZoneConnected() const;
			void setCloseZone();
			void checkServerClose();
			std::string getUnixSocketPath(const DWORD id, const char *ip, const WORD port) const;
			void clearAllZoneData();
			void sendAllZoneData(const DWORD serverConnID, const BYTE serverType) const;
			bool isServerConnIDAdded(const DWORD serverID) const;
			bool waitForClientConnByType(const WORD serverType);
			bool onServerDelayLinkedTest(const Tidy::nServerConn *conn, const protocol::ServerRun::ServerDelayLinkedTestCommand *command) const;
			bool loadGameConfig();

		protected:
			virtual bool createConnManager();
			virtual void destroyConnManager();
			virtual void dealHupSig();

		private:
			virtual bool initServer();
			virtual bool startServer();
			virtual bool finalServer();
			virtual bool completeStartup();

		private:
			bool connectZone();
			bool disconnectZone();
			bool waitForClientConnByID(const protocol::ServerRun::ServerEntity *serverEntity);
			void waitClientConnSucc();
			void parseServerEvent(const DWORD eventID, const char *eventParam, const WORD eventParamLen);
			void setNeedProfile(const bool needProfile);

		private:
			virtual void dealZoneStart(const protocol::admin::ZoneData *zoneData);
			virtual void dealZoneClose(const protocol::admin::ZoneData *zoneData);

		private:
			virtual bool startGameServer() = 0;
			//关闭服务器,这是在服务器由正常状态转到关闭状态时的接口,不同于finalServer是由关闭状态到销毁状态的接口
			virtual bool closeGameServer() = 0;
			virtual bool finalGameServer() = 0;
			virtual bool dealCompleteStartup();
			virtual void dealConnectSuccess(Tidy::nServerConn* conn) = 0;
			virtual void dealConnectClose(Tidy::nServerConn* conn) = 0;
			virtual Tidy::nBaseMessageService* createMessageService() = 0;
			virtual void dealServerEvent(const DWORD eventID, const char *eventParam, const WORD eventParamLen) = 0;

		private:
			virtual Tidy::nBaseTickService* createTickService();
			virtual void dealClientConnRecycle(Tidy::nClientConn *conn);

		private:
			bool createServerClientConn(const protocol::ServerRun::ServerEntity *serverEntity, const bool waitConnected, const bool needReconnect, const DWORD reconnectTimer);
			bool verifyTelnetTaskConn(Tidy::nTaskConn* conn, const void* packet, const DWORD len);
			bool verifyHttpTaskConn(Tidy::nTaskConn* conn, const void* nullCommand, const DWORD len);
            bool verifyGameTaskConn(Tidy::nTaskConn* conn, const void* packet, int len);
            DWORD getNextTempTaskConnID() const;
            bool bindListen(const Tidy::nListenData &listenData, const protocol::ServerRun::ServerEntity &serverInfo);

		public:
			/// @brief	区信息管理容器类型
			typedef std::map<DWORD,ZoneInfo> ZoneInfoMap;
			/// @brief	区信息管理容器类型迭代
			typedef ZoneInfoMap::iterator ZoneInfoMap_iter;
			/// @brief	区信息管理容器
			ZoneInfoMap zoneinfo;

		public:
			nBaseGameMessageService *gameMessageService;

		private:
			DWORD _zone_id;
			char _zone_name[MAX_NAMESIZE];
			WORD zonePort;
			Tidy::nServerConn *_zone_client;
			Tidy::nServerConn *_client_loop_conn;
			Tidy::nServerConn *_task_loop_conn;
			std::string _unix_socket_file;
			/// @brief	关联服务器信息列表访问互斥体
			Tidy::nRWLock _server_list_lock;
			/// @brief	关联服务器信息列表,保证服务器之间的验证关系,按照serverType,serverID的顺序递增,这样方便了服务器依赖关系
			typedef std::set<protocol::ServerRun::ServerEntity, ServerEntityCompare> ServerEntitySet;
			typedef ServerEntitySet::iterator ServerEntitySetIterator;
			ServerEntitySet _server_entity_list;
			/// @brief	线程检测定时器
			Tidy::nTimer _monitor_timer;

		private:
			Tidy::nServerConnManager *_task_conn_manager;
			Tidy::vector<Tidy::nTaskConn*> _sync_task_conn_container;
			Tidy::nRWLock _sync_task_conn_lock;

			Tidy::nServerConnManager *_client_conn_manager;
			Tidy::nServerConnManager *_temp_task_conn_manager;

		private:
			bool _queue_cmd;

		private:
			bool _close_zone;
			bool _check_can_start;

		private:
			Tidy::set<Tidy::nServerConn*> _dealed_success_connect_set;
			typedef Tidy::map<DWORD, protocol::admin::ZoneData*> ZoneDataMap;
			ZoneDataMap _zone_data_map;
            mutable DWORD _temp_task_conn_id;
            std::string _process_name;
            const protocol::ServerRun::ServerEntity *_server_info;
            bool _need_load_map;
            DWORD _tick_time_in_msec;
			bool _is_in_profile;
	};

	extern nGameServer* gameServer;

	template<typename ExecT>
		bool nGameServer::execAllServerEntity(ExecT &exec)
		{
			for(ServerEntitySetIterator iter = _server_entity_list.begin(); iter != _server_entity_list.end(); ++ iter){
				if(exec.exec(&(*iter)) == false){
					return false;
				}
			}
			return true;
		}

	template<typename ExecT>
		bool nGameServer::execTaskConnByType(ExecT& exec, const DWORD serverType)
		{
			return _task_conn_manager->execConnByType(exec, serverType);
		}

	template<typename ExecT>
		bool nGameServer::execEveryTaskConn(ExecT& exec)
		{
			return _task_conn_manager->execEveryConn(exec);
		}

	template<typename MessageServiceT>
		MessageServiceT* nGameServer::createGameMessageService(MessageServiceT *messageService)
		{
			gameMessageService = messageService;
			return messageService;
		}
};
#endif
