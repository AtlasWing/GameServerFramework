#ifndef _DT_LOGIN_SERVICE_H_
#define _DT_LOGIN_SERVICE_H_

#include "base/nObjLogger.h"

namespace Tidy{
	class nServerConn;
};

namespace protocol{
    namespace base{
        struct LoginCommandBase;
    };
    namespace login{
        struct UserLoginData;
    };
};

class dtLoginService : public Tidy::nObjLogger<dtLoginService>
{
    public:
        dtLoginService();
        ~dtLoginService();

    public:
        const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;
        Tidy::nLogger* getLogger() const;
        bool checkAccountEmpty(const char *accountName);
        std::string getNewAccount() const;
        bool canCreateRole(const char *accountName);
        DWORD createNewCharID() const;
        void dealLoginCheckDebug(const char* accountName, protocol::login::UserLoginData *data);

    public:
        bool initService();
        void finalService();

    public:
        bool parseProtoLoginCommand(Tidy::nServerConn* serverConn, protocol::base::LoginCommandBase* nullCommand, const DWORD len);
};
#endif
