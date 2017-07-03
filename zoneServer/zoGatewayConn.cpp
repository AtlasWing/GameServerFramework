#include "base/nMisc.h"
#include "base/Tidy.h"

#include "zoneServer/zoGatewayConn.h"
#include "zoneServer/zoServer.h"

zoGatewayConn::zoGatewayConn(Tidy::nServerConn* conn):
    baseConn(conn),
    _user_num(0),
    _server_info(NULL)
{
    _server_info = server->getServerEntityByID(baseConn->getID());
    assert_debug(_server_info != NULL, "");
}

zoGatewayConn::~zoGatewayConn()
{
}

DWORD zoGatewayConn::getUserNum() const
{
    return _user_num;
}

void zoGatewayConn::incUserNum()
{
    ++ _user_num;
	server->debug("GatewayConn承载数增加，连接：[%u]当前数量[%u]", baseConn->getID(), _user_num);
}

void zoGatewayConn::decUserNum()
{
    if(_user_num == 0){
        server->debug("GatewayConn承载数为0已经不能再减少了[%u]", baseConn->getID());
        return;
    }
    -- _user_num;
    server->debug("GatewayConn承载数减少，连接：[%u]当前数量[%u]", baseConn->getID(), _user_num);
}

bool zoGatewayConn::checkNetType(const BYTE netType) const
{
    if(netType == eNetType_None){
        return true;
    }
    for(DWORD i = 0; i < _server_info->listenDataNum; ++ i){
        if(_server_info->listenDataList[i].netType == eNetType_None || _server_info->listenDataList[i].netType == netType){
            return true;
        }
    }
    return false;
}

bool zoGatewayConn::fillGatewayLoginData(const DWORD maxGatewayUrlLen, char *gatewayUrl, WORD *port) const
{
    *port = 0;
    for(DWORD i = 0; i < _server_info->listenDataNum; ++ i){
        if((*port) == 0){
            copyStr(gatewayUrl, _server_info->listenDataList[i].url, maxGatewayUrlLen);
            *port = _server_info->listenDataList[i].port;
            if(_server_info->listenDataList[i].netType == 2){
                *port = 10982;
            }
        }
    }
    if((*port) == 0){
        copyStr(gatewayUrl, _server_info->internalIP, maxGatewayUrlLen);
        *port = _server_info->internalPort;
    }
	return true;
}

void zoGatewayConn::clearUserNum()
{
	_user_num = 0;
}

void zoGatewayConn::setUserNum(const DWORD num)
{
	_user_num = num;
}
