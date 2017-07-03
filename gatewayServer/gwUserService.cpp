#include "protocol/loginCommand.h"
#include "protocol/userCommand.h"
#include "protocol/userCommon.h"

#include "gatewayServer/gwUserService.h"
#include "gatewayServer/gwUser.h"
#include "gatewayServer/gwServer.h"
#include "gatewayServer/gwMessageService.h"
#include "gatewayServer/gwUserTaskConn.h"
#include "gatewayServer/gwLoginService.h"
#include "gatewayServer/gwMessageService.h"

gwUserService::gwUserService()
{
}

gwUserService::~gwUserService()
{
    assert_debug(_user_map_by_accid.empty() == true, "索引未清空");
}

gwUser* gwUserService::getUserByID(const DWORD userID, Tidy::nScopeRDLock *rdLock)
{
    rdlock();
    gwUser *user = getByID(userID);
    if(user != NULL){
        rdLock->setLock(user->getDataLock());
    }
    unlock();
    return user;
}

gwUser* gwUserService::getUserByID(const DWORD userID, Tidy::nScopeWRLock *wrLock)
{
    rdlock();
    gwUser *user = getByID(userID);
    if(user != NULL){
        wrLock->setLock(user->getDataLock());
    }
    unlock();
    return user;
}

gwUser* gwUserService::getUserByAccountName(const char *accountName, Tidy::nScopeRDLock *rdLock) const
{
    Tidy::nScopeRDLock lock(*const_cast<Tidy::nRWLock*>(&_user_accid_map_lock));
    UserAccidMap::const_iterator iter = _user_map_by_accid.find(accountName);
    if(iter == _user_map_by_accid.end()){
        return NULL;
    }
    rdLock->setLock(iter->second->getDataLock());
    return iter->second;
}

gwUser* gwUserService::getUserByAccountName(const char *accountName, Tidy::nScopeWRLock *wrLock) const
{
    Tidy::nScopeRDLock lock(*const_cast<Tidy::nRWLock*>(&_user_accid_map_lock));
    UserAccidMap::const_iterator iter = _user_map_by_accid.find(accountName);
    if(iter == _user_map_by_accid.end()){
        return NULL;
    }
    wrLock->setLock(iter->second->getDataLock());
    return iter->second;
}

bool gwUserService::addUser(const gwUser* user)
{
    //各种索引
    user->debug("加入到gwUser管理器[%s]", user->getAccountName());
    if(gwUserManagerBase::addUser(const_cast<gwUser*>(user)) == false) {
        return false;
    }
    Tidy::nScopeWRLock lock(_user_accid_map_lock);
    if(_user_map_by_accid.insert(UserAccidMap::value_type(user->getAccountName(), const_cast<gwUser*>(user))).second == false){
        assert_fail("插入accid索引失败[%u]", user->getAccountName());
        gwUserManagerBase::removeUser(const_cast<gwUser*>(user));
        return false;
    }
    return true;
}

bool gwUserService::removeUser(const gwUser* user)
{
    //各种索引
    user->debug("从gwUser管理器中删除");
    Tidy::nScopeWRLock lock(_user_accid_map_lock);
    _user_map_by_accid.erase(user->getAccountName());
    gwUserManagerBase::removeUser(const_cast<gwUser*>(user));
    return true;
}

bool gwUserService::requestLogoutUser(gwUser *unregedUser, const DWORD userID, protocol::login::eUserLogOutSourceType logoutSource)
{
    gwUser *user = NULL;
    Tidy::nScopeRDLock rdLock;
    if(userID == 0 && unregedUser != NULL){
        user = unregedUser;
        rdLock.setLock(user->getDataLock());
    }
    else{
        user = getUserByID(userID, &rdLock);
    }
    if(user == NULL){
        server->debug("没有找到要退出的玩家[%u]", userID);
        return false;
    }
    if(user->getLogoutSource() != protocol::login::eUserLogOutSourceType_None){
        return false;
    }
    user->setLogoutSource(logoutSource);
    user->info("退出[%d,%s,%u]", logoutSource, user->getAccountName(), user->getSessionID());

    if(server->isTerminate() == true || user->isRegged() == false){
        rdLock.release();
        return logoutUser(unregedUser, userID, logoutSource, user->getSessionID());
    }
    protocol::login::LogoutUserCommand send;
    send.logoutData.charID = user->getID();
    send.logoutData.sessionID = user->getSessionID();
    send.logoutData.gatewayConnID = server->getID();
    send.logoutData.logicConnID = user->getLogicConnID();
    rdLock.release();
    send.logoutType = static_cast<BYTE>(logoutSource);
    if(user->getLogicConnID() == 0){
        user->debug("发送到data析构玩家");
        return server->messageService->sendCmdToData(&send, sizeof(send));
    }
    return server->messageService->sendCmdToLogic(user, &send, sizeof(send));
}

bool gwUserService::logoutUser(gwUser *unregedUser, const DWORD userID, protocol::login::eUserLogOutSourceType logoutSource, const DWORD sessionID)
{
    Tidy::nScopeWRLock wrLock;
    gwUser *user = NULL;
    if(userID == 0 && unregedUser != NULL){
        user = unregedUser;
        wrLock.setLock(user->getDataLock());
    }
    else{
        user = getUserByID(userID, &wrLock);
    }
    if(user == NULL){
        server->info("没有找到要退出的玩家[%u]", userID);
        return false;
    }
    if(logoutSource == protocol::login::eUserLogOutSourceType_None){
        if(server->isTerminate() == true){
            logoutSource = protocol::login::eUserLogOutSourceType_ServerClose;
        }
        else{
            logoutSource = protocol::login::eUserLogOutSourceType_NetworkError;
        }
    }
    server->info("用户退出登陆 [%u, %s], [%d]", user->getID(), user->getName(), logoutSource);
    if(user->isRegged() == true){
        wrLock.release();
        if(unregUser(userID) == false){
            assert_fail("注销玩家失败[%u]", userID);
            return false;
        }
    }
    wrLock.release();
    SAFE_DELETE(user);
    return true;
}

bool gwUserService::regUser(gwUser* user, const protocol::login::UserLoginData &userData)
{
    if(user->reg(userData) == false){
        assert_fail("注册失败[%u]", user->getID());
        return false;
    }
    if(addUser(user) == false){
        user->debug("添加到管理器失败[%u]", user->getID());
        return false;
    }
    return true;
}

bool gwUserService::unregUser(const DWORD userID)
{
    server->debug("玩家析构[%u]", userID);
    _unreg_user_lock.lock();
    // 之所以加读锁是因为 Tidy::nServerConn::sendCmdToSceneNoLock的加锁顺序是 scene->_all_lock.rdLock, user->_data_lock.rdLock.而这里gwUser::unreg是 user->_data_lock.rdLock, scene->_all_lock.wrLock.如果是user->wrLock就有可能进入死锁
    Tidy::nScopeRDLock rdLock;
    gwUser *user = getUserByID(userID, &rdLock);
    if(user == NULL){
        _unreg_user_lock.unlock();
        return false;
    }
    rdLock.release();
    removeUser(user);
    _unreg_user_lock.unlock();
    user->unreg();
    return true;
}

void gwUserService::dealAIClose(Tidy::nServerConn *aiConn)
{
}

void gwUserService::dealConnectClose(Tidy::nServerConn *conn)
{
    switch(conn->getServerType())
    {
        case LOGICSERVER:
            {
            }
            break;
    }
}

namespace {
    struct SetNeedSyncTimeExec {
        SetNeedSyncTimeExec()
        {
        }
        bool exec(gwUser* user)
        {
			user->setNeedSyncTime();
            return true;
        }
    };
};

void gwUserService::setNeedSyncTime()
{
	SetNeedSyncTimeExec exec;
	execEveryUser(exec);
}

namespace {
    struct SendUserCmdExec {
        SendUserCmdExec() {}
        bool exec(gwUser *user) {
            if(user->isInLogout() == true){
                return false;
            }
            // TODO user->sendCmdToUser();
            return true;
        }
    };
}
bool gwUserService::sendCmdToAll()
{
    SendUserCmdExec exec;
    return execEveryUser(exec);
}

void gwUserService::dealServerClose()
{
    //TODO 同步给当前gateway的每个客户端关闭连接
}

namespace {
    struct DoTickExec {
        DoTickExec()
        {
        }
        bool exec(gwUser* user)
        {
			user->doTick();
            return true;
        }
    };
};
bool gwUserService::doTick()
{
	DoTickExec exec;
	execEveryUser(exec);
    return true;
}
