#ifndef PROTOCOL_RELATIONCOMMAND_H_
#define PROTOCOL_RELATIONCOMMAND_H_

#include "protocol/baseCommand.h"

#pragma pack(1)
namespace protocol{ namespace relation{

    struct AddFriendCommand : public NullProtoCommandBase(base::RelationCommand, 0)
        DWORD charID;
        DWORD friendID;
    };
    struct RemoveFriendCommand : public NullProtoCommandBase(base::RelationCommand, 1)
        DWORD charID;
        DWORD friendID;
    };
    struct RequestFreshRelationUserCommand : public NullProtoCommandBase(base::RelationCommand, 2)
        DWORD charID;
    };
    struct RequestDeleteRelationUserCommand : public NullProtoCommandBase(base::RelationCommand, 3)
        DWORD charID;
    };

    //通知添加好友事件
    struct NotifyAddFriendCommand : public NullProtoCommandBase(base::RelationCommand, 4)
        DWORD charID;
    };
	struct NotifyRecommendFriendSuccessCommand : public NullProtoCommandBase(base::RelationCommand, 5)
		DWORD charID;
		DWORD exp;
		DWORD star;
	};
    struct SyncUserStopChatCommand : public NullProtoCommandBase(base::RelationCommand, 6)
        DWORD charID;
        DWORD endTime;
    };
    struct SendFriendVitalityCommand : public NullProtoCommandBase(base::RelationCommand, 7)
        DWORD charID;
        DWORD friendID;
    };
    struct GetVitalitySuccCommand : public NullProtoCommandBase(base::RelationCommand, 8)
        DWORD charID;
        DWORD friendID;
    };
};};
#pragma pack()
#endif
