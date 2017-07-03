#include "protocol/mailCommon.h"

#include "game/nTable.h"
#include "game/nTableManager.h"

#include "zoneServer/zoMailService.h"
#include "zoneServer/zoServer.h"
#include "zoneServer/zoUser.h"
#include "zoneServer/zoMessageService.h"

zoMailService::zoMailService() : 
    Game::nMailService<zoServer>(server)
{
}

zoMailService::~zoMailService()
{
}

bool zoMailService::initService()
{
    _sys_mail_title = "系统";
    _sys_mail_content = "系统送您的奖励";
    return true;
}

bool zoMailService::finalService()
{
    return true;
}
