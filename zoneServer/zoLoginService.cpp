#include "protocol/loginCommand.h"

#include "base/nDBConnectionPool.h"
#include "base/nDBConnection.h"

#include "game/nSsqls.h"

#include "zoneServer/zoLoginService.h"
#include "zoneServer/zoServer.h"
#include "zoneServer/zoUser.h"
#include "zoneServer/zoGatewayConn.h"
#include "zoneServer/zoUserService.h"
#include "zoneServer/zoMessageService.h"
#include "zoneServer/zoLoginSessionManager.h"
#include "zoneServer/zoMonitorService.h"

zoLoginService::zoLoginService() : 
    _login_session_manager(NULL),
    _max_zone_user_num(0),
    _login_session_id(0),
    _server_login_state(eServerLoginState_None)
{
}

zoLoginService::~zoLoginService()
{
}

const DWORD zoLoginService::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const
{
    return snprintf(headerBuffer, bufferSize, "[%s]", "Login");;
}

Tidy::nLogger* zoLoginService::getLogger() const
{
    return server->getLogger();
}

bool zoLoginService::initService()
{
    _login_session_manager = new zoLoginSessionManager();
    _max_zone_user_num = Tidy::global["MAX_ZONE_USER_NUM"].getDword();
    return true;
}

void zoLoginService::finalService()
{
    SAFE_DELETE(_login_session_manager);
}

void zoLoginService::dealGatewayClose(Tidy::nServerConn* gatewayConn)
{
    _login_session_manager->dealGatewayClose(gatewayConn);
}

bool zoLoginService::openLogin()
{
    _server_login_state = eServerLoginState_Normal;
    info("开始允许登录");
    return true;
}

bool zoLoginService::closeLogin()
{
    if(_server_login_state == eServerLoginState_Closed){
        assert_fail("重复关闭%u", _server_login_state);
        return false;
    }
    _server_login_state = eServerLoginState_Closed;
    info("关闭登录");

    protocol::login::CloseLoginCommand send;
    return server->sendCmdToTaskConnByType(GATEWAYSERVER, &send, sizeof(send));
}

BYTE zoLoginService::startLoginSession(const char *account, const DWORD sessionID, DWORD *gatewayConnID)
{
    std::string accountName(account);
    Tidy::nStringTool::trim(accountName);
	info("开始登陆会话[%s]", accountName.c_str());
    BYTE errorCode = eLoginToZoneErrorCode_None;
    do{
		if(checkStartLoginSession() == false){
            debug("检查登陆失败[%s]", accountName.c_str());
            errorCode = eLoginToZoneErrorCode_Error;
            break;
		}
        if(_max_zone_user_num > 0){
            if(server->userService->getSize() >= _max_zone_user_num){
                debug("超过最大人数[%s]", accountName.c_str());
                errorCode = eLoginToZoneErrorCode_Error;
                break;
            }
        }
        zoUser *user = server->userService->getUserByAccountName(accountName.c_str());
        const Tidy::nServerConn *gatewayConn = NULL;
        if(gatewayConn == NULL){
            gatewayConn = server->monitorService->selectLeastUserNumGatewayConn(NULL);
        }
        if(gatewayConn == NULL){
            debug("网关连接不存在[%s]", accountName.c_str());
            errorCode = eLoginToZoneErrorCode_GatewayError;
            break;
        }
        *gatewayConnID = gatewayConn->getID();
        const zoLoginSession* session = _login_session_manager->startSession(sessionID, account, gatewayConn, zoLoginSession::SessionState::SessionState_WaitLogin);
        if(session == NULL){
            debug("session初始化错误[%s]", accountName.c_str());
            errorCode = eLoginToZoneErrorCode_GatewayError;
            break;
        }
        protocol::login::StartLoginSessionCommand send;
        send.loginSession.loginSessionID = sessionID;
        copyStr(send.loginSession.accountName, account, sizeof(send.loginSession.accountName));
        if(server->messageService->sendCmdToGatewayByID(gatewayConn->getID(), &send, sizeof(send)) == false){
            errorCode = eLoginToZoneErrorCode_GatewayError;
            debug("发消息到gatewayServer失败[%s]", accountName.c_str());
            _login_session_manager->endSession(session);
            break;
        }
        errorCode = eLoginToZoneErrorCode_None;
    }while(0);
    return errorCode;
}

void zoLoginService::setMaxZoneUserNum(const DWORD maxZoneUserNum)
{
    _max_zone_user_num = maxZoneUserNum;
}

DWORD zoLoginService::getLoginSessionID()
{
    ++ _login_session_id;
    if(_login_session_id == 0){
        ++ _login_session_id;
        fatal("_login_session_id 溢出,重置");
    }
    return _login_session_id;
}

/*
bool zoLoginService::reloginUser(zoUser *user, zoPlatformConn *platformConn, zoClientTaskConn *userClientConn, const protocol::platform::LoginSessionData &data)
{
	user->info("被顶掉重新登陆");
    return true;
}
*/

zoLoginSession* zoLoginService::getSessionByAccountName(const char *accountName) const
{
    return _login_session_manager->getSessionByAccID(accountName);;
}

bool zoLoginService::dealStartLoginSessionSucc(const char *accountName, BYTE netType, DWORD clientID)
{
	const zoLoginSession* session = _login_session_manager->getSessionByAccID(accountName);
	if(session == NULL) {
		assert_fail("没有找到会话");
		return true;
	}
	if(clientID == 0){
		clientID = session->getClientID();
		netType = session->getNetType();
	}
	char gatewayUrl[MAX_URL_LENGTH];
	WORD gatewayPort = 0;
	if(session->fillGatewayLoginData(sizeof(gatewayUrl), gatewayUrl, &gatewayPort) == false){
		assert_fail("获取网关登录失败");
		return true;
	}
    return true;
}

bool zoLoginService::checkStartLoginSession() const
{
    /*
	if(_server_login_state == eServerLoginState_None){
		return false;
	}
    */
	if(server->checkCloseZone() == true){
		return false;
	}
	if(server->getTaskConnSizeByType(GATEWAYSERVER) == 0){
        error("没有网关连接,不能开始登陆会话");
		return false;
	}
	if(server->getTaskConnSizeByType(LOGICSERVER) == 0){
        error("没有LOGIC连接,不能开始登陆会话");
		return false;
	}
	return true;
}

bool zoLoginService::endSessionByID(const DWORD sessionID)
{
    return _login_session_manager->endSession(sessionID);
}

bool zoLoginService::checkSession(const DWORD sessionID)
{
    return _login_session_manager->checkSession(sessionID);
}
