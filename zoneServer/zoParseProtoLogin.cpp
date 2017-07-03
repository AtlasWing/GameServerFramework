#include "protocol/loginCommand.h"
#include "protocol/Proto/UserData.pb.h"

#include "zoneServer/zoLoginService.h"
#include "zoneServer/zoServer.h"
#include "zoneServer/zoUserService.h"
#include "zoneServer/zoMessageService.h"
#include "zoneServer/zoUser.h"
#include "zoneServer/Relation/zoRelationUser.h"
#include "zoneServer/zoLoginSessionManager.h"

bool zoLoginService::parseUserLoginCommand(Tidy::nServerConn* serverConn, const protocol::base::LoginCommandBase* nullCommand, const unsigned int len)
{
    using namespace protocol::login;
    switch(nullCommand->param)
    {
        case CreateAccountResultCommand::PARAM:
            {
                const CreateAccountResultCommand* command = static_cast<const CreateAccountResultCommand*>(nullCommand);

                return true;
            }
            break;
        case LoginRoleCommand::PARAM:
            {
                LoginRoleCommand* command = (LoginRoleCommand*)nullCommand;
                zoUser *user = server->userService->getUserByAccountName(command->loginData.accountName);
                if(serverConn->getServerType() == DATASERVER){
                    if(user == NULL){
                        server->debug("玩家不可能为为空[%u,%u]", command->loginData.accountName, command->loginData.gatewayConnID);
                        return true;
                    }
                    protocol::user::LoginSerializeBinary serializeBinary;
                    if(command->num !=0){
                        if(serializeBinary.ParseFromArray(command->data, command->num) == false){
                            server->error("解析用户数据失败");
                            return true;
                        }
                    }
                    user->setLoginData(&serializeBinary);
                    command->loginData.userState = user->relationUser->getLastLoginState();
                    return server->messageService->sendCmdToLogic(user, command, len);
                }
                if(user != NULL){
                    server->debug("玩家不为空[%u,%u]", command->loginData.accountName, command->loginData.gatewayConnID);
                    return true;
                }
                if(server->userService->createUser(&command->loginData) == NULL) {
                    server->debug("fuck 怎么会失败 [%d]", command->loginData.gatewayConnID);
                    return true;
                }
                //user对象建立之后就可以删除登陆Session了,开始用userManager的accid索引来检查是否重新登陆
                _login_session_manager->endSession(command->loginData.accountName);
                server->debug("发送登陆消息给data");
                return server->messageService->sendCmdToData(command, len);
            }
            break;
        case LogoutUserCommand::PARAM:
            {
                const LogoutUserCommand* command = static_cast<const LogoutUserCommand*>(nullCommand);
                // 掉线保护,同时存在user和loginSession
                if(server->userService->logoutUser(command->logoutData) == false){
                    info("退出登陆失败 [%s] [%u]", command->logoutData.accountName, command->logoutType);
                }
                server->debug("玩家登出消息发送给gatewayServer[%u]", command->logoutData.charID);
                return server->messageService->sendCmdToGatewayByID(command->logoutData.gatewayConnID, command, len);
            }
            break;
        case RollbackUserLoginCommand::PARAM:
            {
                const RollbackUserLoginCommand* command = static_cast<const RollbackUserLoginCommand*>(nullCommand);
                if(server->userService->rollBackLoginUser(command->data) == false){
                    assert_fail("不可能出现Session和User都没有的情况");
                }
                debug("回退登陆完成 [%s]", command->data.accountName);
                return true;
            }
            break;
        case RemoveLoginSesssionCommand::PARAM:
            {
                const RemoveLoginSesssionCommand *command = static_cast<const RemoveLoginSesssionCommand*>(nullCommand);
                _login_session_manager->endSession(command->accountName);
                return true;
            };
            break;
        case ChangeLogicServerSuccessCommand::PARAM:
            {
                const ChangeLogicServerSuccessCommand* command = static_cast<const ChangeLogicServerSuccessCommand*>(nullCommand);
                zoUser* user = server->userService->getUserByID(command->charID);
                if(user == NULL){
                    //不需要做回退处理,正常的退出流程即可
                    return true;
                }
                Tidy::nServerConn *logicConn = server->getTaskConnByID(command->logicConnID);
                if(logicConn == NULL){
                    return true;
                }

				Tidy::nServerConn *oldLogicConn = user->getLogicConn();
                user->setLogicConn(logicConn);
                user->dealChangeLogicSuccess();
                return true;
            }
            break;
        case RequestCloseLoginCommand::PARAM:
            {
                return closeLogin();
            }
            break;
        case RequestOpenLoginCommand::PARAM:
            {
                return openLogin();
            }
            break;
        case LoginRoleSuccessCommand::PARAM:
            {
                const LoginRoleSuccessCommand* command = static_cast<const LoginRoleSuccessCommand*>(nullCommand);
                zoUser* user = server->userService->getUserByID(command->charID);
                if(user == NULL){
                    //不需要做回退处理,正常的退出流程即可
                    return true;
                }
                user->dealLoginSuccess();
            }
            break;
        case StartLoginSessionSuccessCommand::PARAM:
            {
                const StartLoginSessionSuccessCommand *command = static_cast<const StartLoginSessionSuccessCommand*>(nullCommand);
                return dealStartLoginSessionSucc(command->accountName, 0, 0);
            }
            break;
        default:
            {
                assert_fail("未识别消息");
            }
    }

    return true;
}

