#include "protocol/loginCommand.h"
#include "protocol/adminCommon.h"
#include "protocol/userCommand.h"

#include "zoneServer/zoUserService.h"
#include "zoneServer/zoUser.h"
#include "zoneServer/zoServer.h"
#include "zoneServer/zoMessageService.h"
#include "zoneServer/zoMonitorService.h"
#include "zoneServer/zoRelationUserService.h"
#include "zoneServer/Relation/zoRelationUser.h"

zoUserService::zoUserService():
    _tick_timer(2 * ONE_SEC)
{
}

zoUserService::~zoUserService()
{
}

DWORD zoUserService::getNormalUserNum() const
{
	return _user_map_by_accid.size();
}

zoUser* zoUserService::getUserByAccountName(const char *accountName) const
{
    UserAccidMap::const_iterator iter = _user_map_by_accid.find(accountName);
    if(iter == _user_map_by_accid.end()){
        return NULL;
    }
    return iter->second;
}

zoUser* zoUserService::createUser(protocol::login::UserLoginData *data)
{
    Tidy::nServerConn* gatewayConn = server->getTaskConnByID(data->gatewayConnID);
    if(gatewayConn == NULL){
        return NULL;
    }

    Tidy::nServerConn *logicConn = NULL;
    if(data->logicConnID != 0){
        logicConn = server->getTaskConnByID(data->logicConnID);
        if(logicConn->getServerType() != LOGICSERVER){
            assert_fail("服务器类型错误");
            logicConn = NULL;
        }
    }

    if(logicConn == NULL){
        logicConn = server->monitorService->selectLeastUserNumLogicConn(NULL);
    }
    if(logicConn == NULL){
        server->debug("不可能不存在logic服务器");
        return NULL;
    }

    zoRelationUser *relationUser = server->relationUserService->getUserByID(data->charID);
    if(relationUser == NULL){
        if(server->relationUserService->loadRelationUser(data->charID, NULL) == false){
            return NULL;
        }
        relationUser = server->relationUserService->getUserByID(data->charID);
        if(relationUser == NULL){
            assert_fail("注册relationUser失败");
            return NULL;
        }
    }
    zoUser* user = new zoUser(relationUser, data->charID, data->accountName, data->name, gatewayConn, logicConn);
    if(addUser(user) == false){
        assert_fail("添加玩家失败");
        SAFE_DELETE(user);
        return NULL;
    }
    if(user->reg() == false){
        assert_fail("注册玩家失败");
        removeUser(user);
        SAFE_DELETE(user);
        return NULL;
    }

    if(_user_map_by_accid.insert(UserAccidMap::value_type(user->getAccountName(), user)).second == false){
        assert_fail("插入accid索引失败");
        user->unreg();
        removeUser(user);
        SAFE_DELETE(user);
        return NULL;
    }
    data->logicConnID = logicConn->getID();
    user->debug("创建玩家成功logicConnID:[%d]", data->logicConnID);
    return user;
}

bool zoUserService::logoutUser(const protocol::login::UserLogoutData& data)
{
    zoUser* user = getUserByID(data.charID);
    if(user == NULL) {
        //有这种情况,没有发起登陆流程就退出客户端,导致Session没有删除
        server->debug("玩家为空无法析构[%u]", data.charID);
        return false;
    }
    if(data.logicConnID == 0 && user->getLogicConn() != NULL){
        protocol::login::LogoutUserCommand send;
        send.logoutData = data;
        send.logoutType = protocol::login::eUserLogOutSourceType_LoginError;
        server->messageService->sendCmdToLogic(user, &send, sizeof(send));
    }
    user->debug("退出zone成功.");
    return unregUser(user);
}

bool zoUserService::rollBackLoginUser(const protocol::login::UserLogoutData& data)
{
    zoUser* user = getUserByID(data.charID);
    if(user == NULL){
        assert_fail("没有找到玩家");
        return false;
    }

    if(user->getGatewayConn() != NULL){
        protocol::login::RollbackUserLoginCommand send;
        copyStr(send.data.accountName, data.accountName, sizeof(send.data.accountName));
        send.data.charID = data.charID;
        server->messageService->sendCmdToGateway(user, &send, sizeof(send));
    }
    else{
        // 网关关闭会清除LoginSession
    }
    user->debug("回退Login su成功.");

    return unregUser(user);
}

bool zoUserService::rollBackChangeServer(const DWORD charID)
{
    zoUser* user = getUserByID(charID);
    if(user == NULL)
    {
        return false;
    }

    user->debug("回退ChangeServer su成功.");

    return unregUser(user);
}

bool zoUserService::unregUser(zoUser* user)
{
	removeCoreDownRecycle(user->getID());
    server->monitorService->removeUser(user);
    user->unreg();
    removeUser(user);
    _user_map_by_accid.erase(user->getAccountName());
    SAFE_DELETE(user);

    return true;
}

namespace
{
    struct RemoveUserOnGatewayExec
    {
        const Tidy::nServerConn* const _gateway_conn;
        RemoveUserOnGatewayExec(const Tidy::nServerConn* gatewayConn) : _gateway_conn(gatewayConn)
        {
        }
        bool exec(zoUser *user)
        {
            if(user->getGatewayConn() == _gateway_conn)
            {	
				server->userService->addCoreDownRecycle(user->getID());
                user->setGatewayConn(NULL);
            }
            return true;
        }
    };
};

void zoUserService::dealGatewayClose(Tidy::nServerConn *gatewayConn)
{
    RemoveUserOnGatewayExec exec(gatewayConn);
    execEveryUser(exec);
}

namespace
{
    struct GetUserOnLogicExec
    {
        const Tidy::nServerConn* const _logic_conn;
        GetUserOnLogicExec(const Tidy::nServerConn* logicConn) : _logic_conn(logicConn)
        {
        }
        bool exec(zoUser *user) const
        {
            return (user->getLogicConn() == _logic_conn);
        }
    };
};

void zoUserService::dealLogicClose(Tidy::nServerConn *logicConn)
{
    GetUserOnLogicExec exec(logicConn);
    Tidy::vector<zoUser*> recycleUserList;
    get(exec, recycleUserList);
    //不做注销,因为在DataServer上检测到逻辑服务器宕机会发起正常退出流程.
    for(DWORD i = 0; i < recycleUserList.size(); ++ i)
    {
		server->userService->addCoreDownRecycle(recycleUserList[i]->getID());
        recycleUserList[i]->setLogicConn(NULL);
    }
}

void zoUserService::addCoreDownRecycle(const DWORD charID)
{
	_core_down_recycle_set.insert(charID);
}

void zoUserService::removeCoreDownRecycle(const DWORD charID)
{
	_core_down_recycle_set.erase(charID);
}

bool zoUserService::checkCoreDownRecycle(const DWORD charID)
{
	return _core_down_recycle_set.find(charID) != _core_down_recycle_set.end();
}

namespace {
    struct UserTickExec {
        bool exec(zoUser *user) {
            user->doTick();
            return true;
        }
    }userTickExec;
};

void zoUserService::doTick()
{
    if(_tick_timer.isReady(server->time()) == true) {
        execEveryUser(userTickExec);
    }
}

namespace{
    struct UserZeroClockExec{
        UserZeroClockExec(){
        }
        bool exec(zoUser *user){
            user->dealZeroClock();
            return true;
        }
    }userZeroClockExec;
};
bool zoUserService::dealZeroClock(const bool isNormalClocker, const DWORD ringTimes)
{
    return execEveryUser(userZeroClockExec);
}
namespace{
    struct AddConnUserNumExec{
        bool exec(zoUser *user){
			user->addConnUserNum();
            return true;
        }
    };
};

void zoUserService::resetConnUserNum()
{
    AddConnUserNumExec exec;
    execEveryUser(exec);
}
