#include "base/nServerConn.h"

#include "zoneServer/zoMessageService.h"
#include "zoneServer/zoServer.h"
#include "zoneServer/zoLoginService.h"
#include "zoneServer/zoUserService.h"

bool zoMessageService::parseServerCommand(Tidy::nServerConn* serverConn, const protocol::NullProtoCommand *nullCommand, const DWORD len)
{
    return parseProtocolCommand(serverConn, static_cast<const protocol::NullProtoCommand*>(nullCommand), len);
}

bool zoMessageService::parseProtocolCommand(Tidy::nServerConn* serverConn, const protocol::NullProtoCommand *nullCommand, const DWORD len)
{
    using namespace protocol::base;

    switch( nullCommand->cmd )
    {
        case ForwardCommandBase::CMD:
            {
                return parseForwardCommand(serverConn, static_cast<const ForwardCommandBase*>(nullCommand), len);
            }
            break;
        case SerializeCommandBase::CMD:
            {   
                return server->parseSerializeCommand(serverConn, static_cast<const SerializeCommandBase*>(nullCommand), len);
            }   
            break;
        case ServerRunCommandBase::CMD:
            {
                return server->parseServerRunCommand(serverConn, reinterpret_cast<const ServerRunCommandBase*>(nullCommand), len);
            }
            break;
        case LoginCommandBase::CMD:
            {
                return server->loginService->parseUserLoginCommand(serverConn, reinterpret_cast<const LoginCommandBase*>(nullCommand), len);
            }
            break;
        case UserCommandBase::CMD:
            {
                return server->userService->parseProtoUserCommand(serverConn, reinterpret_cast<const UserCommandBase*>(nullCommand), len);
            }
            break;
        default:
            {
                assert_fail("无法识别消息");
            }
    }
    return false;
}
