import os
import time
import sys
import configparser
import logging
import traceback
import glob

try:
#需要配置 begin
	ConfigFileName = "serverVersionData.ini"
#需要配置 end

	print(1)
	cf = configparser.ConfigParser()
	cf.read(sys.path[0] + "\\" + ConfigFileName)
#读取的配置 begin

	PuttyDir= sys.path[0] + "\\"
	if os.path.isfile(PuttyDir + "\PLINK.EXE") == False:
		PuttyDir= cf["Dir"]["PuttyDir"]
	if os.path.isfile(PuttyDir + "\PLINK.EXE") == False:
		print(ConfigFileName + "文件中的PuttyDir配置错误，需要配置putty安装目录")
		os.system("pause")
		exit(1)
	print(PuttyDir)
	cf.set("Dir", "PuttyDir", PuttyDir)
	cf.write(open(sys.path[0] + "\\" + ConfigFileName, "w"))

	UserName= ""
	ServerDir=""
	ServerIP = ""
	ServerDataList = cf["Server"]["ServerList"].split(";")
	ServerList = []
	SelectServerID = 1
	for ServerData in ServerDataList:
		Server = ServerData.split("-")
		ServerList.append(Server)
		print("服务器 " + str(SelectServerID) + " : " + Server[0] + " " + Server[1] + "\n")
		SelectServerID = SelectServerID + 1
	SelectServerID = int(input("选择服务器\n")) -1
	if SelectServerID < len(ServerList) and SelectServerID >= 0:
		Server = ServerList[SelectServerID]
	else:
		exit(1)
	print("1更新到svn的最新版本并重启;2不更新只重启服务器;3回退本地所有修改过得文件")
	SelectOptionID = int(input("选择操作类型\n"))
#读取的配置 end

#用到的 工具和相对路径 begin
	ServerPath=Server[0] + "@" + Server[1] + ":/home/" + Server[0] + "/" + ServerDir

	LogFileName = sys.path[0] + "\\版本制做_" + time.strftime("%Y-%m-%d_%H-%M-%S",time.localtime()) + ".log"
	PrivateKey = sys.path[0] + "\\private.ppk"
#用到的 工具和相对路径 end

#以上是所有可能的、需要版本制做人员修改的[参数部分]
#如果你不希望编辑这个脚本的[功能]，就不需要继续向下看了

	logging.basicConfig(filename=LogFileName,level=logging.DEBUG)
	logging.debug("----日志记录开始----")

	if SelectOptionID == 1:
		os.system(PuttyDir + "PLINK.EXE -ssh -i " + PrivateKey + " " + ServerPath + " \"pwd; cd ~/" + Server[2] + ";svn cleanup;cd data; rm -rf table map;cd ..;svn up;cmake .;make  -j4; cd sql;python dbtool.py update;cd ..;./Run.sh;\"")
	elif SelectOptionID == 2:
		os.system(PuttyDir + "PLINK.EXE -ssh -i " + PrivateKey + " " + ServerPath + " \"pwd; cd ~/" + Server[2] + ";./Run.sh\"")
	elif SelectOptionID == 3:
		os.system(PuttyDir + "PLINK.EXE -ssh -i " + PrivateKey + " " + ServerPath + " \"pwd; cd ~/" + Server[2] + ";svn revert -R Script/ data/;cmake .;make  -j4;./Run.sh\"")
	print("----编译发布完成----")
	print("----版本制做完成---")
except:
	print("----出现异常，开始处理---")
	logging.debug("----版本制做.py出错啦！----")
	logging.debug(sys.exc_info());
	print(sys.exc_info())
	print("----出现异常，处理结束---")
logging.debug("----日志记录结束----")
os.system("pause")
