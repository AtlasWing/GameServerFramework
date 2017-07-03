#include "protocol/forwardCommand.h"

#include "zoneServer/zoMessageService.h"
#include "zoneServer/zoServer.h"
#include "zoneServer/zoUserService.h"
#include "zoneServer/zoUser.h"

bool zoMessageService::parseForwardCommand(Tidy::nServerConn* serverConn, const protocol::base::ForwardCommandBase *nullCommand, const DWORD len)
{
    using namespace protocol::forward;

    switch(nullCommand->param)
    {
        case ForwardServerUserCommand::PARAM:
            {
                const ForwardServerUserCommand* command = static_cast<const ForwardServerUserCommand*>(nullCommand);
                QWORD currTimeInMsec = server->time().msec();
                static QWORD diffInMsec = 0;
                static QWORD maxDelayInMsec = 0;
                static DWORD commandNum = 0;
                static QWORD startCountTimeInMsec = currTimeInMsec;
                if(currTimeInMsec > (startCountTimeInMsec + 10000) && commandNum > 0){
                    server->info("消息平均延迟[%u]=[%llu, %llu]", commandNum, (diffInMsec/commandNum), maxDelayInMsec);
                    commandNum = 0;
                    diffInMsec = 0;
                    maxDelayInMsec = 0;
                    startCountTimeInMsec = currTimeInMsec;
                }
                if(currTimeInMsec > command->sendTimeInMsec){
                    QWORD delayTimeInMsec = currTimeInMsec - command->sendTimeInMsec;
                    if(delayTimeInMsec > maxDelayInMsec){
                        maxDelayInMsec = delayTimeInMsec;
                    }
                    diffInMsec += (currTimeInMsec - command->sendTimeInMsec);
                }
                ++ commandNum;
                zoUser* user = server->userService->getUserByID(command->charID);
                if(user == NULL) {
                    assert_fail("没有找到玩家[%u]", command->charID);
                    return true;
                }
                /* TODO 解析新的客户端消息
                CBorrowedStreamBuf bbuf(const_cast<char*>(command->data),command->num);
                CIStream is(bbuf);
                IEvent *event = _event_fac_manager->MakeEvent(command->clsID);
                int err=event->LoadStreamNoThrow(is);
                if(err != SEC_NO_ERROR){
                    server->debug("解析客户端消息出现错误");
                    return false;
                }
                user->parseUserMessage(event, command->clsID);
                */
                return true;
            };
            break;
        case ForwardOtherZoneServerCommand::PARAM:
            {
                const ForwardOtherZoneServerCommand *command = static_cast<const ForwardOtherZoneServerCommand*>(nullCommand);
                return sendProtoCmdToOtherZoneServer(reinterpret_cast<const protocol::NullProtoCommand*>(command->data), command->num, command->destZoneID, command->destServerID, command->destServerType);
            }
            break;
        case ForwardGlobalServerCommand::PARAM:
            {
                const ForwardGlobalServerCommand *command = static_cast<const ForwardGlobalServerCommand*>(nullCommand);
                return sendProtoCmdToGlobalServer(reinterpret_cast<const protocol::NullProtoCommand*>(command->data), command->num, command->destGlobalServerID, command->destGlobalServerType);
            }
            break;
        case ForwardSysMessageOnTopToWorldCommand::PARAM:
            {
                const ForwardSysMessageOnTopToWorldCommand *command = static_cast<const ForwardSysMessageOnTopToWorldCommand*>(nullCommand);
                // TODO 广播全区消息
                return true;
            }
            break;
        default:
            {
            }
    }

    assert_fail("无法识别的消息");
    return false;
}
