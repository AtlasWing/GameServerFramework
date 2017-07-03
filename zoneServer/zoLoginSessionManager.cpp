#include "zoneServer/zoLoginSessionManager.h"
#include "zoneServer/zoLoginSession.h"
#include "zoneServer/zoServer.h"

zoLoginSessionManager::zoLoginSessionManager()
{
}

zoLoginSessionManager::~zoLoginSessionManager()
{
}

const zoLoginSession* zoLoginSessionManager::startSession(const DWORD sessionID, const char *accountName, const Tidy::nServerConn* gatewayConn, const zoLoginSession::SessionState state)
{
    if(_session_account_map.find(sessionID) != _session_account_map.end()){
        server->debug("已经建立过session，不能重复建立");
        return NULL;
    }
    zoLoginSession* loginSession = new zoLoginSession(sessionID, accountName, gatewayConn, state);
    if(add(loginSession) == false)
    {
        SAFE_DELETE(loginSession);
        return NULL;
    }
    std::string account(accountName);
    _session_account_map[sessionID] = account;

    return loginSession;
}

void zoLoginSessionManager::endSession(const zoLoginSession* session)
{
    remove(session);
	assert_debug(getByName(session->getName()) == NULL, "[%s]", session->getName());
	assert_debug(getSessionByAccID(session->getName()) == NULL, "[%s]", session->getName());
    server->debug("取消登陆会话 [%p,%s]", session, session->getName());
    SAFE_DELETE(session);
}

bool zoLoginSessionManager::endSession(const DWORD sessionID)
{
    SessionAccountIter iter = _session_account_map.find(sessionID);
    if(iter == _session_account_map.end()){
        return false;
    }
    return endSession(iter->second.c_str());
}

bool zoLoginSessionManager::checkSession(const DWORD sessionID)
{
    SessionAccountIter iter = _session_account_map.find(sessionID);
    if(iter == _session_account_map.end()){
        return false;
    }
    return true;
}

bool zoLoginSessionManager::endSession(const char *accountName)
{
    zoLoginSession* session = getByName(accountName);
    if(session != NULL)
    {
        endSession(static_cast<zoLoginSession*>(session));
        return true;
    }
    return false;
}

zoLoginSession* zoLoginSessionManager::getSessionByAccID(const char *accountName)
{
    return getByName(accountName);
}

namespace
{
    struct CheckLoginSessionRemoveExec
    {
        Tidy::vector<zoLoginSession*> recycleSessionList;
        const Tidy::nServerConn* _gateway_conn;
        CheckLoginSessionRemoveExec(const Tidy::nServerConn* gatewayConn) : _gateway_conn(gatewayConn)
        {
        }
        bool exec(zoLoginSession *session)
        {
            if(session->getGatewayConn() == _gateway_conn)
            {
                recycleSessionList.push_back(session);
            }
            return true;
        }
    };
}

void zoLoginSessionManager::dealGatewayClose(Tidy::nServerConn* gatewayConn)
{
    CheckLoginSessionRemoveExec exec(gatewayConn);
    execAll(exec);
    if(exec.recycleSessionList.empty() == false) {
        for(DWORD i = 0; i < exec.recycleSessionList.size(); ++ i) {
            //TODO 如果是还没有在网关上创建成功的session,需要通知平台
            endSession(exec.recycleSessionList[i]);
        }
    }
}
