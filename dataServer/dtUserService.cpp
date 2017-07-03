#include "protocol/loginCommand.h"
#include "protocol/loginCommon.h"
#include "protocol/userCommon.h"
#include "protocol/Proto/UserData.pb.h"

#include "base/nServerConn.h"
#include "base/nDBConnection.h"
#include "base/nDBConnectionPool.h"

#include "game/nSsqls.h"
#include "game/nTableManager.h"
#include "game/nTable.h"

#include "dataServer/dtUser.h"
#include "dataServer/dtUserService.h"  
#include "dataServer/dtServer.h"
#include "dataServer/dtMessageService.h"

dtUserService::dtUserService():
    _tick_timer(5 * ONE_SEC)
{
}

dtUserService::~dtUserService()
{
}

bool dtUserService::initService()
{
    Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
    if(conn == NULL){
        return false;
    }

    Tidy::nDBQuery query = conn->getQuery();
    SqlCharBase charBase;
    Tidy::nDBQueryResult resultSet = query.exeSelect(charBase.table(), charBase.field_list(SqlCharBase_charID, SqlCharBase_name, SqlCharBase_accountName));
    DWORD charID = 0;

    const mysqlpp::String *nameData = NULL;
    const mysqlpp::String *accountNameData = NULL;
    for(DWORD i = 0; i < resultSet.size(); ++ i){
        charID = resultSet[i][SqlCharBase::names[SqlCharBase_charID]];
        nameData = &resultSet[i][SqlCharBase::names[SqlCharBase_name]];
        accountNameData = &resultSet[i][SqlCharBase::names[SqlCharBase_accountName]];
#ifdef _DEBUG
        std::string accountName = static_cast<std::string>(*accountNameData);
        assert_debug(accountNameData->empty() == accountName.empty(), "[%s]", accountName.c_str());
#endif
        std::pair<IDNameMap::iterator, bool> result = _user_id_name_map.insert(IDNameMap::value_type(charID, UserNameData(std::string(nameData->data(), nameData->size()), static_cast<std::string>(*accountNameData))));
        if(result.second == false){
            assert_fail("重复");
            return false;
        }
        const UserNameData &nameData = result.first->second;
        _user_name_id_map[nameData.name.c_str()] = charID;
        _account_name_id_map[nameData.accountName.c_str()] = charID;
    }
    char whereCondition[1024];
    snprintf(whereCondition, sizeof(whereCondition), " %s=%u", SqlCharBase::names[SqlCharBase_userDataState], protocol::login::eUserDataState_WaitCommitLeaveZone);
    charBase.userDataState = protocol::login::eUserDataState_Normal;
    if(query.exeUpdate(charBase.table(), charBase.equal_list(SqlCharBase_userDataState), whereCondition, 0) == Tidy::nDBQuery::ERROR){
        assert_fail("更新状态失败");
        return false;
    }
    return true;
}

void dtUserService::finalService()
{
}

namespace {
    struct UserTickExec {
        bool exec(dtUser *user) {
            user->doTick();
            return true;
        }
    }userTickExec;
};

void dtUserService::doTick()
{
    if(_tick_timer.isReady(server->time()) == true) {
        execEveryUser(userTickExec);
    }
}

dtUser* dtUserService::loginUser(const protocol::login::UserLoginData& data)
{
    Tidy::nServerConn *logicConn = server->getTaskConnByID(data.logicConnID);
    if(logicConn == NULL){
        server->debug("没有连接逻辑服务器，是否宕机了？");
        return NULL;
    }

    Tidy::nServerConn *gatewayConn = server->getTaskConnByID(data.gatewayConnID);
    if(gatewayConn == NULL){
        server->debug("没有连接网关服务器，是否宕机了？");
        return NULL;
    }

    dtUser* user = new dtUser(data.charID, data.accountName, data.name, logicConn, gatewayConn);
    if(addUser(user) == false){
        assert_fail("添加玩家失败");
        SAFE_DELETE(user);
        return NULL;
    }
    if(user->reg(data) == false){
        assert_fail("注册玩家失败");
        removeUser(user);
        SAFE_DELETE(user);
        return NULL;
    }
    return user;
}

bool dtUserService::logoutUser(dtUser* user, const BYTE logoutType, const protocol::login::UserLogoutData *logoutData, const char *data, const DWORD num)
{
    if(unregUser(user) == false){
        assert_fail("注销失败");
    }
    return true;
}

namespace{
    struct GetAllUserExec{
        bool exec(const dtUser *user) const{
            return true;
        }
    };
};
void dtUserService::logoutAll(const BYTE logoutSource)
{
    Tidy::vector<dtUser*> userList;
    userList.reserve(size());
    GetAllUserExec exec;
    get(exec, userList);
    for(DWORD i = 0; i < userList.size(); ++ i){
        logoutUser(userList[i], logoutSource, NULL, NULL, 0);
    }
}

bool dtUserService::unregUser(dtUser* user)
{
    user->unreg();
    removeUser(user);
    SAFE_DELETE(user);

    return true;
}

bool dtUserService::createRole(const char *accountName, const BYTE sex, const DWORD charID, const char *name)
{
    SqlCharBase charBase;
    charBase.userType = 1;
    charBase.userDataState = protocol::login::eUserDataState_Normal;
    charBase.userDataStateChangeTimeInSec = server->time().sec();
    charBase.charID = charID;
    charBase.name = name;
    charBase.accountName = accountName;
    charBase.sex = sex;
    charBase.level = 1;
    charBase.isAdult = 1; // TODO 是否成年
    charBase.silver = Tidy::global["INIT_SILVER"].getDword();
    charBase.diamond = Tidy::global["INIT_DIAMOND"].getDword();
    charBase.bindDiamond = Tidy::global["INIT_BINDDIAMOND"].getDword();
    charBase.bitMask = protocol::user::eCharBaseFlag_NewChar;
    std::vector<bool> charBaseFieldVec(SqlCharBase_NULL, false);
    charBaseFieldVec[SqlCharBase_userType] = true;
    charBaseFieldVec[SqlCharBase_userDataState] = true;
    charBaseFieldVec[SqlCharBase_userDataStateChangeTimeInSec] = true;
    charBaseFieldVec[SqlCharBase_charID] = true;
    charBaseFieldVec[SqlCharBase_name] = true;
    charBaseFieldVec[SqlCharBase_accountName] = true;
    charBaseFieldVec[SqlCharBase_bitMask] = true;
    charBaseFieldVec[SqlCharBase_sex] = true;
    charBaseFieldVec[SqlCharBase_level] = true;
    charBaseFieldVec[SqlCharBase_isAdult] = true;
    charBaseFieldVec[SqlCharBase_bitMask] = true;
    charBaseFieldVec[SqlCharBase_silver] = true;
    charBaseFieldVec[SqlCharBase_diamond] = true;
    charBaseFieldVec[SqlCharBase_bindDiamond] = true;
    Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
    if(conn == NULL){
        assert_fail("没有数据库连接");
        return false;
    }
    Tidy::nDBQuery query = conn->getQuery();
    DWORD retCode = query.exeInsert(charBase, charBaseFieldVec);
    if(retCode == Tidy::nDBQuery::ERROR){
        assert_fail("插入玩家数据失败");
        return false;
    }
    _user_name_id_map.erase(charBase.name.data());
    UserNameData *nameData = _user_id_name_map.fetch(charBase.charID, UserNameData(charBase.name, charBase.accountName));
    _user_name_id_map[nameData->name.c_str()] = charID;
    _account_name_id_map[nameData->accountName.c_str()] = charID;
    server->debug("创建新角色[%s,%u]", accountName, charID);
    return true;
}

bool dtUserService::deleteCharacter(const DWORD charID, BYTE& errorCode, bool realDeleteData)
{
    return false;
}

bool dtUserService::createAccount(const DWORD platformType, const char *accountName)
{
    SqlAccountAdmin accountData;
    accountData.accountName = accountName;
    accountData.createTime.now();
    accountData.lastLoginTime.now();

    std::vector<bool> accountFieldVec(SqlAccountAdmin_NULL, false);
    accountFieldVec[SqlAccountAdmin_accountName] = true;
    accountFieldVec[SqlAccountAdmin_createTime] = true;
    accountFieldVec[SqlAccountAdmin_lastLoginTime] = true;

    Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_Admin);
    if(conn == NULL){
        assert_fail("没有数据库连接");
        return false;
    }
    Tidy::nDBQuery query = conn->getQuery();

    Tidy::nDBQueryResult resultSet = query.exeSelect(accountData.table(), " NULL ", accountData.equal_list(SqlAccountAdmin_accountName), 1);
    if(resultSet.empty() == false){
        return true;
    }
    DWORD retCode = query.exeInsert(accountData, accountFieldVec);
    if(retCode == Tidy::nDBQuery::ERROR){
        assert_fail("插入账号失败");
        return false;
    }
    server->debug("插入帐号成功[%s]", accountName);
    return true;
}

void dtUserService::dealGatewayClose(Tidy::nServerConn *gatewayConn)
{
}

namespace {
    struct LogoutUserOnLogicExec {
        const Tidy::nServerConn* const _logic_conn;
        bool _need_logout_user;
        LogoutUserOnLogicExec(const Tidy::nServerConn* logicConn, const bool needLogoutUser) : _logic_conn(logicConn), _need_logout_user(needLogoutUser)
        {
        }
        bool exec(dtUser *user) {
            if(user->getLogicConn() == _logic_conn) {
                user->setLogicConn(NULL);
                if(_need_logout_user == true){
                    server->userService->logoutUser(user, protocol::login::eUserLogOutSourceType_LogicCoreDown, NULL, NULL, 0);
                }
                else{
                    server->userService->startLogoutUser(user, protocol::login::eUserLogOutSourceType_LogicCoreDown, NULL, 0);
                }
            }
            return true;
        }
    };
};

void dtUserService::dealLogicClose(Tidy::nServerConn *logicConn)
{
    // TODO needLogoutUser
    LogoutUserOnLogicExec exec(logicConn, false);
    execEveryUser(exec);
}

void dtUserService::dealConnectClose(Tidy::nServerConn *conn)
{
    switch(conn->getServerType())
    {
        case LOGICSERVER:
            {
                dealLogicClose(conn);
            }
            break;
        case GATEWAYSERVER:
            {
                dealGatewayClose(conn);
            }
            break;
        default:
            {
            }
    }
}

const std::string* dtUserService::getUserNameByID(const DWORD charID) const
{
    IDNameMap::const_iterator iter = _user_id_name_map.find(charID);
    if(iter == _user_id_name_map.end()){
        assert_fail("没有找到名字");
        return NULL;
    }
    return &iter->second.name;
}

bool dtUserService::getUserIDByAccountName(const std::string &accountName, DWORD *id) const
{
    const DWORD *userID = _account_name_id_map.get(accountName.c_str());
    if(userID == NULL){
        return false;
    }
    *id = *userID;
    return true;
}

bool dtUserService::getUserIDByName(const char *name, DWORD *id) const
{
    NameIDMap::const_iterator iter = _user_name_id_map.find(name);
    if(iter == _user_name_id_map.end()){
        assert_fail("没有找到名字[%s]", name);
        return false;
    }
    *id = iter->second;
    return true;
}

bool dtUserService::checkUserIDValid(const DWORD userID) const
{
    return getUserNameByID(userID) != NULL;
}

namespace{
    struct CollectOnlineUserIDExec{
        std::vector<DWORD> *_user_list;
        CollectOnlineUserIDExec(std::vector<DWORD> *userList) : _user_list(userList){
        }
        bool exec(const dtUser *user) {
            _user_list->push_back(user->getID());
            return true;
        }
    };
};
bool dtUserService::getOnlineUserIDList(std::vector<DWORD> *userList) const
{
    CollectOnlineUserIDExec exec(userList);
    return execAll(exec);
}

void dtUserService::requestLogoutUser(dtUser *user, const BYTE logoutType, const protocol::user::LogoutCallbackData *callbackData)
{
}

void dtUserService::dealZoneStart(const protocol::admin::ZoneData *zoneData)
{
}

void dtUserService::dealZoneClose(const protocol::admin::ZoneData *zoneData)
{
}

void dtUserService::dealStartSaveUserData(const DWORD charID)
{
    if(_wait_complete_save_data_user_set.insert(charID).second == false){
        assert_fail("插入重复失败 [%u]", charID);
    }
}

void dtUserService::dealCompleteSaveUserData(const DWORD charID)
{
    if(_wait_complete_save_data_user_set.erase(charID) != 1){
        assert_fail("删除玩家ID失败 [%u]", charID);
    }
}

bool dtUserService::checkUserDataSaveComplete(const DWORD charID) const
{
    return _wait_complete_save_data_user_set.find(charID) == _wait_complete_save_data_user_set.end();
}

namespace{
    struct SaveAllUserDataExec{
        bool exec(dtUser *user){
            user->checkFlush(NULL);
            return true;
        }
    };
};
void dtUserService::dealCore()
{
    server->info("宕机刷新存档开始");
    SaveAllUserDataExec exec;
    execEveryUser(exec);
    server->info("宕机刷新存档完成");
}

bool dtUserService::startLogoutUser(dtUser* user, const BYTE logoutType, const char *data, const DWORD num)
{
    server->debug("dtUserService startLogoutUser");
    return true;
}

bool dtUserService::readCharList(const DWORD sessionID, const DWORD platformType, const char *accountName, const DWORD channelID, const char *ip, const Tidy::nServerConn *gatewayConn)
{
    server->debug("角色列表返回待做.");
    return true;
}

bool dtUserService::getOnlineHumanUserIDList(std::vector<DWORD> *userList) const
{
    CollectOnlineUserIDExec exec(userList);
    return execAll(exec);
}

bool dtUserService::getAllHumanUserIDList(std::vector<DWORD> *userList) const
{
    for(IDNameMap::const_iterator iter = _user_id_name_map.begin(); iter != _user_id_name_map.end(); ++ iter){
        if(iter->second.accountName.empty() == false){
            userList->push_back(iter->first);
        }
    }
    return true;
}
