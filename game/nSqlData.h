#ifndef GAME_NSQLDATA_H_
#define GAME_NSQLDATA_H_

#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>

sql_create_complete_2(SqlDataBaseVersion,	
        DATABASE_VERSION,
        1,		///< 索引号
        0,		///< 构造函数参数个数
        mysqlpp::sql_int_unsigned,		connectionID,	"CONNECTION_ID",
        mysqlpp::sql_int_unsigned,		versionNum,	    "VERSION_NUM"
        )

sql_create_complete_4(SqlAccount,	
        ACCOUNT,
        1,		///< 索引号
        0,		///< 构造函数参数个数
        mysqlpp::sql_char,		        accountName,  	"ACCOUNT_NAME",
        mysqlpp::sql_int_unsigned,		platformType,  "SRC_PLATFORM_ID",
        mysqlpp::sql_int_unsigned,		createTimeInSec,"CREATE_TIME_IN_SEC",
        mysqlpp::sql_int_unsigned,		rechargeMoney,	"RECHARGE_MONEY"
        )

sql_create_complete_8(SqlAccountAdmin,
        ACCOUNT,
        1,      ///< 索引号
        0,      ///< 构造函数参数个数
        mysqlpp::sql_varchar,           accountName,    "ACCOUNT_NAME",
        mysqlpp::sql_varchar,           nickName,       "NICK_NAME",
        mysqlpp::sql_int_unsigned,      lastSelectZoneID,"LAST_SELECT_ZONE_ID",
        mysqlpp::sql_datetime,          createTime,     "CREATE_TIME",
        mysqlpp::sql_datetime,          lastLoginTime,  "LAST_LOGIN_TIME",
        mysqlpp::sql_varchar,           lastLoginIP,    "LAST_LOGIN_IP",
        mysqlpp::sql_int_unsigned,      accountType,    "ACCOUNT_TYPE",
        mysqlpp::sql_varchar,           password,       "PASSWORD"
        )

sql_create_complete_49(SqlCharBase,	
        CHAR_BASE,
        1,		///< 索引号
        0,		///< 构造函数参数个数
        mysqlpp::sql_tinyint_unsigned,  userType,    "USER_TYPE",
        mysqlpp::sql_tinyint_unsigned,	userDataState,  "USER_DATA_STATE",
        mysqlpp::sql_int_unsigned,		userDataStateChangeTimeInSec,  "USER_DATA_STATE_CHANGE_TIME_IN_SEC",
        mysqlpp::sql_int_unsigned,		charID,		"CHARID",
        mysqlpp::sql_char,				name,		"NAME",
        mysqlpp::sql_char,				accountName,"ACCOUNT_NAME",
        mysqlpp::sql_int_unsigned,		onlineTime,	"ONLINETIME",
        mysqlpp::sql_int_unsigned,		createTime,	"CREATETIME",
        mysqlpp::sql_int_unsigned,		lastOfflineTime,"LASTOFFLINETIME",
        mysqlpp::sql_int_unsigned,		firstConsumeTime,"FIRSTCONSUMETIME",
        mysqlpp::sql_int_unsigned,		createIP,	"CREATEIP",
        mysqlpp::sql_char,		        lastLoginIP,"LASTLOGINIP",
        mysqlpp::sql_tinyint_unsigned,	sex,		"SEX",
        mysqlpp::sql_int_unsigned,		bitMask,	"BITMASK",
        mysqlpp::sql_smallint,			level,		"LEVEL",
        mysqlpp::sql_int_unsigned,		exp,		"EXP",
        mysqlpp::sql_tinyint_unsigned,  isAdult,    "ISADULT",
        mysqlpp::sql_int_unsigned,		loginTime,  "LOGIN_TIME",
        mysqlpp::sql_int_unsigned,		vitality,   "VITALITY",
        mysqlpp::sql_int_unsigned,		skillPoint, "SKILL_POINT",
        mysqlpp::sql_int_unsigned,		silver,     "SILVER",
        mysqlpp::sql_int_unsigned,		diamond,    "DIAMOND",
        mysqlpp::sql_int_unsigned,		bindDiamond,"BIND_DIAMOND",
        mysqlpp::sql_int_unsigned,		honorPoints,"HONOR_POINTS",
        mysqlpp::sql_int_unsigned,		clubPoints,"CLUB_POINTS",
        mysqlpp::sql_int_unsigned,		gmLevel,    "GM_LEVEL",
        mysqlpp::sql_int_unsigned,		monthCardStopTime,"MONTH_CARD_STOP_TIME",
        mysqlpp::sql_int_unsigned,		thoughtValue,"THOUGHT_VALUE",
        mysqlpp::sql_int_unsigned,		fightPower, "FIGHT_POWER",
        mysqlpp::sql_int_unsigned,		vipExp, "VIP_EXP",
        mysqlpp::sql_int_unsigned,		vipLevel, "VIP_LEVEL",
        mysqlpp::sql_blob,		        packageBinary, "PACKAGE_BINARY",
        mysqlpp::sql_blob,		        useSkillBinary, "USE_SKILL_BINARY",
        mysqlpp::sql_blob,		        haveSkillBinary, "HAVE_SKILL_BINARY",
        mysqlpp::sql_blob,		        unlockPassiveSkillBinary, "UNLOCK_PASSIVE_BINARY",
        mysqlpp::sql_blob,		        haveUnlockPetBinary, "HAVE_UNLOCK_PETBINARY",
        mysqlpp::sql_blob,		        willUnlockPetBinary, "WILL_UNLOCK_PETBINARY",
        mysqlpp::sql_blob,		        duplSqlBinary, "DUPL_SQL_BINARY",
        mysqlpp::sql_blob,		        trumpBinary, "TRUMP_BINARY",
        mysqlpp::sql_blob,		        equipPosBinary, "EQUIP_POS_BINARY",
        mysqlpp::sql_blob,		        rewardBinary, "REWARD_BINARY",
        mysqlpp::sql_blob,		        discoveryBinary, "DISCOVERY_BINARY",
        mysqlpp::sql_blob,		        userDataBinary, "USER_DATA_BINARY",
        mysqlpp::sql_blob,              friendDataBinary, "FRIEND_DATA_BINARY",
        mysqlpp::sql_blob,              mysteryShopBinary, "MYSTERY_SHOP_BINARY",
        mysqlpp::sql_blob,		        taskBinary,"TASK_BINARY",
        mysqlpp::sql_blob,		        matchDataBinary,"MATCH_DATA_BINARY",
        mysqlpp::sql_blob,		        wingBinary,"WING_BINARY",
        mysqlpp::sql_blob,		        vipDataBinary,"VIP_DATA_BINARY"
    )

sql_create_complete_4(SqlMiscDataSave,	
        MISC_DATA_SAVE,
        2,		///< 前几个字段组成key
        0,		///< 构造函数参数个数
        mysqlpp::sql_int_unsigned,		dataType,		"DATA_TYPE",
        mysqlpp::sql_int_unsigned,	    dataKey,		"DATA_KEY",
        mysqlpp::sql_int_unsigned,	    saveTimeInSec,  "SAVE_TIME_IN_SEC",
        mysqlpp::sql_blob,				allBinary,		"ALLBINARY"
        )

sql_create_complete_5(SqlAccountCharge,
        ACCOUNT_CHARGE,
        1,		///< 前几个字段组成key
        0,		///< 构造函数参数个数
        mysqlpp::sql_char,	            chargeID,		"CHARGE_ID",
        mysqlpp::sql_char,	            accountName,    "ACCOUNT_NAME",
        mysqlpp::sql_int_unsigned,	    rmb,		    "RMB",
        mysqlpp::sql_int_unsigned,	    point,		    "POINT",
        mysqlpp::sql_datetime,	        datetime,	    "DATETIME"
        )
        sql_create_complete_12(SqlMailDetailData,   
                MAIL_DETAIL_SAVE,
                1,      ///< 索引号
                0,      ///< 构造函数参数个数
                mysqlpp::sql_int_unsigned,      mailID,     "MAIL_ID",
                mysqlpp::sql_tinyint_unsigned,  mailType,   "MAIL_TYPE",
                mysqlpp::sql_int_unsigned,      createTime, "CREATE_TIME",
                mysqlpp::sql_tinyint_unsigned,  mailState,  "MAIL_STATE",
                mysqlpp::sql_int_unsigned,      fromID,     "FROM_ID",
                mysqlpp::sql_int_unsigned,      destID,     "DEST_ID",
                mysqlpp::sql_char,              title,      "TITLE",
                mysqlpp::sql_char,              mainText,   "MAIN_TEXT",
                mysqlpp::sql_blob,              itemData,   "ITEM_DATA",
                mysqlpp::sql_blob,              moneyData,  "MONEY_DATA",
                mysqlpp::sql_tinyint_unsigned,  isDeleteFromInbox, "IS_DELETE_FROM_INBOX",
                mysqlpp::sql_tinyint_unsigned,  isDeleteFromOutbox,"IS_DELETE_FROM_OUTBOX"
                )

        sql_create_complete_18(SqlRelationBase, 
                CHAR_BASE,
                1,      ///< 索引号
                0,      ///< 构造函数参数个数
                mysqlpp::sql_tinyint_unsigned,  userType,    "USER_TYPE",
                mysqlpp::sql_int_unsigned,      charID,     "CHARID",
                mysqlpp::sql_int_unsigned,      userDataState,  "USER_DATA_STATE",
                mysqlpp::sql_char,              accountName,"ACCOUNT_NAME",
                mysqlpp::sql_char,              name,       "NAME",
                mysqlpp::sql_tinyint_unsigned,  sex,        "SEX",
                mysqlpp::sql_int_unsigned,      exp,        "EXP",
                mysqlpp::sql_smallint,          level,      "LEVEL",
                mysqlpp::sql_tinyint_unsigned,  gmLevel,    "GM_LEVEL",
                mysqlpp::sql_int_unsigned,		vipLevel,   "VIP_LEVEL",
                mysqlpp::sql_int_unsigned,		fightPower, "FIGHT_POWER",
                mysqlpp::sql_int_unsigned,		lastOfflineTime,"LASTOFFLINETIME",
                mysqlpp::sql_blob,              userDataBinary, "USER_DATA_BINARY",
                mysqlpp::sql_blob,              vipBinary,  "VIP_BINARY",
                mysqlpp::sql_blob,              clubDataBinary, "CLUB_DATA_BINARY",
                mysqlpp::sql_blob,              messageBinary, "MESSAGE_BINARY",
                mysqlpp::sql_blob,		        matchDataBinary,"MATCH_DATA_BINARY",
                mysqlpp::sql_blob,		        petBinary,"HAVE_UNLOCK_PETBINARY"
                )

        sql_create_complete_13(SqlClub,
                CLUB,
                1,      ///< 索引号
                0,      ///< 构造函数参数个数
                mysqlpp::sql_int_unsigned,      id,             "ID",
                mysqlpp::sql_char,              name,           "NAME",
                mysqlpp::sql_int_unsigned,      createTimeInSec,"CREATE_TIME_IN_SEC",
                mysqlpp::sql_int_unsigned,      ownerID,        "OWNER_ID",
                mysqlpp::sql_smallint_unsigned, flag,           "FLAG",
                mysqlpp::sql_varchar,           contact,        "CONTACT",
                mysqlpp::sql_varchar,           notice,         "NOTICE",
                mysqlpp::sql_int_unsigned,      level,          "LEVEL",
                mysqlpp::sql_int_unsigned,      currExp,        "CURR_EXP",
                mysqlpp::sql_int_unsigned,      totalExp,       "TOTAL_EXP",
                mysqlpp::sql_int_unsigned,      asset,          "ASSET",
                mysqlpp::sql_boolean,           isOpen,         "IS_OPEN",
                mysqlpp::sql_blob,              binaryData,     "BINARY"
                )
        sql_create_complete_6(SqlFriend,    
                FRIEND,
                2,      ///< 前几个字段组成key
                0,      ///< 构造函数参数个数
                mysqlpp::sql_int_unsigned,      userID,     "USERID",
                mysqlpp::sql_int_unsigned,      friendID,   "FRIENDID",
                mysqlpp::sql_int_unsigned,      joinTime,   "JOIN_TIME",
                mysqlpp::sql_int_unsigned,      lastGiveFriendVitalityInSec,   "LAST_GIVE_FRIEND_VITALITY_IN_SEC",
                mysqlpp::sql_int_unsigned,      lastGiveMeVitalityInSec,   "LAST_GIVE_ME_VITALITY_IN_SEC",
                mysqlpp::sql_int_unsigned,      isReceived,   "IS_RECEIVED"
                )
#endif
