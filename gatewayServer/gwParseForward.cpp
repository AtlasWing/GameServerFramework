#include "protocol/forwardCommand.h"

#include "gatewayServer/gwServer.h"
#include "gatewayServer/gwMessageService.h"
#include "gatewayServer/gwUserService.h"
#include "gatewayServer/gwUser.h"
#include "gatewayServer/gwLogicConn.h"

bool gwMessageService::parseForwardCommand(Tidy::nServerConn* serverConn, const protocol::base::ForwardCommandBase* nullCommand, const unsigned int len)
{
    using namespace protocol::forward;

    switch(nullCommand->param)
    {
        case ForwardClientUserCommand::PARAM:
            {
                const ForwardClientUserCommand* command = static_cast<const ForwardClientUserCommand*>(nullCommand);
                Tidy::nScopeRDLock rdLock;
                gwUser* user = server->userService->getUserByID(command->charID, &rdLock);
                if(user == NULL){
                    return true;
                }
                /*
                CBorrowedStreamBuf bbuf(const_cast<char*>(command->data), command->num);
                CIStream is(bbuf);
                IEvent *event = _event_fac_manager->MakeEvent(command->clsID);
                if(event == NULL){
                    assert_fail("消息没有注册[%u]", command->clsID);
                    return false;
                }
                int err=event->LoadStreamNoThrow(is);
                if(err != SEC_NO_ERROR){
                    server->debug("解析客户端消息出现错误");
                    return false;
                }
                if(user->isInLogout() == true){
                    return false;
                }
                if(user->getEventLink() == NULL){
                    user->debug("发消息失败，玩家链接不存在[%u]", command->clsID);
                    return false;
                }
                user->getEventLink()->SendPacket(AppThread(), event);
                */
                // TODO 转发别的进程过来的消息给客户端
                return true;
            }
            break;
        default:
            break;

    }
    return true;
}
