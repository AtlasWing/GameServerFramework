#ifndef _ZONE_USER_MANAGER_H_
#define _ZONE_USER_MANAGER_H_

#include "game/nUserManager.h"
#include "base/nTimer.h"

class zoUser;
namespace Tidy{
	class nServerConn;
};

namespace protocol {
    namespace base {
        struct UserCommandBase;
    };
    namespace login {
        struct UserLoginData;
        struct UserLogoutData;
    };
    namespace user {
        struct ChangeUserNameCommand;
    };
};

class zoUserService : public Game::nUserManager<zoUser, Tidy::nManager<true, Tidy::nIndexByID< Game::UserGetCharIDPolicy<zoUser> >, Tidy::nIndexByName< Tidy::CommonGetNamePolicy<zoUser> > >, false>
{
	public:
		zoUserService();
		~zoUserService() ;

    public:
        zoUser* getUserByAccountName(const char *accountName) const;
        zoUser* createUser(protocol::login::UserLoginData *data);
        bool logoutUser(const protocol::login::UserLogoutData& data);
        bool rollBackLoginUser(const protocol::login::UserLogoutData& data);
        bool rollBackChangeServer(const DWORD charID);
        bool unregUser(zoUser* user);
		DWORD getNormalUserNum() const;

    public:
        void doTick();
        void dealGatewayClose(Tidy::nServerConn *gatewayConn);
        void dealLogicClose(Tidy::nServerConn *logicConn);
		void addCoreDownRecycle(const DWORD charID);
		void removeCoreDownRecycle(const DWORD charID);
		bool checkCoreDownRecycle(const DWORD charID);
		void resetConnUserNum();
        bool dealZeroClock(const bool isNormalClocker, const DWORD ringTimes);

    public:
        bool parseProtoUserCommand(Tidy::nServerConn* serverConn, const protocol::base::UserCommandBase* nullCommand, const DWORD len);

    private:
        typedef Tidy::hash_map<const char*, zoUser*> UserAccidMap;
        UserAccidMap _user_map_by_accid;
		Tidy::hash_set<DWORD> _core_down_recycle_set;
        Tidy::nTimer _tick_timer;
};
#endif

