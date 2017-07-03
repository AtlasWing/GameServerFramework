#ifndef DATASERVER_DTUSER_H_
#define DATASERVER_DTUSER_H_

#include "base/nTimer.h"
#include "base/nSTL.h"

#include "protocol/serializeCommon.h"
#include "protocol/Proto/UserData.pb.h"

#include "game/nUser.h"
#include "game/nSsqls.h"
#include "game/nEvent.h"

namespace protocol{
    namespace base{
        struct UserCommandBase;
    };
    namespace login{
        struct LoginUserSerializeData;
        struct UserLoginData;
        struct RoleInfo;
    };
    namespace base {
        struct NullProtoCommand;
    };
    namespace user {
        struct LoginSerializeBinary;
        struct UserSaveData;
        struct MainUserData;
    }
};

namespace Tidy{
        class nServerConn;
};
struct AsyncSqlCallback;

enum eDataUserBoolDataType{
    eDataUserBoolDataType_IsWaitFetchMailAttachment = 0,
    eDataUserBoolDataType_MAX
};
class dtGMCommand;

class dtUser : public Game::nUser<dtUser>, public Tidy::nObjLogger<dtUser>
{
    public:
        dtUser(const DWORD id, const char *accountName, const char* name, Tidy::nServerConn *logicConn, Tidy::nServerConn *gatewayConn);
        virtual ~dtUser();

    public:
        DWORD getCharID() const;
        const char* getAccountName() const;
        DWORD getID() const;
        const char* getName() const;
        void setName(const char *name);
        Tidy::nLogger* getLogger() const;
        const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;

    public:
        bool doTick();

    public:
        bool reg(const protocol::login::UserLoginData& data);
        void unreg();

    public:
        void checkFlush(AsyncSqlCallback *completeCallback);
        bool fillLoginData(protocol::login::UserLoginData *loginData, protocol::user::MainUserData *mainUserData, protocol::login::RoleInfo *charInfo, protocol::user::LoginSerializeBinary *loginSerializeBinary); 
        void clearChangeLogicData();
        void updateFightPower(const DWORD fightPower);

    public:
        bool parseProtoUserCommand(Tidy::nServerConn* serverConn, const protocol::base::UserCommandBase* nullCommand, const DWORD len);

    public:
        Tidy::nServerConn* getLogicConn() const;
        void setLogicConn(Tidy::nServerConn* logicConn);
        Tidy::nServerConn* getGatewayConn() const;
        void setGatewayConn(Tidy::nServerConn* gatewayConn);

    public:
        void dealLoginSuccess();
        bool syncLoginState(const BYTE newState);
        void updateUserData(protocol::user::UserSaveData &saveData);

    public:
        const DWORD getLastFetchMailID() const;
        void setLastFetchMailID(const DWORD mailID);
        void incUnreadMailNum();
        void decUnreadMailNum();
        void clearUnreadMailNum();
        bool sendUnreadMailNum();

    public:
        void setMiscBoolData(eDataUserBoolDataType data);
        void clearMiscBoolData(eDataUserBoolDataType data);
        bool checkMiscBoolData(eDataUserBoolDataType data) const;

    public:
        void setLevel(const WORD level);
        void setIsAdult(bool isAdult);
        BYTE getLevel() const;

    private:
        bool restoreCharBaseData();
        bool saveCharBaseData(AsyncSqlCallback *completeCallback);
        bool restoreRechargeMoney();
        bool drawRechargeMoney(const DWORD rechargeMoney);

    private:
        void updateLoginTime();
        void updateOnlineTime();
        void setLastLoginIP(const char *ip);
        const char *getLastLoginIP() const;

    private:
        Tidy::nServerConn *_logic_conn;
        Tidy::nServerConn *_gateway_conn;
        Tidy::nTimer _one_min;

    private:
        bool _need_flush_char_base;
        SqlCharBase _char_base_table;
        std::bitset<SqlCharBase_NULL> _char_base_table_save_flag;
        DWORD _recharge_money;
        Tidy::vector<char> *_change_logic_binary;

    private:
        BYTE _login_state;
        WORD _level;
        DWORD _last_fetch_mail_max_id;
        Tidy::BitSet<eDataUserBoolDataType_MAX> _user_misc_bool_data_set;
        DWORD _unread_mail_num;
        dtGMCommand* _gm_command; 
};

#endif
