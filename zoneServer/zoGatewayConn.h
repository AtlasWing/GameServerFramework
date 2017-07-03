#ifndef ZONESERVER_GATEWAYCONN_H_
#define ZONESERVER_GATEWAYCONN_H_

#include "base/nType.h"

namespace Tidy{
	class nServerConn;
};
namespace protocol{
    namespace ServerRun{
        struct ServerEntity;
    };
};
class zoGatewayConn
{
    public:
        zoGatewayConn(Tidy::nServerConn* conn);
        ~zoGatewayConn();

    public:
        DWORD getUserNum() const;
        void incUserNum();
        void decUserNum();
		void clearUserNum();
		void setUserNum(const DWORD num);
        bool checkNetType(const BYTE netType) const;
		bool fillGatewayLoginData(const DWORD maxGatewayIPLen, char *gatewayUrl, WORD *port) const;

    public:
        Tidy::nServerConn* const baseConn;
    
    private:
        DWORD _user_num;
        const protocol::ServerRun::ServerEntity *_server_info;
};
#endif
