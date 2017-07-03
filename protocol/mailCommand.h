#ifndef _PROTOCOL_MAIL_COMMAND_H_
#define _PROTOCOL_MAIL_COMMAND_H_

#include "baseCommand.h"

#pragma pack(1)

namespace protocol
{
    namespace mail
    {
        struct CreateMailCommand : public NullProtoCommandBase(base::MailCommand, 1)
			DWORD destID;
            WORD num;
            char  data[0];
            DWORD allSize() const {
                return sizeof(*this) + num * sizeof(data[0]);
            }
        };

        struct FetchMailTitleListCommand : public NullProtoCommandBase(base::MailCommand, 2)
            DWORD userID;
        };

        struct FetchMailAttachmentCommand : public NullProtoCommandBase(base::MailCommand, 3)
            DWORD userID;
            DWORD mailID;
            WORD num;
            char data[0]; // protocol::mail::MoneyAndItemData
            const DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
        };

        struct FetchMailAttachmentFailCommand : public NullProtoCommandBase(base::MailCommand, 4)
            DWORD userID;
        };
        struct FetchMailAttachmentSuccCommand : public NullProtoCommandBase(base::MailCommand, 5)
            DWORD userID;
            DWORD mailID;
        };

        struct SendSystemMailCommand : public NullProtoCommandBase(base::MailCommand, 6)
            const DWORD allSize() const {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            BYTE srcPlatformType;
            DWORD srcConnID;
            DWORD num;
            char data[0];   //protocol::mail::SendSysMailData
        };

    };
};

#pragma pack()
#endif
