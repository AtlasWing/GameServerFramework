#include "protocol/baseCommand.h"

#include "dataServer/dtMessageService.h"
#include "dataServer/dtServer.h"
#include "dataServer/dtLoginService.h"
#include "dataServer/dtSerializeService.h"
#include "dataServer/dtUserService.h"
#include "dataServer/dtMailService.h"

bool dtMessageService::parseProtocolCommand(Tidy::nServerConn* serverConn, const protocol::NullProtoCommand *nullCommand, const DWORD len)
{
    using namespace protocol::base;

    switch( nullCommand->cmd )
    {
        case LoginCommandBase::CMD:
            {
                return server->loginService->parseProtoLoginCommand(serverConn, (LoginCommandBase*)nullCommand, len);
            }
            break;
        case ServerRunCommandBase::CMD:
            {
                return server->parseServerRunCommand(serverConn, (ServerRunCommandBase*)nullCommand, len);
            }
            break;
		case SerializeCommandBase::CMD:
			{
				return server->serializeService->parseProtoSerializeCommand(serverConn, static_cast<const SerializeCommandBase*>(nullCommand), len);
			}
			break;
        case ForwardCommandBase::CMD:
            {
				return parseForwardCommand(serverConn, (ForwardCommandBase*)(nullCommand), len);
            }
            break;
        case UserCommandBase::CMD:
            {
                return server->userService->parseProtoUserCommand(serverConn, static_cast<const UserCommandBase*>(nullCommand), len);
            }
            break;
        case MailCommandBase::CMD:
            {
                return server->mailService->parseProtoMailCommand(serverConn, static_cast<const MailCommandBase*>(nullCommand), len);
            }
            break;
        case RelationCommandBase::CMD:
            {
                return server->serializeService->parseProtoRelationCommand(serverConn, static_cast<const RelationCommandBase*>(nullCommand), len);
            }
            break;
        default:
            {
                assert_fail("无法识别的消息");
            }
            break;
    }

    return true;
}
