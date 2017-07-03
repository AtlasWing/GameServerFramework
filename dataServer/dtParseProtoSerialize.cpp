#include "protocol/serializeCommand.h"

#include "game/nMiscDataSerialize.h"

#include "dataServer/dtSerializeService.h"
#include "dataServer/dtServer.h"
#include "dataServer/dtUser.h"
#include "dataServer/dtUserService.h"

bool dtSerializeService::parseProtoSerializeCommand(Tidy::nServerConn* serverConn, const protocol::base::SerializeCommandBase* nullCommand, const unsigned int len)
{
	using namespace protocol::serialize;
	switch(nullCommand->param)
	{
        case SaveMiscDataCommand::PARAM:
            {
                const SaveMiscDataCommand* command = static_cast<const SaveMiscDataCommand*>(nullCommand);
                return saveMiscData(static_cast<eMiscDataSerializeType>(command->miscDataSerializeType), command->dataKey, command->data, command->num);
            }
            break;
        case RequestMiscDataCommand::PARAM:
            {
                return true;
            }
            break;
        case ClearMiscDataCommand::PARAM:
            {
                return true;
            }
            break;
        case SaveUserDataCommand::PARAM:
            {
                const SaveUserDataCommand *command = static_cast<const SaveUserDataCommand *>(nullCommand);
                return saveUserData(command->charID, command->data, command->num);
            }
            break;
        case SaveUserRelationDataCommand::PARAM:
            {
                const SaveUserRelationDataCommand *command = static_cast<const SaveUserRelationDataCommand *>(nullCommand);
                return saveUserRelationData(command->charID, command->data, command->num);
            }
            break;
        default:
            {
                assert_fail("未识别消息");
                return false;
            }
	}
	return true;
}
