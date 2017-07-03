#ifndef _PROTOCOL_ITEM_COMMAND_H_
#define _PROTOCOL_ITEM_COMMAND_H_

#include "baseCommand.h"

#include "protocol/itemCommon.h"

#pragma pack(1)

namespace protocol { namespace item {

	///请求道具数据
	struct RequestItemDataCommand : public NullProtoCommandBase(base::ItemCommand, 1)
		DWORD itemOwnerID;
        DWORD requestID;		///< 请求者ID
        DWORD serialID;
		DWORD gatewayConnID;	///< 请求者网关
	};

};};

#pragma pack()
#endif

