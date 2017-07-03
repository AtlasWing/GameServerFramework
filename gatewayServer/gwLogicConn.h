#ifndef _GATEWAY_LOGIC_CONN_H_
#define _GATEWAY_LOGIC_CONN_H_

#include "base/nClientConn.h"
#include "base/nRWLock.h"

class gwUser;
class gwLogicConn : public Tidy::nClientConnNoQueue
{
    public:
        gwLogicConn(const Tidy::nConnectServerData &connectServerData);
		virtual ~gwLogicConn();

    public:
        void dealClose();

};
#endif
