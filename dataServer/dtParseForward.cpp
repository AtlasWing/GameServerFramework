#include "protocol/forwardCommand.h"

#include "dataServer/dtMessageService.h"
#include "dataServer/dtServer.h"
#include "dataServer/dtUser.h"
#include "dataServer/dtUserService.h"

bool dtMessageService::parseForwardCommand(Tidy::nServerConn* serverConn, const protocol::base::ForwardCommandBase* baseCmd, const DWORD len)
{
    using namespace protocol::forward;
    switch(baseCmd->param)
    {
        case ForwardServerUserCommand::PARAM:
            {
                const ForwardServerUserCommand* command = static_cast<const ForwardServerUserCommand*>(baseCmd);
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
                dtUser* user = server->userService->getUserByID(command->charID);
                if(user == NULL) {
                    assert_fail("没有找到玩家[%u]", command->charID);
                    return true;
                }
                /* TODO 转发消息到客户端
                CBorrowedStreamBuf bbuf(const_cast<char*>(command->data),command->num);
                CIStream is(bbuf);
                IEvent *event = _event_manager->MakeEvent(command->clsID);
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
        default:
            {
                assert_fail("unresolved command");
            }
    }

    return true;
}
