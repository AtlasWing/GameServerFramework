#ifndef _ZONE_LOGIN_SESSION_MANAGER_H_
#define _ZONE_LOGIN_SESSION_MANAGER_H_

#include "base/nManager.h"
#include "base/nTimer.h"

#include "zoneServer/zoLoginSession.h"

class zoPlatformConn;
namespace protocol{
	namespace platform{
		class LoginSessionData;
	};
};
class zoLoginSessionManager : public Tidy::nManager<true, Tidy::nIndexByName<Tidy::CommonGetNamePolicy<zoLoginSession> > >
{
    public:
        zoLoginSessionManager();
        ~zoLoginSessionManager();

    public:
        const zoLoginSession* startSession(const DWORD sessionID, const char *accountName, const Tidy::nServerConn* gatewayConn, const zoLoginSession::SessionState state);
        void endSession(const zoLoginSession* session);
        bool endSession(const char *accountName);
        bool endSession(const DWORD sessionID);
        zoLoginSession* getSessionByAccID(const char *accountName);
		//bool checkInLoginSession(const char *accountName, zoPlatformConn *platformConn, const protocol::platform::LoginSessionData &data);
		void dealGatewayClose(Tidy::nServerConn* gatewayConn);
        bool checkSession(const DWORD sessionID);
    private:
        typedef std::map<DWORD, std::string> SessionAccountMap;
        typedef SessionAccountMap::iterator SessionAccountIter;
        SessionAccountMap _session_account_map;
};
#endif
