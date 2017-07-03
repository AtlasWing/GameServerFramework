#ifndef PROTOCOL_LOGINCOMMON_H_
#define PROTOCOL_LOGINCOMMON_H_

#include "baseCommand.h"
#include <string.h>

#pragma pack(1)

namespace protocol
{
    namespace login
    {
        enum eCreateAccountResult{
            eCreateAccountResult_Succ   = 0,
            eCreateAccountResult_Fail   = 1,
            eCreateAccountResult_Repeat = 2,
        };
        const DWORD maxCharacterNumPerAcc = 1;
        enum eLoginState {
            eLoginState_None        = 0,
            eLoginState_Login       = 1,    // 登录
            eLoginState_Loading     = 2,    // 启动游戏前加载
            eLoginState_CreateRole  = 3,    // 创建角色
            eLoginState_Guide       = 4,    // 引导 
            eLoginState_SelectRole  = 5,    // 选角 
            eLoginState_LoginRole   = 6,    // 登录角色 
            eLoginState_MainScene   = 7,    // 主场景 
            eLoginState_Battle      = 8,    // PEV战斗中 
            eLoginState_Discovery   = 9,    // 星海探秘 
            eLoginState_PVP         = 10,   // PVP战斗 
        };

        enum eLoginEvent {
            eLoginEvent_None				=   0,
            eLoginEvent_ConnectSucc			=   0,
            eLoginEvent_Login				=	1,
            eLoginEvent_LoginSucc			=	2,
            eLoginEvent_LoginFail			=	3,
            eLoginEvent_IntoSquare			=	4,
            eLoginEvent_IntoHall			=	5,
            eLoginEvent_IntoHallSucc		=	6,
            eLoginEvent_IntoHallFail		=	7,
            eLoginEvent_CreateRoom			=	8,
            eLoginEvent_CreateRoomSucc		=	9,
            eLoginEvent_CreateRoomFail		=	10,
            eLoginEvent_IntoRoom			=	11,
            eLoginEvent_IntoRoomSucc		=	12,
            eLoginEvent_IntoRoomFail		=	13,
            eLoginEvent_ChangeLogic			=	14,
            eLoginEvent_ChangeLogicSuccIntoHall		=	15,
            eLoginEvent_ChangeRoom			=	16,
            eLoginEvent_ChangeRoomFail		=	17,
            eLoginEvent_IntoCommonMatch		=	18,
            eLoginEvent_IntoRankMatch		=	19,
            eLoginEvent_IntoRoomFailAndNeedIntoSquare =	20,
            eLoginEvent_ChangeLogicSuccIntoSquare=	21,
        };

        // 玩家登陆类型
        enum eUserLogInType
        {
            eUserLogInType_Normal,
            eUserLogInType_ChangeLogic,
        };

        enum eReplyChangeZoneCode{
            eReplyChangeZoneCode_None   = 0,
            eReplyChangeZoneCode_OK     = 1,
            eReplyChangeZoneCode_Fail   = 2,
        };
        enum eSaveChangeZoneDataResult{
            eSaveChangeZoneDataResult_None  = 0,
            eSaveChangeZoneDataResult_OK    = 1,
            eSaveChangeZoneDataResult_FailDB= 2,
            eSaveChangeZoneDataResult_CorruptData= 3,
        };
        enum eCommitChangeZoneDataResult{
            eCommitChangeZoneDataResult_None  = 0,
            eCommitChangeZoneDataResult_OK    = 1,
            eCommitChangeZoneDataResult_FailDB= 2,
        };
        enum eUserDataState{
            eUserDataState_Unkown               = 0,
            eUserDataState_Normal               = 1,
            eUserDataState_InOtherZone          = 2,
            eUserDataState_WaitCommitLeaveZone  = 3,
            eUserDataState_WaitCommitIntoZone   = 4,
            eUserDataState_Deleted              = 5,
        };
        // 跨服类型
        enum eChangeZoneType{
            eChangeZoneType_None            =   0,
            eChangeZoneType_LogoutReturn    =   1 << 0,
            eChangeZoneType_EndMatchReturn  =   1 << 1,
        };

        struct LoginSession
        {
            DWORD loginSessionID;
            char accountName[MAX_ACCOUNT_NAME_SIZE];			///< 账号 
            bool isAdult;
            DWORD channelID;
            DWORD platformType;

            LoginSession()
            {
                accountName[0] = '\0';
            }
        };
        struct UserLoginData
        {
            UserLoginData()
            {
				name[0] = '\0';
                ip[0] = '\0';
            }
            char  accountName[MAX_ACCOUNT_NAME_SIZE];
            BYTE  sex;
            DWORD charID;
            DWORD gatewayConnID;
            DWORD logicConnID;
            char name[MAX_NAMESIZE];
            bool isAdult;
            char ip[MAX_IP_LENGTH];
            WORD userState;
        };

        struct RoleInfo 
        {
			RoleInfo(){
                charID = 0;
                level = 0;
                exp = 0;
                skillPoint = 0;
                vitality = 0;
                sex = 0;
			}
            DWORD charID;
            DWORD level;
            DWORD exp;
            DWORD skillPoint;
            DWORD vitality;
            BYTE  sex;
        };

        struct UserLogoutData
        {
			UserLogoutData(){
				accountName[0] = '\0';
                charID = 0;
                sessionID = 0;
                gatewayConnID = 0;
                logicConnID = 0;
			}
            char accountName[MAX_ACCOUNT_NAME_SIZE];
            DWORD charID;
            DWORD sessionID;
            DWORD gatewayConnID;
            DWORD logicConnID;
        };

        /// @brief 序列化用户存档数据
        struct LoginUserSerializeData
        {
            LoginUserSerializeData() : num(0) {}
            DWORD		num;
            char	 	binary[0];
            const DWORD allSize() const { return sizeof(*this) + dataSize(); }
            const DWORD dataSize() const { return num * sizeof(binary[0]); }
        };

        struct ChangeLogicData
        {
            UserLoginData loginData;
            DWORD num;
            //protocol::user::LoginUserSerializeData
            char userData[0];
        };

        struct CharacterInfo
        {
            CharacterInfo() :
                charID(0),
                sex(0)
            {
                name[0] = '\0';
            }

            const CharacterInfo& operator=(const CharacterInfo &other){
                this->charID = other.charID;
                memcpy(this->name, other.name, sizeof(this->name));
                this->sex = other.sex;
                return *this;
            }
            DWORD	charID;
            char	name[MAX_NAMESIZE];
            BYTE	sex;						///< 性别 SexType
        };
        struct UserCharacterData
        {
            CharacterInfo characterInfo;
        };
        struct CreateCharacterData
        {
            CreateCharacterData():
				userType(1),//protocol::user::eUserType_NormalUser),
                platformType(0),
				channelID(0),
                gatewayConnID(0),
                charID(0),
                sex(0),
                isAdult(false)
            {
				name[0] = '\0';
                ip[0] = '\0';
                accountName[0] = '\0';
                openID[0] = '\0';
            }
			BYTE	userType; // protocol::user::eUserType
            char	name[MAX_NAMESIZE];
            char    accountName[MAX_ACCOUNT_NAME_SIZE];
            DWORD   platformType;
			DWORD   channelID;
            DWORD   gatewayConnID;
            DWORD   charID;
            BYTE	sex;			///< ref SexType
            bool    isAdult;
            char    ip[MAX_IP_LENGTH];
            char    openID[MAX_ACCOUNT_NAME_SIZE];/// 腾讯平台的openID
        };
        enum eUserLogOutSourceType
        {
            eUserLogOutSourceType_None                 = 0,
            eUserLogOutSourceType_UserRequest             ,// 玩家请求
            eUserLogOutSourceType_NetworkError            ,// 网络错误
            eUserLogOutSourceType_LoginGatewayFail        ,// 服务器错误
            eUserLogOutSourceType_GatewayCoreDown         ,// 服务器错误
            eUserLogOutSourceType_LogicCoreDown           ,// 服务器错误
            eUserLogOutSourceType_DataCoreDown            ,// 服务器错误
            eUserLogOutSourceType_ServerClose             ,// 服务器关闭
            eUserLogOutSourceType_Kick                    ,// 踢下线
            eUserLogOutSourceType_Relogin                 ,// 玩家重新登录
            eUserLogOutSourceType_HeartTickKick           ,// 心跳超时踢下线
            eUserLogOutSourceType_LoginAccountOut         ,// 刚登录了账号后就下线了 
            eUserLogOutSourceType_Disconnect              ,// 链接断开 
            eUserLogOutSourceType_ReloginLogout           ,// 链接断开 
            eUserLogOutSourceType_LoginError              ,// 登录错误 
            eUserLogOutSourceType_Max
        };
};
};

#pragma pack()

#endif
