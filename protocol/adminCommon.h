#ifndef _PROTO_VERIFY_COMMON_H_
#define _PROTO_VERIFY_COMMON_H_

#include "baseCommand.h"

#pragma pack(1)
namespace protocol
{
    namespace admin
    {
        // adminServer版本号
        const DWORD adminVersion = 2016040101;
        enum eIDType
        {
            eIDType_Start    = 1,
            eIDType_RoomID   = 1,
            eIDType_CharID   = 2,
            eIDType_ClubID   = 3,
            eIDType_MAX
        };
        enum eNameType
        {
            eNameType_Start      = 1,
            eNameType_CharName   = 1,
            eNameType_ClubName   = 2,
            eNameType_MAX
        };
        struct Range
        {
            DWORD start;
            DWORD end;
        };
        struct ZoneData{
            ZoneData()
            {
            }
            ZoneData(const ZoneData &data):
                zoneID(data.zoneID),
                gameDBVersion(data.gameDBVersion),
                gameProtoVersion(data.gameProtoVersion),
                userCommandVersion(data.userCommandVersion)
            {
                memcpy(zoneName, data.zoneName, sizeof(zoneName));
            }
            DWORD zoneID;
            char  zoneName[MAX_NAMESIZE];
            DWORD gameDBVersion;
            DWORD gameProtoVersion;   // protocol::ServerRun::gameProtoVersion
            DWORD userCommandVersion; // command::login::gameVersion
        };
    };
};

#pragma pack()
#endif
