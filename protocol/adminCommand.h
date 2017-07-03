#ifndef _PROTO_VERIFY_COMMAND_H_
#define _PROTO_VERIFY_COMMAND_H_

#include "baseCommand.h"
#include "adminCommon.h"

#pragma pack(1)
namespace protocol
{
    namespace base
    {
        struct AdminCommandBase : public protocol::NullProtoCommand
        {
            static const BYTE CMD = 1;
            AdminCommandBase(const BYTE paramValue) : protocol::NullProtoCommand(CMD, paramValue)
            {
            }
        };

        template<int ParamValue>
            struct AdminCommand : public AdminCommandBase
        {
            static const BYTE PARAM = ParamValue;
            AdminCommand() : AdminCommandBase(ParamValue)
            {
            }
        };
    };

    namespace admin
    {
        struct S2A_LoginAdminCommand : public NullProtoCommandBase(base::AdminCommand, 1)
            S2A_LoginAdminCommand() : adminVersion(protocol::admin::adminVersion)
            {
            }
            char strIP[MAX_IP_LENGTH];
            WORD port;
            DWORD adminVersion;
            DWORD gameDBVersion;
            DWORD gameProtoVersion;   // protocol::ServerRun::gameProtoVersion
            DWORD userCommandVersion; // command::login::gameVersion
        };
        struct A2S_ServerSyncOKCommand : public NullProtoCommandBase(base::AdminCommand, 2)
            DWORD zoneID;
            char zoneName[MAX_NAMESIZE];
            DWORD gameType;
        };

        struct S2A_VerifyNameCommand : public NullProtoCommandBase(base::AdminCommand, 3)
            BYTE nameType;
            DWORD verifyID;
            WORD num;
            char name[0];
            const DWORD size() const
            {
                return sizeof(*this) + num * sizeof(name[0]);
            }
        };
        struct S2A_RequestIDRangeCommand : public NullProtoCommandBase(base::AdminCommand, 4)
            BYTE idType;
            DWORD idNum;
            DWORD initStartID;
        };

        struct S2A2S_ReturnIDRangeCommand : public NullProtoCommandBase(base::AdminCommand, 5)
            BYTE idType;
            WORD num;
            Range data[0];
            const DWORD size() const {
                return sizeof(*this) + num * sizeof(data[0]);
            }
        };

        struct A2S_ReturnIDRangeErrorCommand : public NullProtoCommandBase(base::AdminCommand, 6)
            BYTE idType;
        };

        struct A2S_ReturnVerifyNameCommand : public NullProtoCommandBase(base::AdminCommand, 7)
            DWORD verifyID;
            bool result;
        };
        struct S2A2S_ForwardProtoCmdToZoneServerCommand : public NullProtoCommandBase(base::AdminCommand, 8)
            S2A2S_ForwardProtoCmdToZoneServerCommand() : num(0){
            }
            DWORD allSize() const{
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD fromZoneID;
            DWORD destZoneID;
            DWORD destServerID;
            WORD  destServerType;
            WORD num;
            char data[0];
        };
        struct A2S_AddZoneServerCommand : public NullProtoCommandBase(base::AdminCommand, 9)
            A2S_AddZoneServerCommand() : num(0){
            }
            DWORD allSize() const{
                return sizeof(*this) + num * sizeof(data[0]);
            }
            WORD num;
            ZoneData data[0];
        };
        struct A2S_RemoveZoneServerCommand : public NullProtoCommandBase(base::AdminCommand, 10)
            DWORD zoneID;
        };
        struct S2A_DeleteNameCommand : public NullProtoCommandBase(base::AdminCommand, 11)
            S2A_DeleteNameCommand() : num(0) {
            }
            BYTE nameType;
            WORD num;
            NameStr nameList[0];
            DWORD allSize() const {
                return sizeof(*this) + num * sizeof(nameList[0]);
            }
        };
        struct A2S_SyncTimeCommand : public NullProtoCommandBase(base::AdminCommand, 12)
            QWORD timeInMsec;
        };
        struct G2A_GlobalLoginAdminCommand : public NullProtoCommandBase(base::AdminCommand, 13)
            G2A_GlobalLoginAdminCommand() {
            }
            WORD serverType;
            char strIP[MAX_IP_LENGTH];
            WORD port;
            DWORD protoCommandVersion;
        };
        struct A2G_GlobalServerSyncOKCommand : public NullProtoCommandBase(base::AdminCommand, 14)
            DWORD serverID;
            char  serverName[MAX_NAMESIZE];
        };
        struct G2Z_ForwardProtoCmdFromGlobalToZoneServerCommand : public NullProtoCommandBase(base::AdminCommand, 15)
            G2Z_ForwardProtoCmdFromGlobalToZoneServerCommand() : num(0){
            }
            DWORD allSize() const{
                return sizeof(*this) + num * sizeof(data[0]);
            }
            WORD fromGlobalServerID;
            DWORD destZoneID;
            DWORD destServerID;
            WORD  destServerType;
            WORD num;
            char data[0];
        };
        struct Z2G_ForwardProtoCmdFromZoneToGlobalServerCommand : public NullProtoCommandBase(base::AdminCommand, 16)
            Z2G_ForwardProtoCmdFromZoneToGlobalServerCommand() : num(0){
            }
            DWORD allSize() const{
                return sizeof(*this) + num * sizeof(data[0]);
            }
            WORD  fromZoneID;
            DWORD destGlobalServerID;
            WORD  destGlobalServerType;
            WORD num;
            char data[0];
        };
        struct G2G_ForwardProtoCmdFromGlobalToGlobalServerCommand : public NullProtoCommandBase(base::AdminCommand, 17)
            G2G_ForwardProtoCmdFromGlobalToGlobalServerCommand() : num(0){
            }
            DWORD allSize() const{
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD fromGlobalServerID;
            DWORD destGlobalServerID;
            WORD  destGlobalServerType;
            WORD num;
            char data[0];
        };
    };
};

#pragma pack()
#endif
