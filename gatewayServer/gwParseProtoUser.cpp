#include "protocol/userCommand.h"

#include "gatewayServer/gwUserService.h"
#include "gatewayServer/gwUser.h"
#include "gatewayServer/gwServer.h"
#include "gatewayServer/gwMessageService.h"

bool gwUserService::parseProtoUserCommand(Tidy::nServerConn* serverConn, const protocol::base::UserCommandBase* nullCommand, const DWORD len)
{
    using namespace protocol::user;
    const UserCommandBase<0> *command = reinterpret_cast<const UserCommandBase<0> *>(nullCommand);
    Tidy::nScopeRDLock rdLock;
    gwUser *user = getUserByID(command->charID, &rdLock);
    if(user == NULL){
        server->debug("parseProtoUserCommand没有找到要处理的玩家[%u][%u,%u]", command->charID, nullCommand->cmd, nullCommand->param);
        return true;
    }
    return user->parseProtoUserCommand(serverConn, nullCommand, len);
}

bool gwUser::parseProtoUserCommand(Tidy::nServerConn* serverConn, const protocol::base::UserCommandBase* nullCommand, const DWORD len)
{
    using namespace protocol::user;
    switch(nullCommand->param)
    {
        default:
            {
                assert_fail("无法识别的消息[%u]", nullCommand->param);
            }
    }
    return true;
}

