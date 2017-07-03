#include "protocol/mailCommon.h"

#include "protocol/mailCommand.h"
#include "protocol/Proto/Mail.pb.h"

#include "dataServer/dtServer.h"
#include "dataServer/dtMailService.h"
#include "dataServer/dtUserService.h"
#include "dataServer/dtMessageService.h"

bool dtMailService::parseProtoMailCommand(Tidy::nServerConn* serverConn, const protocol::base::MailCommandBase* nullCommand, const unsigned int len)
{
    using namespace protocol::mail;
    switch(nullCommand->param)
    {
        case CreateMailCommand::PARAM:
            {
                const CreateMailCommand* command = static_cast<const CreateMailCommand*>(nullCommand);
                protocol::mail::CreateMailData mailData;
                if(mailData.ParseFromArray(command->data, command->num) == false){
                    assert_fail("解析数据失败");
                    return true;
                }
				return createMail(mailData, &command->destID, 1);
            };
            break;
        case FetchMailTitleListCommand::PARAM:
            {
                const FetchMailTitleListCommand* command = static_cast<const FetchMailTitleListCommand*>(nullCommand);
                dtUser* user = server->userService->getUserByID(command->userID);
                if(user == NULL)
                {
                    assert_debug("没有找到玩家", "");
                    return true;
                }
                return fetchMailList(user);
            }
            break;
        case FetchMailAttachmentSuccCommand::PARAM:
            {
                const FetchMailAttachmentSuccCommand* command = static_cast<const FetchMailAttachmentSuccCommand*>(nullCommand);
                return dealFetchMailAttachmentSucc(command->userID, command->mailID);
            }
            break;
        case FetchMailAttachmentFailCommand::PARAM:
            {
                const FetchMailAttachmentFailCommand *command = static_cast<const FetchMailAttachmentFailCommand*>(nullCommand);
                return dealFetchMailAttachmentFail(command->userID);
            }
            break;
        case SendSystemMailCommand::PARAM:
            {
                const SendSystemMailCommand *command = static_cast<const SendSystemMailCommand*>(nullCommand);
                bool result = true;
				protocol::mail::SendSysMailData sysMailData;
				if(sysMailData.ParseFromArray(command->data, command->num) == false){
					assert_fail("解析数据失败");
					return true;
				}
				do{
					Tidy::vector<DWORD> destUserList;
					if(sysMailData.grouptype() == protocol::mail::eSendMailGroupType_UserList){
						DWORD destID = 0;
						for(DWORD i = 0; i < sysMailData.destusernamelist_size(); ++ i){
                            destID = 0;
							if(server->userService->getUserIDByName(sysMailData.destusernamelist(i).c_str(), &destID) == true){
								destUserList.push_back(destID);
							}
							else{
								result = false;
								break;
							}
						}
						for(DWORD i = 0; i < sysMailData.destuseridlist_size(); ++ i){
							if(server->userService->checkUserIDValid(sysMailData.destuseridlist(i)) == false){
								result = false;
								break;
							}
							destUserList.push_back(sysMailData.destuseridlist(i));
						}
						if(result == false){
							break;
						}
					}
					else if(sysMailData.grouptype() == protocol::mail::eSendMailGroupType_OnlineUser){
						server->userService->getOnlineHumanUserIDList(&destUserList);
					}
					else if(sysMailData.grouptype() == protocol::mail::eSendMailGroupType_AllUser){
						server->userService->getAllHumanUserIDList(&destUserList);
					}
					const protocol::mail::CreateMailData &mailData = sysMailData.createmaildata();
					result = createMail(mailData, destUserList.data(), destUserList.size());
					break;
				}while(false);

				if(sysMailData.has_srcconnid() == true){
                    // TODO 邮件数据日志
				}
				server->info("发送系统邮件[%u]", result);
				return true;
            }
            break;
        default:
            {
                assert_fail("无法处理的消息");
            }
    }

    return true;
}
