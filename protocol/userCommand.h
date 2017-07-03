#ifndef PROTOCOL_USERCOMMAND_H_
#define PROTOCOL_USERCOMMAND_H_

#include "userCommon.h"

#pragma pack(1)
namespace protocol
{
    namespace user
    {
        template<int ParamValue>
            struct UserCommandBase : public base::UserCommand<ParamValue>
        {
            DWORD charID;
        };

        template<>
            struct UserCommandBase<0> : public base::UserCommand<0>
        {
            DWORD charID;
        };

        struct UserDieCommand : public NullProtoCommandBase(protocol::user::UserCommandBase, 0)
        };

    };
};

#pragma pack()

#endif
