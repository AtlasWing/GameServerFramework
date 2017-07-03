#ifndef LOGICSERVER_LG_TEST_SERVICE_H_
#define LOGICSERVER_LG_TEST_SERVICE_H_

#include "base/nObjLogger.h"

namespace protocol{
    struct NullProtoCommand;
};

namespace Tidy{
	class nServerConn;
};

class gwTelnetService : public Tidy::nObjLogger<gwTelnetService>
{
    public:
        gwTelnetService();
        ~gwTelnetService();

    public:
        const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;
        Tidy::nLogger* getLogger() const;

    public:
        bool initService();
        void finalService();

    public:
        bool parseProtoCommand(Tidy::nServerConn* serverConn, const void* nullCommand, const DWORD len);
};

#endif
