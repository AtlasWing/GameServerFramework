#include "protocol/relationCommand.h"

#include "dataServer/dtSerializeService.h"
#include "dataServer/dtServer.h"
#include "dataServer/dtMessageService.h"
#include "dataServer/dtUserService.h"

bool dtSerializeService::parseProtoRelationCommand(Tidy::nServerConn* serverConn, const protocol::base::RelationCommandBase* nullCommand, const unsigned int len)
{
	using namespace protocol::relation;
	switch(nullCommand->param)
	{
        default:
            {
                assert_fail("无法识别的消息");
            }
    }

    return true;
}
