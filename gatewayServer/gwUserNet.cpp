#include "protocol/forwardCommand.h"

#include "gatewayServer/gwUser.h"
#include "gatewayServer/gwServer.h"
#include "gatewayServer/gwUserTaskConn.h"
#include "gatewayServer/gwMessageService.h"

bool gwUser::sendCmdToLogic(const protocol::NullProtoCommand *nullCommand, const DWORD len) const
{
    if(_logic_conn == NULL) {
        return true;
    }
    return _logic_conn->sendCmd(nullCommand, len);
}
