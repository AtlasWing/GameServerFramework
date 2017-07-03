#ifndef _PROTOCOL_SERIALIZE_COMMAND_H_
#define _PROTOCOL_SERIALIZE_COMMAND_H_

#include "baseCommand.h"
#include "serializeCommon.h"

#pragma pack(1)

namespace protocol { namespace serialize {

    struct SaveMiscDataCommand : public NullProtoCommandBase(base::SerializeCommand, 1)
		WORD  index;
        DWORD miscDataSerializeType;
        DWORD dataKey;
        DWORD num;
        char data[0];
        const DWORD size() const
        {
            return sizeof(*this) + num * sizeof(data[0]);
        }
    };

    struct RequestMiscDataCommand : public NullProtoCommandBase(base::SerializeCommand, 2)
        DWORD miscDataSerializeType;
        DWORD dataKey;
        bool needClear;
    };

    struct ReturnMiscDataCommand : public NullProtoCommandBase(base::SerializeCommand, 3)
        DWORD miscDataSerializeType;
        DWORD dataKey;
        DWORD saveTimeInSec;
        DWORD num;
        char data[0];
        const DWORD size() const
        {
            return sizeof(*this) + num * sizeof(data[0]);
        }
    };

    struct ClearMiscDataCommand : public NullProtoCommandBase(base::SerializeCommand, 4) 
        DWORD miscDataSerializeType;
        DWORD dataKey;
    };

    struct SaveUserDataCommand : public NullProtoCommandBase(base::SerializeCommand, 5) 
        DWORD charID;
        DWORD num;
        char data[0];
        DWORD allSize() const {
            return sizeof(*this) + num * sizeof(data[0]);
        }
    };
    struct SaveUserRelationDataCommand : public NullProtoCommandBase(base::SerializeCommand, 6) 
        DWORD charID;
        DWORD num;
        char data[0]; // protocol::user::UserRelationSaveData
        DWORD allSize() const {
            return sizeof(*this) + num * sizeof(data[0]);
        }   
    };
    // 请求好友数据
    struct RequestFriendDataCommand : public NullProtoCommandBase(base::SerializeCommand, 7) 
        DWORD charID;
    };
    // 发送好友数据
    struct SendFriendDataCommand : public NullProtoCommandBase(base::SerializeCommand, 8) 
        DWORD charID;
        WORD  num;
        FriendData friendList[0];
        DWORD allSize() const{
            return sizeof(*this) + num * sizeof(friendList[0]);
        }   
    };
};
};

#pragma pack()

#endif
