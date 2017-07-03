#ifndef _N_MAIL_SERVICE_H_
#define _N_MAIL_SERVICE_H_

#include "protocol/Proto/Mail.pb.h"
#include "protocol/moneyCommon.h"
#include "protocol/mailCommon.h"
#include "protocol/mailCommand.h"

#include "protocol/Proto/Mail.pb.h"

#include "game/nGameServer.h"
#include "game/nItem.h"
#include "game/nTable.h"
#include "game/nTableManager.h"

namespace Game
{
	template<typename ServerT>
		class nMailService
		{
			public:
				nMailService(ServerT* server) : _server(server)
			{
			}
				virtual ~nMailService()
				{
				}

			public:
				bool sendSystemTextMail(const DWORD destUserID, const Tidy::string &title, const Tidy::string &text)
				{
					std::stringstream ss;
					ss<<destUserID;
                    std::vector<protocol::mail::MailMoneyData> moneyData;
					return sendSystemMail(0, NULL, protocol::mail::eSendMailGroupType_UserList, title, text, "", ss.str(), "", moneyData);
				}

				bool sendSystemTextMail(const Tidy::string &userNameListStr, const Tidy::string &title, const Tidy::string &text)
				{
                    std::vector<protocol::mail::MailMoneyData> moneyData;
					return sendSystemMail(0, NULL, protocol::mail::eSendMailGroupType_UserList, title, text, userNameListStr, "", "", moneyData);
				}

                bool sendSystemMail(const BYTE srcPlatformType, const Tidy::nServerConn *conn, const protocol::mail::eSendMailGroupType groupType, const Tidy::string &title, const Tidy::string &text, const Tidy::string &userNameListStr, const Tidy::string &userIDListStr, const Tidy::string &itemListStr, const std::vector<protocol::mail::MailMoneyData> &moneyData)
                {
                    Tidy::vector<Tidy::string> userNameList;
                    Tidy::nStringTool::splitValue(userNameListStr, ';', userNameList);
					Tidy::vector<DWORD> userIDList;
					Tidy::nStringTool::splitValue(userIDListStr, ';', userIDList);
					return sendSystemMail(srcPlatformType, conn, groupType, title, text, userNameList.data(), userNameList.size(), userIDList.data(), userIDList.size(), itemListStr, moneyData);
                }

                bool sendSystemMail(const BYTE srcPlatformType, const Tidy::nServerConn *conn, const protocol::mail::eSendMailGroupType groupType, const Tidy::string &title, const Tidy::string &text, const Tidy::string *userNameList, const DWORD userNameListSize, const DWORD *userIDList, const DWORD userIDListSize, const Tidy::string &itemListStr, const std::vector<protocol::mail::MailMoneyData> &moneyData, const bool needNewItemType = false)
                {
					protocol::mail::SendSysMailData sysMailData;
					sysMailData.set_grouptype(groupType);
					if(srcPlatformType > 0){
						sysMailData.set_srcplatformtype(srcPlatformType);
					}
					if(conn != NULL){
						sysMailData.set_srcconnid(conn->getServerID());
					}
                    for(DWORD i = 0; i < userNameListSize; ++ i){
                        sysMailData.add_destusernamelist(userNameList[i]);
                    }
                    for(DWORD i = 0; i < userIDListSize; ++ i){
						sysMailData.add_destuseridlist(userIDList[i]);
					}
					
                    protocol::mail::CreateMailData *mailData = sysMailData.mutable_createmaildata();
					mailData->set_mailtype(protocol::mail::MailType_Sys);
                    mailData->set_title(title);
                    mailData->set_maintext(text);
                    mailData->set_fromid(Tidy::global["SYS_MAIL_FROM_ID"].getDword());
                    Tidy::vector< Tidy::vector<DWORD> > itemList;
                    Tidy::nStringTool::splitValue(itemListStr, ';', '-', itemList);
                    protocol::mail::MailItem *mailItem = NULL;
                    WORD quality = 0;
                    for(DWORD i = 0; i < itemList.size(); ++ i){
						if(itemList[i].size() != 5){
							assert_fail("发送邮件的附件道具列表配置错误[%s]", itemListStr.c_str());
							return false;
						}
                        mailItem = mailData->add_itemlist();
                        mailItem->set_itembaseid(itemList[i][0]);
                        if(needNewItemType == true){
                            mailItem->set_needbind(itemList[i][1] > 0);
                            mailItem->set_num(itemList[i][2]);
                        }
                        else{
                            mailItem->set_num(itemList[i][1]);
                            mailItem->set_needbind(itemList[i][2] > 0);
                        }
                        quality = itemList[i][3];
                        if(quality == 0){
                            const Game::ItemBaseTable *itemTable = Game::TableManager::Select<const Game::ItemBaseTable>(itemList[i][0]);
                            if(itemTable == NULL){
                                assert_fail("不存在这个道具啊[%u]", itemList[i][0]);
                                return false;
                            }
                            quality = itemTable->quality;
                        }
                        mailItem->set_quality(quality);
                        mailItem->set_identify(itemList[i][4]);
                    }
                    protocol::mail::MoneyDataBinary mailMoneyBinary; 
                    for(WORD i = 0; i < moneyData.size(); ++ i){
                        protocol::mail::MoneyData *mailMoney = mailMoneyBinary.add_moneydata(); 
                        mailMoney->set_money(moneyData[i].money);
                        mailMoney->set_moneytype(moneyData[i].moneyType);
                    }
                    if(mailMoneyBinary.SerializeToString(mailData->mutable_moneydatabinary()) == false){
                        assert_fail("序列化失败");
                        return false;
                    }
					if(mailData->itemlist_size() > MAX_MAIL_ATTACHMENT_NUM){
						return false;
					}
                    BUFFER_PROTO_CMD(protocol::mail::SendSystemMailCommand, send);
                    if(sysMailData.SerializeToArray(send->data, PROTO_CMD_CAPACITY(protocol::mail::SendSystemMailCommand)) == false){
                        _server->fatal("序列化发送邮件失败");
                        return false;
                    }
                    send->num = sysMailData.GetCachedSize();
					if(itemList.empty() == true){
						return _server->messageService->sendCmdToData(send, send->allSize());
					}
					else{
						Tidy::nServerConn *logicConn = _server->selectConnByType(LOGICSERVER);
						if(logicConn == NULL){
							_server->fatal("没有逻辑服务器无法发送邮件");
							return false;
						}
						return logicConn->sendCmd(send, send->allSize());
					}
					return true;
                }
            private:
                ServerT *_server;
        };
};
#endif
