#ifndef _ZO_LOGIN_SERVICE_H_
#define _ZO_LOGIN_SERVICE_H_

#include "base/nObjLogger.h"

class zoLoginSessionManager;
class zoPlatformConn;
class zoUser;
class zoLoginSession;

namespace Tidy{
	class nServerConn;
};

namespace protocol{
    namespace base{
        struct LoginCommandBase;
        struct PlatformCommandBase;
    };
    namespace login{
        struct UserLoginData;
    };
    namespace platform{
        struct StartLoginSessionCommand;
        class LoginSessionData;
    };
};

enum eServerLoginState{
    eServerLoginState_None,
    eServerLoginState_Normal,
    eServerLoginState_WaitClose,
    eServerLoginState_Closed,
};
class zoLoginService : public Tidy::nObjLogger<zoLoginService>
{
    public:
        zoLoginService();
        ~zoLoginService();

    public:
        const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;
        Tidy::nLogger* getLogger() const;

    public:
        bool initService();
        void finalService();

    public:
        void dealGatewayClose(Tidy::nServerConn* gatewayConn);
        bool openLogin();
        bool closeLogin();

    public:
        bool parseUserLoginCommand(Tidy::nServerConn* serverConn, const protocol::base::LoginCommandBase* nullCommand, const DWORD len);
        bool parsePlatformCommand(Tidy::nServerConn* serverConn, const protocol::base::PlatformCommandBase *nullCommand, const DWORD len);

    public:
        BYTE startLoginSession(const char *accountName, const DWORD sessionID, DWORD *gatewayConnID);
        void setMaxZoneUserNum(const DWORD maxZoneUserNum);
        DWORD getLoginSessionID();
        zoLoginSession* getSessionByAccountName(const char *accountName) const;
        bool endSessionByID(const DWORD sessionID);
        bool checkSession(const DWORD sessionID);

	private:
		//bool reloginUser(zoUser *user, zoPlatformConn *platformConn, zoClientTaskConn *userClientConn, const protocol::platform::LoginSessionData &data);
		bool dealStartLoginSessionSucc(const char *accountName, BYTE netType, DWORD clientID);
		bool checkStartLoginSession() const;

    private:
        zoLoginSessionManager* _login_session_manager;
        DWORD _max_zone_user_num;
        DWORD _login_session_id;
        BYTE _server_login_state;
};
#endif
