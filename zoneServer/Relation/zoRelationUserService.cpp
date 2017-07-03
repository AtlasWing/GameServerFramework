#include "protocol/loginCommon.h"
#include "protocol/userCommand.h"
#include "protocol/loginCommand.h"
#include "protocol/mailCommand.h"
#include "protocol/Proto/UserData.pb.h"

#include "base/nDBConnection.h"
#include "base/nDBConnectionPool.h"

#include "game/nSsqls.h"
#include "game/nTable.h"
#include "game/nTableManager.h"

#include "zoneServer/zoRelationUserService.h"
#include "zoneServer/zoMessageService.h"
#include "zoneServer/zoServer.h"
#include "zoneServer/zoUser.h"
#include "zoneServer/zoUserService.h"
#include "zoneServer/Relation/zoRelationUser.h"

zoRelationUserService::zoRelationUserService():
    _save_relation_user_data_timer(TEN_SEC)
{
}

zoRelationUserService::~zoRelationUserService()
{
    destroyAll();
}

bool zoRelationUserService::init()
{
    return true;
}

zoRelationUser *zoRelationUserService::getUserByID(DWORD userID)
{
    return static_cast<zoRelationUser*>(getByID(userID));
}

zoRelationUser *zoRelationUserService::getUserByName(const char* userName)
{
    return static_cast<zoRelationUser*>(getByName(userName));
}

bool zoRelationUserService::initAllRelationUser()
{
    return loadRelationUser(0, NULL);
}

bool zoRelationUserService::loadRelationUser(const DWORD charID, zoRelationUser *user)
{
    assert_debug(user == NULL || user->getID() == charID, "数据错误");

    SqlRelationBase relationBaseTable;
    std::vector<bool> relationBaseFieldVec(SqlRelationBase_NULL, true);
    relationBaseFieldVec[SqlRelationBase_userDataState] = false;
    Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
    if(conn == NULL){
        server->error("不能从数据库连接池获取连接句柄 %s",__PRETTY_FUNCTION__);
        return false;
    }
    Tidy::nDBQuery query = conn->getQuery();
    char whereCondition[128] = {'\0'};
    DWORD limitNum = 0;
    if(charID == 0){
        snprintf(whereCondition, sizeof(whereCondition), " %s=%u ", SqlRelationBase::names[SqlRelationBase_userDataState], protocol::login::eUserDataState_Normal);
    }
    else{
        snprintf(whereCondition, sizeof(whereCondition), " %s=%u and %s=%u ", SqlRelationBase::names[SqlRelationBase_userDataState], protocol::login::eUserDataState_Normal, SqlRelationBase::names[SqlRelationBase_charID], charID);
        limitNum = 1;
    }
    Tidy::nDBQueryResult result = query.exeSelect(relationBaseTable, relationBaseFieldVec, whereCondition, limitNum);
    if(result.empty()){
        if(charID == 0){
            server->warn("relationUserManager 为空");
            return true;
        }
        else{
            assert_fail("没有找到玩家[%u]", charID);
            return false;
        }
    }

	static std::vector<DWORD> fieldIndexVec = result.getResultFieldIndexMap<SqlRelationBase>(SqlRelationBase_NULL);
	CreateRelationUserData createRelationUserData;
    RelationUserData &relationData = createRelationUserData.relationUserData;
    protocol::user::MiscUserDataBinary miscUserDataBinary;
    protocol::user::MessageSaveData messageBinary;
	DWORD VALID_ELO_HISTORY_TIME_IN_SEC = server->time().sec() - Tidy::global["MAX_ELO_HISTORY_TIME_IN_DAY"].getDword() * ONE_DAY_IN_SEC;
    for(DWORD i = 0; i < result.size(); ++ i){
        relationData.charID = result[i][fieldIndexVec[SqlRelationBase_charID]];
        copyStr(relationData.charName, (const char*)(result[i][fieldIndexVec[SqlRelationBase_name]]), sizeof(relationData.charName));
        relationData.sex = result[i][fieldIndexVec[SqlRelationBase_sex]];
        relationData.level = result[i][fieldIndexVec[SqlRelationBase_level]];
        relationData.exp = result[i][fieldIndexVec[SqlRelationBase_exp]];
        relationData.gmLevel = result[i][fieldIndexVec[SqlRelationBase_gmLevel]];
		if(result[i][fieldIndexVec[SqlRelationBase_userDataBinary]].is_null() == false){
            if(miscUserDataBinary.ParsePartialFromArray(result[i][fieldIndexVec[SqlRelationBase_userDataBinary]].c_str(), result[i][fieldIndexVec[SqlRelationBase_userDataBinary]].size()) == false){
                assert_fail("解析失败");
                return false;
            }
        }
        SqlFriend friendSql;
        char condition[128] = {'\0'};
        snprintf(condition, sizeof(condition), " %s=%u ", SqlFriend::names[SqlFriend_userID], charID);
		if(user == NULL){
            user = createRelationUser(result[i][fieldIndexVec[SqlRelationBase_accountName]], createRelationUserData);
            std::string accountName;
            result[i][fieldIndexVec[SqlRelationBase_accountName]].to_string(accountName);
            if(user == NULL){
                assert_fail("加入关系用户管理器失败[%s]", accountName.c_str());
                return false;
            }
        }
        else{
            user->freshRelationData(relationData);
        }

        createRelationUserData.reset();
        miscUserDataBinary.Clear();
        user = NULL;
    }

    return true;
}

zoRelationUser* zoRelationUserService::createRelationUser(const char *accountName, const CreateRelationUserData &relationData)
{
    zoRelationUser *relationUser = new zoRelationUser(accountName);
    if(relationUser->init(relationData) == false){
        assert_fail("初始失败");
        SAFE_DELETE(relationUser);
        return NULL;
    }
    if(addRelationUser(relationUser) == false){
        assert_fail("添加失败");
        SAFE_DELETE(relationUser);
        return NULL;
    }
    return relationUser;
}

bool zoRelationUserService::addRelationUser(zoRelationUser *relationUser)
{
    if(add(relationUser) == false) {
        if( getByName(relationUser->getName()) != NULL) {
            server->debug("名字重复");
        }
        else {
            server->debug("ID重复");
        }
        assert_fail("添加失败[%u]", relationUser->getID());
        return false;
    }
#ifdef _DEBUG_YOULONG
    relationUser->debug("添加RelationUser [%u]", relationUser->getID());
#endif
    return true;
}

void zoRelationUserService::removeRelationUser(zoRelationUser *user)
{
    remove(user);
}

bool zoRelationUserService::freshRelationUser(const DWORD charID)
{
    zoRelationUser *user = getUserByID(charID);
    if(user == NULL){
        return true;
    }
    return loadRelationUser(charID, user);
}

bool zoRelationUserService::deleteRelationUser(const DWORD charID)
{
    zoRelationUser *user = getUserByID(charID);
    if(user == NULL){
        return true;
    }
    user->trace("删除角色");
    removeRelationUser(user);
    return true;
}

bool zoRelationUserService::doTick()
{
    if(_save_relation_user_data_timer(server->time())== true){
        saveRelationUserData();
    }
    return true;
}

bool zoRelationUserService::saveRelationUserData()
{
    for(DWORD i = 0; i < _need_serialize_user_list.size(); ++ i){
        _need_serialize_user_list[i]->saveData();
    }
    _need_serialize_user_list.clear();
    return true;
}

bool zoRelationUserService::addNeedSerializeUser(zoRelationUser *user)
{
    _need_serialize_user_list.push_back(user);
    return true;
}

void zoRelationUserService::dealCore()
{
	saveRelationUserData();
}
