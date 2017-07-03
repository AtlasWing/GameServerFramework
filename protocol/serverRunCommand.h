#ifndef _PROTOCOL_SERVER_RUN_COMMAND_H_
#define _PROTOCOL_SERVER_RUN_COMMAND_H_

#include <string.h>

#include "commonType.h"

#include "platformCommand.h"
#include "baseCommand.h"
#include "serverRunCommon.h"
#include "adminCommon.h"

#pragma pack(1)

namespace protocol{
    namespace ServerRun{
        struct ServerConnectCommand : public NullProtoCommandBase(base::ServerRunCommand, 1)
            ServerConnectCommand() : gameProtoVersion(protocol::ServerRun::gameProtoVersion){
            }
            const DWORD size() const {
                return sizeof(*this) + num * sizeof(data[0]);
            }
			DWORD baseImpCodeVersion;
			DWORD serverCodeVersion;
            DWORD gameProtoVersion;
            WORD serverID;
            WORD serverType;
            BYTE num;
            char data[0][MAX_IP_LENGTH];
        };

        struct ServerConnectCloseCommand : public NullProtoCommandBase(base::ServerRunCommand, 2)
        };

        struct ServerNotifyMeInitCommand : public NullProtoCommandBase(base::ServerRunCommand, 3)
            ServerEntity serverInfo;
            SDWORD adjustTime;
        };

        struct ServerNotifyMeStartCommand : public NullProtoCommandBase(base::ServerRunCommand, 4)
            WORD num;
            ServerEntity data[0];
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
        };

        struct ServerSyncOKCommand : public NullProtoCommandBase(base::ServerRunCommand, 5)
        };

        struct NotifyOtherWaitStartCommand : public NullProtoCommandBase(base::ServerRunCommand, 6)
            WORD serverID;
            ServerEntity serverEntity;
        };

        struct NotifyOtherWaitStartRespondCommand : public NullProtoCommandBase(base::ServerRunCommand, 7)
            WORD serverID;
        };

        struct ServerStartupComplete : public NullProtoCommandBase(base::ServerRunCommand, 8)
        };

        struct RequestCloseZoneCommand : public NullProtoCommandBase(base::ServerRunCommand, 9)
        };

        struct CloseServerCommand : public NullProtoCommandBase(base::ServerRunCommand, 10)
        };

        struct SendGMCommandListCommand : public NullProtoCommandBase(base::ServerRunCommand, 12)
            WORD num;
            char data[0];
            const DWORD size() const
            {
                return sizeof(*this) + dataSize();
            }
            const DWORD dataSize() const
            {
                return num * sizeof(data[0]);
            }
        };

        struct AdjustServerTimeCommand : public NullProtoCommandBase(base::ServerRunCommand, 13)
            SDWORD adjustTimeInSec;
        };

        struct NotifyStartupCompleteCommand : public NullProtoCommandBase(base::ServerRunCommand, 14)
            ServerEntity serverInfo;
        };
        struct AddScheduleEventCommand : public NullProtoCommandBase(base::ServerRunCommand, 15)
            AddScheduleEventCommand() : num(0){
            }
            DWORD allSize() const{
                return sizeof(*this) + num * sizeof(eventParam[0]);
            }
            DWORD scheduleServerType;
            DWORD scheduleServerID;
            SWORD month;
            SWORD monthDay;
            SWORD weekDay;
            BYTE  hour;
            BYTE  minute;
            BYTE  sec;
            DWORD eventID;
            WORD  num;
            char  eventParam[0];
        };
        struct NotifyServerEventCommand : public NullProtoCommandBase(base::ServerRunCommand, 16)
            DWORD eventID;
            WORD  num;
            char  eventParam[0];
            NotifyServerEventCommand() : num(0){
            }
            DWORD allSize() const{
                return sizeof(*this) + num * sizeof(eventParam[0]);
            }
        };
        struct AddZoneServerCommand : public NullProtoCommandBase(base::ServerRunCommand, 17)
            AddZoneServerCommand() : num(0){
            }
            DWORD allSize() const{
                return sizeof(*this) + num * sizeof(data[0]);
            }
            WORD num;
            protocol::admin::ZoneData data[0];
        };
        struct RemoveZoneServerCommand : public NullProtoCommandBase(base::ServerRunCommand, 18)
            DWORD zoneID;
        };
        struct ClearZoneDataCommand : public NullProtoCommandBase(base::ServerRunCommand, 19)
        };
        struct SetZoneInfoCommand : public NullProtoCommandBase(base::ServerRunCommand, 20)
            DWORD zoneID;
            char zoneName[MAX_NAMESIZE];
        };
		struct FilterUserCmdCommand : public NullProtoCommandBase(base::ServerRunCommand, 21)
			BYTE operType;				///< @ref eCommandFilterOperType
			BYTE cmd;				
			BYTE param;				
		};
		struct SetSaveCommandTimer : public NullProtoCommandBase(base::ServerRunCommand, 22)
			DWORD timeInSec;
		};
		struct CheckUserCmdCommand : public NullProtoCommandBase(base::ServerRunCommand, 23)
			BYTE operType;				///< @ref eCheckUserCommandOperType
            char accountName[MAX_ACCOUNT_NAME_SIZE];
		};
        struct ServerDelayLinkedTestCommand : public NullProtoCommandBase(base::ServerRunCommand, 24)
            ServerDelayLinkedTestCommand() : 
                charID(0),
                testID(0),
                startTimeInMsec(0),
                sendTimeInMsec(0),
                num(0)
            {
            }
            DWORD charID;
            DWORD testID;
            QWORD startTimeInMsec;
            QWORD sendTimeInMsec;
            DWORD num;
            ServerDelayTestInfo infoList[0];
            const DWORD allSize() const {
                return sizeof(*this) + num * sizeof(infoList[0]);
            }
            ServerDelayLinkedTestCommand &operator= (const ServerDelayLinkedTestCommand& r) {
                charID = r.charID;
                testID = r.testID;
                startTimeInMsec = r.startTimeInMsec;
                sendTimeInMsec = r.sendTimeInMsec;
                num = r.num;
                for(DWORD i = 0; i < r.num; ++i) {
                    infoList[i] = r.infoList[i];
                }
                return *this;
            }
        };
        struct SetGlobalParamCommand : public NullProtoCommandBase(base::ServerRunCommand, 25)
            char key[128];
            WORD  valueLen;
            char  value[0];
            SetGlobalParamCommand() : valueLen(0){
            }
            DWORD allSize() const{
                return sizeof(*this) + valueLen * sizeof(value[0]);
            }
        };
        struct StopScheduleCommand : public NullProtoCommandBase(base::ServerRunCommand, 26)
            DWORD configID;
        };
        struct NotifyNeedCreateShareMemoryCommand : public NullProtoCommandBase(base::ServerRunCommand, 27)
        };
    };
};

#pragma pack()
#endif
