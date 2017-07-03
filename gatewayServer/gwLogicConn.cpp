#include "protocol/loginCommon.h"

#include "gatewayServer/gwLogicConn.h"
#include "gatewayServer/gwUser.h"
#include "gatewayServer/gwServer.h"
#include "gatewayServer/gwUserService.h"

gwLogicConn::gwLogicConn(const Tidy::nConnectServerData &connectServerData) :
    Tidy::nClientConnNoQueue(connectServerData)
{
}

gwLogicConn::~gwLogicConn()
{
}

namespace{
    struct RecycleUserExec{
        gwLogicConn *_logic_conn;
        Tidy::vector<DWORD> _user_list;
        RecycleUserExec(gwLogicConn *logicConn) : _logic_conn(logicConn){
        }
        bool exec(gwUser *user){
            if(user->checkResetLogicConn(_logic_conn) == true){
                _user_list.push_back(user->getID());
            }
            return true;
        }
    };
};
void gwLogicConn::dealClose()
{
    RecycleUserExec exec(this);
    // 内部加锁了，应该没有问题
    server->userService->execEveryUser(exec);
    for(DWORD i = 0; i < exec._user_list.size(); ++ i){
        server->userService->requestLogoutUser(NULL, exec._user_list[i], protocol::login::eUserLogOutSourceType_LogicCoreDown);
    }
}
