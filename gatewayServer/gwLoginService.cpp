#include "protocol/loginCommand.h"

#include "gatewayServer/gwLoginService.h"
#include "gatewayServer/gwServer.h"
#include "gatewayServer/gwMessageService.h"
#include "gatewayServer/gwUserTaskConn.h"
#include "gatewayServer/gwUser.h"
#include "gatewayServer/gwUserService.h"
#include "gatewayServer/gwLoginSessionManager.h"

DWORD gwLoginService::maxOfflineProtectTimeInSec = 0;

gwLoginService::gwLoginService() :
    _login_session_manager(NULL),
    _one_sec_timer(ONE_SEC)
{
}

gwLoginService::~gwLoginService()
{
}

bool gwLoginService::initService()
{
    _login_session_manager = new gwLoginSessionManager();
    maxOfflineProtectTimeInSec = Tidy::global["OFFLINE_PROTECT_TIME_IN_SEC"].getDword();
    return true;
}

void gwLoginService::finalService()
{
    SAFE_DELETE(_login_session_manager);
}

bool gwLoginService::doTick()
{
    if(_login_session_manager->doTick() == false)
    {
        return false;
    }
    if(_one_sec_timer.isReady() == true){
        checkOfflineTimeOut();
    }
    return true;
}

const DWORD gwLoginService::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const
{
    return snprintf(headerBuffer, bufferSize, "[%s]", "Login");;
}

Tidy::nLogger* gwLoginService::getLogger() const
{
    return server->getLogger();
}

void gwLoginService::dealConnectSuccess(Tidy::nServerConn* conn)
{
    switch(conn->getServerType())
    {
        case USERCLIENT:
            {
            }
            break;
        default:
            {
            }
    }
}

void gwLoginService::dealConnectClose(Tidy::nServerConn* conn)
{
    switch(conn->getServerType())
    {
        case USERCLIENT:
            {
            }
            break;
        default:
            {
            }
    }
}

bool gwLoginService::verifyLoginSession(gwUserTaskConn *conn, const std::string &accountName)
{
    if(_login_session_manager->verifyAccount(conn, accountName) == false) {
        error("没有找到登陆Session [%s]", accountName.c_str());
        // TODO 没找到要返回错误
        return false;
    }

    return true;
}

bool gwLoginService::verifyVersion(gwUserTaskConn *conn, const BYTE versionType, const DWORD versionNum)
{
    // TODO 验证版本号
    return true;
}

void gwLoginService::requestCharList(gwUserTaskConn *userConn)
{
    info("请求角色列表[%u,%s,%p]", userConn->getID(), userConn->getAccountName(), userConn);
    protocol::login::RequestCharacterList send;
    send.sessionID = userConn->getID();
    send.platformType = userConn->getPlatformType();
    copyStr(send.accountName, userConn->getAccountName(), sizeof(send.accountName));
    send.channelID = userConn->getChannelID();
    copyStr(send.ip, userConn->getIP(), sizeof(send.ip));
    server->messageService->sendCmdToData(&send, sizeof(send));
}

bool gwLoginService::verifyUserClientConn(Tidy::nTaskConn* conn, const protocol::NullProtoCommand* nullCommand, const DWORD len)
{
    return true;
}

void gwLoginService::startOfflineProtect(gwUser *user)
{
    assert_debug(user->isOfflineProtect() == false, "");
    user->setOfflineProtect(true);
    user->dealOfflineProtect();
    _offline_protect_user_map_lock.wrlock();
    if(_offline_protect_user_map.insert(OfflineProtectUserMap::value_type(user->getAccountName(), OfflineProtectUserData(user->getID(), server->time().sec() + maxOfflineProtectTimeInSec))).second == false){
        assert_fail("离线保护重复[%u]", user->getID());
    }
    _offline_protect_user_map_lock.unlock();
}

void gwLoginService::endOfflineProtect(gwUser *user)
{
    assert_debug(user->isOfflineProtect() == true, "");
    user->setOfflineProtect(false);
    _offline_protect_user_map_lock.wrlock();
    _offline_protect_user_map.erase(user->getAccountName());
    _offline_protect_user_map_lock.unlock();
}

bool gwLoginService::checkOfflineTimeOut()
{
    Tidy::vector<std::pair<std::string, DWORD> > timeOutOfflineUserList;
    const DWORD nowTimeInSec = server->time().sec();
    _offline_protect_user_map_lock.rdlock();
    for(OfflineProtectUserMap::iterator iter = _offline_protect_user_map.begin(); iter != _offline_protect_user_map.end(); ++ iter){
        if(iter->second.second <= nowTimeInSec){
            timeOutOfflineUserList.push_back(std::pair<std::string, DWORD>(iter->first, iter->second.first));
        }
    }
    _offline_protect_user_map_lock.unlock();
    return true;
}

bool gwLoginService::closeLogin()
{
    closeAllUserConn();
    _login_session_manager->destroyAllLoginSession();
    return true;
}

namespace{
    struct CloseUserConnExec{
        bool exec(Tidy::nServerConn *conn){
            gwUserTaskConn *userConn = CLASS_CAST(gwUserTaskConn, conn);
            userConn->setNeedClose();
            return true;
        }
    }closeUserConnExec;
};
void gwLoginService::closeAllUserConn()
{
    server->execTaskConnByType(closeUserConnExec, USERCLIENT);
}

bool gwLoginService::endLoginSession(const std::string &accountName)
{
    return _login_session_manager->endLoginSession(accountName);
}
