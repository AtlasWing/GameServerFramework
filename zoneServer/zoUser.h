#ifndef ZONESERVER_ZOUSER_H_
#define ZONESERVER_ZOUSER_H_

#include "game/nUser.h"
#include "protocol/relationCommon.h"

namespace protocol{
    namespace login{
        struct ReturnCharacterList;
    };
    namespace base {
        struct NullProtoCommand;
        struct UserCommandBase;
    };
    namespace user{
        struct LoginSerializeBinary;
    };
};

namespace Tidy{
	class nServerConn;
};

class zoRelationUser;
class zoUser : public Game::nUser<zoUser>, public Tidy::nObj
{
    public:
        zoUser(zoRelationUser *ownerRelationUser, const DWORD id, const char *accountName, const char* name, Tidy::nServerConn* gatewayConn, Tidy::nServerConn *logicConn);
        virtual ~zoUser();

    public:
        DWORD getCharID() const;
        const char* getAccountName() const;
        void setName(const char *name);

    public:
        bool reg();
        void unreg();
		void setWaitLogout();
		bool isWaitLogout() const;
        void doTick();
        void dealZeroClock();

    public:
        Tidy::nServerConn* getGatewayConn() const;
        DWORD getGatewayConnID() const;
        void setGatewayConn(Tidy::nServerConn* gatewayConn);
        Tidy::nServerConn* getLogicConn() const;
        void setLogicConn(Tidy::nServerConn* logicConn);
		void addConnUserNum();

    public:
        bool parseProtoUserCommand(Tidy::nServerConn* serverConn, const protocol::base::UserCommandBase* nullCommand, const DWORD len);
        void setLoginData(const protocol::user::LoginSerializeBinary *binary);

    public:
        void dealLoginSuccess();
        void dealChangeLogicSuccess();
        void dealResumeOnline();
        void checkSaveData();

    public:
        zoRelationUser *relationUser;

    private:
        std::string _account_name;
        Tidy::nServerConn *_gateway_conn;
        Tidy::nServerConn *_logic_conn;
		bool _is_wait_logout;
        Tidy::nTimer _save_data_timer;
};

#endif
