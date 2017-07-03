#ifndef _DT_MESSAGE_SERVICE_H_
#define _DT_MESSAGE_SERVICE_H_

#include "base/nObjLogger.h"

#include "game/nGameMessageService.h"

namespace Tidy{
	class nServerConn;
};
class dtUser;

namespace protocol{
    namespace base{
        struct NullProtoCommand;
        struct ForwardCommandBase;
    };
};

class dtMessageService : public Game::nGameMessageService<dtMessageService>, public Tidy::nObjLogger<dtMessageService>
{
    public:
        dtMessageService();
        ~dtMessageService();

    public:
        const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;
        Tidy::nLogger* getLogger() const;

    private:
        virtual bool init();
        virtual void final();

	public:
		bool parseServerCommand(Tidy::nServerConn* serverConn, const protocol::NullProtoCommand *nullCommand, const DWORD len);

    public:
        bool sendCmdToZone(const protocol::NullProtoCommand* cmd, const DWORD) const;
        bool sendCmdToLogicAll(const protocol::NullProtoCommand* cmd, const DWORD len) const;
        bool sendCmdToLogicAllExceptOne(const DWORD serverID, const protocol::NullProtoCommand* cmd, const DWORD len) const;
        bool sendCmdToLogicByID(DWORD serverID, const protocol::NullProtoCommand* cmd, const DWORD len) const;
        bool sendCmdToLogic(dtUser* user, const protocol::NullProtoCommand* cmd, const DWORD) const;
        bool sendCmdToGateway(const dtUser *user, const protocol::NullProtoCommand* cmd, const DWORD len) const;
        bool sendCmdToGateway(const protocol::NullProtoCommand* cmd, const DWORD len) const;

    public:
        void dealConnectSuccess(Tidy::nServerConn* conn);
        void dealConnectClose(Tidy::nServerConn* conn);

    private:
        bool parseProtocolCommand(Tidy::nServerConn* serverConn, const protocol::NullProtoCommand *nullCommand, const DWORD len);
        bool parseForwardCommand(Tidy::nServerConn* serverConn, const protocol::base::ForwardCommandBase *nullCmd, const DWORD len);
};
#endif
