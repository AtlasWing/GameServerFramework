#include "protocol/loginCommand.h"

#include "gatewayServer/gwLoginSession.h"
#include "gatewayServer/gwServer.h"

gwLoginSession::gwLoginSession(const protocol::login::LoginSession& session, const char *platformLoginData, const DWORD platformLoginDataSize) : 
    _start_time(server->time().sec()),
    _login_session_data(session),
    _platform_login_data(platformLoginData, platformLoginDataSize)
{
}

gwLoginSession::~gwLoginSession()
{
}

DWORD gwLoginSession::getID() const
{
    return _login_session_data.loginSessionID;
}

const char* gwLoginSession::getName() const
{
    return _login_session_data.accountName;
}

const bool gwLoginSession::checkTimeOut() const
{
    if(server->time().sec() < (_start_time + Tidy::global["LOGIN_SESSION_TIME_OUT_IN_SEC"].getDword())) {
        return false;
    }
    return true;
}

void gwLoginSession::notifyTimeOut() const
{
}

const protocol::login::LoginSession& gwLoginSession::getData() const
{
    return _login_session_data;
}

const std::string& gwLoginSession::getPlatformLoginData() const
{
    return _platform_login_data;
}

DWORD gwLoginSession::getStartTimeInSec() const
{
	return _start_time;
}
