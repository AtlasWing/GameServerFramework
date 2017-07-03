#include "protocol/baseCommand.h"

#include "base/nServerConn.h"

#include "dataServer/dtMessageService.h"
#include "dataServer/dtServer.h"
#include "dataServer/dtUser.h"

dtMessageService::dtMessageService()
{
}

dtMessageService::~dtMessageService()
{
}

bool dtMessageService::init()
{
    return Game::nGameMessageService<dtMessageService>::init();
}

void dtMessageService::final()
{
}

bool dtMessageService::parseServerCommand(Tidy::nServerConn* serverConn, const protocol::NullProtoCommand *nullCommand, const DWORD len)
{
    /* TODO Http接收
    if(serverConn->getServerType() == HTTPCLIENT){
        return server->httpService->parseProtoCommand(serverConn, nullCommand, len);
    }
    else */ 
    if(isGameServer(serverConn->getServerType()) == true){
        return parseProtocolCommand(serverConn, nullCommand, len);
    }
    assert_fail("未处理类型连接[%u]", serverConn->getServerType());
	return true;
}

bool dtMessageService::sendCmdToZone(const protocol::NullProtoCommand* cmd, const DWORD len) const
{
    return server->sendCmdToZone(cmd, len);
}

bool dtMessageService::sendCmdToLogic(dtUser *user, const protocol::NullProtoCommand* cmd, const DWORD len) const
{
	const Tidy::nServerConn *logicConn = user->getLogicConn();
	if(logicConn == NULL){
        user->debug("发送到逻辑服失败");
		return true;
	}
	return logicConn->sendCmd(cmd, len);
}

bool dtMessageService::sendCmdToLogicByID(DWORD serverID, const protocol::NullProtoCommand* cmd, const DWORD len) const
{
    return server->sendCmdToTaskConnByID(serverID, cmd, len);
}

bool dtMessageService::sendCmdToLogicAll(const protocol::NullProtoCommand* cmd, const DWORD len) const
{
    return server->sendCmdToTaskConnByType(LOGICSERVER, cmd, len);
}

bool dtMessageService::sendCmdToLogicAllExceptOne(const DWORD serverID, const protocol::NullProtoCommand* cmd, const DWORD len) const
{
    return server->sendCmdToTaskConnByTypeExceptOne(LOGICSERVER, serverID, cmd, len);
}

void dtMessageService::dealConnectSuccess(Tidy::nServerConn* conn)
{
    Game::nGameMessageService<dtMessageService>::dealConnectSuccess(conn);
    switch(conn->getServerType())
    {
        default:
            {
            }
    }
}

void dtMessageService::dealConnectClose(Tidy::nServerConn* conn)
{
    Game::nGameMessageService<dtMessageService>::dealConnectClose(conn);
    switch(conn->getServerType())
    {
        default:
            {
            }
    }
}

bool dtMessageService::sendCmdToGateway(const protocol::NullProtoCommand *cmd, const DWORD len)const
{
    return true;
}

bool dtMessageService::sendCmdToGateway(const dtUser *user, const protocol::NullProtoCommand *cmd, const DWORD len)const
{
    const Tidy::nServerConn *gatewayConn = user->getGatewayConn();
    if(gatewayConn == NULL) {
        return true;
    }
    return gatewayConn->sendCmd(cmd, len);
}
