#ifndef _ZO_MESSAGE_SERVICE_H_
#define _ZO_MESSAGE_SERVICE_H_

#include "base/nObjLogger.h"

#include "game/nGameMessageService.h"

class nServerConn;
class zoUser;

namespace protocol{
    namespace base{
        struct NullProtoCommand;
        struct PlatformCommandBase;
        struct ForwardCommandBase;
        struct AdminCommandBase;
    };
};

namespace Tidy{
	class nServerConn;
};
enum eLoginToZoneErrorCode
{
    eLoginToZoneErrorCode_None          = 0,
    eLoginToZoneErrorCode_AccountEmpty  = 1, // 帐号为空
    eLoginToZoneErrorCode_DBError       = 2, // 数据库错误
    eLoginToZoneErrorCode_GatewayError  = 3, // 网关连接错误
    eLoginToZoneErrorCode_GatewayDataError  = 4,  // 网关数据错误
    eLoginToZoneErrorCode_Error         = 5,  //其它错误 
    eLoginToZoneErrorCode_VersionError  = 6  //版本号错误 
};

class zoMessageService : public Game::nGameMessageService<zoMessageService>, public Tidy::nObjLogger<zoMessageService> 
{
    public:
        zoMessageService();
        ~zoMessageService();

    public:
        const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;
        Tidy::nLogger* getLogger() const;

    private:
        virtual bool init();
        virtual void final();

    public:
        bool parseServerCommand(Tidy::nServerConn* serverConn, const protocol::NullProtoCommand *nullCommand, const DWORD len);

    public:
        void dealConnectSuccess(Tidy::nServerConn* conn);
        void dealConnectClose(Tidy::nServerConn* conn);

    public:
		bool doTick();
        bool startService();

    public:
        bool sendCmdToData(const protocol::NullProtoCommand* cmd, const DWORD len) const;
        bool sendCmdToLogicAll(const protocol::NullProtoCommand* cmd, const DWORD len) const;
        bool sendCmdToLogicAllExceptOne(const DWORD serverID, const protocol::NullProtoCommand* cmd, const unsigned int len) const;
        bool sendCmdToLogicByID(DWORD serverID, const protocol::NullProtoCommand* cmd, const unsigned int len) const;
        bool sendCmdToLogic(const zoUser *user, const protocol::NullProtoCommand *cmd, const DWORD len) const;
        bool sendCmdToGateway(const zoUser *user, const protocol::NullProtoCommand* cmd, const DWORD len) const;
        bool sendCmdToLogic(zoUser *user, const protocol::NullProtoCommand* cmd, const unsigned int len) const;

    public:
        Tidy::nServerConn* getLogicConn() const;
        bool connectAdminServer();
        bool hasSlaveAdminServerConn() const;
        bool checkIsMasterAdminServerConn(Tidy::nServerConn *conn) const;
        void disconnectSlaveAdminServerConn();
        bool sendCmdToAdminServer(const protocol::base::AdminCommandBase* command, const DWORD len) const;
        const bool hasAdminServerConn() const;

    private:
        bool parseProtocolCommand(Tidy::nServerConn* serverConn, const protocol::NullProtoCommand *nullCommand, const DWORD len);
        bool parseForwardCommand(Tidy::nServerConn* serverConn, const protocol::base::ForwardCommandBase *nullCmd, const DWORD len);

    private:
        Tidy::nServerConn* _data_server_conn;
        Tidy::nServerConn *_admin_server_conn;

    private:
        const protocol::ServerRun::ServerEntity *_master_admin_server_data;
        const protocol::ServerRun::ServerEntity *_slave_admin_server_data;
};
#endif
