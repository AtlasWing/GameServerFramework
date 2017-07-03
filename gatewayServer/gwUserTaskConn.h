#ifndef GATEWAYSERVER_GWUSERTASKCONN_H
#define GATEWAYSERVER_GWUSERTASKCONN_H

#include "protocol/loginCommon.h"

#include "base/nTaskConn.h"
#include "base/nSTL.h"

class gwUser;
class gwLoginSession;

namespace protocol {
    namespace platform {
        struct LoginSession;
    };
};
namespace Tidy{
	class nScopeWRLock;
};
namespace Tidy{
	class nScopeRDLock;
};

class gwUserTaskConn : public Tidy::nTaskConnNoQueue
{
    public:
        gwUserTaskConn(const int sock, const struct sockaddr *addr, const struct Tidy::SocketPortData* listenPortData);
        virtual ~gwUserTaskConn();

    public:
		void setLoginSession(const gwLoginSession *loginSession);
		const gwLoginSession* getLoginSession() const;
		void clearLoginSession();

    public:
        void setUser(gwUser *user);
        gwUser* getUser(Tidy::nScopeRDLock *rdLock);
        gwUser* getUser(Tidy::nScopeWRLock *wrLock);
        DWORD getUserID() const;
        void addCharacter(const protocol::login::UserCharacterData &userData);
        void removeCharacter( DWORD charID );
        void clearCharacterList();
		void deleteCharacter(const DWORD charID, const BYTE errorCode);
		bool hasCharacter() const;
        const protocol::login::UserCharacterData* getSelectedCharacter(const DWORD charID) const;

    public:
        const char* getAccountName() const;
        bool getIsAdult() const;
        DWORD getChannelID() const;
        DWORD getPlatformType() const;
		bool checkHasLoadCharData() const;
		void setHasLoadCharData();
        bool hasPlatformLoginData() const;
        const std::string& getPlatformLoginData() const;
        void clearPlatformLoginData();
		std::string getInitCharName() const;
		DWORD getLoginSessionWaitTimeInSec() const;

    private:
        gwUser *_user;

        typedef Tidy::map< DWORD, protocol::login::UserCharacterData > UserInfoContainer;
        UserInfoContainer _user_info;
    private:
        Tidy::string _account_name;
        bool _is_adult;
        DWORD _channel_id;
		bool _has_load_char_data;
        DWORD _platform_type;
        std::string *_platform_login_data;// protocol::login::PlatformLoginData
		const gwLoginSession *_login_session;
};

#endif
