#include "protocol/userCommand.h"

#include "dataServer/dtUserService.h"
#include "dataServer/dtUser.h"
#include "dataServer/dtServer.h"
#include "dataServer/dtMessageService.h"

bool dtUserService::parseProtoUserCommand(Tidy::nServerConn* serverConn, const protocol::base::UserCommandBase* nullCommand, const DWORD len)
{
    using namespace protocol::user;
    const UserCommandBase<0> *command = reinterpret_cast<const UserCommandBase<0> *>(nullCommand);
    dtUser *user = getUserByID(command->charID);
    if(user == NULL){
        assert_fail("没有找到玩家");
        return true;
    }
    return user->parseProtoUserCommand(serverConn, nullCommand, len);
}

bool dtUser::parseProtoUserCommand(Tidy::nServerConn* serverConn, const protocol::base::UserCommandBase* nullCommand, const DWORD len)
{
    using namespace protocol::user;
    switch(nullCommand->param)
    {
		case UserDieCommand::PARAM:
			{
                return true;
			}
			break;
        default:
            {
                assert_fail("无法识别的消息");
            }
    }
    return true;
}
