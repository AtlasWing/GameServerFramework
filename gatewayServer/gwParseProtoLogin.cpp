#include "base/nServerConn.h"

#include "game/nTable.h"
#include "game/nTableManager.h"

#include "protocol/loginCommand.h"
#include "protocol/packageCommon.h"
#include "protocol/Proto/UserData.pb.h"

#include "gatewayServer/gwServer.h"
#include "gatewayServer/gwLoginService.h"
#include "gatewayServer/gwUserService.h"
#include "gatewayServer/gwUser.h"
#include "gatewayServer/gwUserTaskConn.h"
#include "gatewayServer/gwLoginSessionManager.h"
#include "gatewayServer/gwMessageService.h"

bool gwLoginService::parseProtoLoginCommand(Tidy::nServerConn* serverConn, protocol::base::LoginCommandBase* nullCommand, const unsigned int len)
{
    using namespace protocol::login;
    switch(nullCommand->param)
    {
        // TODO 网关的登陆消息
        default:
            {
                assert_fail("未知消息[%u]", nullCommand->param);
                return false;
            }
    }
    return true;
}

void gwLoginService::fillPlayerData(std::vector<command::player::CPlayerData> *playerData, const char* data, const WORD num, const DWORD bitMisk)
{
    //TODO 初始化玩家的数据
    server->debug("解析玩家数据成功并发送");
}
