#include "gatewayServer/gwUserTaskConn.h"

#include "protocol/loginCommand.h"

#include "gatewayServer/gwUser.h"
#include "gatewayServer/gwServer.h"
#include "gatewayServer/gwUserService.h"
#include "gatewayServer/gwLoginSession.h"

gwUserTaskConn::gwUserTaskConn(const int sock, const struct sockaddr *addr, const struct Tidy::SocketPortData* listenPortData) : 
    Tidy::nTaskConnNoQueue(sock, addr, listenPortData),
    _user( NULL ),
    _is_adult(false),
    _channel_id(0),
	_has_load_char_data(false),
    _platform_type(0),
    _platform_login_data(NULL),
	_login_session(NULL)
{
    setServerType(USERCLIENT);
}

gwUserTaskConn::~gwUserTaskConn()
{
    _user = NULL;
    SAFE_DELETE(_platform_login_data);
	SAFE_DELETE(_login_session);
}

void gwUserTaskConn::addCharacter(const protocol::login::UserCharacterData &userData)
{
    debug("角色ID：[%u], 名字：[%s],性别： [%u]", userData.characterInfo.charID, userData.characterInfo.name, userData.characterInfo.sex);
    _user_info[userData.characterInfo.charID] = userData;
}

void gwUserTaskConn::removeCharacter( DWORD charID )
{
    _user_info.erase(charID);
}

void gwUserTaskConn::clearCharacterList()
{
    _user_info.clear();
}

bool gwUserTaskConn::hasCharacter() const
{
	return _user_info.empty() == false;
}

const protocol::login::UserCharacterData* gwUserTaskConn::getSelectedCharacter(const DWORD charID) const
{
    UserInfoContainer::const_iterator iter = _user_info.find(charID);
    if(iter == _user_info.end()) {
        return NULL;
    }

    return &iter->second;
}

void gwUserTaskConn::setUser(gwUser *user)
{
    _user = user;
}

gwUser* gwUserTaskConn::getUser(Tidy::nScopeRDLock *rdLock)
{
    if(_user == NULL){
        return NULL;
    }
    rdLock->setLock(_user->getDataLock());
    return _user;
}

gwUser* gwUserTaskConn::getUser(Tidy::nScopeWRLock *wrLock)
{
    if(_user == NULL){
        return NULL;
    }
    wrLock->setLock(_user->getDataLock());
    return _user;
}

DWORD gwUserTaskConn::getUserID() const
{
    if(_user == NULL){
        return 0;
    }
    return _user->getID();
}

void gwUserTaskConn::deleteCharacter(const DWORD charID, const BYTE errorCode)
{
    //TODO 删除角色
}

void gwUserTaskConn::setLoginSession(const gwLoginSession *loginSession)
{
	const protocol::login::LoginSession &loginSessionData = loginSession->getData();
	const std::string& platformLoginData = loginSession->getPlatformLoginData();
    setServerID(loginSessionData.loginSessionID);
    _account_name = loginSessionData.accountName;
    _is_adult = loginSessionData.isAdult;
    _channel_id = loginSessionData.channelID;
    _platform_type = loginSessionData.platformType;
    if(platformLoginData.empty() == false){
        _platform_login_data = new std::string(platformLoginData);
    }
	_login_session = loginSession;
}

const char* gwUserTaskConn::getAccountName() const
{
    return _account_name.c_str();
}

DWORD gwUserTaskConn::getPlatformType() const
{
    return _platform_type;
}

bool gwUserTaskConn::getIsAdult() const
{
    return _is_adult; 
}

DWORD gwUserTaskConn::getChannelID() const
{
    return _channel_id;
}

bool gwUserTaskConn::checkHasLoadCharData() const
{
	return _has_load_char_data;
}

void gwUserTaskConn::setHasLoadCharData()
{
	_has_load_char_data = true;
}

bool gwUserTaskConn::hasPlatformLoginData() const
{
    return _platform_login_data != NULL; 
}

const std::string& gwUserTaskConn::getPlatformLoginData() const
{
    assert_debug(_platform_login_data != NULL, "");
    return *_platform_login_data;
}

void gwUserTaskConn::clearPlatformLoginData()
{
    SAFE_DELETE(_platform_login_data);
}

std::string gwUserTaskConn::getInitCharName() const
{
    // TODO 初始化名字
    return "panqingyun";
}

const gwLoginSession* gwUserTaskConn::getLoginSession() const
{
	return _login_session;
}

void gwUserTaskConn::clearLoginSession()
{
	_login_session = NULL;
}

DWORD gwUserTaskConn::getLoginSessionWaitTimeInSec() const
{
	assert_debug(_login_session != NULL, "");
	return server->time().sec() - _login_session->getStartTimeInSec();
}
