#!/usr/bin/python
# -*- coding: utf-8 -*-

import os,re,sys,commands
import database

ingore = ["DATABASE_VERSION", "LOG_RESET_DATA"]

help = "使用方法(新手慎用):  %s [create,createadmin,createplatform,install,installGame,installInfo,installadmin,installplatform,update,updateadmin,updateplatform,resetgame,resetinfo,dropgame,dropinfo,dumpzone],\n \
		create：			创建并安装区数据库 \n \
		createadmin：		创建并安装admin数据库 \n \
		createplatform：	创建并安装platform数据库 \n \
		install：			安装区数据库，installGame和installInfo中的操作 \n \
		installGame：		安装区数据库，installGame.sql中的操作 \n \
		installInfo：		安装区数据库，installInfo.sql中的操作 \n \
		installadmin：		安装admin数据库，adminserver.sql中的操作 \n \
		installplatform：	安装platform数据库，platformserver.sql中的操作 \n \
		dropgame:			删除除ingore表的其他所有表 \n \
		dropinfo:			删除除ingore表的其他所有表 \n \
		update：			执行区最近的一次更新操作（最近的注释#行之后的操作） \n \
		updateadmin：		执行admin最近的一次更新操作（最近的注释#行之后的操作） \n \
		updateplatform：	执行platform最近的一次更新操作（最近的注释#行之后的操作） \n \
		updatemonitor：		执行monitor最近的一次更新操作（最近的注释#行之后的操作） \n \
		resetgame：			清空除游戏数据库的ingore表其他的所有数据库记录 \n\
		resetinfo：			清空除Info数据库的ingore表其他的所有数据库记录 \n\
		dumpzone:			导出区的安装文件,不包括数据 \n\
		dumpplatform:		导出PLATFORM的安装文件,不包括数据 \n\
		dumpadmin:			导出ADMIN的安装文件,不包括数据 \n\
		dumpmonitor:		导出monitor的安装文件,不包括数据 \n\
		installmonitor:		导出monitor的安装文件,不包括数据 \n\
		"%(sys.argv[0],)

#------------------------------------------------------------------------------
def getCmdStr(dbConfig, str):
	cmd = 'mysql -u ' + dbConfig[1]
	if dbConfig[2] != "":
		cmd = cmd + ' -p' + dbConfig[2]
	if dbConfig[0] != "":
		cmd = cmd + ' -h ' + dbConfig[0]
	cmd = cmd + ' -e '+'\"'+"set character_set_client=utf8; SET character_set_connection=UTF8; use " + dbConfig[3] + "; " + str + '\"'
	return cmd

#------------------------------------------------------------------------------
def dropMark(str):
	str = re.sub("--.*\n","",str)
	return str

#------------------------------------------------------------------------------
def doSql(dbConfig, str):
	str = dropMark(str)
	str = str.replace("`","\`")
	cmd = getCmdStr(dbConfig, str)
	print cmd 
	print "\n"
	status,output = commands.getstatusoutput(cmd)
	if status != 0:
		print "执行SQL错误!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		print status
		print output
		sys.exit(0)

#------------------------------------------------------------------------------
def doUpdate(dbConfig):
	update = open(dbConfig[5], "r")
	text = ""
	localVersion = "0"
	cmd = getCmdStr(dbConfig, "select version_num from DATABASE_VERSION where CONNECTION_ID = " + dbConfig[7] +";")
	cmd = cmd + " | sed -n \'2p\'"
	print cmd
	localVersion = commands.getoutput(cmd)
	sqlVersion = localVersion
	for line in update:
		if line.find("#") != -1 and line.find("#") == 0 and line.replace("#","").replace("\n","").replace(" ","").isdigit():
			if sqlVersion > localVersion:
				doSql(dbConfig, text)
				text = ' REPLACE INTO DATABASE_VERSION (CONNECTION_ID,VERSION_NUM)  VALUES (' + dbConfig[7] + ', '+sqlVersion+')'
				doSql(dbConfig, text)
				text = ""
				localVersion = sqlVersion
			sqlVersion = line.replace("#", "").replace("\n", "").replace(" ", "")
			if sqlVersion == "": print "ERROR 没写数据库版本号!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		elif sqlVersion > localVersion :
			text += line
	if text != '':
		doSql(dbConfig, text)
	if sqlVersion != '' and sqlVersion > localVersion:
		text = ' REPLACE INTO DATABASE_VERSION (CONNECTION_ID,VERSION_NUM)  VALUES (' + dbConfig[7] + ', '+sqlVersion+')'
		doSql(dbConfig, text)

#------------------------------------------------------------------------------
def createDatabase(dbConfig):
	cmd = 'mysql -u ' + dbConfig[1] + ' -p' + dbConfig[2] + ' -h ' + dbConfig[0] + ' -e ' + '\"' + "CREATE DATABASE IF NOT EXISTS " + dbConfig[3] + " default charset utf8 COLLATE utf8_general_ci; SET character_set_connection=UTF8;" + '\"'
	print cmd 
	print "\n"
	os.system(cmd)

#------------------------------------------------------------------------------
def createZone():
	text = "REPLACE INTO ZONE_INFO(ZONE_ID, SERVER_IP, SERVER_PORT, ZONE_NAME) values(" + database.zoneNum + ", \'" + database.zoneServerIP + "\', \'" + database.zoneServerPort + "\', \'" + database.zoneName + "\');"
	doSql(database.adminDB, text)
	text = "REPLACE INTO ZONE_INFO(GAME_TYPE, ZONE_ID, ZONE_NAME, NEED_VERIFY_PASSWORD, NEED_VERIFY_ACCOUNT, VISIBLE_TYPE, IS_RECOMMEND, MAIN_CLIENT_RESOURCE_ID) values(0, " + database.zoneNum + ", \'" + database.zoneName + "\', 0, 0, 0, 0, 1);"
	#doSql(database.platformDB, text)

	createDatabase(database.zoneGameDB)
	installDatabase(database.zoneGameDB)

#------------------------------------------------------------------------------
def createAdmin():
	createDatabase(database.adminDB)
	installDatabase(database.adminDB)

#------------------------------------------------------------------------------
def createPlatform():
	createDatabase(database.platformDB)
	installDatabase(database.platformDB)

#------------------------------------------------------------------------------
def installDatabase(dbConfig):
	install = open(dbConfig[4], "r")
	text = ""
	for line in install:
		text += line;
	doSql(dbConfig, text)
	updateVersion(dbConfig)

def dumpInstall(dbConfig):
	cmd = 'mysqldump -u' + dbConfig[1] + ' -p' + dbConfig[2] + ' -h' + dbConfig[0] + ' -P3306 -n -d --skip-set-charset -R -E  ' + dbConfig[3] + ' | sed "s/\/\*\!50020 DEFINER=.*\*\// /g" | sed "s/\/\*\!50017 DEFINER=.*\*\// /g">' + dbConfig[3] + dbConfig[4]
	os.system(cmd)

#------------------------------------------------------------------------------
def dropTable(dbConfig):
	drop = open(dbConfig[4], "r")
	text = ""
	for line in drop:
		if(line.find("CREATE TABLE") != -1 ):
			table = re.compile("`.*`").search(line).group(0)
			table = table.replace("`","")
			if(table in ingore ):
				continue
			text = "DROP TABLE IF EXISTS " + table + ";"
			doSql(dbConfig, text)

#------------------------------------------------------------------------------
def resetDB(dbConfig):
	reset = open(dbConfig[4], "r")
	text = ""
	for line in reset:
		if(line.find("CREATE TABLE") != -1):
			table = re.compile("`.*`").search(line).group(0)
			table = table.replace("`","")
			if(table in ingore ):
				continue
			text = "TRUNCATE TABLE " + table + ";"
			doSql(dbConfig, text)
	date = commands.getoutput("date +%s")
	zoneData = re.split(r'\_', dbConfig[3])
	text = "INSERT INTO LOG_RESET_DATA values(" + zoneData[1] + ", " + date + ", 0);"
	doSql(dbConfig, text)

#------------------------------------------------------------------------------
def resetName():
	text = "DELETE FROM NAME_VERIFY WHERE ZONE_ID=" + database.zoneNum + ";"
	doSql(database.adminDB, text)

#------------------------------------------------------------------------------
def updateVersion(dbConfig):
	version = open(dbConfig[6], "r")
	for line in version:
		if(line.find("REPLACE") != -1 ):
			doSql(dbConfig, line)

#------------------------------------------------------------------------------
if __name__ == "__main__":
	if len(sys.argv) != 2:
		print help
	elif sys.argv[1] == "create" or sys.argv[1] == "createzone":
		createZone()
	elif sys.argv[1] == "createadmin":
		createAdmin()
	elif sys.argv[1] == "createplatform":
		createPlatform()
	elif sys.argv[1] == "install" or sys.argv[1] == "installzone":
		installDatabase(database.zoneGameDB)
	elif sys.argv[1] == "installGame" :
		installDatabase(database.zoneGameDB)
	elif sys.argv[1] == "installInfo" :
		installDatabase(database.zoneInfoDB)
	elif sys.argv[1] == "installadmin":
		installDatabase(database.adminDB)
	elif sys.argv[1] == "installplatform":
		installDatabase(database.platformDB)
	elif sys.argv[1] == "update" or sys.argv[1] == "updatezone":
		doUpdate(database.zoneGameDB)
	elif sys.argv[1] == "updateadmin":
		doUpdate(database.adminDB)
	elif sys.argv[1] == "updateplatform":
		doUpdate(database.platformDB)
	elif sys.argv[1] == "updatemonitor":
		doUpdate(database.monitorDB)
	elif sys.argv[1] == "resetgame":
		resetDB(database.zoneGameDB)
		resetName()
	elif sys.argv[1] == "resetinfo":
		resetDB(database.zoneInfoDB)
	elif sys.argv[1] == "resetname":
		resetName()
	elif sys.argv[1] == "dropgame":
		dropTable(database.zoneGameDB)
	elif sys.argv[1] == "dropinfo":
		dropTable(database.zoneInfoDB)
	elif sys.argv[1] == "dumpzone":
		dumpInstall(database.zoneGameDB)
		dumpInstall(database.zoneInfoDB)
	elif sys.argv[1] == "dumpplatform":
		dumpInstall(database.platformDB)
	elif sys.argv[1] == "dumpadmin":
		dumpInstall(database.adminDB)
	elif sys.argv[1] == "dumpmonitor":
		dumpInstall(database.monitorDB)
	elif sys.argv[1] == "installmonitor":
		installDatabase(database.monitorDB)
	elif sys.argv[1] == "updatemonitor":
		doUpdate(database.monitorDB)
	else:
		commands.getoutput(sys.argv[1])

