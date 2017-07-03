#include "protocol/loginCommand.h"

#include "gatewayServer/gwUser.h"
#include "gatewayServer/gwUserTaskConn.h"
#include "gatewayServer/gwServer.h"
#include "gatewayServer/gwMessageService.h"
#include "gatewayServer/gwUserService.h"
#include "gatewayServer/gwLoginSession.h"

gwUser::gwUser(const char *accountName, protocol::login::eLoginState initLoginState) : 
    _account_name(accountName),
    _login_state(initLoginState),
    _logout_source(protocol::login::eUserLogOutSourceType_None),
    _ping_time_in_msec(0),
    _last_sync_time_time_in_sec(0),
	_login_session(NULL),
	_level(0),
    _selected_entity_data(0, 4),
    _session_id(0),
    _logic_conn(NULL)
{
    debug("登陆gw成功.[%p]", this);
}

gwUser::~gwUser()
{
	SAFE_DELETE(_login_session);
    //assert_debug(getID() == 0 , "");
    debug("销毁gwUser成功.[%p]", this);
}

void gwUser::doTick()
{
}

bool gwUser::initData(const protocol::login::LoginUserSerializeData *userData)
{
	//_level = userData->mainUserData.level;
	return true;
}

DWORD gwUser::getCharID() const
{
    return getID();
}

const char* gwUser::getAccountName() const
{
    return _account_name.c_str();
}

void gwUser::setSessionID(const DWORD sessionID)
{
    _session_id = sessionID;
}

DWORD gwUser::getSessionID() const
{
    return _session_id;
}

void gwUser::setName(const char *name)
{
    Tidy::nObj::setName(name);
}

const char *gwUser::getIP() const
{
    return _ip.c_str();
}

void gwUser::setIP(const char* ip)
{
    _ip = ip;
    debug("玩家登录IP为:[%s]", _ip.c_str());
}

bool gwUser::reg(const protocol::login::UserLoginData &userData)
{
    assert_debug(userData.charID != 0, "");
    setID(userData.charID);
    setName((const char*)userData.name);

    return true;
}

void gwUser::unreg()
{
    _logic_conn = NULL;
    setID(0);
}

const DWORD gwUser::getLogicConnID() const
{
    return (_logic_conn != NULL) ? _logic_conn->getID() : 0;
}

void gwUser::setLogicConn(Tidy::nServerConn* serverConn)
{
    _logic_conn = serverConn;
}

bool gwUser::checkResetLogicConn(Tidy::nServerConn *logicConn)
{
    Tidy::nScopeWRLock lock(_data_lock);
    if(_logic_conn != logicConn){
        return false;
    }
    _logic_conn = NULL;
    return true;
}

bool gwUser::sendLoginEvent(const BYTE event)
{
    using namespace protocol::login;
    switch(getLoginState())
    {
        case eLoginState_None:
            {
                switch(event)
                {
                }
            }
            break;
        default:
            {
            }
    }

#ifdef _DEBUG
    assert_fail("无法处理的状态转换[%u][%u, %u]", getID(), _login_state, event);
#else
    fatal("无法处理的状态转换[%u][%u, %u]", getID(), _login_state, event);
#endif
    return false;
}

bool gwUser::transitLoginState(const BYTE newState) 
{
    return true;
}

const bool gwUser::isRegged() const
{
    return getID() != 0;
}

Tidy::nRWLock& gwUser::getDataLock()
{
    return _data_lock;
}

void gwUser::setSelectedEntity(const DWORD selectedEntityID, const BYTE selectedEntityType)
{
    _selected_entity_data.entityID = selectedEntityID;
    _selected_entity_data.entityType = selectedEntityType;
}

const bool gwUser::checkSelectedEntity(const DWORD entityID, const BYTE entityType) const
{
    return _selected_entity_data.entityID == entityID && _selected_entity_data.entityType == entityType;
}

const BYTE gwUser::getLoginState() const
{
    return _login_state;
}

const bool gwUser::isInLoginState(const BYTE state) const
{
    return _login_state == state;
}

void gwUser::setLoginState(const BYTE state)
{
    _login_state = state;
}

protocol::login::eUserLogOutSourceType gwUser::getLogoutSource() const
{
    return static_cast<protocol::login::eUserLogOutSourceType>(_logout_source);
}

void gwUser::setLogoutSource(const protocol::login::eUserLogOutSourceType logoutSource)
{
    _logout_source = static_cast<protocol::login::eUserLogOutSourceType>(logoutSource);
}

bool gwUser::isInLogout()
{
    return _logout_source != protocol::login::eUserLogOutSourceType_None;
}

void gwUser::setNeedOfflineProtect()
{
}

void gwUser::clearNeedOfflineProtect()
{
}

bool gwUser::checkNeedOfflineProtect() const
{
    return true;
}

void gwUser::dealOfflineProtect()
{
	setUserDataFlag(eGateUserDataFlag_IsSyncTimeTick, false);
}

void gwUser::setUserDataFlag(eGateUserDataFlag flag, bool value)
{
}

bool gwUser::syncPingTimeInMsec()
{
    return true;
}

bool gwUser::sendPingTimeToLogic()
{
    //TODO 发送 ping 给逻辑服务器
    return true;
}

QWORD gwUser::getPingTimeInMsec() const
{
    return _ping_time_in_msec;
}

bool gwUser::syncTime()
{
    // TODO 同步时间
    return true;
}

void gwUser::dealLoginSuccess()
{
    if(_ping_time_in_msec > 0){
        sendPingTimeToLogic();
    }
    else{
        setNeedSyncTime();
    }
}

void gwUser::setNeedSyncTime()
{
    // 5秒内同步一次
    if(server->time().sec() < (_last_sync_time_time_in_sec + 5)){
        return;
    }
    _last_sync_time_time_in_sec = server->time().sec();
    setUserDataFlag(eGateUserDataFlag_IsSyncTimeTick, true);
}

bool gwUser::setHasPassGuideMap()
{
    setUserDataFlag(eGateUserDataFlag_HasPassGuideMap, true);
    return true;
}

bool gwUser::fillLogoutData(protocol::login::UserLogoutData *logoutData) const
{
    copyStr(logoutData->accountName, getAccountName(), sizeof(logoutData->accountName));
    logoutData->charID = getCharID();
    logoutData->sessionID = getSessionID();
	return true;
}

const gwLoginSession* gwUser::getLoginSession() const
{
	return _login_session;
}

void gwUser::clearLoginSession()
{
	_login_session = NULL;
}

void gwUser::dealResumeOnline()
{
}

bool gwUser::setLevel(const WORD level)
{
	_level = level;
	return true;
}

WORD gwUser::getLevel() const
{
	return _level;
}
