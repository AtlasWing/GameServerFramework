#ifndef _DATA_SERVER_H_
#define _DATA_SERVER_H_

#include "game/nGameServer.h"

namespace Tidy{
    class nDBConnectionPool;
};

class dtUserService;
class dtLoginService;
class dtMessageService;
class dtSerializeService;
class dtMailService;
class TableManager;

namespace protocol{
    namespace base{
        struct LoginCommandBase;
        struct ServerRunCommmandBase;
    };
};

class dtServer : public Game::nGameServer
{
    public:
        dtServer();
        virtual ~dtServer();

    public:
        bool doServerTimetick();

    public:
        Tidy::nDBConnectionPool* getDBConnPool() const;

    private:
        virtual bool startGameServer();
        virtual bool closeGameServer();
        virtual bool finalGameServer();
        virtual bool dealServerStartComplete(const DWORD serverID, const DWORD serverType);
        virtual Tidy::nBaseMessageService* createMessageService();
        virtual void dealServerEvent(const DWORD eventID, const char *eventParam, const WORD eventParamLen);
        virtual void dealConnectSuccess(Tidy::nServerConn* conn);
        virtual void dealConnectClose(Tidy::nServerConn* conn);
        virtual void dealDBClosed(Tidy::nDBConnection *connection);
        virtual void dealZoneStart(const protocol::admin::ZoneData *zoneData);
        virtual void dealZoneClose(const protocol::admin::ZoneData *zoneData);
        virtual void dealCore(int sig, siginfo_t *info, void *secret);

    public:
        bool parseForwardCommand(Tidy::nServerConn* serverConn, protocol::base::ForwardCommandBase* baseCmd, const unsigned int len);
        bool parseServerRunCommand(Tidy::nServerConn* serverConn, const protocol::base::ServerRunCommandBase *nullCommand, const unsigned int len);
        bool parseSerializeCommand(Tidy::nServerConn* serverConn, const protocol::base::SerializeCommandBase *nullCommand, const unsigned int len);

    private:
        bool initConfig();
        bool initService();
        bool finalService();
    public:
        bool createDBConnection(Tidy::nDBConnectionPool *dbConnPool, const char *configName, const DWORD connectionID);

    public:
        bool checkDBVersion(const DWORD connectionID, const DWORD versionNum);

    public:
        dtLoginService* loginService;
        dtUserService* userService;
        dtMessageService* messageService;
        dtSerializeService* serializeService;
        dtMailService* mailService;
        TableManager *tableManager;

    private:
        Tidy::nDBConnectionPool *_db_conn_pool;
};

extern dtServer* server;

#endif
