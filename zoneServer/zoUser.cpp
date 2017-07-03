#include "protocol/loginCommand.h"
#include "protocol/forwardCommand.h"
#include "protocol/serializeCommand.h"
#include "protocol/Proto/UserData.pb.h"

#include "base/nServerConn.h"

#include "zoneServer/zoServer.h"
#include "zoneServer/zoMessageService.h"
#include "zoneServer/zoUser.h"
#include "zoneServer/zoGatewayConn.h"
#include "zoneServer/zoLogicConn.h"
#include "zoneServer/zoRelationUserService.h"
#include "zoneServer/Relation/zoRelationUser.h"

zoUser::zoUser(zoRelationUser *ownerRelationUser, const DWORD id, const char *accountName, const char* name, Tidy::nServerConn* gatewayConn, Tidy::nServerConn *logicConn) :
    relationUser(ownerRelationUser),
    Tidy::nObj(id, name),
    _account_name(accountName),
    _gateway_conn(gatewayConn),
    _logic_conn(logicConn),
	_is_wait_logout(false),
    _save_data_timer(6 * ONE_SEC)
{
    debug("构造[%s]", accountName);
}

zoUser::~zoUser()
{
#ifdef _DEBUG
    debug("玩家析构");
#endif
}

const char* zoUser::getAccountName() const
{
    return _account_name.c_str();
}

DWORD zoUser::getCharID() const
{
    return getID();
}

void zoUser::setName(const char *name)
{
    Tidy::nObj::setName(name);
}

bool zoUser::reg()
{
    if(relationUser == NULL){
        assert_fail("重大问题，没有RelationUser");
        return false;
    }
    if(relationUser->reg(this) == false){
        return false;
    }
    return true;
}

void zoUser::unreg()
{
    relationUser->unreg(this);
}

void zoUser::setWaitLogout()
{
	_is_wait_logout = true;
}

bool zoUser::isWaitLogout() const
{
	return _is_wait_logout;
}

void zoUser::doTick()
{
    if(_save_data_timer.isReady(server->time()) == true){
        checkSaveData();
    }
    relationUser->doTick();
}

Tidy::nServerConn* zoUser::getGatewayConn() const
{
    return _gateway_conn;
}

DWORD zoUser::getGatewayConnID() const
{
    return _gateway_conn->getID();
}

void zoUser::setGatewayConn(Tidy::nServerConn* gatewayConn)
{
    _gateway_conn = gatewayConn;
}

Tidy::nServerConn* zoUser::getLogicConn() const
{
    return _logic_conn;
}

void zoUser::setLogicConn(Tidy::nServerConn* logicConn)
{
    _logic_conn = logicConn;
}

void zoUser::dealLoginSuccess()
{
}

void zoUser::dealChangeLogicSuccess()
{
}

void zoUser::dealResumeOnline()
{
}

void zoUser::addConnUserNum()
{
	if(_gateway_conn != NULL){
		_gateway_conn->getData<zoGatewayConn>()->incUserNum();
	}
	if(_logic_conn != NULL){
		_logic_conn->getData<zoLogicConn>()->incUserNum();
	}
}

void zoUser::setLoginData(const protocol::user::LoginSerializeBinary *binary)
{
    // TODO 登陆数据初始化
}

void zoUser::checkSaveData()
{
}

void zoUser::dealZeroClock()
{
}
