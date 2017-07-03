#ifndef _GATEWAY_LOGIN_SERVICE_H_
#define _GATEWAY_LOGIN_SERVICE_H_

#include "base/nObjLogger.h"
#include "base/nServerTickTimer.h"
#include "base/nRWLock.h"
#include "base/nSTL.h"

namespace protocol{ 
    struct NullProtoCommand;
    namespace base{
        struct LoginCommandBase;
        struct PlatformCommandBase;
    };
};
namespace command{
    namespace player{
        struct CPlayerData;
    };
};
namespace Tidy{
	class nServerConn;
};
namespace Tidy{
	class nTaskConn;
};
class gwLoginSessionManager;
class gwUserTaskConn;
class gwUser;

class gwLoginService : public Tidy::nObjLogger<gwLoginService>
{
    public:
        gwLoginService();
        ~gwLoginService();

    public:
        const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;
        Tidy::nLogger* getLogger() const;

    public:
        bool initService();
        void finalService();
        bool doTick();
        void dealConnectSuccess(Tidy::nServerConn* conn);
        void dealConnectClose(Tidy::nServerConn* conn);

    public:
        bool parseProtoLoginCommand(Tidy::nServerConn* serverConn, protocol::base::LoginCommandBase* baseCmd, const unsigned int len);

    public:
        bool verifyUserClientConn(Tidy::nTaskConn* conn, const protocol::NullProtoCommand* ptNullCmd, const DWORD len);
        void requestCharList(gwUserTaskConn *userConn);
        void startOfflineProtect(gwUser *user);
        void endOfflineProtect(gwUser *user);
        bool endLoginSession(const std::string &accountName);
        void fillPlayerData(std::vector<command::player::CPlayerData> *playerData, const char* data, const WORD num, const DWORD bitMisk);

    private:
		bool verifyVersion(gwUserTaskConn *conn, const BYTE versionType, const DWORD versionNum);
		bool verifyLoginSession(gwUserTaskConn *conn, const std::string &accountName);
        bool checkOfflineTimeOut();
        bool closeLogin();
        void closeAllUserConn();

    private:
        gwLoginSessionManager* _login_session_manager;
        typedef std::pair<DWORD, DWORD> OfflineProtectUserData;
        typedef Tidy::map<std::string, OfflineProtectUserData > OfflineProtectUserMap;
        OfflineProtectUserMap _offline_protect_user_map;
        Tidy::nRWLock _offline_protect_user_map_lock;
        Tidy::nServerTickTimer _one_sec_timer;

    public:
        static DWORD maxOfflineProtectTimeInSec;
};
#endif
