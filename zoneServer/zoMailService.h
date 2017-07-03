#ifndef _ZONE_MAIL_SERVICE_H_
#define _ZONE_MAIL_SERVICE_H_

#include "game/nMailService.h"

class zoServer;
class zoUser;
extern zoServer* server;
namespace protocol{
    namespace mail{
        struct ItemData;
    };
};

class zoMailService : public Game::nMailService<zoServer>
{
    public:
        zoMailService();
        virtual ~zoMailService();

    public:
        bool initService();
        bool finalService();

    public:
        bool parseProtoMailCommand(Tidy::nServerConn* conn, const protocol::base::MailCommandBase* nullCommand, const DWORD len);

    private:
		std::string _sys_mail_title;
		std::string _sys_mail_content;
};
#endif
