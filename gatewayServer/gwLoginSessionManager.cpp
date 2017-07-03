#include "gatewayServer/gwLoginSessionManager.h"
#include "gatewayServer/gwUserTaskConn.h"
#include "gatewayServer/gwServer.h"
#include "gatewayServer/gwUserService.h"
#include "gatewayServer/gwUser.h"

gwLoginSessionManager::gwLoginSessionManager() :
    _check_login_session_timer(Tidy::global["loginSessionTimeOutTimeInMsec"].getDword())
{
}

gwLoginSessionManager::~gwLoginSessionManager()
{
    destroyAllLoginSession();
}

const gwLoginSession* gwLoginSessionManager::startSession(const protocol::login::LoginSession& session, const char *platformLoginData, const DWORD platformLoginDataSize)
{
    gwLoginSession* loginSession = new gwLoginSession(session, platformLoginData, platformLoginDataSize);
    Tidy::nScopeWRLock lock(_lock);
    if(add(loginSession) == false){
        assert_fail("创建登录会话重复[%s]", session.accountName);
        SAFE_DELETE(loginSession);
        return NULL;
    }
    assert_debug(loginSession == getByName(loginSession->getName()), "");
    return loginSession;
}

bool gwLoginSessionManager::verifyAccount(gwUserTaskConn *conn, const std::string &accountName)
{
    _lock.rdlock();
    const gwLoginSession* session = getByName(accountName.c_str());
	if(session != NULL){
		_lock.unlock();
		_lock.wrlock();
		//double check.必须重新get
		session = getByName(accountName.c_str());
		if(session != NULL){
			remove(session);
		}
		_lock.unlock();
	}
	else{
		_lock.unlock();
	}

	if(session == NULL){
		Tidy::nScopeWRLock wrLock;
		gwUser *user = server->userService->getUserByAccountName(accountName.c_str(), &wrLock);
		if(user != NULL && user->isOfflineProtect() == true){
			session = user->getLoginSession();
			user->clearLoginSession();
			user->info("断线重连");
		}
	}
	if(session == NULL){
		return false;
	}

	conn->setLoginSession(session);

    return true;
}

namespace {
    struct CheckSessionTimeOutExec {
        Tidy::vector<DWORD> recycleSessionList;
        bool exec(gwLoginSession *session) {
            if(session->checkTimeOut() == true) {
                recycleSessionList.push_back(session->getID());
            }
            return true;
        }
    };
};

bool gwLoginSessionManager::doTick()
{
    if(_check_login_session_timer.isReady(server->time()) == true) {
        checkLoginSessionTimeOut();
    }
    return true;
}

bool gwLoginSessionManager::checkLoginSessionTimeOut()
{
    //因为读锁不能升级为写锁,只能采取这种办法了
    _lock.rdlock();
    CheckSessionTimeOutExec exec;
    execAll(exec);
    _lock.unlock();
    if(exec.recycleSessionList.size() == 0) {
        return true;
    }

    _lock.wrlock();
    //必须存ID,否则有野指针
	const gwLoginSession *session = NULL;
    for(DWORD i = 0; i < exec.recycleSessionList.size(); ++ i) {
        session = getByID(exec.recycleSessionList[i]);
        if(session != NULL) {
            server->info("登陆session超时[%u,%s]", session->getID(), session->getName());
            remove(session);
            session->notifyTimeOut();
            SAFE_DELETE(session);
        }
    }
    _lock.unlock();

    return true;
}

void gwLoginSessionManager::destroyAllLoginSession()
{
    Tidy::nScopeWRLock lock(_lock);
    destroyAll();
}

bool gwLoginSessionManager::endLoginSession(const std::string &accountName)
{
    _lock.rdlock();
    const gwLoginSession* session = getByName(accountName.c_str());
    if(session == NULL){
        _lock.unlock();
        return false;
    }
    _lock.unlock();

    _lock.wrlock();
    //double check.必须重新get
    session = getByName(accountName.c_str());
    if(session == NULL){
        _lock.unlock();
        return false;
    }
    remove(session);
    _lock.unlock();

    SAFE_DELETE(session);
    return true;
}
