#include "protocol/forwardCommand.h"
#include "protocol/loginCommand.h"

#include "base/nServerConn.h"

#include "gatewayServer/gwMessageService.h"
#include "gatewayServer/gwServer.h"
#include "gatewayServer/gwUserTaskConn.h"
#include "gatewayServer/gwUser.h"
#include "gatewayServer/gwTelnetService.h"
#include "gatewayServer/gwUserService.h"
#include "gatewayServer/gwFilterService.h"

gwMessageService::gwMessageService()
{
}

gwMessageService::~gwMessageService()
{
}

bool gwMessageService::init()
{
    return Game::nGameMessageService<gwMessageService>::init();
}

void gwMessageService::final()
{
}

bool gwMessageService::sendCmdToZone(const protocol::NullProtoCommand *cmd, const DWORD len) const
{
    return server->sendCmdToZone(cmd, len);
}

bool gwMessageService::sendCmdToLogicByID(DWORD serverID, const protocol::NullProtoCommand *cmd, const DWORD len) const
{
    return server->sendCmdToClientConnByID(serverID, cmd, len);
}

bool gwMessageService::sendCmdToLogic(const gwUser *user, const protocol::NullProtoCommand *cmd, const DWORD len) const
{
    return user->sendCmdToLogic(cmd, len);
}

bool gwMessageService::sendPlatformCommandToServer(Tidy::nServerConn* conn, const protocol::base::PlatformCommandBase* command, const DWORD len) const
{
    BUFFER_PROTO_CMD(protocol::forward::ForwardPlatformServerCommand, send);
    assert_debug(PROTO_CMD_CAPACITY(protocol::forward::ForwardPlatformServerCommand) >= len,"消息包超长[%u,%u]",command->cmd,command->param);
	send->num = len;
    bcopy(command, send->data, len);
    return conn->sendCmd(send, send->size());
}

void gwMessageService::dealConnectSuccess(Tidy::nServerConn* conn)
{
    Game::nGameMessageService<gwMessageService>::dealConnectSuccess(conn);
    switch(conn->getServerType())
    {
            
        default:
            {
            }
    }
}

void gwMessageService::dealConnectClose(Tidy::nServerConn* conn)
{
    Game::nGameMessageService<gwMessageService>::dealConnectClose(conn);
    switch(conn->getServerType())
    {
        default:
            {
            }
    }
}

bool gwMessageService::parseServerCommand(Tidy::nServerConn* serverConn, const protocol::NullProtoCommand *nullCommand, const DWORD nCmdLen)
{
    if(isGameServer(serverConn->getServerType()) == true) {
        return parseProtocolCmd(serverConn, nullCommand, nCmdLen);
    }
    else{
        assert_fail("未处理服务器类型[%u]", serverConn->getServerType());
    }

    return true;
}


bool gwMessageService::doTick()
{
	return true;
}

bool gwMessageService::startService()
{
    server->debug("gwMessageServer初始化完成");
    return true;
}
