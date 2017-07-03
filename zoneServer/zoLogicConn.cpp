#include "base/nMisc.h"
#include "base/Tidy.h"

#include "zoneServer/zoLogicConn.h"
#include "zoneServer/zoServer.h"

zoLogicConn::zoLogicConn(Tidy::nServerConn* conn):
    baseConn(conn),
    _room_num(0),
    _user_num(0)
{
}

zoLogicConn::~zoLogicConn()
{
}

DWORD zoLogicConn::getRoomNum() const
{
    return _room_num;
}

DWORD zoLogicConn::getUserNum() const
{
    return _user_num;
}

void zoLogicConn::incRoomNum()
{
    ++ _room_num;
}

void zoLogicConn::decRoomNum()
{
    assert_debug(_room_num > 0, "");
    -- _room_num;
}

void zoLogicConn::incUserNum()
{
    ++ _user_num;
#ifdef _DEBUG
	server->debug("logicConn承载数增加，连接：[%u]当前数量[%u]", baseConn->getID(), _user_num);
#endif
}

void zoLogicConn::decUserNum()
{
    assert_debug(_user_num > 0, "");
    -- _user_num;
#ifdef _DEBUG
	server->debug("logicConn承载数减少，连接：[%u]当前数量[%u]", baseConn->getID(), _user_num);
#endif
}

void zoLogicConn::clearUserNum()
{
	_user_num = 0;
}

void zoLogicConn::setUserNum(const DWORD num)
{
	_user_num = num;
}
