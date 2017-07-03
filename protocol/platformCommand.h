#ifndef _PROTOCOL_PLATFORM_COMMAND_H_
#define _PROTOCOL_PLATFORM_COMMAND_H_

#include "baseCommand.h"
#include <string.h>

#pragma pack(1)
namespace protocol {
    namespace base {
        struct PlatformCommandBase : public protocol::NullProtoCommand
        {
            static const BYTE CMD = 1;
            PlatformCommandBase(const BYTE paramValue) : protocol::NullProtoCommand(CMD, paramValue)
            {
            }
        };

        template<int ParamValue>
            struct PlatformCommand : public PlatformCommandBase
        {
            static const BYTE PARAM = ParamValue;
            PlatformCommand() : PlatformCommandBase(ParamValue)
            {
            }
        };
    };

    namespace platform
    {
        const DWORD platformProtoVersion = 2013032001;

        /// @brief	登陆Session结构
        struct LoginSession
        {
            DWORD sessionID;
            WORD platformID;			    ///< 登陆服务器ID 
            BYTE state;						///< 登陆状态 
            char accountName[MAX_ACCOUNT_NAME_SIZE];			///< 账号 
            bool isAdult;
            DWORD channelID;
            DWORD srcPlatformID;
            DWORD loginSessionID;
            BYTE  netType;

            LoginSession()
            {
                accountName[0] = '\0';
            }
        };

        //TODO protocol buffer
        struct S2P_LoginPlatformCommand : public NullProtoCommandBase(base::PlatformCommand, 1)
            S2P_LoginPlatformCommand() : platformProtoVersion(protocol::platform::platformProtoVersion)
            {
            }
            DWORD platformProtoVersion;
            DWORD zoneID;
            char zoneName[MAX_NAMESIZE];
            BYTE gameType;
        };
        struct P2S_ServerSyncOKCommand : public NullProtoCommandBase(base::PlatformCommand, 2)
        };
        struct P2S_StartLoginSessionCommand : public NullProtoCommandBase(base::PlatformCommand, 3)
            P2S_StartLoginSessionCommand() : num(0){
            }
            DWORD allSize() const{
                return sizeof(*this) + num * sizeof(data[0]);
            }
            WORD num;
            char data[0];// protocol::platform::LoginSessionData
        };
        struct S2P_FailLoginSessionCommand : public NullProtoCommandBase(base::PlatformCommand, 7)
            S2P_FailLoginSessionCommand() : num(0){
            }
            DWORD allSize() const{
                return sizeof(*this) + num * sizeof(data[0]);
            }
            WORD num;
            char data[0];// protocol::platform::LoginFailData
        };
        struct S2P_StartLoginSessionSuccessCommand : public NullProtoCommandBase(base::PlatformCommand, 8)
            S2P_StartLoginSessionSuccessCommand() : num(0){
            }
            DWORD allSize() const{
                return sizeof(*this) + num * sizeof(data[0]);
            }
            WORD num;
            char data[0];// protocol::platform::LoginFailData
        };
        struct P2S_RequestZoneStateDataCommand : public NullProtoCommandBase(base::PlatformCommand, 9)
		};
        struct S2P_SendZoneStateDataCommand : public NullProtoCommandBase(base::PlatformCommand, 10)
            S2P_SendZoneStateDataCommand() : num(0){
            }
            DWORD allSize() const{
                return sizeof(*this) + num * sizeof(data[0]);
            }
            WORD num;
            char data[0];// protocol::platform::ZoneState
		};
    };
};

#pragma pack()
#endif
