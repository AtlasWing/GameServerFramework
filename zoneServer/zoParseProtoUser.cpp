#include "protocol/userCommand.h"

#include "zoneServer/zoUserService.h"
#include "zoneServer/zoUser.h"
#include "zoneServer/zoServer.h"
#include "zoneServer/zoMessageService.h"
#include "zoneServer/Relation/zoRelationUser.h"

bool zoUserService::parseProtoUserCommand(Tidy::nServerConn* serverConn, const protocol::base::UserCommandBase* nullCommand, const DWORD len)
{
    using namespace protocol::user;
    const UserCommandBase<0> *command = reinterpret_cast<const UserCommandBase<0> *>(nullCommand);
    zoUser *user = getUserByID(command->charID);
    if(user == NULL){
        assert_fail("没有找到玩家[%u,%u]", command->charID, nullCommand->param);
        return true;
    }
    return user->parseProtoUserCommand(serverConn, nullCommand, len);
}

bool zoUser::parseProtoUserCommand(Tidy::nServerConn* serverConn, const protocol::base::UserCommandBase* nullCommand, const DWORD len)
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
