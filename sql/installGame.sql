-- MySQL dump 10.13  Distrib 5.1.65, for unknown-linux-gnu (x86_64)
--
-- Host: 10.1.101.233    Database: zoneGame_1
-- ------------------------------------------------------
-- Server version	5.1.56-LTOPS-log
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `ACCOUNT`
--

DROP TABLE IF EXISTS `ACCOUNT`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ACCOUNT` (
  `ACCOUNT_NAME` varchar(120) NOT NULL DEFAULT '',
  `SRC_PLATFORM_ID` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '来源平台ID',
  `RECHARGE_MONEY` int(11) unsigned NOT NULL DEFAULT '0',
  `CREATE_TIME_IN_SEC` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ACCOUNT_NAME`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `AUCTION_SAVE`
--

--
-- Table structure for table `CHAR_BASE`
--

DROP TABLE IF EXISTS `CHAR_BASE`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `CHAR_BASE` (
  `USER_TYPE` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '用户类型',
  `CHARID` int(10) unsigned NOT NULL,
  `ACCOUNT_NAME` varchar(120) NOT NULL DEFAULT '',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `SRC_PLATFORM_ID` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '来源平台ID',
  `BORNDATE` int(10) unsigned NOT NULL DEFAULT '0',
  `ONLINETIME` int(10) unsigned NOT NULL DEFAULT '0',
  `CREATETIME` int(10) unsigned NOT NULL DEFAULT '0',
  `LASTOFFLINETIME` int(10) unsigned NOT NULL DEFAULT '0',
  `FIRSTCONSUMETIME` int(10) unsigned NOT NULL DEFAULT '0',
  `CREATEIP` int(10) unsigned NOT NULL DEFAULT '0',
  `LASTLOGINIP` varchar(32) NOT NULL DEFAULT '',
  `LASTMAC` bigint(20) unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `LEVEL` int(10) unsigned NOT NULL DEFAULT '1',
  `BITMASK` int(10) unsigned NOT NULL DEFAULT '0',
  `EXP` int(10) unsigned NOT NULL DEFAULT '0',
  `RECHARGE_MONEY` int(10) unsigned NOT NULL DEFAULT '0',
  `ISADULT` smallint(2) unsigned NOT NULL DEFAULT '0',
  `USER_DATA_STATE` int(10) unsigned NOT NULL,
  `USER_DATA_STATE_CHANGE_TIME_IN_SEC` int(10) unsigned NOT NULL DEFAULT '0',
  `LOGIN_TIME` int(10) unsigned NOT NULL DEFAULT '0',
  `VITALITY` int(8) unsigned NOT NULL DEFAULT '0',
  `SKILL_POINT` int(8) unsigned NOT NULL DEFAULT '0',
  `SILVER` int(16) unsigned NOT NULL DEFAULT '0',
  `DIAMOND` int(16) unsigned NOT NULL DEFAULT '0',
  `BIND_DIAMOND` int(16) unsigned NOT NULL DEFAULT '0',
  `HONOR_POINTS` int(16) unsigned NOT NULL DEFAULT '0',
  `CLUB_POINTS` int(16) unsigned NOT NULL DEFAULT '0',
  `GM_LEVEL` int(8) unsigned NOT NULL DEFAULT '0',
  `THOUGHT_VALUE` int(8) unsigned NOT NULL DEFAULT '0',
  `FIGHT_POWER` int(10) unsigned NOT NULL DEFAULT '0',
  `MONTH_CARD_STOP_TIME` int(10) unsigned NOT NULL DEFAULT '0',
  `VIP_EXP` int(10) unsigned NOT NULL DEFAULT '0',
  `VIP_LEVEL` int(10) unsigned NOT NULL DEFAULT '0',
  `PACKAGE_BINARY` blob DEFAULT NULL COMMENT '背包数据',
  `USE_SKILL_BINARY` blob DEFAULT NULL COMMENT '使用技能数据',
  `HAVE_SKILL_BINARY` blob DEFAULT NULL COMMENT '使用技能数据',
  `UNLOCK_PASSIVE_BINARY` blob DEFAULT NULL COMMENT '解锁的被动技能栏',
  `HAVE_UNLOCK_PETBINARY` blob DEFAULT NULL COMMENT '已召唤的宠物',
  `WILL_UNLOCK_PETBINARY` blob DEFAULT NULL COMMENT '可以召唤的宠物',
  `DUPL_SQL_BINARY` blob DEFAULT NULL COMMENT '副本数据',
  `TRUMP_BINARY` blob DEFAULT NULL COMMENT '法宝数据',
  `EQUIP_POS_BINARY` blob DEFAULT NULL COMMENT '装备强化数据',
  `REWARD_BINARY` blob DEFAULT NULL COMMENT '奖励数据',
  `DISCOVERY_BINARY` blob DEFAULT NULL COMMENT '星海探秘数据',
  `USER_DATA_BINARY` blob DEFAULT NULL COMMENT '玩家数据',
  `VIP_BINARY` blob DEFAULT NULL COMMENT 'VIP数据',
  `CLUB_DATA_BINARY` blob DEFAULT NULL COMMENT '工会数据',
  `FRIEND_DATA_BINARY` blob DEFAULT NULL COMMENT '好友相关数据',
  `MESSAGE_BINARY` blob DEFAULT NULL COMMENT '系统信息数据',
  `MYSTERY_SHOP_BINARY` blob DEFAULT NULL COMMENT '神秘商店相关数据',
  `TASK_BINARY` blob DEFAULT NULL COMMENT '任务相关数据',
  `MATCH_DATA_BINARY` blob DEFAULT NULL COMMENT '玩家比赛数据',
  `WING_BINARY` blob DEFAULT NULL COMMENT '翅膀相关数据',
  `VIP_DATA_BINARY` blob DEFAULT NULL COMMENT 'VIP相关数据',
  PRIMARY KEY (`CHARID`),
  UNIQUE KEY `NAME` (`NAME`),
  KEY `ACCOUNT_NAME` (`ACCOUNT_NAME`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `DATABASE_VERSION`
--

DROP TABLE IF EXISTS `DATABASE_VERSION`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `DATABASE_VERSION` (
  `CONNECTION_ID` int(11) NOT NULL,
  `VERSION_NUM` int(11) NOT NULL,
  PRIMARY KEY (`CONNECTION_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `MAIL_DETAIL_SAVE`
--

DROP TABLE IF EXISTS `MAIL_DETAIL_SAVE`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `MAIL_DETAIL_SAVE` (
  `CREATE_TIME` int(11) NOT NULL,
  `MAIL_ID` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `MAIL_TYPE` tinyint(3) unsigned NOT NULL,
  `MAIL_STATE` tinyint(3) unsigned NOT NULL,
  `FROM_ID` int(11) unsigned NOT NULL,
  `DEST_ID` int(11) unsigned NOT NULL,
  `TITLE` varchar(255) NOT NULL,
  `MAIN_TEXT` varchar(1680) NOT NULL,
  `ITEM_DATA` blob DEFAULT NULL,
  `MONEY_DATA` blob DEFAULT NULL,
  `IS_DELETE_FROM_INBOX` smallint(2) unsigned NOT NULL,
  `IS_DELETE_FROM_OUTBOX` smallint(2) unsigned NOT NULL,
  KEY (`FROM_ID`),
  KEY (`DEST_ID`),
  KEY (`IS_DELETE_FROM_INBOX`),
  KEY (`IS_DELETE_FROM_OUTBOX`),
  PRIMARY KEY (`MAIL_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `MISC_DATA_SAVE`
--

DROP TABLE IF EXISTS `MISC_DATA_SAVE`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `MISC_DATA_SAVE` (
  `DATA_TYPE` int(11) NOT NULL,
  `ALLBINARY` blob,
  `DATA_KEY` int(11) NOT NULL,
  `SAVE_TIME_IN_SEC` int(11) NOT NULL,
  PRIMARY KEY (`DATA_TYPE`,`DATA_KEY`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ACCOUNT_CHARGE`
--

DROP TABLE IF EXISTS `ACCOUNT_CHARGE`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ACCOUNT_CHARGE` (
  `CHARGE_ID` varchar(120) NOT NULL DEFAULT '' COMMENT '充值流水ID',
  `ACCOUNT_NAME` varchar(120) NOT NULL DEFAULT '' COMMENT '玩家账号',
  `RMB` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值RMB数',
  `POINT` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值点数(游戏钻石数量)',
  `DATETIME` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '日期时间',
  KEY (`ACCOUNT_NAME`),
  KEY (`DATETIME`),
  PRIMARY KEY (`CHARGE_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='充值数据统计';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `CLUB`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `CLUB` (
      `ID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '唯一ID',
      `NAME` varchar(120) NOT NULL DEFAULT '' COMMENT '唯一名字',
      `CREATE_TIME_IN_SEC` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间戳',
      `OWNER_ID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '会长ID',
      `FLAG` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '俱乐部标志',
      `CONTACT` varchar(120) NOT NULL DEFAULT '' COMMENT '联系方式,现在只有QQ群',
      `NOTICE` varchar(1680) NOT NULL DEFAULT '' COMMENT '公告',
      `LEVEL` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '俱乐部等级',
      `CURR_EXP` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '当前经验',
      `TOTAL_EXP` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '总经验',
      `ASSET` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '资产',
      `IS_OPEN` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '是否开启加入',
      `BINARY` blob DEFAULT NULL COMMENT '二进制存档',
      PRIMARY KEY (`ID`),
      UNIQUE KEY `NAME` (`NAME`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='俱乐部数据档案';
/*!40101 SET character_set_client = @saved_cs_client */;

DROP TABLE IF EXISTS `FRIEND`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `FRIEND` (
      `USERID` int(10) NOT NULL DEFAULT '0',
      `FRIENDID` int(10) NOT NULL DEFAULT '0',
      `DEGREE` int(10) NOT NULL DEFAULT '0',
      `JOIN_TIME` int(10) NOT NULL DEFAULT '0',
      `LAST_GIVE_FRIEND_VITALITY_IN_SEC` int(10) NOT NULL DEFAULT '0',
      `LAST_GIVE_ME_VITALITY_IN_SEC` int(10) NOT NULL DEFAULT '0',
      `IS_RECEIVED` int(10) NOT NULL DEFAULT '0',
      KEY (`USERID`),
      KEY (`FRIENDID`),
      PRIMARY KEY (`USERID`,`FRIENDID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
