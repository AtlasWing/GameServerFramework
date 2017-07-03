#include "protocol/loginCommand.h"
#include "protocol/forwardCommand.h"
#include "protocol/serializeCommand.h"
#include "protocol/userCommand.h"
#include "protocol/userCommon.h"
#include "protocol/Proto/UserData.pb.h"
#include "protocol/mailCommon.h"

#include "base/nServerConn.h"
#include "base/nDBConnection.h"
#include "base/nDBConnectionPool.h"
#include "game/nTableManager.h"
#include "game/nTable.h"

#include "dataServer/dtUser.h"
#include "dataServer/dtServer.h"
#include "dataServer/dtMessageService.h"
#include "dataServer/dtSerializeService.h"
#include "dataServer/dtUserService.h"
#include "dataServer/dtGMCommand.h"

dtUser::dtUser(const DWORD id, const char *accountName, const char* name, Tidy::nServerConn *logicConn, Tidy::nServerConn* gatewayConn) :
    _logic_conn(logicConn),
    _gateway_conn(gatewayConn),
    _one_min(60 * 1000),
    _need_flush_char_base(false),
    _recharge_money(0),
    _change_logic_binary(NULL),
    _last_fetch_mail_max_id(0),
    _unread_mail_num(0),
    _login_state(protocol::login::eLoginState_None),
    _level(0)
{
    _char_base_table.accountName = accountName;
    _char_base_table.charID = id;
    _char_base_table.name = name;
}

dtUser::~dtUser()
{
    SAFE_DELETE(_change_logic_binary);
}

DWORD dtUser::getCharID() const
{
    return _char_base_table.charID;
}

const char* dtUser::getAccountName() const
{
    return _char_base_table.accountName.c_str();
}

DWORD dtUser::getID() const
{
    return _char_base_table.charID;
}

const char* dtUser::getName() const
{
    return _char_base_table.name.data();
}

void dtUser::setName(const char *name)
{
    _char_base_table.name = name;
    _need_flush_char_base = true;
    _char_base_table_save_flag[SqlCharBase_name] = true;
}

Tidy::nLogger* dtUser::getLogger() const
{
    return server->getLogger();
}

const DWORD dtUser::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const
{
    return snprintf(headerBuffer, bufferSize, "[%u,%s]", getID(), getName());
}

bool dtUser::fillLoginData(protocol::login::UserLoginData *loginData, protocol::user::MainUserData *mainUserData, protocol::login::RoleInfo *charInfo, protocol::user::LoginSerializeBinary *loginSerializeBinary)
{
    charInfo->charID = getID();
    charInfo->level = _char_base_table.level;
    charInfo->exp = _char_base_table.exp;
    charInfo->skillPoint = _char_base_table.skillPoint;
    charInfo->vitality = _char_base_table.vitality;
    charInfo->sex = _char_base_table.sex;
    mainUserData->userType = _char_base_table.userType;
    mainUserData->sex = _char_base_table.sex;
    mainUserData->isAdult = _char_base_table.isAdult;
    mainUserData->level = _char_base_table.level;
    mainUserData->lastOfflineTime = _char_base_table.lastOfflineTime;
    mainUserData->createIP = _char_base_table.createIP;
    mainUserData->onlineTime = _char_base_table.onlineTime;
    mainUserData->createTime = _char_base_table.createTime;
    mainUserData->bitMask = _char_base_table.bitMask;
    mainUserData->curExp = _char_base_table.exp;
    mainUserData->silver = _char_base_table.silver;
    mainUserData->diamond = _char_base_table.diamond;
    mainUserData->bindDiamond = _char_base_table.bindDiamond;
    mainUserData->gmLevel = _char_base_table.gmLevel;

    if(_char_base_table.packageBinary.empty() == false){
        loginSerializeBinary->set_packagebinary(_char_base_table.packageBinary.data(), _char_base_table.packageBinary.size());
    }
    if(_char_base_table.useSkillBinary.empty() == false){
        loginSerializeBinary->set_useskillbinary(_char_base_table.useSkillBinary.data(), _char_base_table.useSkillBinary.size());
    }
    if(_char_base_table.haveSkillBinary.empty() == false){
        loginSerializeBinary->set_haveskillbinary(_char_base_table.haveSkillBinary.data(), _char_base_table.haveSkillBinary.size());
    }
    if(_char_base_table.unlockPassiveSkillBinary.empty() == false){
        loginSerializeBinary->set_havepassivebinary(_char_base_table.unlockPassiveSkillBinary.data(), _char_base_table.unlockPassiveSkillBinary.size());
    }
    if(_char_base_table.haveUnlockPetBinary.empty() == false){
        loginSerializeBinary->set_petdatabinary(_char_base_table.haveUnlockPetBinary.data(), _char_base_table.haveUnlockPetBinary.size());
    }
    if(_char_base_table.duplSqlBinary.empty() == false){
        loginSerializeBinary->set_duplbinary(_char_base_table.duplSqlBinary.data(), _char_base_table.duplSqlBinary.size());
    }
    if(_char_base_table.equipPosBinary.empty() == false){
        loginSerializeBinary->set_equipposbinary(_char_base_table.equipPosBinary.data(), _char_base_table.equipPosBinary.size());
    }
    if(_char_base_table.rewardBinary.empty() == false){
        loginSerializeBinary->set_rewardbinary(_char_base_table.rewardBinary.data(), _char_base_table.rewardBinary.size());
    }
    if(_char_base_table.trumpBinary.empty() == false){
        loginSerializeBinary->set_trumpbinary(_char_base_table.trumpBinary.data(), _char_base_table.trumpBinary.size());
    }
    if(_char_base_table.discoveryBinary.empty() == false){
        loginSerializeBinary->set_discoverybinary(_char_base_table.discoveryBinary.data(), _char_base_table.discoveryBinary.size());
    }
    if(_char_base_table.userDataBinary.empty() == false){
        loginSerializeBinary->set_userdatabinary(_char_base_table.userDataBinary.data(), _char_base_table.userDataBinary.size());
    }
    if(_char_base_table.friendDataBinary.empty() == false){
        loginSerializeBinary->set_frienddatabinary(_char_base_table.friendDataBinary.data(), _char_base_table.friendDataBinary.size());
    }
    if(_char_base_table.mysteryShopBinary.empty() == false){
        loginSerializeBinary->set_mysteryshopbinary(_char_base_table.mysteryShopBinary.data(), _char_base_table.mysteryShopBinary.size());
    }
    if(_char_base_table.taskBinary.empty() == false){
        loginSerializeBinary->set_taskbinary(_char_base_table.taskBinary.data(), _char_base_table.taskBinary.size());
    }
    if(_char_base_table.wingBinary.empty() == false){
        loginSerializeBinary->set_wingbinary(_char_base_table.wingBinary.data(), _char_base_table.wingBinary.size());
    }
    if(_char_base_table.vipDataBinary.empty() == false){
        loginSerializeBinary->set_vipdatabinary(_char_base_table.vipDataBinary.data(), _char_base_table.vipDataBinary.size());
    }
    if(_char_base_table.matchDataBinary.empty() == false){
        loginSerializeBinary->set_matchdatabinary(_char_base_table.matchDataBinary.data(), _char_base_table.matchDataBinary.size());
    }
    return true;
}

bool dtUser::restoreCharBaseData()
{
    static std::vector<bool> restoreCharBaseDataFlag(SqlCharBase_NULL, true);

    // 这几个属性不需要读档
    restoreCharBaseDataFlag[SqlCharBase_charID] = false;
    restoreCharBaseDataFlag[SqlCharBase_accountName] = false;
    restoreCharBaseDataFlag[SqlCharBase_name] = false;

    Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
    if(conn == NULL){
        error("不能从数据库连接池获取连接句柄 %s",__PRETTY_FUNCTION__);
        return false;
    }

    Tidy::nDBQuery query = conn->getQuery();
    Tidy::nDBQueryResult result = query.exeSelect(_char_base_table, restoreCharBaseDataFlag, _char_base_table.equal_list(SqlCharBase_charID), 1);
    if(result.empty() == true){
        return false;
    }

    static std::vector<DWORD> fieldIndexVecData = result.getResultFieldIndexMap<SqlCharBase>(SqlCharBase_NULL);
    static DWORD *fieldIndexVec = &fieldIndexVecData[0];
    _char_base_table.userType = result[0][fieldIndexVec[SqlCharBase_userType]];
    _char_base_table.userDataState = result[0][fieldIndexVec[SqlCharBase_userDataState]];
    _char_base_table.userDataStateChangeTimeInSec = result[0][fieldIndexVec[SqlCharBase_userDataStateChangeTimeInSec]];
    _char_base_table.level = result[0][fieldIndexVec[SqlCharBase_level]];
    _char_base_table.sex = result[0][fieldIndexVec[SqlCharBase_sex]];
    _char_base_table.isAdult = result[0][fieldIndexVec[SqlCharBase_isAdult]];
    _char_base_table.lastOfflineTime = result[0][fieldIndexVec[SqlCharBase_lastOfflineTime]];
    _char_base_table.lastLoginIP = result[0][fieldIndexVec[SqlCharBase_lastLoginIP]].data();
    _char_base_table.createIP = result[0][fieldIndexVec[SqlCharBase_createIP]];
    _char_base_table.onlineTime = result[0][fieldIndexVec[SqlCharBase_onlineTime]];
    _char_base_table.loginTime = result[0][fieldIndexVec[SqlCharBase_loginTime]];
    _char_base_table.createTime = result[0][fieldIndexVec[SqlCharBase_createTime]];
    _char_base_table.bitMask= result[0][fieldIndexVec[SqlCharBase_bitMask]];
    _char_base_table.exp = result[0][fieldIndexVec[SqlCharBase_exp]];
    _char_base_table.vitality = result[0][fieldIndexVec[SqlCharBase_vitality]];
    _char_base_table.skillPoint = result[0][fieldIndexVec[SqlCharBase_skillPoint]];
    _char_base_table.silver = result[0][fieldIndexVec[SqlCharBase_silver]];
    _char_base_table.diamond = result[0][fieldIndexVec[SqlCharBase_diamond]];
    _char_base_table.bindDiamond = result[0][fieldIndexVec[SqlCharBase_bindDiamond]];
    _char_base_table.honorPoints = result[0][fieldIndexVec[SqlCharBase_honorPoints]];
    _char_base_table.clubPoints = result[0][fieldIndexVec[SqlCharBase_clubPoints]];
    _char_base_table.gmLevel = result[0][fieldIndexVec[SqlCharBase_gmLevel]];
    _char_base_table.monthCardStopTime = result[0][fieldIndexVec[SqlCharBase_monthCardStopTime]];
    _char_base_table.vipExp = result[0][fieldIndexVec[SqlCharBase_vipExp]];
    _char_base_table.vipLevel = result[0][fieldIndexVec[SqlCharBase_vipLevel]];
    _char_base_table.thoughtValue = result[0][fieldIndexVec[SqlCharBase_thoughtValue]];
    _char_base_table.fightPower = result[0][fieldIndexVec[SqlCharBase_fightPower]];

    if(result[0][fieldIndexVec[SqlCharBase_packageBinary]].is_null() == false){
        _char_base_table.packageBinary = result[0][fieldIndexVec[SqlCharBase_packageBinary]];
    }
    if(result[0][fieldIndexVec[SqlCharBase_useSkillBinary]].is_null() == false){
        _char_base_table.useSkillBinary = result[0][fieldIndexVec[SqlCharBase_useSkillBinary]];
    }
    if(result[0][fieldIndexVec[SqlCharBase_haveSkillBinary]].is_null() == false){
        _char_base_table.haveSkillBinary = result[0][fieldIndexVec[SqlCharBase_haveSkillBinary]];
    }
    if(result[0][fieldIndexVec[SqlCharBase_unlockPassiveSkillBinary]].is_null() == false){
        _char_base_table.unlockPassiveSkillBinary = result[0][fieldIndexVec[SqlCharBase_unlockPassiveSkillBinary]];
    }
    if(result[0][fieldIndexVec[SqlCharBase_haveUnlockPetBinary]].is_null() == false){
        _char_base_table.haveUnlockPetBinary = result[0][fieldIndexVec[SqlCharBase_haveUnlockPetBinary]];
    }
    if(result[0][fieldIndexVec[SqlCharBase_duplSqlBinary]].is_null() == false){
        _char_base_table.duplSqlBinary = result[0][fieldIndexVec[SqlCharBase_duplSqlBinary]];
    }
    if(result[0][fieldIndexVec[SqlCharBase_trumpBinary]].is_null() == false){
        _char_base_table.trumpBinary = result[0][fieldIndexVec[SqlCharBase_trumpBinary]];
    }
    if(result[0][fieldIndexVec[SqlCharBase_equipPosBinary]].is_null() == false){
        _char_base_table.equipPosBinary = result[0][fieldIndexVec[SqlCharBase_equipPosBinary]];
    }
    if(result[0][fieldIndexVec[SqlCharBase_rewardBinary]].is_null() == false){
        _char_base_table.rewardBinary = result[0][fieldIndexVec[SqlCharBase_rewardBinary]];
    }
    if(result[0][fieldIndexVec[SqlCharBase_discoveryBinary]].is_null() == false){
        _char_base_table.discoveryBinary = result[0][fieldIndexVec[SqlCharBase_discoveryBinary]];
    }
    if(result[0][fieldIndexVec[SqlCharBase_userDataBinary]].is_null() == false){
        _char_base_table.userDataBinary = result[0][fieldIndexVec[SqlCharBase_userDataBinary]];
    }
    if(result[0][fieldIndexVec[SqlCharBase_friendDataBinary]].is_null() == false){
        _char_base_table.friendDataBinary = result[0][fieldIndexVec[SqlCharBase_friendDataBinary]];
    }
    if(result[0][fieldIndexVec[SqlCharBase_mysteryShopBinary]].is_null() == false){
        _char_base_table.mysteryShopBinary = result[0][fieldIndexVec[SqlCharBase_mysteryShopBinary]];
    }
    if(result[0][fieldIndexVec[SqlCharBase_taskBinary]].is_null() == false){
        _char_base_table.taskBinary = result[0][fieldIndexVec[SqlCharBase_taskBinary]];
    }
    if(result[0][fieldIndexVec[SqlCharBase_wingBinary]].is_null() == false){
        _char_base_table.wingBinary = result[0][fieldIndexVec[SqlCharBase_wingBinary]];
    }
    if(result[0][fieldIndexVec[SqlCharBase_matchDataBinary]].is_null() == false){
        _char_base_table.matchDataBinary = result[0][fieldIndexVec[SqlCharBase_matchDataBinary]];
    }
    if(result[0][fieldIndexVec[SqlCharBase_vipDataBinary]].is_null() == false){
        _char_base_table.vipDataBinary = result[0][fieldIndexVec[SqlCharBase_vipDataBinary]];
    }
    SqlMailDetailData mailTitle;
    Tidy::nDBQuery mailQuery = conn->getQuery();
    std::ostringstream whereCondition;
    whereCondition << "DEST_ID="<< getID()<<" AND MAIL_STATE="<< protocol::mail::MailState_New<<" AND IS_DELETE_FROM_INBOX != 1";
    Tidy::nDBQueryResult resultSet = mailQuery.exeSelect(mailTitle.table(), " COUNT(*) AS UNREAD_MAIL_NUM", whereCondition.str(), 0);
    if(resultSet.empty() == false){
        _unread_mail_num = resultSet[0]["UNREAD_MAIL_NUM"];
    }

    _level = _char_base_table.level;
    return true;
}

bool dtUser::saveCharBaseData(AsyncSqlCallback *completeCallback)
{
    assert_debug(_need_flush_char_base == true, "");
    assert_debug(_char_base_table_save_flag.none() == false, "");
    static std::vector<bool> saveCharBaseDataFlag;
    saveCharBaseDataFlag.assign(SqlCharBase_NULL, false);
    for(DWORD i = 0; i < SqlCharBase_NULL; ++ i){
        if(_char_base_table_save_flag[i] == true){
            saveCharBaseDataFlag[i] = true;
        }
    }
    // 每次存档刷新一下下线时间
    _char_base_table.lastOfflineTime = server->time().sec();
    saveCharBaseDataFlag[SqlCharBase_lastOfflineTime] = true;
    static Tidy::nDBQuery query(NULL);
    std::string sql = query.getUpdateSql(_char_base_table, saveCharBaseDataFlag, _char_base_table.equal_list(SqlCharBase_charID), 1);
    server->serializeService->exeAsyncSql(eDBConnectionID_ZoneMain, sql, completeCallback);

    // TODO 同步到zone下线时间
    debug("存档成功时间[%u]", _char_base_table.lastOfflineTime);
    return true;
}

void dtUser::checkFlush(AsyncSqlCallback *completeCallback)
{
    if(_need_flush_char_base == true){
        if(saveCharBaseData(completeCallback) == true){
            _need_flush_char_base = false;
            _char_base_table_save_flag.reset();
        }
    }
}

bool dtUser::doTick()
{
    //不在这里加Timer,一秒存一次,压力大的时候再调整时间
    checkFlush(NULL);
    return true;
}

bool dtUser::reg(const protocol::login::UserLoginData& data)
{
    assert_debug(server->userService->checkUserDataSaveComplete(getID()), "没有完成存档,数据被覆盖[%u]", getID());
    if(restoreCharBaseData() == false){
        return false;
    }

    _gm_command = Game::nGMCommand::createInstance<dtGMCommand>(this);
    setLastLoginIP(data.ip);
    return true;
}

namespace{
    struct CompleteSaveUserDataExec : public AsyncSqlCallback{
        DWORD _char_id;
        CompleteSaveUserDataExec(const DWORD charID) : _char_id(charID){
            server->userService->dealStartSaveUserData(_char_id);
        }
        virtual bool dealExecResult(bool result){
            server->userService->dealCompleteSaveUserData(_char_id);
            return true;
        }
    };
};
void dtUser::unreg()
{
    // 保证存档下线时间
    _need_flush_char_base = true;
    _char_base_table_save_flag[SqlCharBase_lastOfflineTime] = true;
    updateOnlineTime();

    CompleteSaveUserDataExec *saveUserDataCallback = new CompleteSaveUserDataExec(getID());
    checkFlush(saveUserDataCallback);
}

Tidy::nServerConn* dtUser::getLogicConn() const
{
    return _logic_conn;
}

void dtUser::setLogicConn(Tidy::nServerConn* logicConn)
{
    _logic_conn = logicConn;
}

Tidy::nServerConn* dtUser::getGatewayConn() const
{
    return _gateway_conn;
}

void dtUser::setGatewayConn(Tidy::nServerConn* gatewayConn)
{
    _gateway_conn = gatewayConn;
}

void dtUser::dealLoginSuccess()
{
    if(_unread_mail_num > 0){
        //server->chatService->sendDebugMessageToUser(this, "您有新邮件!");
    }
    sendUnreadMailNum();
    updateLoginTime();
}


void dtUser::updateUserData(protocol::user::UserSaveData &saveData)
{
    if(saveData.has_curexp() == true){
        if(_char_base_table.exp != saveData.curexp()){
            _char_base_table.exp = saveData.curexp();
            _char_base_table_save_flag[SqlCharBase_exp] = true;
            _need_flush_char_base = true;
        }
    }
    if(saveData.has_level() == true){
        if(_char_base_table.level != saveData.level()){
            _char_base_table.level = saveData.level();
            _char_base_table_save_flag[SqlCharBase_level] = true;
            _need_flush_char_base = true;
        }
    }
    if(saveData.has_bitmask() == true){
        if(_char_base_table.bitMask != saveData.bitmask()){
            _char_base_table.bitMask = saveData.bitmask();
            _char_base_table_save_flag[SqlCharBase_bitMask] = true;
            _need_flush_char_base = true;
        }
    }
    if(saveData.has_vitality() == true){
        if(_char_base_table.vitality != saveData.vitality()){
            _char_base_table.vitality = saveData.vitality();
            _char_base_table_save_flag[SqlCharBase_vitality] = true;
            _need_flush_char_base = true;
        }
    }
    if(saveData.has_thoughtvalue() == true){
        if(_char_base_table.thoughtValue != saveData.thoughtvalue()){
            _char_base_table.thoughtValue = saveData.thoughtvalue();
            _char_base_table_save_flag[SqlCharBase_thoughtValue] = true;
            _need_flush_char_base = true;
        }
    }
    if(saveData.has_skillpoint() == true){
        if(_char_base_table.skillPoint != saveData.skillpoint()){
            _char_base_table.skillPoint = saveData.skillpoint();
            _char_base_table_save_flag[SqlCharBase_skillPoint] = true;
            _need_flush_char_base = true;
        }
    }
    if(saveData.has_silver() == true){
        if(_char_base_table.silver != saveData.silver()){
            _char_base_table.silver = saveData.silver();
            _char_base_table_save_flag[SqlCharBase_silver] = true;
            _need_flush_char_base = true;
        }
    }
    if(saveData.has_diamond() == true){
        if(_char_base_table.diamond != saveData.diamond()){
            _char_base_table.diamond = saveData.diamond();
            _char_base_table_save_flag[SqlCharBase_diamond] = true;
            _need_flush_char_base = true;
        }
    }
    if(saveData.has_binddiamond() == true){
        if(_char_base_table.bindDiamond != saveData.binddiamond()){
            _char_base_table.bindDiamond = saveData.binddiamond();
            _char_base_table_save_flag[SqlCharBase_bindDiamond] = true;
            _need_flush_char_base = true;
        }
    }
    if(saveData.has_honorpoints() == true){
        if(_char_base_table.honorPoints != saveData.honorpoints()){
            _char_base_table.honorPoints = saveData.honorpoints();
            _char_base_table_save_flag[SqlCharBase_honorPoints] = true;
            _need_flush_char_base = true;
        }
    }
    if(saveData.has_clubpoints() == true){
        if(_char_base_table.clubPoints != saveData.clubpoints()){
            _char_base_table.clubPoints = saveData.clubpoints();
            _char_base_table_save_flag[SqlCharBase_clubPoints] = true;
            _need_flush_char_base = true;
        }
    }
    if(saveData.has_gmlevel() == true){
        if(_char_base_table.gmLevel != saveData.gmlevel()){
            _char_base_table.gmLevel = saveData.gmlevel();
            _char_base_table_save_flag[SqlCharBase_gmLevel] = true;
            _need_flush_char_base = true;
        }
    }
    if(saveData.has_monthcardstoptime() == true){
        if(_char_base_table.monthCardStopTime != saveData.monthcardstoptime()){
            _char_base_table.monthCardStopTime = saveData.monthcardstoptime();
            _char_base_table_save_flag[SqlCharBase_monthCardStopTime] = true;
            _need_flush_char_base = true;
        }
    }
    if(saveData.has_vipexp() == true){
        if(_char_base_table.vipExp != saveData.vipexp()){
            _char_base_table.vipExp = saveData.vipexp();
            _char_base_table_save_flag[SqlCharBase_vipExp] = true;
            _need_flush_char_base = true;
        }
    }
    if(saveData.has_viplevel() == true){
        if(_char_base_table.vipLevel != saveData.viplevel()){
            _char_base_table.vipLevel = saveData.viplevel();
            _char_base_table_save_flag[SqlCharBase_vipLevel] = true;
            _need_flush_char_base = true;
        }
    }
    if(saveData.has_packagebinary() == true){
        if(saveData.packagebinary().compare(0, saveData.packagebinary().size(), _char_base_table.packageBinary.data(), _char_base_table.packageBinary.size()) != 0){
            _char_base_table.packageBinary = saveData.packagebinary();
            _char_base_table_save_flag[SqlCharBase_packageBinary] = true;
            _need_flush_char_base = true;
        }
        else{
            error("包裹数据未变化[%u]", _char_base_table.charID);
        }
    }
    if(saveData.has_useskillbinary() == true){
        if(saveData.useskillbinary().compare(0, saveData.useskillbinary().size(), _char_base_table.useSkillBinary.data(), _char_base_table.useSkillBinary.size()) != 0){
            _char_base_table.useSkillBinary = saveData.useskillbinary();
            _char_base_table_save_flag[SqlCharBase_useSkillBinary] = true;
            _need_flush_char_base = true;
        }
        else{
            error("使用技能列表未变化[%u]", _char_base_table.charID);
        }
    }
    if(saveData.has_haveskillbinary() == true){
        if(saveData.haveskillbinary().compare(0, saveData.haveskillbinary().size(), _char_base_table.haveSkillBinary.data(), _char_base_table.haveSkillBinary.size()) != 0) {
            _char_base_table.haveSkillBinary = saveData.haveskillbinary();
            _char_base_table_save_flag[SqlCharBase_haveSkillBinary] = true;
            _need_flush_char_base = true;
        }
        else{
            error("拥有技能列表未变化[%u]", _char_base_table.charID);
        }
    }
    if (saveData.has_havepassivebinary() == true){
        if(saveData.havepassivebinary().compare(0 ,saveData.havepassivebinary().size(), _char_base_table.unlockPassiveSkillBinary.data(), _char_base_table.unlockPassiveSkillBinary.size()) != 0){
            _char_base_table.unlockPassiveSkillBinary = saveData.havepassivebinary();
            _char_base_table_save_flag[SqlCharBase_unlockPassiveSkillBinary] = true;
            _need_flush_char_base = true;
        }
        else{

        }
    }
    if (saveData.has_unlockpetdata() == true){
        if(saveData.unlockpetdata().compare(0 ,saveData.unlockpetdata().size(), _char_base_table.haveUnlockPetBinary.data(), _char_base_table.haveUnlockPetBinary.size()) != 0){
            _char_base_table.haveUnlockPetBinary = saveData.unlockpetdata();
            _char_base_table_save_flag[SqlCharBase_haveUnlockPetBinary] = true;
            _need_flush_char_base = true;
        }
    }
    if(saveData.has_duplbinary() == true){
        if(saveData.duplbinary().compare(0 ,saveData.duplbinary().size(), _char_base_table.duplSqlBinary.data(), _char_base_table.duplSqlBinary.size()) != 0){
            _char_base_table.duplSqlBinary = saveData.duplbinary();
            _char_base_table_save_flag[SqlCharBase_duplSqlBinary] = true;
            _need_flush_char_base = true;
        }
        else{
            error("副本数据未变化[%u]", _char_base_table.charID);
        }
    }
    if(saveData.has_trumpbinary() == true){
        if(saveData.trumpbinary().compare(0 ,saveData.trumpbinary().size(), _char_base_table.trumpBinary.data(), _char_base_table.trumpBinary.size()) != 0){
            _char_base_table.trumpBinary = saveData.trumpbinary();
            _char_base_table_save_flag[SqlCharBase_trumpBinary] = true;
            _need_flush_char_base = true;
        }
        else{
            error("法宝数据未变化[%u]", _char_base_table.charID);
        }
    }
    if(saveData.has_equipposbinary() == true){
        if(saveData.equipposbinary().compare(0 ,saveData.equipposbinary().size(), _char_base_table.equipPosBinary.data(), _char_base_table.equipPosBinary.size()) != 0){
            _char_base_table.equipPosBinary = saveData.equipposbinary();
            _char_base_table_save_flag[SqlCharBase_equipPosBinary] = true;
            _need_flush_char_base = true;
        }
        else{
            error("装备强化数据未变化[%u]", _char_base_table.charID);
        }
    }
    if(saveData.has_rewardbinary() == true){
        if(saveData.rewardbinary().compare(0 ,saveData.rewardbinary().size(), _char_base_table.rewardBinary.data(), _char_base_table.rewardBinary.size()) != 0){
            _char_base_table.rewardBinary = saveData.rewardbinary();
            _char_base_table_save_flag[SqlCharBase_rewardBinary] = true;
            _need_flush_char_base = true;
        }
        else{
            error("奖励数据未变化[%u]", _char_base_table.charID);
        }
    }
    if(saveData.has_discoverybinary() == true){
        if(saveData.discoverybinary().compare(0 ,saveData.discoverybinary().size(), _char_base_table.discoveryBinary.data(), _char_base_table.discoveryBinary.size()) != 0){
            _char_base_table.discoveryBinary = saveData.discoverybinary();
            _char_base_table_save_flag[SqlCharBase_discoveryBinary] = true;
            _need_flush_char_base = true;
        }
        else{
            error("星海探秘数据未变化[%u]", _char_base_table.charID);
        }
    }
    if(saveData.has_userdatabinary() == true){
        if(saveData.userdatabinary().compare(0 ,saveData.userdatabinary().size(), _char_base_table.userDataBinary.data(), _char_base_table.userDataBinary.size()) != 0){
            _char_base_table.userDataBinary = saveData.userdatabinary();
            _char_base_table_save_flag[SqlCharBase_userDataBinary] = true;
            _need_flush_char_base = true;
        }
        else{
            error("玩家数据未变化[%u]", _char_base_table.charID);
        }
    }
    if(saveData.has_frienddatabinary() == true){
        if(saveData.frienddatabinary().compare(0 ,saveData.frienddatabinary().size(), _char_base_table.friendDataBinary.data(), _char_base_table.friendDataBinary.size()) != 0){
            _char_base_table.friendDataBinary = saveData.frienddatabinary();
            _char_base_table_save_flag[SqlCharBase_friendDataBinary] = true;
            _need_flush_char_base = true;
        }
        else{
            error("好友关系数据未变化[%u]", _char_base_table.charID);
        }
    }
    if(saveData.has_mysteryshopbinary() == true){
        if(saveData.mysteryshopbinary().compare(0 ,saveData.mysteryshopbinary().size(), _char_base_table.mysteryShopBinary.data(), _char_base_table.mysteryShopBinary.size()) != 0){
            _char_base_table.mysteryShopBinary = saveData.mysteryshopbinary();
            _char_base_table_save_flag[SqlCharBase_mysteryShopBinary] = true;
            _need_flush_char_base = true;
        }
        else{
            error("神秘商店关系数据未变化[%u]", _char_base_table.charID);
        }
    }
    if(saveData.has_taskbinary() == true){
        if(saveData.taskbinary().compare(0 ,saveData.taskbinary().size(), _char_base_table.taskBinary.data(), _char_base_table.taskBinary.size()) != 0){
            _char_base_table.taskBinary = saveData.taskbinary();
            _char_base_table_save_flag[SqlCharBase_taskBinary] = true;
            _need_flush_char_base = true;
        }
        else{
            error("任务数据未变化[%u]", _char_base_table.charID);
        }
    }
    if(saveData.has_wingbinary() == true){
        if(saveData.wingbinary().compare(0 ,saveData.wingbinary().size(), _char_base_table.wingBinary.data(), _char_base_table.wingBinary.size()) != 0){
            _char_base_table.wingBinary = saveData.wingbinary();
            _char_base_table_save_flag[SqlCharBase_wingBinary] = true;
            _need_flush_char_base = true;
        }
        else{
            error("翅膀数据未变化[%u]", _char_base_table.charID);
        }
    }
    if(saveData.has_matchdatabinary() == true){
        if(saveData.matchdatabinary().compare(0 ,saveData.matchdatabinary().size(), _char_base_table.matchDataBinary.data(), _char_base_table.matchDataBinary.size()) != 0){
            _char_base_table.matchDataBinary = saveData.matchdatabinary();
            _char_base_table_save_flag[SqlCharBase_matchDataBinary] = true;
            _need_flush_char_base = true;
        }
        else{
            error("比赛数据未变化[%u]", _char_base_table.charID);
        }
    }
    if(saveData.has_vipdatabinary() == true){
        if(saveData.vipdatabinary().compare(0 ,saveData.vipdatabinary().size(), _char_base_table.vipDataBinary.data(), _char_base_table.vipDataBinary.size()) != 0){
            _char_base_table.vipDataBinary = saveData.vipdatabinary();
            _char_base_table_save_flag[SqlCharBase_vipDataBinary] = true;
            _need_flush_char_base = true;
        }
        else{
            error("vip数据未变化[%u]", _char_base_table.charID);
        }
    }
}

void dtUser::updateFightPower(const DWORD fightPower)
{
    if(_char_base_table.fightPower == fightPower){
        return;
    }
    _char_base_table.fightPower = fightPower;
    _char_base_table_save_flag[SqlCharBase_fightPower] = true;
    _need_flush_char_base = true;
}

void dtUser::clearChangeLogicData()
{
    SAFE_DELETE(_change_logic_binary);
}

void dtUser::setLevel(const WORD level)
{
    _level = level;
}

void dtUser::setIsAdult(bool isAdult)
{
    assert_debug(static_cast<bool>(_char_base_table.isAdult) != isAdult, "数值未变");
    _char_base_table.isAdult = isAdult;
    _need_flush_char_base = true;
    _char_base_table_save_flag[SqlCharBase_isAdult] = true;
}

bool dtUser::syncLoginState(const BYTE newState)
{
    using namespace protocol::login;

    const BYTE oldState = _login_state;
    debug("loginState [%u]-->[%u]", oldState, newState);
    _login_state = newState;
    return true;
}

bool dtUser::restoreRechargeMoney()
{
    info("读取充值数据成功");
    return true;
}

bool dtUser::drawRechargeMoney(const DWORD rechargeMoney)
{
    info("玩家提取充值成功");
    return true;
}

void dtUser::updateLoginTime()
{
    if(_char_base_table.loginTime == server->time().sec()){
        return;
    }
    _char_base_table.loginTime = server->time().sec();
    debug("刷新登录时间[%u]", _char_base_table.loginTime);
    _need_flush_char_base = true;
    _char_base_table_save_flag[SqlCharBase_loginTime] = true;
}

void dtUser::updateOnlineTime()
{
    DWORD onlineTime = server->time().sec() - _char_base_table.loginTime;
    if(onlineTime == 0){
        return;
    }
    _char_base_table.onlineTime += onlineTime;
    _need_flush_char_base = true;
    _char_base_table_save_flag[SqlCharBase_onlineTime] = true;
}

void dtUser::setLastLoginIP(const char *ip)
{
    if(Tidy::nStringTool::compare(_char_base_table.lastLoginIP.data(), ip) == 0){
        return;
    }
    // 不需要立即存档
    _char_base_table.lastLoginIP = ip;
    _char_base_table_save_flag[SqlCharBase_lastLoginIP] = true;
}

const char * dtUser::getLastLoginIP() const
{
    return _char_base_table.lastLoginIP.c_str();
}

BYTE dtUser::getLevel() const
{
    return _char_base_table.level;
}

const DWORD dtUser::getLastFetchMailID() const
{
    return _last_fetch_mail_max_id; 
}

void dtUser::setLastFetchMailID(const DWORD mailID)
{
    _last_fetch_mail_max_id = mailID;
}

void dtUser::incUnreadMailNum()
{
    ++_unread_mail_num;
    sendUnreadMailNum();
}

void dtUser::decUnreadMailNum()
{
    assert_debug(_unread_mail_num > 0, "未读邮件数量为0");
    --_unread_mail_num;
    sendUnreadMailNum();
}

void dtUser::clearUnreadMailNum()
{
    if(_unread_mail_num != 0){
        _unread_mail_num = 0;
        sendUnreadMailNum();
    }
}

bool dtUser::sendUnreadMailNum()
{
    debug("通知客户端未读邮件数量[%u]", _unread_mail_num);
    return true;
}

void dtUser::setMiscBoolData(eDataUserBoolDataType data)
{
    _user_misc_bool_data_set.set(data, true);
}

void dtUser::clearMiscBoolData(eDataUserBoolDataType data)
{
    _user_misc_bool_data_set.reset(data);
}

bool dtUser::checkMiscBoolData(eDataUserBoolDataType data) const
{
    return _user_misc_bool_data_set.test(data);
}
