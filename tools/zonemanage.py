#!/usr/bin/python
# -*- coding: utf-8 -*-

import os,re,sys,commands,platform

osName = platform.system()

zoneListFile = "./tools/zonelist.txt"
releaseFileServerIP = "10.1.101.233"
releaseFileUserName = "banben1"
releaseFileZoneID = "1"
SSH_COMMAND = "ssh -oStrictHostKeyChecking=no "

zoneOptionHead = """
--------------------------------------
q	退出
"""

zoneOptionTail = """
--------------------------------------
请输入区编号:
"""

functionMap = {}
functionMap["q"] = "回到上层"
functionMap["s"] = "登录到区"
functionMap["r"] = "重做版本"
functionMap["b"] = "做正式版本"
functionMap["d"] = "启动服务器"
functionMap["t"] = "重启服务器"
functionMap["p"] = "关闭服务器"
functionMap["c"] = "查看core文件"
functionMap["n"] = "分布式版本同步"

zoneTypeFuncMap = {}
zoneTypeFuncMap["0"] = ["q", "s", "r", "d", "t", "p", "c"]
zoneTypeFuncMap["1"] = ["q", "s", "r", "d", "t", "p", "c", "n"]
zoneTypeFuncMap["2"] = ["q", "s", "r", "d", "t", "p", "c"]
zoneTypeFuncMap["3"] = ["q", "s", "r"]
zoneTypeFuncMap["4"] = ["q", "s", "r", "d", "t", "p", "c"]
zoneTypeFuncMap["5"] = ["q", "s", "r", "d", "t", "p", "c"]
zoneTypeFuncMap["10"] = ["q", "r"]
zoneTypeFuncMap["11"] = ["q", "r"]
zoneTypeFuncMap["12"] = ["q", "r"]
zoneTypeFuncMap["13"] = ["q", "r"]

checkAssertLogOption = """
--------------------------------------
q	回到上层
1	查看当天的所有日志断言
2	查看当前小时的所有日志断言
--------------------------------------
"""

def getSshCmdStr(zone):
	if not zone in zoneList:
		return ""
	zoneInfo = zoneList[zone];
	text = ""
	if osName == "Linux":
		text = SSH_COMMAND + zoneInfo["username"] + "@" + zoneInfo["ip"] + " -p " + zoneInfo["port"]
	elif osName == "Windows":
		text = "PLINK.EXE -ssh -i private.ppk " + zoneInfo["username"] + "@" + zoneInfo["ip"] + ":/home/" + zoneInfo["username"]
	return text

def initZoneList():
	zoneFile = open(zoneListFile, "r")
	zoneInfo = ""
	global zoneList, zoneOptionList
	zoneList = {}
	zoneOptionList = ""
	for zone in zoneFile:
		if zone.find("#") != -1: continue
		zoneInfo = zone.split("\t")
		tempDict = {}
		tempDict["id"] = zoneInfo[0].rstrip()
		tempDict["name"] = zoneInfo[1].rstrip()
		tempDict["type"] = zoneInfo[2].rstrip()
		tempDict["username"] = zoneInfo[3].rstrip()
		tempDict["ip"] = zoneInfo[4].rstrip()
		tempDict["port"] = zoneInfo[5].rstrip()
		tempDict["workdir"] = zoneInfo[6].rstrip()
		tempDict["branchdir"] = zoneInfo[7].rstrip()
		zoneList[zoneInfo[0]] = tempDict
		zoneOptionList += zoneInfo[0] + "\t" + zoneInfo[1] + "\n"


def showFunctionByZone(zone):
	zoneInfo = zoneList[zone]
	while True:
		print "----------当前区：" + zone + "[" + zoneInfo["name"] + "]---------"
		for functionKey in zoneTypeFuncMap[zoneInfo["type"]]:
			print functionKey + " " + functionMap[functionKey]
		print "---------------请选择操作--------------"
		reply = raw_input()
		initZoneList()
		if reply == "q":
			break
		elif reply == "s":
			logonToZone(zone)
		elif reply == "r":
			rebuildVersion(zone)
		elif reply == "b":
			buildOfficalVersion(zone)
		elif reply == "t":
			restartServer(zone)
		elif reply == "d":
			startServer(zone)
		elif reply == "p":
			stopServer(zone)
		elif reply == "c":
			checkZoneCore(zone)
		elif reply == "n":
			distRelease(zone)
		else:
			continue


def showZoneOption():
	reply = raw_input(zoneOptionHead + zoneOptionList + zoneOptionTail)
	print "--------------!!!!!!!!!!!--------------"
	return reply


#return 0停止状态 1运行状态
#已AutoRun为标记，不准确，不启用
def getServerStatus(zone):
	if not zone in zoneList:
		return False
	
	zoneInfo = zoneList[zone]
	text = getSshCmdStr(zone) + " \"cd " + zoneInfo["workdir"] + "; ps x| grep AutoRun.sh | grep -v grep | wc -l \""
	serverStatus = commands.getoutput(text)
	return serverStatus

def logonToZone(zone):
	if not zone in zoneList:
		return False

	zoneInfo = zoneList[zone]
	if zoneInfo["type"] != "1":
		print "登录到服务器：" + zoneInfo["username"] + "@" + zoneInfo["ip"]
		text = getSshCmdStr(zone)
		os.system(text)
		return True
		
	while True:
		text = getSshCmdStr(zone) + " \"cd " + zoneInfo["workdir"] + "; ./tools/AutoTools.sh showdistconfig " + zoneInfo["port"] + "\""
		serverList = commands.getoutput(text);
		ipMap = {}
		for line in re.split('\n', serverList):
			if line.find("id") == -1:
				ipData = line.split()
				print "ipData=" + ipData[0] + " " + ipData[1]
				ipMap[ipData[0]] = ipData[2]
		if len(ipMap) == 1:
			print "登录到服务器：" + zoneInfo["username"] + "@" + ipMap['1']
			text = SSH_COMMAND + " -p " + zoneInfo["port"] + " "  + zoneInfo["username"] + "@" + ipMap['1']
			os.system(text)
			break
		print serverList
		reply = raw_input("--------请输入要登录的IP编号-------\n")
		if reply == "q":
			break
		elif reply == "" or not reply in ipMap:
			continue
		print "登录到服务器：" + zoneInfo["username"] + "@" + ipMap[reply]
		text = SSH_COMMAND + " -p " + zoneInfo["port"] + " "  + zoneInfo["username"] + "@" + ipMap[reply]
		os.system(text)
	return True

def buildOfficalVersion(zone):
	if not zone in zoneList:
		return False
	
	zoneInfo = zoneList[zone]
	#外网版本
	if zoneInfo["type"] == "1":
		while True:
			text = getSshCmdStr(releaseFileZoneID) + " \"cd " + zoneInfo["branchdir"] + "; cd yunwei; pwd; ls -lrt | grep release_ 2>/dev/null\""
			os.system(text)
			reply = raw_input("请输入要发布的文件名:\nq 回到上层\nn 做新版本\n--------------------------\n")
			if reply == "n":
				text = getSshCmdStr(releaseFileZoneID) + " \"cd " + zoneInfo["branchdir"] + "; ./tools/AutoTools.sh buildofficalversion\""
				os.system(text)
				continue
			elif reply == "q":
				break
			else:
				text = getSshCmdStr(releaseFileZoneID) + " \"cd " + zoneInfo["branchdir"] + "; ./tools/AutoTools.sh releasezone " + zoneInfo["username"] + " " + zoneInfo["ip"] + " " + zoneInfo["port"] + " " + reply + "\""
				#os.system(text)
				print "功能不可用，未知流程"
				print "[" + zoneInfo["name"] + "]版本发布完成:" + reply;
				break

def rebuildVersion(zone):
	if not zone in zoneList:
		return False
	
	zoneInfo = zoneList[zone]
	#内网版本
	if zoneInfo["type"] == "0":
		text = getSshCmdStr(zone) + " \"cd " + zoneInfo["branchdir"] + "; ./tools/AutoTools.sh rebuildversion;\""
		os.system(text)
	#外网版本
	elif zoneInfo["type"] == "1":
		while True:
			text = getSshCmdStr(releaseFileZoneID) + " \"cd " + zoneInfo["branchdir"] + "; cd server; pwd; ls -lrt | grep ^d | grep release_ 2>/dev/null\""
			os.system(text)
			reply = raw_input("请输入要发布的文件名:\nq 回到上层\nn 做新版本\n--------------------------\n")
			if reply == "n":
				text = getSshCmdStr(releaseFileZoneID) + " \"cd " + zoneInfo["branchdir"] + "; ./tools/AutoTools.sh rebuildrelease\""
				os.system(text)
				continue
			elif reply == "q":
				break
			else:
				text = getSshCmdStr(releaseFileZoneID) + " \"cd " + zoneInfo["branchdir"] + "; ./tools/AutoTools.sh releasezone " + zoneInfo["username"] + " " + zoneInfo["ip"] + " " + zoneInfo["port"] + " " + reply + "\""
				os.system(text)
				print "[" + zoneInfo["name"] + "]版本发布完成:" + reply;
				break
	#客户端资源
	elif zoneInfo["type"] == "3":
		while True:
			text = getSshCmdStr(releaseFileZoneID) + " \"cd " + zoneInfo["branchdir"] + "; pwd; ls -lrt ClientRes_*.zip 2>/dev/null\""
			os.system(text)
			reply = raw_input("请输入要发布的文件名:\nq 回到上层\n--------------------------\n")
			if reply == "q":
				break
			else:
				text = getSshCmdStr(releaseFileZoneID) + " \"cd " + zoneInfo["branchdir"] + "; ./AutoTools.sh releaseclient " + zoneInfo["username"] + " " + zoneInfo["ip"] + " " + zoneInfo["port"] + " " + reply + " " + zoneInfo["workdir"] + "\""
				os.system(text)
				print "[" + zoneInfo["name"] + "]版本发布完成:" + reply;
				break
	#外网版本admin
	elif zoneInfo["type"] == "2":
		while True:
			text = getSshCmdStr(releaseFileZoneID) + " \"cd " + zoneInfo["branchdir"] + "; cd admin; pwd; ls -lrt | grep ^d | grep release_ 2>/dev/null\""
			os.system(text)
			reply = raw_input("请输入要发布的文件名:\nq 回到上层\nn 做新版本:\n--------------------------\n")
			if reply == "n":
				text = getSshCmdStr(releaseFileZoneID) + " \"cd " + zoneInfo["branchdir"] + "; ./tools/AutoTools.sh rebuildadmin\""
				os.system(text)
				continue
			elif reply == "q":
				break
			else:
				text = getSshCmdStr(releaseFileZoneID) + " \"cd " + zoneInfo["branchdir"] +"; ./tools/AutoTools.sh releaseadmin " + zoneInfo["username"] + " " + zoneInfo["ip"] + " " + zoneInfo["port"] + " " + reply + "\""
				os.system(text)
				print "[" + zoneInfo["name"] + "]版本发布完成:" + reply;
				break

	return True

def restartServer(zone):
	if not zone in zoneList:
		return False
	
	zoneInfo = zoneList[zone]
	#内网版本,外网版本
	if zoneInfo["type"] == "0" or zoneInfo["type"] == "1":
		text = getSshCmdStr(zone) + " \"cd " + zoneInfo["workdir"] + "; pkill AutoRun.sh -u " + zoneInfo["username"] + "; chmod u+x ./Run.sh; ./Run.sh; chmod u+x ./tools/AutoRun.sh; (./tools/AutoRun.sh &) >/dev/null </dev/null 2>&1;\""
		os.system(text)
	#外网版本admin
	elif zoneInfo["type"] == "2":
		text = getSshCmdStr(zone) + " \"cd " + zoneInfo["workdir"] + "; chmod u+x ./Run.sh; ./Run.sh admin;\""
		os.system(text)
	else: return False

	return True

def startServer(zone):
	if not zone in zoneList:
		return False
	
	if getServerStatus(zone) != "0":
		print "!!!!!!!!!!!!!!!!服务器正在运行，无需启动!!!!!!!!!!!!!!!!!!!!"
		return True
	zoneInfo = zoneList[zone]
	#内网版本,外网版本
	if zoneInfo["type"] == "0" or zoneInfo["type"] == "1":
		text = getSshCmdStr(zone) + " \"cd " + zoneInfo["workdir"] + "; chmod u+x ./Run.sh; ./Run.sh; chmod u+x ./tools/AutoRun.sh; (./tools/AutoRun.sh &) >/dev/null </dev/null 2>&1;\""
		os.system(text)
	#外网版本admin
	elif zoneInfo["type"] == "2":
		text = getSshCmdStr(zone) + " \"cd " + zoneInfo["workdir"] + "; chmod u+x ./Run.sh; ./Run.sh admin;\""
		os.system(text)
	else: return False

	return True

def stopServer(zone):
	if not zone in zoneList:
		return False
	
	zoneInfo = zoneList[zone]
	#内网版本,外网版本
	if zoneInfo["type"] == "0" or zoneInfo["type"] == "1":
		text = getSshCmdStr(zone) + " \"cd " + zoneInfo["workdir"] + "; chmod u+x ./Run.sh; pkill AutoRun.sh -u " + zoneInfo["username"] + "; ./Run.sh stop;\""
		os.system(text)
	#外网版本admin
	elif zoneInfo["type"] == "2":
		text = getSshCmdStr(zone) + " \"cd " + zoneInfo["workdir"] + "; chmod u+x ./Run.sh; ./Run.sh stopadmin;\""
		os.system(text)
	else: return False

	return True

def checkAllZoneCore():
	for zone in zoneList.keys():
		checkZoneCore(zone)

def checkZoneCore(zone):
	zoneInfo = zoneList[zone]
	text = getSshCmdStr(zone) + " \"cd " + zoneInfo["workdir"] + "; ./tools/AutoTools.sh checkcore " + zoneInfo["port"] + " " + zoneInfo["workdir"] + ";\""
	os.system(text)
	return True

def distRelease(zone):
	zoneInfo = zoneList[zone]
	text = getSshCmdStr(zone) + " \"cd " + zoneInfo["workdir"] + "; ./tools/AutoTools.sh distrelease " + zoneInfo["port"] + "\""
	os.system(text)
	return True

def mainMenu():
	while True:
		reply = showZoneOption()
		initZoneList()
		if reply == "q":
			break
		elif reply in zoneList:
			showFunctionByZone(reply)
		else:
			continue

initZoneList()
#传参直接调用函数
if len(sys.argv) > 2:
	param = sys.argv[1]
	value = sys.argv[2]
	if param == "show_server_status":
		print getServerStatus(value)
	elif param == "start_server":
		startServer(value)
	elif param == "restart_server":
		restartServer(value)
	elif param == "stop_server":
		stopServer(value)
#进入交互模式
else:
	mainMenu()
