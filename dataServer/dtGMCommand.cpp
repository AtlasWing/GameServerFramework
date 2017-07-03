#include "game/nConvert.h"
#include "game/nMailService.h"

#include "protocol/itemCommon.h"
#include "protocol/Proto/Mail.pb.h"

#include "dataServer/dtUser.h"
#include "dataServer/dtServer.h"
#include "dataServer/dtGMCommand.h"
#include "dataServer/dtMailService.h"
#include "dataServer/dtMessageService.h"

void dtGMCommand::registerCmdFunc()
{
}

bool dtGMCommand::handleFetchMail(std::string param)
{
    return true;
}
