#include "protocol/mailCommand.h"
#include "protocol/Proto/Mail.pb.h"

#include "base/nDBConnection.h"
#include "base/nDBConnectionPool.h"

#include "game/nSsqls.h"
#include "game/nTable.h"
#include "game/nTableManager.h"

#include "dataServer/dtMailService.h"
#include "dataServer/dtUserService.h"
#include "dataServer/dtUser.h"
#include "dataServer/dtServer.h"
#include "dataServer/dtSerializeService.h"
#include "dataServer/dtMessageService.h"

// 增加一个时间的随机,避免不同的区在一个数据库上同时操作,导致数据库缓存空间不足
dtMailService::dtMailService():
	Game::nMailService<dtServer>(server),
    _delete_timeout_mail_clocker(3 * ONE_HOUR /1000 + Tidy::randBetween(1, 500), ONE_DAY / 1000)
{
}

dtMailService::~dtMailService()
{
}

bool dtMailService::initService()
{
	_sys_mail_title = "系统邮件"; // TODO 读取配置
    deleteAllTimeoutMail();
    return true;
}

void dtMailService::finalService()
{
}

bool dtMailService::doTick()
{
    if(_delete_timeout_mail_clocker(server->time()) == true) {
        deleteAllTimeoutMail();
    }
    return true;
}

const DWORD dtMailService::getMinEffectMailTime()
{
    static const DWORD maxMailEffectTimeInSec = Tidy::global["MAX_MAIL_EFFECT_TIME_IN_DAY"].getDword() * 24 * 3600;
    assert_debug(maxMailEffectTimeInSec > 0, "");
    return server->time().sec() - maxMailEffectTimeInSec;
}

bool dtMailService::createMail(const protocol::mail::CreateMailData &mailData, const DWORD *destIDList, const DWORD destIDListNum)
{
	return createMail(mailData.mailtype(), mailData.has_fromid() ? mailData.fromid() : 0, destIDList, destIDListNum, (mailData.has_title()) ? mailData.title().c_str() : "", (mailData.has_maintext()) ? mailData.maintext().c_str() : "", mailData.has_moneydatabinary() ? mailData.moneydatabinary().data() : NULL, mailData.has_moneydatabinary() ? mailData.moneydatabinary().size() : 0, mailData.has_itemdata() ? mailData.itemdata().data() : NULL, mailData.has_itemdata() ? mailData.itemdata().size() : 0);
}

bool dtMailService::createMail(const BYTE mailType, const DWORD fromID, const DWORD *destIDList, const DWORD destIDListNum, const char* title, const char* mainText, const char* moneyData, const DWORD moneyDataLen, const char* itemData, const DWORD itemDataLen)
{
    assert_debug(mainText != NULL, "");

    SqlMailDetailData mailDetail;
    mailDetail.mailID = 0;
    mailDetail.mailType = mailType;
    mailDetail.mailState = protocol::mail::MailState_New;
    mailDetail.createTime = server->time().sec();
    mailDetail.fromID = fromID;
    mailDetail.title = title;
    mailDetail.mainText = mainText;
    mailDetail.isDeleteFromInbox = protocol::mail::MailDeleteState_None;
    mailDetail.isDeleteFromOutbox = protocol::mail::MailDeleteState_None;

    std::vector<bool> mailDetailColBitSet(SqlMailDetailData_NULL, false);
    mailDetailColBitSet[SqlMailDetailData_mailType] = true;
    mailDetailColBitSet[SqlMailDetailData_mailState] = true;
    mailDetailColBitSet[SqlMailDetailData_createTime] = true;
    mailDetailColBitSet[SqlMailDetailData_fromID] = true;
    mailDetailColBitSet[SqlMailDetailData_destID] = true;
    mailDetailColBitSet[SqlMailDetailData_title] = true;
    mailDetailColBitSet[SqlMailDetailData_mainText] = true;
    mailDetailColBitSet[SqlMailDetailData_isDeleteFromInbox] = true;
    mailDetailColBitSet[SqlMailDetailData_isDeleteFromOutbox] = true;
    
    if(itemData != NULL && itemDataLen > 0){
        mailDetail.itemData = std::string(itemData, itemDataLen);
        mailDetailColBitSet[SqlMailDetailData_itemData] = true;
    }
    if(moneyData != NULL && moneyDataLen > 0){
		mailDetail.moneyData = std::string(moneyData, moneyDataLen);
        mailDetailColBitSet[SqlMailDetailData_moneyData] = true;
    }

	Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
	if(conn == NULL){
        assert_fail("无法获得数据可连接");
		return false;
	}
	Tidy::nDBQuery query = conn->getQuery();
    DWORD destID = 0;
    dtUser* fromUser = server->userService->getUserByID(fromID);
    const std::string *fromName = &_sys_mail_title;
    if(fromID == 0){
        assert_fail("fromID不可能为0");
        return false;
    }
    if(fromID != Tidy::global["SYS_MAIL_FROM_ID"].getDword()){
        fromName = server->userService->getUserNameByID(fromID);
    }
    if(fromName == NULL){
        assert_debug(fromName != NULL, "fromID=[%u]", fromID);
        return false;
    }
    protocol::mail::MailTitleList mailList;
    protocol::mail::MailTitle* mail = mailList.add_mail();
    mail->set_mailtype(mailType);
    mail->set_fromid(fromID);
    mail->set_fromusername(*fromName);
    mail->set_title(title);
    mail->set_mailstate(protocol::mail::MailState_New);
    mail->set_createtimeinsec(mailDetail.createTime);
    if((moneyData != NULL && moneyDataLen > 0) || (itemData != NULL && itemDataLen > 0)){
        mail->set_haveattachment(true);
    }

    // 通过一次事务提交,可以提高数据的插入速度
    Tidy::vector<DWORD> mailIDList;
    mailIDList.reserve(destIDListNum);
	mysqlpp::Transaction *insertMailTransaction = NULL;
	if(destIDListNum > 1){
		insertMailTransaction = new mysqlpp::Transaction(*conn);
	}
	for(DWORD i = 0; i < destIDListNum; ++ i){
		destID = destIDList[i];
		mailDetail.destID = destID;
		mailDetail.mailID = query.exeInsert(mailDetail, mailDetailColBitSet);
		if(mailDetail.mailID == Tidy::nDBQuery::ERROR){
			assert_fail("插入邮件正文失败");
			SAFE_DELETE(insertMailTransaction);
			return false;
		}
		mailIDList.push_back(mailDetail.mailID);

		Tidy::nThread::getRunningThread()->setRunning();
	}
	if(insertMailTransaction != NULL){
		insertMailTransaction->commit();
		SAFE_DELETE(insertMailTransaction);
	}

    // TODO 创建一封邮件
    return true;
}

bool dtMailService::fetchMailList(dtUser* user)
{
    if(user->getLastFetchMailID() != 0){
        return true;
    }
    SqlMailDetailData mailTitle;
	Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
    if(conn == NULL){
        assert_fail("无法获得连接");
        return false;
    }
    Tidy::nDBQuery query = conn->getQuery();
    char whereCondition[1024];
    if(user->getLastFetchMailID() == 0){
        snprintf(whereCondition, sizeof(whereCondition), "( (%s='%u' AND %s='%u') OR (%s='%u' AND %s='%u') ) AND %s > '%u' ORDER BY %s DESC", SqlMailDetailData::names[SqlMailDetailData_fromID], user->getID(), SqlMailDetailData::names[SqlMailDetailData_isDeleteFromOutbox], protocol::mail::MailDeleteState_None, SqlMailDetailData::names[SqlMailDetailData_destID], user->getID(), SqlMailDetailData::names[SqlMailDetailData_isDeleteFromInbox], protocol::mail::MailDeleteState_None, SqlMailDetailData::names[SqlMailDetailData_createTime], getMinEffectMailTime(), SqlMailDetailData::names[SqlMailDetailData_mailID]);
    }
    else{
        snprintf(whereCondition, sizeof(whereCondition), "%s > '%u' AND ( (%s='%u' AND %s='%u') OR (%s='%u' AND %s='%u') ) AND %s > '%u' ORDER BY %s DESC", SqlMailDetailData::names[SqlMailDetailData_mailID], user->getLastFetchMailID(), SqlMailDetailData::names[SqlMailDetailData_fromID], user->getID(), SqlMailDetailData::names[SqlMailDetailData_isDeleteFromOutbox], protocol::mail::MailDeleteState_None, SqlMailDetailData::names[SqlMailDetailData_destID], user->getID(), SqlMailDetailData::names[SqlMailDetailData_isDeleteFromInbox], protocol::mail::MailDeleteState_None, SqlMailDetailData::names[SqlMailDetailData_createTime], getMinEffectMailTime(), SqlMailDetailData::names[SqlMailDetailData_mailID]);
    }
    char columnList[1024];
	snprintf(columnList, sizeof(columnList), " %s, %s, %s, %s, %s, %s, %s, (%s is not null or %s is not null) as haveAttachment ", SqlMailDetailData::names[SqlMailDetailData_mailType], SqlMailDetailData::names[SqlMailDetailData_mailID], SqlMailDetailData::names[SqlMailDetailData_fromID], SqlMailDetailData::names[SqlMailDetailData_destID], SqlMailDetailData::names[SqlMailDetailData_title], SqlMailDetailData::names[SqlMailDetailData_mailState], SqlMailDetailData::names[SqlMailDetailData_createTime], SqlMailDetailData::names[SqlMailDetailData_itemData], SqlMailDetailData::names[SqlMailDetailData_moneyData]);
    Tidy::nDBQueryResult resultSet = query.exeSelect(mailTitle.table(), columnList, whereCondition, 0);
    if(resultSet.empty() == true){
        return true;
    }
    // TODO 获取邮件列表
    return true;
}

bool dtMailService::fetchMail(dtUser* user, const DWORD mailID)
{
	Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
    if(conn == NULL){
        assert_fail("无法获得连接");
        return false;
    }
    SqlMailDetailData mailDetail;
	mailDetail.mailID = mailID;
    char whereCondition[1024] = "";
    snprintf(whereCondition, sizeof(whereCondition), " %s='%u' AND( (%s='%u' AND %s='%u') OR (%s='%u' AND %s='%u')) AND %s > '%u'", SqlMailDetailData::names[SqlMailDetailData_mailID], mailID, SqlMailDetailData::names[SqlMailDetailData_fromID], user->getID(), SqlMailDetailData::names[SqlMailDetailData_isDeleteFromOutbox], protocol::mail::MailDeleteState_None, SqlMailDetailData::names[SqlMailDetailData_destID], user->getID(), SqlMailDetailData::names[SqlMailDetailData_isDeleteFromInbox], protocol::mail::MailDeleteState_None, SqlMailDetailData::names[SqlMailDetailData_createTime], getMinEffectMailTime());
    Tidy::nDBQuery query = conn->getQuery();
    Tidy::nDBQueryResult resultSet = query.exeSelect(mailDetail.table(), mailDetail.field_list(SqlMailDetailData_destID, SqlMailDetailData_mailState, SqlMailDetailData_mainText, SqlMailDetailData_moneyData, SqlMailDetailData_itemData), whereCondition, 1);
    if(resultSet.empty() == true){
        return true;
    }

    if(static_cast<BYTE>(resultSet[0][SqlMailDetailData::names[SqlMailDetailData_mailState]]) == protocol::mail::MailState_New && static_cast<DWORD>(resultSet[0][SqlMailDetailData::names[SqlMailDetailData_destID]]) == user->getID()){
		user->decUnreadMailNum();
        mailDetail.mailState = protocol::mail::MailState_Readed;
        if(query.exeUpdate(mailDetail.table(), mailDetail.equal_list(SqlMailDetailData_mailState), mailDetail.equal_list(SqlMailDetailData_mailID), 1) != 1){
            assert_fail("设置状态失败");
            return false;
        }
    }

    // TODO
    return true;
}

bool dtMailService::fetchMailAttachmentAll(dtUser* user)
{
	Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
    if(conn == NULL){
        assert_fail("无法获得连接");
        return false;
    }
    SqlMailDetailData mailDetail;
    char whereCondition[1024] = "";
    snprintf(whereCondition, sizeof(whereCondition), " %s='%u' AND %s='%u' AND %s>'%u' ", SqlMailDetailData::names[SqlMailDetailData_destID], user->getID(), SqlMailDetailData::names[SqlMailDetailData_isDeleteFromInbox], protocol::mail::MailDeleteState_None, SqlMailDetailData::names[SqlMailDetailData_createTime], getMinEffectMailTime());
    Tidy::nDBQuery query = conn->getQuery();
    Tidy::nDBQueryResult resultSet = query.exeSelect(mailDetail.table(), mailDetail.field_list(SqlMailDetailData_mailID, SqlMailDetailData_moneyData, SqlMailDetailData_itemData, SqlMailDetailData_mailState), whereCondition, 0);
    if(resultSet.empty() == true){
        return false;
    }
    for(DWORD i = 0; i < resultSet.num_rows(); ++ i){
        DWORD mailID = resultSet[i][SqlMailDetailData::names[SqlMailDetailData_mailID]];
        if(resultSet[i][SqlMailDetailData::names[SqlMailDetailData_itemData]].is_null() == true && resultSet[i][SqlMailDetailData::names[SqlMailDetailData_moneyData]].is_null() == true){
            continue;
        }
        mailDetail.mailID = mailID;
        if(static_cast<BYTE>(resultSet[i][SqlMailDetailData::names[SqlMailDetailData_mailState]]) == protocol::mail::MailState_New){
            mailDetail.mailState = protocol::mail::MailState_Readed;
            if(query.exeUpdate(mailDetail.table(), mailDetail.equal_list(SqlMailDetailData_mailState), mailDetail.equal_list(SqlMailDetailData_mailID), 1) != 1){
                assert_fail("设置状态失败[%u]", mailID);
            }
        }

        user->setMiscBoolData(eDataUserBoolDataType_IsWaitFetchMailAttachment);
        BUFFER_PROTO_CMD(protocol::mail::FetchMailAttachmentCommand, send);
        send->userID = user->getID();
        send->mailID = mailID;
        protocol::mail::MoneyAndItemData data;
        if(resultSet[0][SqlMailDetailData::names[SqlMailDetailData_moneyData]].is_null() == false){
            protocol::mail::MoneyDataBinary moneyDataBinary;
            moneyDataBinary.ParseFromArray(resultSet[0][SqlMailDetailData::names[SqlMailDetailData_moneyData]].c_str(), resultSet[0][SqlMailDetailData::names[SqlMailDetailData_moneyData]].length());
            if(moneyDataBinary.SerializeToString(data.mutable_moneydatabinary()) == false){
                assert_fail("序列化失败");
                return false;
            }
        }
        if(resultSet[0][SqlMailDetailData::names[SqlMailDetailData_itemData]].is_null() == false){
            protocol::mail::AllItemData allData;
            allData.ParseFromArray(resultSet[0][SqlMailDetailData::names[SqlMailDetailData_itemData]].c_str(), resultSet[0][SqlMailDetailData::names[SqlMailDetailData_itemData]].length());
            if(allData.SerializeToString(data.mutable_itemdata()) == false){
                assert_fail("序列化失败");
                return false;
            }
        }
        if(data.SerializeToArray(send->data, PROTO_CMD_CAPACITY(protocol::mail::FetchMailAttachmentCommand)) == false){
            assert_fail("消息空间不足，序列化失败");
            break;
        }
        send->num = data.GetCachedSize(); 
        user->trace("[邮件系统],请求提取附件,[%u]", mailID);
        server->messageService->sendCmdToLogic(user, send, send->size());
    }
    return true;
}

bool dtMailService::fetchMailAttachment(dtUser* user, const DWORD mailID)
{
	Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
    if(conn == NULL){
        assert_fail("无法获得连接");
        return false;
    }
    SqlMailDetailData mailDetail;
    char whereCondition[1024] = "";
    snprintf(whereCondition, sizeof(whereCondition), " %s='%u' AND %s='%u' AND %s='%u' AND %s>'%u' ", SqlMailDetailData::names[SqlMailDetailData_mailID], mailID, SqlMailDetailData::names[SqlMailDetailData_destID], user->getID(), SqlMailDetailData::names[SqlMailDetailData_isDeleteFromInbox], protocol::mail::MailDeleteState_None, SqlMailDetailData::names[SqlMailDetailData_createTime], getMinEffectMailTime());
    Tidy::nDBQuery query = conn->getQuery();
    Tidy::nDBQueryResult resultSet = query.exeSelect(mailDetail.table(), mailDetail.field_list(SqlMailDetailData_moneyData, SqlMailDetailData_itemData), whereCondition, 1);
    if(resultSet.empty() == true){
        return false;
    }
    if(resultSet[0][SqlMailDetailData::names[SqlMailDetailData_itemData]].is_null() == true && resultSet[0][SqlMailDetailData::names[SqlMailDetailData_moneyData]].is_null() == true){
        return false;
    }

    user->setMiscBoolData(eDataUserBoolDataType_IsWaitFetchMailAttachment);
    BUFFER_PROTO_CMD(protocol::mail::FetchMailAttachmentCommand, send);
    send->userID = user->getID();
    send->mailID = mailID;
    protocol::mail::MoneyAndItemData data;
    if(resultSet[0][SqlMailDetailData::names[SqlMailDetailData_moneyData]].is_null() == false){
        protocol::mail::MoneyDataBinary moneyDataBinary;
        moneyDataBinary.ParseFromArray(resultSet[0][SqlMailDetailData::names[SqlMailDetailData_moneyData]].c_str(), resultSet[0][SqlMailDetailData::names[SqlMailDetailData_moneyData]].length());
        if(moneyDataBinary.SerializeToString(data.mutable_moneydatabinary()) == false){
            assert_fail("序列化失败");
            return false;
        }
    }
    if(resultSet[0][SqlMailDetailData::names[SqlMailDetailData_itemData]].is_null() == false){
        protocol::mail::AllItemData allData;
        allData.ParseFromArray(resultSet[0][SqlMailDetailData::names[SqlMailDetailData_itemData]].c_str(), resultSet[0][SqlMailDetailData::names[SqlMailDetailData_itemData]].length());
        if(allData.SerializeToString(data.mutable_itemdata()) == false){
            assert_fail("序列化失败");
            return false;
        }
    }
    if(data.SerializeToArray(send->data, PROTO_CMD_CAPACITY(protocol::mail::FetchMailAttachmentCommand)) == false){
        assert_fail("消息空间不足，序列化失败");
        return false;
    }
    send->num = data.GetCachedSize(); 
	user->trace("[邮件系统],请求提取附件,[%u]", mailID);
    return server->messageService->sendCmdToLogic(user, send, send->size());
}

bool dtMailService::deleteMail(dtUser* user, const DWORD mailID)
{
	Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
    if(conn == NULL){
        assert_fail("无法获得连接");
        return false;
    }
    SqlMailDetailData mailDetail; 
    char whereCondition[1024] = "";
    Tidy::nDBQuery query = conn->getQuery();
    snprintf(whereCondition, sizeof(whereCondition), " %s='%u' AND( (%s='%u' AND %s='%u') OR (%s='%u' AND %s='%u'))", SqlMailDetailData::names[SqlMailDetailData_mailID], mailID, SqlMailDetailData::names[SqlMailDetailData_fromID], user->getID(), SqlMailDetailData::names[SqlMailDetailData_isDeleteFromOutbox], protocol::mail::MailDeleteState_None, SqlMailDetailData::names[SqlMailDetailData_destID], user->getID(), SqlMailDetailData::names[SqlMailDetailData_isDeleteFromInbox], protocol::mail::MailDeleteState_None);
    Tidy::nDBQueryResult resultSet = query.exeSelect(mailDetail.table(), mailDetail.field_list(SqlMailDetailData_fromID, SqlMailDetailData_destID, SqlMailDetailData_mailState, SqlMailDetailData_isDeleteFromInbox, SqlMailDetailData_isDeleteFromOutbox), whereCondition, 1);
    if(resultSet.empty() == true){
		assert_fail("找不到邮件");
        return true;
    }
    bool isDeleteFromInBox = (static_cast<DWORD>(resultSet[0][SqlMailDetailData::names[SqlMailDetailData_isDeleteFromInbox]]) == protocol::mail::MailDeleteState_Delete);
    bool isDeleteFromOutBox = (static_cast<DWORD>(resultSet[0][SqlMailDetailData::names[SqlMailDetailData_isDeleteFromOutbox]]) == protocol::mail::MailDeleteState_Delete);
    char columnCondition[1024] = "";
    if(user->getID() == static_cast<DWORD>(resultSet[0][SqlMailDetailData::names[SqlMailDetailData_fromID]])){   
        snprintf(whereCondition, sizeof(whereCondition), "%s='%u'", SqlMailDetailData::names[SqlMailDetailData_mailID], mailID);
        snprintf(columnCondition, sizeof(columnCondition), "%s='%u'", SqlMailDetailData::names[SqlMailDetailData_isDeleteFromOutbox], protocol::mail::MailDeleteState_Delete);
        if(query.exeUpdate(mailDetail.table(), columnCondition, whereCondition, 1) != 1){
			assert_fail("设置状态失败");
			return false;
		}
        isDeleteFromOutBox = true;
    }
    else if(user->getID() == static_cast<DWORD>(resultSet[0][SqlMailDetailData::names[SqlMailDetailData_destID]])){
		if(static_cast<BYTE>(resultSet[0][SqlMailDetailData::names[SqlMailDetailData_mailState]]) == protocol::mail::MailState_New){
			user->decUnreadMailNum();
			snprintf(columnCondition, sizeof(columnCondition), "%s='%u'", SqlMailDetailData::names[SqlMailDetailData_mailState], protocol::mail::MailState_Readed);
			if(query.exeUpdate(mailDetail.table(), columnCondition, whereCondition,1) != 1){
				assert_fail("设置状态失败");
				return false;
			}
		}
        snprintf(whereCondition, sizeof(whereCondition), "%s='%u'", SqlMailDetailData::names[SqlMailDetailData_mailID], mailID);
        snprintf(columnCondition, sizeof(columnCondition), "%s='%u'", SqlMailDetailData::names[SqlMailDetailData_isDeleteFromInbox], protocol::mail::MailDeleteState_Delete);
        if(query.exeUpdate(mailDetail.table(), columnCondition, whereCondition, 1) != 1){
			assert_fail("设置状态失败");
			return false;
		}
        isDeleteFromInBox = true;
    }
    if(isDeleteFromOutBox == true && isDeleteFromInBox == true){
        snprintf(whereCondition, sizeof(whereCondition), "%s='%u'", SqlMailDetailData::names[SqlMailDetailData_mailID], mailID);
        if(query.exeDelete(mailDetail.table(), whereCondition, 1) != 1){
            assert_fail("删除邮件失败[%u]", mailID);
        }
    }
	user->trace("[邮件系统],删除邮件,[%u]", mailID);
    return true;
}

bool dtMailService::deleteAllMail(dtUser* user)
{
	Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
    if(conn == NULL){
        assert_fail("无法获得连接");
        return false;
    }
	user->trace("[邮件系统],删除所有邮件");
    Tidy::nDBQuery query = conn->getQuery();
	user->clearUnreadMailNum();
    return deleteAllMail(user->getID(), false, query);
}

bool dtMailService::deleteAllMail(const DWORD charID, const bool isRobot, Tidy::nDBQuery &query)
{
    static SqlMailDetailData mailDetailData;

    mailDetailData.fromID = charID;
    mailDetailData.mailState = protocol::mail::MailState_Readed;
    mailDetailData.isDeleteFromOutbox = protocol::mail::MailDeleteState_Delete;
    query.exeUpdate(mailDetailData.table(), mailDetailData.equal_list(SqlMailDetailData_isDeleteFromOutbox), mailDetailData.equal_list(" AND ", SqlMailDetailData_fromID, SqlMailDetailData_mailState), 0);

    mailDetailData.destID = charID;
    mailDetailData.isDeleteFromInbox = protocol::mail::MailDeleteState_Delete;
    query.exeUpdate(mailDetailData.table(), mailDetailData.equal_list(SqlMailDetailData_isDeleteFromInbox), mailDetailData.equal_list(" AND ", SqlMailDetailData_destID, SqlMailDetailData_mailState), 0);

    return true;
}

const DWORD dtMailService::getNewMailNum(dtUser* user)
{
	Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
    if(conn == NULL){
        assert_fail("无法获得连接");
        return 0;
    }
    Tidy::nDBQuery query = conn->getQuery();
    static SqlMailDetailData mailDetailData;
    char whereCondition[1024] = "";
    snprintf(whereCondition, sizeof(whereCondition), "%s='%u' AND %s > '%u' AND %s='%u'", SqlMailDetailData::names[SqlMailDetailData_destID], user->getID(), SqlMailDetailData::names[SqlMailDetailData_createTime], getMinEffectMailTime(), SqlMailDetailData::names[SqlMailDetailData_mailState], protocol::mail::MailState_New);
    Tidy::nDBQueryResult resultSet = query.exeSelect(mailDetailData.table(), " COUNT(*) ", whereCondition, 0);
    if(resultSet.empty() == true){
        assert_fail("结果为空");
        return 0;
    }
    return static_cast<DWORD>(resultSet[0][0]);
}

bool dtMailService::deleteAllTimeoutMail()
{
	server->trace("[邮件系统],删除过期邮件");
    static Tidy::nDBQuery query(NULL);
    static SqlMailDetailData mailDetailData;
    char whereCondition[1024] = "";
    snprintf(whereCondition, sizeof(whereCondition), "%s<'%u'", SqlMailDetailData::names[SqlMailDetailData_createTime], getMinEffectMailTime());
    std::string sql = query.getDeleteSql(mailDetailData, whereCondition, Tidy::global["MAX_DELETE_MAIL_NUM_ONE_TIME"].getDword());
    return server->serializeService->exeAsyncSql(eDBConnectionID_ZoneMain, sql, NULL);
}
        
bool dtMailService::dealFetchMailAttachmentSucc(const DWORD userID, const DWORD mailID)
{
    dtUser *user = server->userService->getUserByID(userID);
    if(user != NULL){
        user->clearMiscBoolData(eDataUserBoolDataType_IsWaitFetchMailAttachment);
    }
	Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
    if(conn == NULL){
        assert_fail("无法获得连接");
        return false;
    }
    Tidy::nDBQuery query = conn->getQuery();
    SqlMailDetailData mailData;
    mailData.mailID = mailID;
    mailData.itemData.it_is_null();
    mailData.moneyData.it_is_null();
	
    if(query.exeUpdate(mailData.table(), mailData.equal_list(SqlMailDetailData_itemData, SqlMailDetailData_moneyData), mailData.equal_list(SqlMailDetailData_mailID), 1) != 1){
        assert_fail("设置状态失败");
        return false;
    }
	server->trace("[邮件系统],删除邮件附件,[%u]", mailID);
    return true;
}

bool dtMailService::dealFetchMailAttachmentFail(const DWORD userID)
{
    dtUser *user = server->userService->getUserByID(userID);
    if(user == NULL){
        return true;
    }
    user->clearMiscBoolData(eDataUserBoolDataType_IsWaitFetchMailAttachment);
    return true;
}

bool dtMailService::dealDeleteAllRobot()
{
	Tidy::nDBConnection *conn = server->getDBConnPool()->getConnection(eDBConnectionID_ZoneMain);
	if(conn == NULL){
		server->error("不能从数据库连接池获取连接句柄 %s",__PRETTY_FUNCTION__);
		return false;
	}
	Tidy::nDBQuery query = conn->getQuery();

    char sql[1024];
    SqlMailDetailData sqlMailDetailDataTable;
    SqlCharBase sqlCharBaseTable;
    snprintf(sql, sizeof(sql), " DELETE %s FROM %s INNER JOIN %s ON %s.%s = %s.%s AND %s.%s=''", sqlMailDetailDataTable.table(), sqlMailDetailDataTable.table(), sqlCharBaseTable.table(), sqlMailDetailDataTable.table(), SqlMailDetailData::names[SqlMailDetailData_destID], sqlCharBaseTable.table(), SqlCharBase::names[SqlCharBase_charID], sqlCharBaseTable.table(), SqlCharBase::names[SqlCharBase_accountName]);
    query << sql;
	DWORD deleteMailNum = query.exeDelete();
    if(deleteMailNum == Tidy::nDBQuery::ERROR){
        assert_fail("删除机器人邮件数据失败");
        return false;
    }
    server->info("删除机器人邮件数据[%u]", deleteMailNum);
    return true;
}
