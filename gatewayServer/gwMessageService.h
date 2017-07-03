#ifndef _GATEWAY_MESSAGE_SERVICE_H_
#define _GATEWAY_MESSAGE_SERVICE_H_

#include "game/nGameMessageService.h"

namespace command{
    struct NullUserCommand;
};

namespace protocol {
    struct NullProtoCommand;
    namespace base{
        struct PlatformCommandBase;
    }
}

namespace Tidy{
	class nServerConn;
};
class gwUser;
class gwUserTaskConn;

class gwMessageService : public Game::nGameMessageService<gwMessageService> 
{
    public:
        gwMessageService();
        ~gwMessageService();


    private:
        virtual bool init();
        virtual void final();

    public:
        bool parseServerCommand(Tidy::nServerConn* serverConn, const protocol::NullProtoCommand *command, const DWORD len);

    public:
        bool sendCmdToLogicByID(DWORD serverID, const protocol::NullProtoCommand * cmd, const DWORD len) const;
        bool sendCmdToLogic(const protocol::NullProtoCommand *cmd, const DWORD len) const;
        bool sendCmdToLogic(const gwUser *user, const protocol::NullProtoCommand *cmd, const DWORD len) const;
        bool sendPlatformCommandToServer(Tidy::nServerConn* conn, const protocol::base::PlatformCommandBase* command, const DWORD len) const;
        bool sendCmdToZone(const protocol::NullProtoCommand * cmd, const DWORD) const;

    public:
        void dealConnectSuccess(Tidy::nServerConn* conn);
        void dealConnectClose(Tidy::nServerConn* conn);
		bool doTick();
        bool startService();
        Tidy::nServerConn* getLogicConn() const;

    private:
        bool parseProtocolCmd(Tidy::nServerConn* serverConn, const protocol::NullProtoCommand *nullCmd, const DWORD len);
        bool parseForwardCommand(Tidy::nServerConn* serverConn, const protocol::base::ForwardCommandBase* nullCommand, const DWORD len);

};
#endif
