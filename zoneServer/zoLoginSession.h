#ifndef _ZONE_LOGIN_SESSION_H_
#define _ZONE_LOGIN_SESSION_H_

#include "base/nObj.h"

namespace protocol {
    namespace platform {
        struct LoginSession;
        class LoginSessionData;
    };
};

namespace Tidy{
	class nServerConn;
};

class zoPlatformConn;
class zoLoginSession
{
    public:
        enum SessionState
        {
            SessionState_WaitGatewayCreate  =   0,
            SessionState_WaitLogin          =   1,
        };

    public:
        zoLoginSession(const DWORD sessionID, const char *accountName, const Tidy::nServerConn *gatewayConn, const SessionState state);
        ~zoLoginSession();

    public:
        const char* getName() const;
        const DWORD getClientID() const;
        const DWORD getPlatformID() const;
        void setSessionState(const zoLoginSession::SessionState state);
        const Tidy::nServerConn* getGatewayConn() const;
		BYTE getNetType() const;

	public:
		bool fillGatewayLoginData(const DWORD maxGatewayIPLen, char *gatewayIP, WORD *port) const;

    private:
        const std::string _account_name;
        DWORD _client_id;
		BYTE _net_type;
        DWORD _platform_ID;
        const Tidy::nServerConn* const _gateway_conn;
        BYTE _session_state;
        DWORD _session_id ;
};
#endif
