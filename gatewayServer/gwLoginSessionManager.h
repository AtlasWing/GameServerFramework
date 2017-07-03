#ifndef _GATEWAY_LOGIN_SESSION_MANAGER_H_
#define _GATEWAY_LOGIN_SESSION_MANAGER_H_

#include "base/nManager.h"
#include "base/nRWLock.h"
#include "base/nTimer.h"

#include "gatewayServer/gwLoginSession.h"

class gwUserTaskConn;

class gwLoginSessionManager : public Tidy::nManager<true, Tidy::nIndexByID< Tidy::CommonGetIDPolicy<gwLoginSession> >, Tidy::nIndexByName< Tidy::CommonGetNamePolicy<gwLoginSession> > >
{
    public:
        gwLoginSessionManager();
        ~gwLoginSessionManager();

    public:
        const gwLoginSession* startSession(const protocol::login::LoginSession& session, const char *platformLoginData, const DWORD platformLoginDataSize);
        bool verifyAccount(gwUserTaskConn *conn, const std::string &accountName);
        bool endLoginSession(const std::string &accountName);
        bool doTick();
        void destroyAllLoginSession();

    private:
        bool checkLoginSessionTimeOut();

    private:
        Tidy::nRWLock _lock;
        Tidy::nTimer _check_login_session_timer;
};
#endif
