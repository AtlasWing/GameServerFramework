#include "protocol/serializeCommand.h"

#include "zoneServer/zoServer.h"
#include "zoneServer/zoUserService.h"
#include "zoneServer/zoMessageService.h"
#include "zoneServer/zoUser.h"

bool zoServer::parseSerializeCommand(Tidy::nServerConn* serverConn, const protocol::base::SerializeCommandBase* nullCommand, const unsigned int len)
{
	using namespace protocol::serialize;
	switch(nullCommand->param)
	{
        case SendFriendDataCommand::PARAM:
            {
				return true;
            }
            break;
        default:
            {
                assert_fail("未识别消息[%u]", nullCommand->param);
            }
    }

    return true;
}

