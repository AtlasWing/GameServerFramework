#ifndef _ZONE_SERVER_H_
#define _ZONE_SERVER_H_

#include "base/nByteBuffer.h"

#include "game/nGameServer.h"
#include "zoneServer/zoUser.h"

namespace Tidy{
	class nDBConnectionPool;
};

class zoUserService;
class zoMessageService;
class zoMonitorService;
class zoMailService;
class zoConfigService;
class zoTaskConn;
namespace Game{
	class nMiscDataSerialize;
};
class zoLoginService;
class zoScheduleService;
class zoClientTaskConn;
class zoScriptService;
class zoRelationUserService;
class zoMapService;

namespace protocol{
    namespace base{
        struct NullProtoCommand;
        struct ForwardCommandBase;
        struct LoginCommandBase;
    };
};

struct RunningServerCountData {
    BYTE count[MAX_SERVERTYPE];
    RunningServerCountData()
    {
        for(DWORD i = 0; i < MAX_SERVERTYPE; ++i) {
            count[i] = 0;
        }
    }
    BYTE getRunningServerCountByType(DWORD serverType)
    {
        if(serverType >= MAX_SERVERTYPE) {
            assert_fail("错误的服务器类型");
            return 0;
        }
        return count[serverType];
    }
};

class zoServer : public Game::nGameServer
{
    public:
        zoServer();
        virtual ~zoServer();

    public:
        bool doServerTimetick();

    public:
        bool getAllTaskConnEntity(Tidy::vector<protocol::ServerRun::ServerEntity>& taskConnEntityVec);

    public:
        Tidy::nDBConnectionPool* getDBConnPool() const;

    public:
        const bool hasAdminServerConn() const;

    public:
        bool saveCloseZoneTime();
        bool sendServerEvent(const DWORD scheduleServerType, const DWORD scheduleServerID, const DWORD eventID, const char *eventParam, const WORD eventParamLen);
        void addAdjustServerTime(const SDWORD adjustTimeInSec);
        void normalCloseZone();
        void reloadConfig();
        bool setGlobalParam(const DWORD serverType, const std::string &key, const std::string &value);
        DWORD getClientConnID();
        void checkAllPastClock();
        bool doZeroClock(const bool isNormailClocker, const DWORD ringTimes);

    private:
        virtual bool startGameServer();
        virtual bool closeGameServer();
        virtual bool finalGameServer();
        virtual bool dealServerStartComplete(const DWORD serverID, const DWORD serverType);
        virtual Tidy::nTaskConn* createTaskConn(const int sock, const struct sockaddr *addr, const Tidy::SocketPortData* listenPortData) const;
        virtual Tidy::nBaseMessageService* createMessageService();
        virtual Tidy::nBaseMonitorService* createMonitorService();
        virtual bool createConnManager();
        virtual void destroyConnManager();
        virtual void dealHupSig();
        virtual void dealCtrlcSig();
        virtual void dealConnectSuccess(Tidy::nServerConn* conn);
        virtual void dealConnectClose(Tidy::nServerConn* conn);
        virtual void dealServerEvent(const DWORD eventID, const char *eventParam, const WORD eventParamLen);
        virtual BYTE syncTaskConn(Tidy::nTaskConn* conn, const void* packet = NULL, const DWORD len=0);
        virtual bool verifyTaskConn(Tidy::nTaskConn* conn, const void* packet, const DWORD len);
        virtual bool checkClientSyncResult(Tidy::nClientConn* conn, const void* packet, const DWORD len);
        virtual bool loginToServer(Tidy::nServerConn* conn);
        virtual bool addTaskConnID(const Tidy::nTaskConn* conn);
        virtual bool removeTaskConnID(const Tidy::nTaskConn* conn);
        virtual bool addTaskConn(Tidy::nTaskConn* conn);
        virtual bool removeTaskConn(Tidy::nTaskConn* conn);
        virtual void dealCore(int sig, siginfo_t *info, void *secret);

    private:
        bool initConfig();
        bool initService();
        void finalService();

    public:
        bool parseServerRunCommand(Tidy::nServerConn* serverConn, const protocol::base::ServerRunCommandBase *command, const DWORD nCmdLen);
        bool parseSerializeCommand(Tidy::nServerConn* serverConn, const protocol::base::SerializeCommandBase* nullCommand, const unsigned int len);

    private:
        void outputServerDelayResult(zoUser *user, const DWORD delayTimeInMsec, const DWORD fromSvrType, const DWORD fromSvrID, const DWORD toSvrType, const DWORD toSvrID);
        const bool checkServerStarted(const DWORD serverID);
        bool verifyGameTaskConn(Tidy::nTaskConn* conn, const void* packet, const DWORD len);

    private:
        bool sendZoneInfo(Tidy::nServerConn *conn) const;
        bool onReturnServerDelayTest(const Tidy::nServerConn *conn, const protocol::ServerRun::ServerDelayLinkedTestCommand *command);
        void addRunningServerCount(DWORD serverType);
        void removeRunningServerCount(DWORD serverType);
        void dealAllServerStartComplete();
        void dealZoneStartComplete();
        void clearAllZoneData();
        void setZoneInfo(const DWORD &zoneID, const char* zoneName, const DWORD gameType);

    private:
        bool loadZoneConfig();
        bool loadServerList(const DWORD lastServerCloseTimeInSec);
		DWORD getServerTypeByString(const char *str) const;

    public:
        Game::nMiscDataSerialize* miscDataSerialize;
        zoLoginService* loginService;
        zoUserService* userService;
        zoMessageService* messageService;
        zoScheduleService *scheduleService;
        zoMonitorService *monitorService;
        zoMailService *mailService;
        zoRelationUserService *relationUserService;
        zoConfigService *configService;
        zoMapService *mapService;

    private:
		Tidy::nDBConnectionPool *_DB_conn_pool;

    private:
        Tidy::nServerConnManager *_user_task_conn_manager;
        bool _logic_ai_gateway_is_all_running;
        RunningServerCountData _running_server_count_data;
        bool _is_start_complete;
		bool _is_first_connect_admin;
        WORD _client_conn_id;
        DWORD _game_type;
};

extern zoServer* server;
#endif
