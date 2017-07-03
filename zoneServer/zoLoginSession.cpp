#include "protocol/loginCommand.h"

#include "zoneServer/zoLoginSession.h"
#include "zoneServer/zoServer.h"
#include "zoneServer/zoLoginService.h"
#include "zoneServer/zoGatewayConn.h"

zoLoginSession::zoLoginSession(const DWORD sessionID, const char *accountName, const Tidy::nServerConn *gatewayConn, const SessionState state) :
    _session_id(sessionID),
    _account_name(accountName),
    _client_id(0),
	_net_type(0),
    _platform_ID(0),
    _gateway_conn(gatewayConn),
    _session_state(state)
{
}

zoLoginSession::~zoLoginSession()
{
}

const char* zoLoginSession::getName() const
{
    return _account_name.c_str();
}

const DWORD zoLoginSession::getClientID() const
{
    return _client_id;
}

const DWORD zoLoginSession::getPlatformID() const
{
    return _platform_ID;
}

void zoLoginSession::setSessionState(const zoLoginSession::SessionState state)
{
    _session_state = state;
}

const Tidy::nServerConn* zoLoginSession::getGatewayConn() const
{
    return _gateway_conn;
}

BYTE zoLoginSession::getNetType() const
{
	return _net_type;
}

bool zoLoginSession::fillGatewayLoginData(const DWORD maxGatewayIPLen, char *gatewayIP, WORD *port) const
{
	zoGatewayConn *gatewayConn = _gateway_conn->getData<zoGatewayConn>();
	return gatewayConn->fillGatewayLoginData(maxGatewayIPLen, gatewayIP, port);
}
