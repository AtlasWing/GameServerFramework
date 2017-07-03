#ifndef ZONESERVER_LOGICCONN_H_
#define ZONESERVER_LOGICCONN_H_

#include "base/nType.h"

namespace Tidy{
	class nServerConn;
};

class zoLogicConn
{
    public:
        zoLogicConn(Tidy::nServerConn* conn);
        ~zoLogicConn();

    public:
        DWORD getRoomNum() const;
        DWORD getUserNum() const;
        void incRoomNum();
        void decRoomNum();
        void incUserNum();
        void decUserNum();
		void clearUserNum();
		void setUserNum(const DWORD num);

    public:
        Tidy::nServerConn* const baseConn;

    private:
        DWORD _room_num;
        DWORD _user_num;
};
#endif
