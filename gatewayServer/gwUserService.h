#ifndef _GATEWAY_USER_MANAGER_H_
#define _GATEWAY_USER_MANAGER_H_

#include "protocol/loginCommon.h"

#include "base/nHashListTable.h"

#include "game/nUserManager.h"

#include <vector>
#include <queue>

class gwUser;
namespace Tidy{
	class nServerConn;
};
class gwUserTaskConn;

namespace protocol {
    namespace login {
        struct UserLogoutData;
    };
};

namespace Tidy{
	class nScopeRDLock;
};
namespace Tidy{
	class nScopeWRLock;
};
typedef Game::nUserManager<gwUser, Tidy::nManager<true, Tidy::nIndexByID< Game::UserGetCharIDPolicy<gwUser>, Tidy::nHashListTable<DWORD, gwUser*, Game::UserGetCharIDPolicy<gwUser> > > >, true> gwUserManagerBase;
class gwUserService : public gwUserManagerBase
{
    public:
        gwUserService();
        virtual ~gwUserService();

    public:
        gwUser* getUserByID(const DWORD userID, Tidy::nScopeRDLock *rdLock);
        gwUser* getUserByID(const DWORD userID, Tidy::nScopeWRLock *wrLock);
        bool doTick();

    public:
        bool logoutUser(gwUser *unregedUser, const DWORD userID, protocol::login::eUserLogOutSourceType logoutSource, const DWORD sessionID);
        bool requestLogoutUser(gwUser *unregedUser, const DWORD userID, protocol::login::eUserLogOutSourceType logoutSource);

    public:
        gwUser* getUserByAccountName(const char *accountName, Tidy::nScopeRDLock *rdLock) const;
        gwUser* getUserByAccountName(const char *accountName, Tidy::nScopeWRLock *wrLock) const;
        bool addUser(const gwUser* user);
        bool removeUser(const gwUser* user);

    public:
        void dealConnectClose(Tidy::nServerConn *conn);

    public:
        bool parseProtoUserCommand(Tidy::nServerConn* serverConn, const protocol::base::UserCommandBase* nullCommand, const DWORD len);
        bool sendCmdToAll();

    public:
        bool regUser(gwUser* user, const protocol::login::UserLoginData &userData);
        bool unregUser(const DWORD userID);
		void setNeedSyncTime();
        void dealServerClose();

    private:
        void dealAIClose(Tidy::nServerConn *aiConn);

    private:
        typedef Tidy::hash_map<const char*, gwUser*> UserAccidMap;
        Tidy::nRWLock _user_accid_map_lock;
        UserAccidMap _user_map_by_accid;
        Tidy::nMutex _unreg_user_lock;
};

#endif
