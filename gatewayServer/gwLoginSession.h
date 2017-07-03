#ifndef _GATEWAY_LOGIN_SESSION_H_
#define _GATEWAY_LOGIN_SESSION_H_

#include "protocol/loginCommon.h"

#include "base/nObj.h"

class gwLoginSession
{
    public:
        gwLoginSession(const protocol::login::LoginSession& session, const char *platformLoginData, const DWORD platformLoginDataSize);
        ~gwLoginSession();

    public:
        DWORD getID() const;
        const char* getName() const;
        const protocol::login::LoginSession& getData() const;
		const std::string& getPlatformLoginData() const;
		DWORD getStartTimeInSec() const;

    public:
        const bool checkTimeOut() const;
        void notifyTimeOut() const;

    private:
        DWORD _start_time;
        const protocol::login::LoginSession _login_session_data;
        const std::string _platform_login_data;
};
#endif
