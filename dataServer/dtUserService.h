#ifndef _DATA_USER_SERVICE_H_
#define _DATA_USER_SERVICE_H_

#include "base/nTimer.h"

#include "game/nUserManager.h"

namespace Tidy{
    class nServerConn;
};
class dtUser;

namespace protocol{
    namespace login{
        struct UserLoginData;
        struct UserLogoutData;
        struct CreateCharacterData;
    };
    namespace admin{
        struct ZoneData;
    };
    namespace user{
        struct LogoutCallbackData;
    };
};

class dtUserService : public Game::nUserManager<dtUser, Tidy::nManager<true, Tidy::nIndexByID< Game::UserGetCharIDPolicy<dtUser> >, Tidy::nIndexByName< Tidy::CommonGetNamePolicy<dtUser> > >, false>
{
    public:
        dtUserService();
        ~dtUserService() ;

    public:
        void doTick();
        bool initService();
        void finalService();
        void dealCore();

    public:
        dtUser* loginUser(const protocol::login::UserLoginData& data);
        bool logoutUser(dtUser* user, const BYTE logoutType, const protocol::login::UserLogoutData *logoutData, const char *data, const DWORD num);
        bool unregUser(dtUser* user);
        bool startLogoutUser(dtUser* user, const BYTE logoutType, const char *data, const DWORD num);

    public:
        bool createRole(const char *accountName, const BYTE sex, const DWORD charID, const char *name);
        bool deleteCharacter(const DWORD charID, BYTE& errorCode, bool realDeleteData);
        bool readCharList(const DWORD sessionID, const DWORD srcPlatformID, const char *accountName, const DWORD channelID, const char *ip, const Tidy::nServerConn *gatewayConn);
        const std::string* getUserNameByID(const DWORD charID) const;
        bool getUserIDByName(const char *name, DWORD *id) const;
        bool getUserIDByAccountName(const std::string &accountName, DWORD *id) const;
        bool checkUserIDValid(const DWORD userID) const;
        bool checkUserDataSaveComplete(const DWORD charID) const;
        void dealCompleteSaveUserData(const DWORD charID);
        void dealStartSaveUserData(const DWORD charID);
        bool getOnlineUserIDList(std::vector<DWORD> *userList) const;
        void requestLogoutUser(dtUser *user, const BYTE logoutType, const protocol::user::LogoutCallbackData *callbackData);
        bool createAccount(const DWORD srcPlatformID, const char *accountName);
        bool getAllHumanUserIDList(std::vector<DWORD> *userList) const;
        bool getOnlineHumanUserIDList(std::vector<DWORD> *userList) const;

    public:
        void dealConnectClose(Tidy::nServerConn *conn);
        void dealZoneStart(const protocol::admin::ZoneData *zoneData);
        void dealZoneClose(const protocol::admin::ZoneData *zoneData);

    public:
        bool parseProtoUserCommand(Tidy::nServerConn* serverConn, const protocol::base::UserCommandBase* nullCommand, const DWORD len);

    private:
        void dealLogicClose(Tidy::nServerConn *logicConn);
        void dealGatewayClose(Tidy::nServerConn *gatewayConn);

    private:
        void logoutAll(const BYTE logoutSource);

    private:
        struct UserNameData {
            std::string name;
            std::string accountName;
            UserNameData(const std::string &userName, const std::string &userAccountName):name(userName), accountName(userAccountName) {
            }
        };
        Tidy::nTimer _tick_timer;
        typedef Tidy::hash_map<DWORD, UserNameData> IDNameMap;
        IDNameMap _user_id_name_map;
        typedef Tidy::hash_map<const char*, DWORD> NameIDMap;
        NameIDMap _user_name_id_map;
        NameIDMap _account_name_id_map;
        typedef std::set<DWORD> UserSet;
        UserSet _wait_complete_save_data_user_set;
};
#endif
