#ifndef GAME_NCHAT_H_
#define GAME_NCHAT_H_

#include "protocol/commonType.h"
#include "protocol/forwardCommand.h"

#include "base/nMisc.h"
#include "base/nSTL.h"

#include "game/nGameServer.h"
#include "game/nTable.h"
#include "game/nTableManager.h"

namespace Game
{
    class nItem;
	enum eBroadcastMessageType {
		eBroadcastMessageType_None       =   0,
		eBroadcastMessageType_Pop        =   1,      ///< 头顶窗口
		eBroadcastMessageType_Around     =   2,      ///< 周围玩家
		eBroadcastMessageType_Scene      =   3,      ///< 当前地图
		eBroadcastMessageType_World      =   5,      ///< 服务器上所有玩家
        eBroadcastMessageType_Club       =   6,      ///< 公会
		eBroadcastMessageType_Max
	};
    template<typename UserT, typename ItemT>
    struct HyperLinkItem
    {
        const UserT *ownerUser;
        const ItemT *itemData;
        HyperLinkItem(UserT* user, ItemT* item):
            ownerUser(user),
            itemData(item)
        {
			assert_debug(user != NULL && item != NULL, "");
        }
    };
    struct HyperLinkUser
    {
        const std::string userName;
		const DWORD userID;
        HyperLinkUser(const char *name, const DWORD id):
            userName(name),
            userID(id)
        {
        }
        HyperLinkUser(const std::string &name, const DWORD id):
            userName(name),
            userID(id)
        {
        }
    };
    struct HyperLinkMonsterRoom
    {
        const DWORD roomID;
        HyperLinkMonsterRoom(const DWORD theRoomID) :
            roomID(theRoomID)
        {
        }
    };
    struct FormatType
    {
        char _data[command::chat::MAX_SYS_NOTICE];
        FormatType(const char* data){
			strncpy(_data, data, sizeof(_data));
        }
        FormatType(const DWORD data){
            snprintf(_data, sizeof(_data), "%u", data);
        }
        FormatType(const WORD data){
            snprintf(_data, sizeof(_data), "%u", data);
        }
        FormatType(const BYTE data){
            snprintf(_data, sizeof(_data), "%u", data);
        }
        FormatType(const float data){
            snprintf(_data, sizeof(_data), "%f", data);
        }
        FormatType(const int data){
            snprintf(_data, sizeof(_data), "%d", data);
        }
        FormatType(const long unsigned int data){
            snprintf(_data, sizeof(_data), "%lu", data);
        }
		template<typename UserT>
        FormatType(UserT* user){
            snprintf(_data, sizeof(_data), "%u,%u,%u,%s", user->getCharID(), 0, 0, user->getName());
        }
        FormatType(HyperLinkUser* user){
            snprintf(_data, sizeof(_data), "%u,%u,%u,%s", user->userID, 0, 0, user->userName.c_str());
        }
		template<typename UserT, typename ItemT>
        FormatType(const HyperLinkItem<UserT, ItemT> &itemData){
            const UserT* user = itemData.ownerUser;
			const ItemT *item = itemData.itemData;
			snprintf(_data, sizeof(_data), "%u,%u,%u,%s", user->getCharID(), item->serialID, item->baseItemData->getID(), item->baseItemData->getName());
		}
        FormatType(const HyperLinkMonsterRoom& hyperLink)
        {
            snprintf(_data, sizeof(_data), "%u,0,0,0", hyperLink.roomID);
        }
        const char *getStr(){
            return _data;     
        }                                                                        

    };           
	/// @brief	聊天系统
	template<typename UserT, typename ServerT>
		class nChatService 
		{
			protected:
				nChatService();
				~nChatService();

			public:
				static std::string getSysMessage(const WORD id);
				static std::string getSysMessage(const WORD id, const char *info, ...);

			public:
				bool sendDebugMessageToUser(const UserT *user, const char *info,...) __attribute__((format(printf,3,4)));
				bool sendDebugMessageToUserList(const Tidy::vector<UserT*> &userList, const char *info,...) __attribute__((format(printf,3,4)));
				bool sendDebugMessageToWorld(const char *info,...) __attribute__((format(printf,2,3)));
				bool sendDebugMessageToAllZone(const char *info,...) __attribute__((format(printf,2,3)));
            
            public:
                bool sendSysMessageToUser(const UserT *user, const WORD id);
                template<typename T>
                    bool sendSysMessageToUser(const UserT *user, const WORD id, const T &p1);
                template<typename T1, typename T2>
                    bool sendSysMessageToUser(const UserT *user, const WORD id, const T1 &p1, const T2 &p2);
                template<typename T1, typename T2, typename T3>
                    bool sendSysMessageToUser(const UserT *user, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3);
                template<typename T1, typename T2, typename T3, typename T4>
                    bool sendSysMessageToUser(const UserT *user, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4);
                bool sendSysMessageToUserList(const Tidy::vector<UserT*> &userList, const WORD id);
                template<typename T>
                    bool sendSysMessageToUserList(const Tidy::vector<UserT*> &userList, const WORD id, const T &p1);
                template<typename T1, typename T2>
                    bool sendSysMessageToUserList(const Tidy::vector<UserT*> &userList, const WORD id, const T1 &p1, const T2 &p2);
                template<typename T1, typename T2, typename T3>
                    bool sendSysMessageToUserList(const Tidy::vector<UserT*> &userList, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3);               
                bool sendSysMessageToWorld(const WORD id);
                template<typename T>
                    bool sendSysMessageToWorld(const WORD id, const T &p1);
                template<typename T1, typename T2>
                    bool sendSysMessageToWorld(const WORD id, const T1 &p1, const T2 &p2);
                template<typename T1, typename T2, typename T3>
                    bool sendSysMessageToWorld(const WORD id, const T1 &p1, const T2 &p2, const T3 &p3);
                template<typename T1, typename T2, typename T3, typename T4>
                    bool sendSysMessageToWorld(const WORD id, const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4);
                template<typename T1, typename T2, typename T3, typename T4, typename T5>
                    bool sendSysMessageToWorld(const WORD id, const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5);
				bool sendSysMessageToWorld(const char *info, DWORD len);
                bool sendSysMessageToWorldExceptUserList(const DWORD *exceptIDList, const BYTE exceptIDListSize, const WORD id);
                template<typename T>
                    bool sendSysMessageToWorldExceptUserList(const DWORD *exceptIDList, const BYTE exceptIDListSize, const WORD id, const T &p1);
                template<typename T1, typename T2>
                    bool sendSysMessageToWorldExceptUserList(const DWORD *exceptIDList, const BYTE exceptIDListSize, const WORD id, const T1 &p1, const T2 &p2);
                template<typename T1, typename T2, typename T3>
                    bool sendSysMessageToWorldExceptUserList(const DWORD *exceptIDList, const BYTE exceptIDListSize, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3);
                bool sendSysMessageToAllZone(const WORD id);
                template<typename T>
                    bool sendSysMessageToAllZone(const WORD id, const T &p1);
                template<typename T1, typename T2>
                    bool sendSysMessageToAllZone(const WORD id, const T1 &p1, const T2 &p2);
                template<typename T1, typename T2, typename T3>
                    bool sendSysMessageToAllZone(const WORD id, const T1 &p1, const T2 &p2, const T3 &p3);
                bool sendSysMessageToClub(const DWORD clubID, const WORD id);
                template<typename T>
                    bool sendSysMessageToClub(const DWORD clubID, const WORD id, const T &p1);
                template<typename T1, typename T2>
                    bool sendSysMessageToClub(const DWORD clubID, const WORD id, const T1 &p1, const T2 &p2);
                template<typename T1, typename T2, typename T3>
                    bool sendSysMessageToClub(const DWORD clubID, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3);
                template<typename T1, typename T2, typename T3, typename T4>
                    bool sendSysMessageToClub(const DWORD clubID, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4);
                template<typename T1, typename T2, typename T3, typename T4, typename T5>
                    bool sendSysMessageToClub(const DWORD clubID, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5);
				bool sendSysMessageToClub(const DWORD clubID, const char *info, DWORD len);
				template<typename UserManager>
					bool sendSysMessageToUserManager(UserManager &userManager, const WORD id);
				template<typename UserManager, typename T1>
					bool sendSysMessageToUserManager(UserManager &userManager, const WORD id, const T1 &p1);
				template<typename UserManager, typename T1, typename T2>
					bool sendSysMessageToUserManager(UserManager &userManager, const WORD id, const T1 &p1, const T2 &p2);

			public:
				bool sendSysMessageOnTopToWorld(const BYTE times, const BYTE priority, const DWORD lastTimeInSec, const bool needAllZone, const WORD id);
				template<typename T>
					bool sendSysMessageOnTopToWorld(const BYTE times, const BYTE priority, const DWORD lastTimeInSec, const bool needAllZone, const WORD id, const T &p1);
				template<typename T1, typename T2>
					bool sendSysMessageOnTopToWorld(const BYTE times, const BYTE priority, const DWORD lastTimeInSec, const bool needAllZone, const WORD id, const T1 &p1, const T2 &p2);
				template<typename T1, typename T2, typename T3>
					bool sendSysMessageOnTopToWorld(const BYTE times, const BYTE priority, const DWORD lastTimeInSec, const bool needAllZone, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3);
				template<typename T1, typename T2, typename T3, typename T4>
					bool sendSysMessageOnTopToWorld(const BYTE times, const BYTE priority, const DWORD lastTimeInSec, const bool needAllZone, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4);
				template<typename T1, typename T2, typename T3, typename T4, typename T5>
					bool sendSysMessageOnTopToWorld(const BYTE times, const BYTE priority, const DWORD lastTimeInSec, const bool needAllZone, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5);

			public:
                void setNeedSendDebugMessage(bool needSendDebugMessage);

            public:
                bool sendSystemNotice(UserT *user, const WORD messageID);
				template<typename T>
                    bool sendSystemNotice(UserT *user, const WORD messageID, const T &p1);
				template<typename T1, typename T2>
                    bool sendSystemNotice(UserT *user, const WORD messageID, const T1 &p1, const T2 &p2);
				template<typename T1, typename T2, typename T3>
                    bool sendSystemNotice(UserT *user, const WORD messageID, const T1 &p1, const T2 &p2, const T3 &p3);
				template<typename T1, typename T2, typename T3, typename T4>
                    bool sendSystemNotice(UserT *user, const WORD messageID, const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4);

            private:
                bool sendSysMessageToUser(const UserT *user, const char *info, DWORD len);
				bool sendSysMessageToUserList(const Tidy::vector<UserT*> &userList, const char *info, DWORD len);
				bool sendSysMessageToWorldExceptUserList(const DWORD *exceptIDList, const BYTE exceptIDListSize, const char *info, DWORD len);
				bool sendSysMessageToAllZone(const char *info, DWORD len);
				bool sendSysMessageOnTopToWorld(const BYTE times, const BYTE priority, const DWORD lastTimeInSec, const bool needAllZone, const char *info, DWORD len);
				bool sendSysMessageOnTopToWorld(const command::chat::S2C_SendSystemMessageOnTop *command, const bool needSendAllZone);

			private:
				bool _need_send_debug_message;

			public:
				typedef UserT UserType;
				template<typename ChatServiceT>
					struct SendSysMessageToUserManagerExec{
						ChatServiceT *_chat_service;
						const char *_info;
						const DWORD _len;
						SendSysMessageToUserManagerExec(ChatServiceT *chatService, const char *info, const DWORD len) : _chat_service(chatService), _info(info), _len(len){
						}
						bool exec(typename ChatServiceT::UserType *user){
							_chat_service->sendSysMessageToUser(user, _info, _len);
							return true;
						}
					};
				friend class SendSysMessageToUserManagerExec<nChatService<UserT, ServerT> >;
		};

	template<typename UserT, typename ServerT>
		nChatService<UserT, ServerT>::nChatService()
		{
		}

	template<typename UserT, typename ServerT>
		nChatService<UserT, ServerT>::~nChatService()
		{
		}

	template<typename UserT, typename ServerT>
		bool nChatService<UserT, ServerT>::sendSysMessageToUser(const UserT *user, const char *info, DWORD len)
		{
			assert_debug(user != NULL, "");
            if(len >= command::chat::MAX_SYS_CHATINFO){
                len = command::chat::MAX_SYS_CHATINFO - 1;
                assert_fail("系统消息字符串截断");
            }
			BUFFER_USER_CMD(command::chat::S2C_SendSystemMessage, send);
            send->msgInfo = info;
			return const_cast<UserT*>(user)->sendCmdToUser(send);
		}
    template<typename UserT, typename ServerT>
        bool nChatService<UserT, ServerT>::sendSysMessageToUser(const UserT *user, const WORD id)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            DWORD len = snprintf(info, sizeof(info), "%u", id);
            return sendSysMessageToUser(user, info, len);
        }

    template<typename UserT, typename ServerT>
        template<typename T>
        bool nChatService<UserT, ServerT>::sendSysMessageToUser(const UserT *user, const WORD id, const T &p1)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            DWORD len = snprintf(info, sizeof(info), "%u|%s", id, hyperLink1.getStr());
            return sendSysMessageToUser(user, info, len);
        }
    template<typename UserT, typename ServerT>
        template<typename T1, typename T2>
        bool nChatService<UserT, ServerT>::sendSysMessageToUser(const UserT *user, const WORD id, const T1 &p1, const T2 &p2)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr());
            return sendSysMessageToUser(user, info, len);
        }
    template<typename UserT, typename ServerT>
        template<typename T1, typename T2, typename T3>
        bool nChatService<UserT, ServerT>::sendSysMessageToUser(const UserT *user, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            FormatType hyperLink3(p3);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr(), hyperLink3.getStr());
            return sendSysMessageToUser(user, info, len);
        }

    template<typename UserT, typename ServerT>
        template<typename T1, typename T2, typename T3, typename T4>
        bool nChatService<UserT, ServerT>::sendSysMessageToUser(const UserT *user, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            FormatType hyperLink3(p3);
            FormatType hyperLink4(p4);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr(), hyperLink3.getStr(), hyperLink4.getStr());
            return sendSysMessageToUser(user, info, len);
        }
	template<typename UserT, typename ServerT>
		bool nChatService<UserT, ServerT>::sendSysMessageToUserList(const Tidy::vector<UserT*> &userList, const char *info, DWORD len)
		{
			if(userList.empty()){
				return true;
			}           
            if( len >= command::chat::MAX_SYS_CHATINFO){
                len = command::chat::MAX_SYS_CHATINFO - 1;
                assert_fail("系统消息字符串截断");
				return false;
            }
			BUFFER_USER_CMD(command::chat::S2C_SendSystemMessage, send);
            send->msgInfo = info;
            for(typename Tidy::vector<UserT*>::const_iterator iter = userList.begin(); iter != userList.end(); ++ iter){
				(*iter)->sendCmdToUser(send);
			}
			return true;
		}

    template<typename UserT, typename ServerT>
        bool nChatService<UserT, ServerT>::sendSysMessageToUserList(const Tidy::vector<UserT*> &userList, const WORD id)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            DWORD len = snprintf(info, sizeof(info), "%u", id);
            return sendSysMessageToUserList(userList, info, len);
        }
    template<typename UserT, typename ServerT>
        template<typename T>
        bool nChatService<UserT, ServerT>::sendSysMessageToUserList(const Tidy::vector<UserT*> &userList, const WORD id, const T &p1)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            DWORD len = snprintf(info, sizeof(info), "%u|%s", id, hyperLink1.getStr());
            return sendSysMessageToUserList(userList, info, len);
        }
    template<typename UserT, typename ServerT>
        template<typename T1, typename T2>
        bool nChatService<UserT, ServerT>::sendSysMessageToUserList(const Tidy::vector<UserT*> &userList, const WORD id, const T1 &p1, const T2 &p2)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr());
            return sendSysMessageToUserList(userList, info, len);
        }
    template<typename UserT, typename ServerT>
        template<typename T1, typename T2, typename T3>
        bool nChatService<UserT, ServerT>::sendSysMessageToUserList(const Tidy::vector<UserT*> &userList, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            FormatType hyperLink3(p3);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr(), hyperLink3.getStr());
            return sendSysMessageToUserList(userList, info, len);
        }

	template<typename UserT, typename ServerT>
		bool nChatService<UserT, ServerT>::sendSysMessageToWorld(const char *info, DWORD len)
		{
            if(len >= command::chat::MAX_SYS_CHATINFO){
                len = command::chat::MAX_SYS_CHATINFO - 1;
                assert_fail("系统消息字符串截断");
            }
			BUFFER_USER_CMD(command::chat::S2C_SendSystemMessage, send);
            send->msgInfo = info;
            return static_cast<ServerT*>(gameServer)->messageService->sendUserCmdToWorld(send);
		}
    template<typename UserT, typename ServerT>
        bool nChatService<UserT, ServerT>::sendSysMessageToWorld(const WORD id)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            DWORD len = snprintf(info, sizeof(info), "%u", id);
            return sendSysMessageToWorld(info, len);
        }
    template<typename UserT, typename ServerT>
        template<typename T>
        bool nChatService<UserT, ServerT>::sendSysMessageToWorld(const WORD id, const T &p1)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            DWORD len = snprintf(info, sizeof(info), "%u|%s", id, hyperLink1.getStr());
            return sendSysMessageToWorld(info, len);
        }
    template<typename UserT, typename ServerT>
        template<typename T1, typename T2>
        bool nChatService<UserT, ServerT>::sendSysMessageToWorld(const WORD id, const T1 &p1, const T2 &p2)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr());
            return sendSysMessageToWorld(info, len);
        }

    template<typename UserT, typename ServerT>
        template<typename T1, typename T2, typename T3>
        bool nChatService<UserT, ServerT>::sendSysMessageToWorld(const WORD id, const T1 &p1, const T2 &p2, const T3 &p3)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            FormatType hyperLink3(p3);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr(), hyperLink3.getStr());
            return sendSysMessageToWorld(info, len);
        }

	template<typename UserT, typename ServerT>
        template<typename T1, typename T2, typename T3, typename T4>
        bool nChatService<UserT, ServerT>::sendSysMessageToWorld(const WORD id, const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            FormatType hyperLink3(p3);
            FormatType hyperLink4(p4);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr(), hyperLink3.getStr(), hyperLink4.getStr());
            return sendSysMessageToWorld(info, len);
        }

	template<typename UserT, typename ServerT>
        template<typename T1, typename T2, typename T3, typename T4, typename T5>
        bool nChatService<UserT, ServerT>::sendSysMessageToWorld(const WORD id, const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            FormatType hyperLink3(p3);
            FormatType hyperLink4(p4);
            FormatType hyperLink5(p5);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s|%s|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr(), hyperLink3.getStr(), hyperLink4.getStr(), hyperLink5.getStr());
            return sendSysMessageToWorld(info, len);
        }

	template<typename UserT, typename ServerT>
		bool nChatService<UserT, ServerT>::sendSysMessageToWorldExceptUserList(const DWORD *exceptIDList, const BYTE exceptIDListSize, const char *info, DWORD len)
		{
            if( len >= command::chat::MAX_SYS_CHATINFO){
                len = command::chat::MAX_SYS_CHATINFO - 1;
                assert_fail("系统消息字符串截断");
            }
			BUFFER_USER_CMD(command::chat::S2C_SendSystemMessage, send);
			send->msgInfo = info;
            return static_cast<ServerT*>(gameServer)->messageService->sendUserCmdToWorldExcept(exceptIDList, exceptIDListSize, send);
		}
    template<typename UserT, typename ServerT>
        bool nChatService<UserT, ServerT>::sendSysMessageToWorldExceptUserList(const DWORD *exceptIDList, const BYTE exceptIDListSize, const WORD id)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            DWORD len = snprintf(info, sizeof(info), "%u", id);
            return sendSysMessageToWorldExceptUserList(exceptIDList, exceptIDListSize, info, len);
        }
    template<typename UserT, typename ServerT>
        template<typename T>
        bool nChatService<UserT, ServerT>::sendSysMessageToWorldExceptUserList(const DWORD *exceptIDList, const BYTE exceptIDListSize, const WORD id, const T &p1)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            DWORD len = snprintf(info, sizeof(info), "%u|%s", id, hyperLink1.getStr());
            return sendSysMessageToWorldExceptUserList(exceptIDList, exceptIDListSize, info, len);
        }
    template<typename UserT, typename ServerT>
        template<typename T1, typename T2>
        bool nChatService<UserT, ServerT>::sendSysMessageToWorldExceptUserList(const DWORD *exceptIDList, const BYTE exceptIDListSize, const WORD id, const T1 &p1, const T2 &p2)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr());
            return sendSysMessageToWorldExceptUserList(exceptIDList, exceptIDListSize, info, len);
        }
    template<typename UserT, typename ServerT>
        template<typename T1, typename T2, typename T3>
        bool nChatService<UserT, ServerT>::sendSysMessageToWorldExceptUserList(const DWORD *exceptIDList, const BYTE exceptIDListSize, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            FormatType hyperLink3(p3);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr(), hyperLink3.getStr());
            return sendSysMessageToWorldExceptUserList(exceptIDList, exceptIDListSize, info, len);
        }
	template<typename UserT, typename ServerT>
		bool nChatService<UserT, ServerT>::sendSysMessageToAllZone(const char *info,  DWORD len)
		{
            if(len >= command::chat::MAX_SYS_CHATINFO){
                len = command::chat::MAX_SYS_CHATINFO - 1;
                assert_fail("系统消息字符串截断");
            }
			BUFFER_USER_CMD(command::chat::S2C_SendSystemMessage, send);
			send->msgInfo = info;
            return static_cast<ServerT*>(gameServer)->messageService->sendUserCmdToAllZoneUser(send);
		}

    template<typename UserT, typename ServerT>
        bool nChatService<UserT, ServerT>::sendSysMessageToAllZone(const WORD id)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            DWORD len = snprintf(info, sizeof(info), "%u", id);
            return sendSysMessageToAllZone(info, len);
        }
    template<typename UserT, typename ServerT>
        template<typename T>
        bool nChatService<UserT, ServerT>::sendSysMessageToAllZone(const WORD id, const T &p1)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            DWORD len = snprintf(info, sizeof(info), "%u|%s", id, hyperLink1.getStr());
            return sendSysMessageToAllZone(info, len);
        }
    template<typename UserT, typename ServerT>
        template<typename T1, typename T2>
        bool nChatService<UserT, ServerT>::sendSysMessageToAllZone(const WORD id, const T1 &p1, const T2 &p2)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr());
            return sendSysMessageToAllZone(info, len);
        }
    template<typename UserT, typename ServerT>
        template<typename T1, typename T2, typename T3>
        bool nChatService<UserT, ServerT>::sendSysMessageToAllZone(const WORD id, const T1 &p1, const T2 &p2, const T3 &p3)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            FormatType hyperLink3(p3);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr(), hyperLink3.getStr());
            return sendSysMessageToAllZone(info, len);
        }

	template<typename UserT, typename ServerT>
		std::string nChatService<UserT, ServerT>::getSysMessage(const WORD id)
		{
			return nChatService<UserT, ServerT>::getSysMessage(id, "");
		}

	template<typename UserT, typename ServerT>
		std::string nChatService<UserT, ServerT>::getSysMessage(const WORD id, const char *info, ...)
		{
			std::string str;
			const Game::SystemNoticeInfoTable *sysMsg = Game::TableManager::Select<Game::SystemNoticeInfoTable>(id);
			if(sysMsg == NULL){
				assert_fail("找不到提示信息%u", id);
				return str;
			}
			str = sysMsg->SystemMessageText;
			DWORD len = 0;
			char buffer[command::chat::MAX_SYS_CHATINFO];
			FormatString(buffer, command::chat::MAX_SYS_CHATINFO, info, len);
			buffer[len - 1] = 0;
			Tidy::vector<std::string> valueList;
			Tidy::nStringTool::splitValue(buffer, '|', valueList);
			std::string param;
			for(DWORD i = 0; i < valueList.size(); ++ i){
				param = "";
				param = Tidy::nStringTool::append(param, "<p%u>", i + 1);
				Tidy::nStringTool::replace(str, param, valueList[i]);
				param = "";
				param = Tidy::nStringTool::append(param, "<P%u>", i + 1);
				Tidy::nStringTool::replace(str, param, valueList[i]);
			}
			return str;
		}

	template<typename UserT, typename ServerT>
	template<typename T>
		bool nChatService<UserT, ServerT>::sendSysMessageOnTopToWorld(const BYTE times, const BYTE priority, const DWORD lastTimeInSec, const bool needAllZone, const WORD id, const T &p1)
		{
			char info[command::chat::MAX_SYS_CHATINFO] = {0};
			Game::FormatType hyperLink1(p1);
			DWORD len = snprintf(info, sizeof(info), "%u|%s", id, hyperLink1.getStr());
			return sendSysMessageOnTopToWorld(times, priority, lastTimeInSec, needAllZone, info, len);
		}

	template<typename UserT, typename ServerT>
	template<typename T1, typename T2>
		bool nChatService<UserT, ServerT>::sendSysMessageOnTopToWorld(const BYTE times, const BYTE priority, const DWORD lastTimeInSec, const bool needAllZone, const WORD id, const T1 &p1, const T2 &p2)
		{
			char info[command::chat::MAX_SYS_CHATINFO] = {0};
			Game::FormatType hyperLink1(p1);
			Game::FormatType hyperLink2(p2);
			DWORD len = snprintf(info, sizeof(info), "%u|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr());
			return sendSysMessageOnTopToWorld(times, priority, lastTimeInSec, needAllZone, info, len);
		}

	template<typename UserT, typename ServerT>
	template<typename T1, typename T2, typename T3>
		bool nChatService<UserT, ServerT>::sendSysMessageOnTopToWorld(const BYTE times, const BYTE priority, const DWORD lastTimeInSec, const bool needAllZone, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3)
		{
			char info[command::chat::MAX_SYS_CHATINFO] = {0};
			Game::FormatType hyperLink1(p1);
			Game::FormatType hyperLink2(p2);
			Game::FormatType hyperLink3(p3);
			DWORD len = snprintf(info, sizeof(info), "%u|%s|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr(), hyperLink3.getStr());
			return sendSysMessageOnTopToWorld(times, priority, lastTimeInSec, needAllZone, info, len);
		}

	template<typename UserT, typename ServerT>
	template<typename T1, typename T2, typename T3, typename T4>
		bool nChatService<UserT, ServerT>::sendSysMessageOnTopToWorld(const BYTE times, const BYTE priority, const DWORD lastTimeInSec, const bool needAllZone, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4)
		{
			char info[command::chat::MAX_SYS_CHATINFO] = {0};
			Game::FormatType hyperLink1(p1);
			Game::FormatType hyperLink2(p2);
			Game::FormatType hyperLink3(p3);
			Game::FormatType hyperLink4(p4);
			DWORD len = snprintf(info, sizeof(info), "%u|%s|%s|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr(), hyperLink3.getStr(), hyperLink4.getStr());
			return sendSysMessageOnTopToWorld(times, priority, lastTimeInSec, needAllZone, info, len);
		}

	template<typename UserT, typename ServerT>
	template<typename T1, typename T2, typename T3, typename T4, typename T5>
		bool nChatService<UserT, ServerT>::sendSysMessageOnTopToWorld(const BYTE times, const BYTE priority, const DWORD lastTimeInSec, const bool needAllZone, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5)
		{
			char info[command::chat::MAX_SYS_CHATINFO] = {0};
			Game::FormatType hyperLink1(p1);
			Game::FormatType hyperLink2(p2);
			Game::FormatType hyperLink3(p3);
			Game::FormatType hyperLink4(p4);
			Game::FormatType hyperLink5(p5);
			DWORD len = snprintf(info, sizeof(info), "%u|%s|%s|%s|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr(), hyperLink3.getStr(), hyperLink4.getStr(), hyperLink5.getStr());
			return sendSysMessageOnTopToWorld(times, priority, lastTimeInSec, needAllZone, info, len);
		}

	template<typename UserT, typename ServerT>
		bool nChatService<UserT, ServerT>::sendSysMessageOnTopToWorld(const BYTE times, const BYTE priority, const DWORD lastTimeInSec, const bool needSendAllZone, const WORD id)
		{
			char info[command::chat::MAX_SYS_CHATINFO] = {0};
			DWORD len = snprintf(info, sizeof(info), "%u", id);
			return sendSysMessageOnTopToWorld(times, priority, lastTimeInSec, needSendAllZone, info, len);
		}

	template<typename UserT, typename ServerT>
		bool nChatService<UserT, ServerT>::sendSysMessageOnTopToWorld(const BYTE times, const BYTE priority, const DWORD lastTimeInSec, const bool needSendAllZone, const char *info, DWORD len)
		{
			if(len >= command::chat::MAX_SYS_CHATINFO){
				len = command::chat::MAX_SYS_CHATINFO;
				assert_fail("系统消息截断");
			}
			BUFFER_USER_CMD(command::chat::S2C_SendSystemMessageOnTop, send);
			send->msgInfo = info;
			send->times = times;
			send->priority = priority;
			send->overdueTimeInSec = static_cast<ServerT*>(gameServer)->time().sec() + lastTimeInSec;
			return sendSysMessageOnTopToWorld(send, needSendAllZone);
		}
	
	template<typename UserT, typename ServerT>
		bool nChatService<UserT, ServerT>::sendSysMessageOnTopToWorld(const command::chat::S2C_SendSystemMessageOnTop *command, const bool needSendAllZone)
		{
            CDynamicStreamBuf buff;
            COStream to(buff);
            command->SaveStreamNoThrow(to);
			if(command->overdueTimeInSec > static_cast<ServerT*>(gameServer)->time().sec()){
				BUFFER_PROTO_CMD(protocol::forward::ForwardSysMessageOnTopToWorldCommand, sendCmd);
                assert_debug(PROTO_CMD_CAPACITY(protocol::forward::ForwardSysMessageOnTopToWorldCommand) >= to.GetBuf()->Size(), "消息包大小过长");
                sendCmd->num = to.GetBuf()->Size();
                bcopy(to.GetBuf()->GetBytes(), sendCmd->data, sendCmd->num);
                sendCmd->clsID = command->GetCLSID();
				static_cast<ServerT*>(gameServer)->messageService->sendCmdToZone(sendCmd, sendCmd->num);
			}
			else{
				static_cast<ServerT*>(gameServer)->messageService->sendUserCmdToWorld(command, to.GetBuf()->Size());
			}
			if(needSendAllZone == true){
				BUFFER_PROTO_CMD(protocol::forward::ForwardSysMessageOnTopToWorldCommand, sendCmd);
                assert_debug(PROTO_CMD_CAPACITY(protocol::forward::ForwardSysMessageOnTopToWorldCommand) >= to.GetBuf()->Size(), "消息包大小过长");
                sendCmd->num = to.GetBuf()->Size();
                bcopy(to.GetBuf()->GetBytes(), sendCmd->data, sendCmd->num);
                sendCmd->clsID = command->GetCLSID();
				static_cast<ServerT*>(gameServer)->messageService->sendProtoCmdToOtherZoneServer(sendCmd, sendCmd->num, 0, 0, ZONESERVER);
			}
			return true;
		}
	template<typename UserT, typename ServerT>
		bool nChatService<UserT, ServerT>::sendSysMessageToClub(const DWORD clubID, const char *info, DWORD len)
		{
            if(len >= command::chat::MAX_SYS_CHATINFO){
                len = command::chat::MAX_SYS_CHATINFO - 1;
                assert_fail("系统消息字符串截断");
            }
			BUFFER_USER_CMD(command::chat::S2C_SendSystemMessage, send);
			send->msgInfo = info;
            return static_cast<ServerT*>(gameServer)->messageService->sendUserCmdToClub(clubID, send);
		}
    template<typename UserT, typename ServerT>
        bool nChatService<UserT, ServerT>::sendSysMessageToClub(const DWORD clubID, const WORD id)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            DWORD len = snprintf(info, sizeof(info), "%u", id);
            return sendSysMessageToClub(clubID, info, len);
        }
    template<typename UserT, typename ServerT>
        template<typename T>
        bool nChatService<UserT, ServerT>::sendSysMessageToClub(const DWORD clubID, const WORD id, const T &p1)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            DWORD len = snprintf(info, sizeof(info), "%u|%s", id, hyperLink1.getStr());
            return sendSysMessageToClub(clubID, info, len);
        }
    template<typename UserT, typename ServerT>
        template<typename T1, typename T2>
        bool nChatService<UserT, ServerT>::sendSysMessageToClub(const DWORD clubID, const WORD id, const T1 &p1, const T2 &p2)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr());
            return sendSysMessageToClub(clubID, info, len);
        }

    template<typename UserT, typename ServerT>
        template<typename T1, typename T2, typename T3>
        bool nChatService<UserT, ServerT>::sendSysMessageToClub(const DWORD clubID, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            FormatType hyperLink3(p3);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr(), hyperLink3.getStr());
            return sendSysMessageToClub(clubID, info, len);
        }

	template<typename UserT, typename ServerT>
        template<typename T1, typename T2, typename T3, typename T4>
        bool nChatService<UserT, ServerT>::sendSysMessageToClub(const DWORD clubID, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            FormatType hyperLink3(p3);
            FormatType hyperLink4(p4);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr(), hyperLink3.getStr(), hyperLink4.getStr());
            return sendSysMessageToClub(clubID, info, len);
        }

	template<typename UserT, typename ServerT>
        template<typename T1, typename T2, typename T3, typename T4, typename T5>
        bool nChatService<UserT, ServerT>::sendSysMessageToClub(const DWORD clubID, const WORD id, const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            FormatType hyperLink3(p3);
            FormatType hyperLink4(p4);
            FormatType hyperLink5(p5);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s|%s|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr(), hyperLink3.getStr(), hyperLink4.getStr(), hyperLink5.getStr());
            return sendSysMessageToClub(clubID, info, len);
        }

    template<typename UserT, typename ServerT>
        template<typename UserManagerT>
        bool nChatService<UserT, ServerT>::sendSysMessageToUserManager(UserManagerT &userManager, const WORD id)
        {
            char info[command::chat::MAX_SYS_CHATINFO] = {0};
            DWORD len = snprintf(info, sizeof(info), "%u", id);
			SendSysMessageToUserManagerExec<nChatService<UserT, ServerT> > exec(this, info, len);
            return userManager.execAll(exec);
        }
    template<typename UserT, typename ServerT>
        template<typename UserManagerT, typename T1>
        bool nChatService<UserT, ServerT>::sendSysMessageToUserManager(UserManagerT &userManager, const WORD id, const T1 &p1)
        {
            char info[command::chat::MAX_SYS_CHATINFO];
            FormatType hyperLink1(p1);
            DWORD len = snprintf(info, sizeof(info), "%u|%s", id, hyperLink1.getStr());
			SendSysMessageToUserManagerExec<nChatService<UserT, ServerT> > exec(this, info, len);
            return userManager.execAll(exec);
        }
    template<typename UserT, typename ServerT>
        template<typename UserManagerT, typename T1, typename T2>
        bool nChatService<UserT, ServerT>::sendSysMessageToUserManager(UserManagerT &userManager, const WORD id, const T1 &p1, const T2 &p2)
        {
            char info[command::chat::MAX_SYS_CHATINFO];
            FormatType hyperLink1(p1);
            FormatType hyperLink2(p2);
            DWORD len = snprintf(info, sizeof(info), "%u|%s|%s", id, hyperLink1.getStr(), hyperLink2.getStr());
			SendSysMessageToUserManagerExec<nChatService<UserT, ServerT> > exec(this, info, len);
            return userManager.execAll(exec);
        }
        template<typename UserT, typename ServerT>
        bool nChatService<UserT, ServerT>::sendSystemNotice(UserT *user, const WORD messageID)
        {
            command::chat::S2C_SendSystemNotice send;
            send.messageID = messageID;
            send.msgInfo = "";
            return user->sendCmdToUser(&send);
        }

        template<typename UserT, typename ServerT>
            template<typename T>
            bool nChatService<UserT, ServerT>::sendSystemNotice(UserT *user, const WORD messageID, const T &p1)
            {
                char info[command::chat::MAX_SYS_CHATINFO] = {0};
                FormatType hyperLink1(p1);
                snprintf(info, sizeof(info), "%s", hyperLink1.getStr());
                command::chat::S2C_SendSystemNotice send;
                send.messageID = messageID;
                send.msgInfo = info; 
                return user->sendCmdToUser(&send);
            }

        template<typename UserT, typename ServerT>
            template<typename T1, typename T2>
            bool nChatService<UserT, ServerT>::sendSystemNotice(UserT *user, const WORD messageID, const T1 &p1, const T2 &p2)
            {
                char info[command::chat::MAX_SYS_CHATINFO] = {0};
                FormatType hyperLink1(p1);
                FormatType hyperLink2(p2);
                snprintf(info, sizeof(info), "%s|%s", hyperLink1.getStr(), hyperLink2.getStr());
                command::chat::S2C_SendSystemNotice send;
                send.messageID = messageID;
                send.msgInfo = info; 
                return user->sendCmdToUser(&send);
            }

        template<typename UserT, typename ServerT>
            template<typename T1, typename T2, typename T3>
            bool nChatService<UserT, ServerT>::sendSystemNotice(UserT *user, const WORD messageID, const T1 &p1, const T2 &p2, const T3 &p3)
            {
                char info[command::chat::MAX_SYS_CHATINFO] = {0};
                FormatType hyperLink1(p1);
                FormatType hyperLink2(p2);
                FormatType hyperLink3(p3);
                snprintf(info, sizeof(info), "%s|%s|%s", hyperLink1.getStr(), hyperLink2.getStr(), hyperLink3.getStr());
                command::chat::S2C_SendSystemNotice send;
                send.messageID = messageID;
                send.msgInfo = info; 
                return user->sendCmdToUser(&send);
            }
    template<typename UserT, typename ServerT>
        template<typename T1, typename T2, typename T3, typename T4>
            bool nChatService<UserT, ServerT>::sendSystemNotice(UserT *user, const WORD messageID, const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4)
            {
                char info[command::chat::MAX_SYS_CHATINFO] = {0};
                FormatType hyperLink1(p1);
                FormatType hyperLink2(p2);
                FormatType hyperLink3(p3);
                FormatType hyperLink4(p4);
                snprintf(info, sizeof(info), "%s|%s|%s|%s", hyperLink1.getStr(), hyperLink2.getStr(), hyperLink3.getStr(), hyperLink4.getStr());
                command::chat::S2C_SendSystemNotice send;
                send.messageID = messageID;
                send.msgInfo = info; 
                return user->sendCmdToUser(&send);
            }
};
#endif
