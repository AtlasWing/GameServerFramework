#include "protocol/userCommon.h"
#include "protocol/serializeCommand.h"
#include "protocol/Proto/UserData.pb.h"
#include "protocol/loginCommon.h"
#include "protocol/relationCommon.h"
#include "protocol/userCommand.h"

#include "base/nDBConnection.h"
#include "base/nDBConnectionPool.h"

#include "game/nSsqls.h"
#include "game/nTable.h"
#include "game/nTableManager.h"

#include "zoneServer/zoServer.h"
#include "zoneServer/zoUser.h"
#include "zoneServer/zoConfigService.h"
#include "zoneServer/zoRelationUserService.h"
#include "zoneServer/zoMessageService.h"
#include "zoneServer/zoUserService.h"
#include "zoneServer/Relation/zoRelationUser.h"

void RelationUserData::reset()
{
	bzero(this, sizeof(*this));
}

zoRelationUser::zoRelationUser(const char* accountName):
	ownerUser(NULL),
    _account_name(accountName),
    _common_list_data_version(server->time().sec()),
    _extend_list_data_version(server->time().sec()),
    _last_login_state(protocol::login::eLoginState_None)
{
}

zoRelationUser::~zoRelationUser()
{
}

const char* zoRelationUser::getName() const
{
    return relationUserData.charName;
}

const char *zoRelationUser::getShowName() const
{
	return relationUserData.charName;
}

void zoRelationUser::setName(const char *name)
{
    copyStr(relationUserData.charName, name, sizeof(relationUserData.charName));
    updateCommonListDataVersion(true);
}

const char* zoRelationUser::getAccountName() const
{
    return _account_name.c_str();
}

const DWORD zoRelationUser::getID() const
{
    return relationUserData.charID;
}

const DWORD zoRelationUser::getCharID() const
{
	return getID();
}

Tidy::nLogger* zoRelationUser::getLogger() const
{
    return server->getLogger();
}

const DWORD zoRelationUser::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const
{
    return snprintf(headerBuffer, bufferSize, "[%u,%s]", getID(), getName());
}

BYTE zoRelationUser::getSex() const
{
    return relationUserData.sex;
}

bool zoRelationUser::init(const CreateRelationUserData &createRelationData)
{
    memcpy(&relationUserData, &createRelationData.relationUserData, sizeof(relationUserData));
    return true;
}

bool zoRelationUser::freshRelationData(const RelationUserData &relationData)
{
    server->relationUserService->removeRelationUser(this);
    memcpy(&relationUserData, &relationData, sizeof(relationUserData));
    return server->relationUserService->addRelationUser(this);
}

bool zoRelationUser::reg(zoUser *user)
{
	assert_debug(user != NULL, "");
	ownerUser = user;
    updateCommonListDataVersion(true);
	return true;
}

void zoRelationUser::unreg(zoUser *user)
{
	assert_debug(user != NULL, "");

    updateCommonListDataVersion(true);
	ownerUser = NULL;
}

void zoRelationUser::dealLoginSuccess()
{
}

void zoRelationUser::dealResumeOnline()
{
}

bool zoRelationUser::doTick()
{
	return true;
}

DWORD zoRelationUser::getCommonListDataVersion() const
{
    return _common_list_data_version;
}

DWORD zoRelationUser::getExtendListDataVersion() const
{
    return _extend_list_data_version;
}

void zoRelationUser::setLevel(const WORD level)
{
    const WORD oldLevel = relationUserData.level;
    relationUserData.level = level;
    updateCommonListDataVersion(true);
}

WORD zoRelationUser::getLevel() const
{
    return relationUserData.level;
}

void zoRelationUser::setDiamond(const DWORD diamond)
{
    relationUserData.diamond = diamond;
    updateExtendListDataVersion();
}

void zoRelationUser::setDiamondConsume(const DWORD diamondConsume)
{
    relationUserData.diamondConsume = diamondConsume;
    updateExtendListDataVersion();
}

void zoRelationUser::setExp(const DWORD exp, const DWORD totalExp)
{
    relationUserData.exp = exp;
    relationUserData.totalExp = totalExp;
    updateExtendListDataVersion();
}

void zoRelationUser::setRunOffTimes(const DWORD runOffTimes)
{
    relationUserData.runOffTimes = runOffTimes;
    debug("玩家的逃跑次数为: [%u]", relationUserData.runOffTimes);
    updateCommonListDataVersion(true);
}

void zoRelationUser::fillCommonListData(protocol::relation::CommonListData *data) const
{
    data->userID = getID();
    data->commandListUserDataState = 0;
    data->sex = relationUserData.sex;
    data->level = relationUserData.level;
    Tidy::nStringTool::copyStr(data->charName, getName(), sizeof(data->charName)); 
    data->winTimes = relationUserData.winTimes;
    data->loseTimes = relationUserData.loseTimes;
    data->runOffTimes = relationUserData.runOffTimes;
}

void zoRelationUser::updateCommonListDataVersion(const bool needNotifyUser)
{
    _common_list_data_version = server->time().sec();
    updateExtendListDataVersion();
}

void zoRelationUser::updateExtendListDataVersion()
{
    _extend_list_data_version = server->time().sec();
}

void zoRelationUser::setNeedSerialize(eRelationUserDataNeedSaveFlag flag)
{
	if(_relation_user_data_need_save_flag_set.none() == true){
		server->relationUserService->addNeedSerializeUser(this);
	}
	_relation_user_data_need_save_flag_set.set(flag);
}

bool zoRelationUser::saveData()
{
	if(_relation_user_data_need_save_flag_set.none() == true){
		assert_fail("没有存档标志");
		return true;
	}
	protocol::user::UserRelationSaveData relationSaveData;
	relationSaveData.set_charid(getID());
    if(_relation_user_data_need_save_flag_set.test(eRelationUserDataNeedSaveFlag_MessageData) == true){
        if(saveMessageData(&relationSaveData) == false){
            assert_fail("存档失败");
        }
    }
	_relation_user_data_need_save_flag_set.reset();

	BUFFER_PROTO_CMD(protocol::serialize::SaveUserRelationDataCommand, send);
	send->charID = getID();
	if(relationSaveData.SerializeToArray(send->data, PROTO_CMD_CAPACITY(protocol::serialize::SaveUserRelationDataCommand)) == false){
		assert_fail("序列化失败");
		return true;
	}
	send->num = relationSaveData.GetCachedSize();
	return server->messageService->sendCmdToData(send, send->allSize());
}

bool zoRelationUser::saveMessageData(protocol::user::UserRelationSaveData *relationSaveData)
{
    protocol::user::MessageSaveData saveData;
    for(Tidy::map<DWORD, std::string>::iterator iter = _online_need_sys_message_map.begin(); iter !=  _online_need_sys_message_map.end(); ++iter){
        protocol::user::MessageData *messageData = saveData.add_messagedata();
        messageData->set_messageid(iter->first);
        if(iter->second.size() > 0){
            messageData->set_info(iter->second);
        }
    }
    if(saveData.SerializeToString(relationSaveData->mutable_message()) == false){
        assert_fail("那啥错了");
        return false;
    }
    return true;
}

void zoRelationUser::setLastLoginState(const BYTE loginState)
{
    _last_login_state = loginState;
}

BYTE zoRelationUser::getLastLoginState() const
{
    return _last_login_state;
}
