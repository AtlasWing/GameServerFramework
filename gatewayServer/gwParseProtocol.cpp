#include "base/nServerConn.h"
#include "protocol/loginCommand.h"

#include "gatewayServer/gwServer.h"
#include "gatewayServer/gwLoginService.h"
#include "gatewayServer/gwMessageService.h"
#include "gatewayServer/gwUserService.h"
#include "gatewayServer/gwUser.h"

bool gwMessageService::parseProtocolCmd(Tidy::nServerConn* serverConn, const protocol::NullProtoCommand *nullCmd, const unsigned int len)
{
    using namespace protocol::base;
    switch( nullCmd->cmd )
    {
        case ServerRunCommandBase::CMD:
            {
                return server->parseServerRunCommand(serverConn, static_cast<const ServerRunCommandBase*>(nullCmd), len);
            }
            break;
        case LoginCommandBase::CMD:
            {
                return server->loginService->parseProtoLoginCommand(serverConn, (protocol::base::LoginCommandBase*)nullCmd, len);
            }
            break;
        case ForwardCommandBase::CMD:
            {
                return parseForwardCommand(serverConn, (ForwardCommandBase*)(nullCmd), len);
            }
            break;
        case SerializeCommandBase::CMD:
            {
                return true;
            }
            break;
        case UserCommandBase::CMD:
            {
                return server->userService->parseProtoUserCommand(serverConn, (UserCommandBase*)nullCmd, len);;
            }
            break;
        default:
            {
                assert_fail("未识别的消息[%u,%u]", nullCmd->cmd, nullCmd->param);
            }
            break;
    }

    return true;
}

