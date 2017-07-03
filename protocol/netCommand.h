#ifndef _PROTOCOL_NET_COMMAND_H
#define _PROTOCOL_NET_COMMAND_H
#include <string.h>
#include "commonType.h"
#include "baseCommand.h"

#pragma pack(1)
namespace protocol{ namespace net{
    struct ServerTick : public NullProtoCommandBase(base::NetTickCommand, 1)
        ServerTick(){
        }
    };

    struct ClientTick : public NullProtoCommandBase(base::NetTickCommand, 2)
        ClientTick(){
        }
    };
}}

#pragma pack()
#endif
