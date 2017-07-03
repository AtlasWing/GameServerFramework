#ifndef _GATEWAY_SERVER_H_
#define _GATEWAY_SERVER_H_
#include "game/nGameServer.h"
#include "base/nDBConnectionPool.h"

namespace protocol {
    namespace base {
        struct ServerRunCommandBase;
        struct ForwardCommandBase;
        struct SerializeCommandBase;
    };
};

class gwTaskConn;
class gwUserService;
class gwUserTaskConn;
class gwLoginSessionManager;
class gwLoginService;
class gwConfigService;
class gwMessageService;
class gwTelnetService;
class gwChatService;
class gwFilterService;

namespace Game{
    class nMiscDataSerialize;
};
class gwServer : public Game::nGameServer 
{
    public:
        gwServer();
        virtual ~gwServer();

    public:
        bool doServerTimetick();

    public:
        bool getAllTaskConnEntity(Tidy::vector<protocol::ServerRun::ServerEntity>& taskConnEntityVec);
        gwUserTaskConn* getUserClientConnByID(const DWORD sessionID);
        virtual Tidy::nClientConn* createClientConn(const protocol::ServerRun::ServerEntity* serverInfo, const bool needReconnect, const DWORD reconnectTimer, const bool waitConnected) const;

    private:
        virtual bool startGameServer();
        virtual bool closeGameServer();
        virtual bool finalGameServer();
        virtual bool dealServerStartComplete(const DWORD serverID, const DWORD serverType);
        virtual bool dealCompleteStartup();
        virtual Tidy::nTaskConn* createTaskConn(const int sock, const struct sockaddr *addr, const Tidy::SocketPortData* listenPortData) const;
        virtual void dealConnectSuccess(Tidy::nServerConn* conn);
        virtual void dealConnectClose(Tidy::nServerConn* conn);
        virtual Tidy::nBaseMessageService* createMessageService();
        virtual void dealServerEvent(const DWORD eventID, const char *eventParam, const WORD eventParamLen);
        virtual BYTE syncTaskConn(Tidy::nTaskConn* conn, const void* packet = NULL, const DWORD len=0);
        virtual bool verifyTaskConn(Tidy::nTaskConn* conn, const void* packet, const DWORD len);
        virtual void dealAjustTimeChange();


    public:
        bool parseServerRunCommand(Tidy::nServerConn* serverConn, const protocol::base::ServerRunCommandBase *nullCommand, const unsigned int len);
        bool parseSerializeCommand(Tidy::nServerConn* serverConn, const protocol::base::SerializeCommandBase* nullCommand, const unsigned int len);

    private:
        bool initService();
        void finalService();

    public:
        gwLoginService* loginService;
        gwMessageService* messageService;
        gwConfigService* configService;
        gwTelnetService* telnetService;
        gwUserService* userService;
        gwChatService* chatService;
        gwFilterService* filterService;

};
extern gwServer* server;
#endif
