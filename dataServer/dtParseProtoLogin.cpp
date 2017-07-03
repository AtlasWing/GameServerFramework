#include "protocol/loginCommand.h"
#include "protocol/Proto/UserData.pb.h"
#include "protocol/userCommon.h"

#include "dataServer/dtLoginService.h"
#include "dataServer/dtUser.h"
#include "dataServer/dtServer.h"
#include "dataServer/dtUserService.h"
#include "dataServer/dtMessageService.h"
#include "game/nSsqls.h"
#include "base/nDBConnection.h"

bool dtLoginService::parseProtoLoginCommand(Tidy::nServerConn* serverConn, protocol::base::LoginCommandBase* nullCommand, const unsigned int len)
{
    using namespace protocol::login;
    switch(nullCommand->param)
    {
        // TODO 登陆消息相关
        default:
            {
                assert_fail("未识别消息");
            }
    }
    return true;
}

