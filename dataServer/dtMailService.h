#ifndef _DATA_MAIL_SERVICE_H_
#define _DATA_MAIL_SERVICE_H_

#include "base/Tidy.h"
#include "base/nTimer.h"

#include "game/nMailService.h"

class dtUser;
class dtServer;
namespace Tidy{
	class nDBQuery;
	class nServerConn;
};

namespace protocol{
    namespace base{
        struct MailCommandBase;
    };
};

namespace protocol{
	namespace mail{
		struct CreateMailData;
	};
};
class dtMailService : public Game::nMailService<dtServer>
{
    public:
        dtMailService();
        ~dtMailService();

    public:
        bool initService();
        void finalService();
        bool doTick();

    public:
        bool parseProtoMailCommand(Tidy::nServerConn* serverConn, const protocol::base::MailCommandBase* nullCommand, const unsigned int len);

    public:
        bool createMail(const BYTE mailType, const DWORD fromID, const DWORD *destIDList, const DWORD destIDListNum, const char* title, const char* mainText, const char* moneyData = NULL, const DWORD moneyDataLen = 0, const char* itemData = NULL, const DWORD itemDataLen = 0);
        bool createMail(const protocol::mail::CreateMailData &mailData, const DWORD *destIDList, const DWORD destIDListNum);
        bool fetchMailList(dtUser* user);
        bool fetchMail(dtUser* user, const DWORD mailID);
        bool fetchMailAttachment(dtUser* user, const DWORD mailID);
        bool fetchMailAttachmentAll(dtUser* user);
        bool deleteMail(dtUser* user, const DWORD mailID);
        bool deleteAllMail(dtUser* user);
        bool deleteAllMail(const DWORD charID, const bool isRobot, Tidy::nDBQuery &query);
        const DWORD getNewMailNum(dtUser* user);
        bool dealDeleteAllRobot();

    private: 
        const DWORD getMinEffectMailTime();
        bool deleteAllTimeoutMail();
        bool dealFetchMailAttachmentFail(const DWORD userID);
        bool dealFetchMailAttachmentSucc(const DWORD userID, const DWORD mailID);

    private:
		std::string _sys_mail_title;
        Tidy::nClocker _delete_timeout_mail_clocker;
};
#endif
