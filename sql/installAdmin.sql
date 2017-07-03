-- MySQL dump 10.13  Distrib 5.1.56, for unknown-linux-gnu (x86_64)
--
-- Host: 10.1.101.233    Database: verify
-- ------------------------------------------------------
-- Server version	5.1.56-LTOPS-log
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

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
-- Table structure for table `ID_RANGE`
--

DROP TABLE IF EXISTS `ID_RANGE`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ID_RANGE` (
  `ID_TYPE` smallint(5) unsigned NOT NULL,
  `START` int(10) unsigned NOT NULL,
  `END` int(11) unsigned NOT NULL,
  PRIMARY KEY (`ID_TYPE`,`START`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `NAME_VERIFY`
--

DROP TABLE IF EXISTS `NAME_VERIFY`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `NAME_VERIFY` (
  `NAME_TYPE` smallint(5) unsigned NOT NULL,
  `NAME` varchar(255) CHARACTER SET utf8 NOT NULL,
  `ZONE_ID` smallint(6) NOT NULL DEFAULT '0',
  PRIMARY KEY (`NAME_TYPE`,`NAME`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ZONE_INFO`
--

DROP TABLE IF EXISTS `ZONE_INFO`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ZONE_INFO` (
  `NET_TYPE` tinyint(4) DEFAULT NULL,
  `GAME_TYPE` smallint(6) NOT NULL DEFAULT '0',
  `ZONE_ID` smallint(6) NOT NULL DEFAULT '0',
  `SERVER_IP` varchar(100) DEFAULT NULL,
  `SERVER_PORT` smallint(6) DEFAULT NULL,
  `ZONE_NAME` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`GAME_TYPE`,`ZONE_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping events for database 'verify'
--

--
-- Dumping routines for database 'verify'
--
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2013-08-04 18:42:19
DROP TABLE IF EXISTS `ACCOUNT`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ACCOUNT` (
  `ACCOUNT_NAME` varchar(100) NOT NULL,
  `PASSWORD` varchar(255) DEFAULT NULL,
  `LAST_SELECT_ZONE_ID` int(11) NOT NULL DEFAULT '0' COMMENT '上次选区区号',
  `CREATE_TIME` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '注册日期时间',
  `ACCOUNT_TYPE` int(11) NOT NULL DEFAULT '0' COMMENT '账号类型',
  `LAST_LOGIN_TIME` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '上次登陆时间',
  `LAST_LOGIN_IP` varchar(32) NOT NULL COMMENT '上次登陆ip',
  `NICK_NAME` varchar(100) DEFAULT NULL COMMENT '玩家昵称',
  PRIMARY KEY (`ACCOUNT_NAME`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
