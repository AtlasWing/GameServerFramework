#ifndef _PROTOCOL_LOGIN_COMMAND_H_
#define _PROTOCOL_LOGIN_COMMAND_H_

#include "protocol/baseCommand.h"
#include "protocol/loginCommon.h"
#include "protocol/userCommon.h"

#pragma pack(1)
namespace protocol { namespace login {
    struct LogoutUserCommand : public NullProtoCommandBase(base::LoginCommand, 0)
        BYTE logoutType;
        UserLogoutData logoutData;
    };

    struct CreateAccountCommand : public NullProtoCommandBase(base::LoginCommand, 1)
        char accountName[MAX_ACCOUNT_NAME_SIZE];
        char password[MAX_PASSWORD];
        DWORD sessionID;
    };

    struct RequestCharacterList : public NullProtoCommandBase(base::LoginCommand, 2)
        DWORD sessionID;
        char  accountName[MAX_ACCOUNT_NAME_SIZE];
        DWORD platformType;
        DWORD channelID;
        char  ip[MAX_IP_LENGTH];
    };

    struct ReturnCharacterListCommand : public NullProtoCommandBase(base::LoginCommand, 3)
        ReturnCharacterListCommand():
            num(0)
        {
        }
        WORD allSize() const {
            return sizeof(*this) + dataSize();
        }
        WORD dataSize() const { 
            return num * sizeof(data[0]); 
        }
        BYTE accountState; // enum eAccountState
        BYTE num;
        DWORD sessionID;
        DWORD otherZoneID;
        UserCharacterData data[0];
    };

    struct RemoveLoginSesssionCommand : public NullProtoCommandBase(base::LoginCommand, 4)
        char accountName[MAX_ACCOUNT_NAME_SIZE];
    };

    struct RollbackUserLoginCommand : public NullProtoCommandBase(base::LoginCommand, 5)
        UserLogoutData data;
        DWORD gatewayConnID;
    };

    struct RequestLogOutCommand : public NullProtoCommandBase(base::LoginCommand, 6)
        RequestLogOutCommand() : num(0){
        }
        DWORD allSize() const{
            return sizeof(*this) + num * sizeof(data[0]);
        }
        DWORD charID;
        BYTE sourceType;
        DWORD num;
        char  data[0];
    };

    struct ChangeLogicFailCommand : public NullProtoCommandBase(base::LoginCommand, 7)
        DWORD charID;
    };

    struct LogoutReSelectCharSuccessCommand : public NullProtoCommandBase(base::LoginCommand, 9)
        char accountName[MAX_ACCOUNT_NAME_SIZE];
    };

    struct ChangeLogicServerSuccessCommand : public NullProtoCommandBase(base::LoginCommand, 10)
        DWORD charID;
        DWORD logicConnID;
    };

    struct RefreshClientNetInfo  : public NullProtoCommandBase(base::LoginCommand, 11)
        DWORD charID;
        DWORD lastLoginIP;			///< 登录IP
    };

    struct SyncUserLoginStateCommand : public NullProtoCommandBase(base::LoginCommand, 12)
        DWORD charID;
        BYTE loginState;
    };

    struct RequestIntoLoginStateCommand : public NullProtoCommandBase(base::LoginCommand, 13)
        DWORD charID;
        BYTE loginState;
    };

    struct SendLoginEventCommand : public NullProtoCommandBase(base::LoginCommand, 14)
        DWORD charID;
        BYTE event;
    };

    struct ChangeLogicCommand : public NullProtoCommandBase(base::LoginCommand, 15)
        ChangeLogicCommand() : num(0){
        }
        DWORD allSize() const{
            return sizeof(*this) + num * sizeof(data[0]);
        }
        UserLoginData loginData;
        DWORD roomID;
        BYTE  roomPKMode;
		BYTE  loginStateBeforeRequestIntoRoom;
        DWORD num;
		char  data[0]; // protocol::user::ChangeLogicExtendData
    };

    struct ReturnCheckUserDataStateCommand : public NullProtoCommandBase(base::LoginCommand, 16)
        DWORD sessionID;
        char  accountName[MAX_ACCOUNT_NAME_SIZE];
        DWORD charID;
        BYTE  userDataState;
        DWORD gatewayConnID;
    };
    struct RequestFreshMoneyCommand : public NullProtoCommandBase(base::LoginCommand, 17)
        char  accountName[MAX_ACCOUNT_NAME_SIZE];
        DWORD charID;
        DWORD rechargeMoney;
    };
    struct RequestCloseLoginCommand : public NullProtoCommandBase(base::LoginCommand, 18)
    };
    struct CloseLoginCommand : public NullProtoCommandBase(base::LoginCommand, 19)
    };
    struct RequestOpenLoginCommand : public NullProtoCommandBase(base::LoginCommand, 20)
    };
    struct StartLoginSessionCommand : public NullProtoCommandBase(base::LoginCommand, 21)
        LoginSession loginSession;
    };
    struct FailLoginSessionCommand : public NullProtoCommandBase(base::LoginCommand, 22)
        BYTE errorCode;
        char accountName[MAX_ACCOUNT_NAME_SIZE];
    };
    struct StartLoginSessionSuccessCommand : public NullProtoCommandBase(base::LoginCommand, 23)
		BYTE netType;
        char accountName[MAX_ACCOUNT_NAME_SIZE];
    };
    struct CreateAccountResultCommand : public NullProtoCommandBase(base::LoginCommand, 24)
        char accountName[MAX_ACCOUNT_NAME_SIZE];
        BYTE result;    // 0 成功， 1 失败， 2 账号重复
        DWORD sessionID;
    };
    struct RequestLoginAccountCommand : public NullProtoCommandBase(base::LoginCommand, 25)
        char accountName[MAX_ACCOUNT_NAME_SIZE];                             
        BYTE result;
        DWORD sessionID;
        UserLoginData loginData;
    };
    struct RequestCreateRoleCommand : public NullProtoCommandBase(base::LoginCommand, 26)
        char accountName[MAX_ACCOUNT_NAME_SIZE];                             
        BYTE sex;
        char name[MAX_ACCOUNT_NAME_SIZE];                             
        DWORD sessionID;
        DWORD gatewayConnID;
    };
    struct ReturnCreateRoleCommand : public NullProtoCommandBase(base::LoginCommand, 27) 
        enum {
            NONE,
            REPEAT,     ///< 角色名重复
            MAX
        };
        ReturnCreateRoleCommand():
            errorCode(NONE),
            sessionID(0),
            charID(0)
            {
            }
        BYTE errorCode;
        DWORD sessionID;
        DWORD charID;
        char name[MAX_ACCOUNT_NAME_SIZE];                             
        char accountName[MAX_ACCOUNT_NAME_SIZE];                             
    };
    struct LoginRoleCommand : public NullProtoCommandBase(base::LoginCommand, 28)
        LoginRoleCommand() : num(0){
        }
        DWORD allSize() const{
            return sizeof(*this) + num * sizeof(data[0]);
        }
        UserLoginData loginData;
        RoleInfo roleInfo;
        protocol::user::MainUserData mainUserData;
        WORD num;
		char  data[0]; // protocol::user::LoginSerializeBinary
    };
    struct LoginRoleSuccessCommand : public NullProtoCommandBase(base::LoginCommand, 29)
        DWORD charID;
    };
};
};
#pragma pack()
#endif
