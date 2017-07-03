#!/bin/bash

DEFAULT_PORT=59878
SSH_COMMAND="ssh -oStrictHostKeyChecking=no "

translateExample()
{
	userClientType="USERCLIENT"
	if [ $platformType -eq 2 ]
	then
		userClientType="QQCLOUDUSERCLIENT"
	fi
    file=`echo "$1" | sed 's/\.example//'`
    sed -e 's/{zoneNum}/'$zoneNum'/g' \
    -e 's/{platformType}/'$platformType'/g' \
    -e 's/{logPath}/'$logPath'/g' \
    -e 's/{zoneGameDBServerIP}/'$zoneGameDBServerIP'/g' \
    -e 's/{zoneGameDBServerUserName}/'$zoneGameDBServerUserName'/g' \
    -e 's/{zoneGameDBServerUserPassword}/'$zoneGameDBServerUserPassword'/g' \
    -e 's/{zoneInfoDBServerIP}/'$zoneInfoDBServerIP'/g' \
    -e 's/{zoneInfoDBServerUserName}/'$zoneInfoDBServerUserName'/g' \
    -e 's/{zoneInfoDBServerUserPassword}/'$zoneInfoDBServerUserPassword'/g' \
    -e 's/{platformServerIP}/'$platformServerIP'/g' \
    -e 's/{platformServerPort}/'$platformServerPort'/g' \
    -e 's/{platformDBServerIP}/'$platformDBServerIP'/g' \
    -e 's/{platformDBServerUserName}/'$platformDBServerUserName'/g' \
    -e 's/{platformDBServerUserPassword}/'$platformDBServerUserPassword'/g' \
    -e 's/{platformLogUrl_1}/'$platformLogUrl_1'/g' \
    -e 's/{platformLogGameID_1}/'$platformLogGameID_1'/g' \
    -e 's/{platformLogGameVerifyKey_1}/'$platformLogGameVerifyKey_1'/g' \
    -e 's/{adminServerIP}/'$adminServerIP'/g' \
    -e 's/{adminServerPort}/'$adminServerPort'/g' \
    -e 's/{adminDBServerIP}/'$adminDBServerIP'/g' \
    -e 's/{adminDBServerUserName}/'$adminDBServerUserName'/g' \
    -e 's/{adminDBServerUserPassword}/'$adminDBServerUserPassword'/g' \
    -e 's/{zoneName}/'$zoneName'/g' \
    -e 's/{zoneServerPort}/'$zoneServerPort'/g' \
    -e 's/{dataPath}/'$dataPath'/g' \
    -e 's/{externalIP_CT}/'$externalIP_CT'/g' \
    -e 's/{externalIP_CU}/'$externalIP_CU'/g' \
    -e 's/{userClientType}/'$userClientType'/g' \
    -e 's/{internalIP}/'$internalIP'/g' "$1" >"$file"
}

function installZone()
{
	# checkout 代码
	svn co $codeRepos $codeDir --username $svnUserName --password $svnPassword --non-interactive --trust-server-cert

	cp InstallConfig.sh $codeDir/tools
	cd $codeDir
	
	basePath=`svn info |grep URL | sed 's/地址(URL)：//g' | sed 's/URL://g' |sed 's/\/server//g'`
	svn switch $basePath/NetCommand ./NetCommand --username $svnUserName --password $svnPassword --ignore-ancestry
	svn switch $basePath/ThirdParty ./ThirdParty --username $svnUserName --password $svnPassword --ignore-ancestry

	cp builders/cmake/Custom.txt.example builders/cmake/Custom.txt
	cmake .
	make -j12

	echo "新区建立完成 ZoneServerIP=[$internalIP] ZoneServerPort=[$zoneServerPort] GatewayServerPort=[$gatewayServerPort]"
}

function initZone()
{
	# 创建日志目录
	mkdir -p $orgLogPath;
	mkdir -p $orgDataPath;

	chmod u+x ./tools/AutoTools.sh
	chmod +x Run.sh
	chmod +x Log.sh
	
	# 拷贝example文件
	translateExample "config/ZoneConfig.xml.example"
	zoneServerPort=`sed -n '/ZoneServer/ s/.*port=\"\([0-9]*\)\".*/\1/ p' config/ZoneConfig.xml`
	gatewayServerPort=`sed -n '/GatewayServer/ s/.*port=\"\([0-9]*\)\".*/\1/ p' config/ZoneConfig.xml`
	translateExample "sql/database.py.example"

	# 创建数据库
	cd sql
	python dbtool.py createzone
	cd ..

	echo "新区初始化完成 ZoneServerIP=[$internalIP] ZoneServerPort=[$zoneServerPort] GatewayServerPort=[$gatewayServerPort]"
}

function updateDatabaseConfig()
{
	zoneServerPort=`sed -n '/ZoneServer/ s/.*port=\"\([0-9]*\)\".*/\1/ p' config/ZoneConfig.xml`
	gatewayServerPort=`sed -n '/GatewayServer/ s/.*port=\"\([0-9]*\)\".*/\1/ p' config/ZoneConfig.xml`
	translateExample "sql/database.py.example"
}

function updateZoneConfig()
{
	translateExample "config/ZoneConfig.xml.example"
}

function backupCore()
{
	CORE_COUNT=`ls -l core.* 2>/dev/null | wc -l`
	if [ $CORE_COUNT -lt 1 ]
	then
		return
	fi
	binFileList="`file core.* | awk -F"'" '{print $2}' | sort | uniq`";
	backupDirName=backup_`date +%y_%m_%d_%H_%M_%S`
	mkdir $backupDirName;
	mv core.* $backupDirName;
	for binFile in $binFileList
	do  
		cp $binFile/$binFile $backupDirName;
	done
}

function rebuildVersion()
{
	backupCore;
	svn cleanup;
	svn up; 
	cmake .;
	make -j4;
	if [ $? -ne 0 ]
	then
		echo "编译错误!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		exit 1
	fi
	#./tools/AutoTools.sh updatedbconfig
	#cd sql;
	#python dbtool.py update;
	#if [ $? -ne 0 ]
	#then
	#	echo "更新数据库错误!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	#	exit 1
	#fi
	#cd ..; 
	./Run.sh stop;
	sleep 10
	./Run.sh
	echo "版本制作完成"
}

function checkCore()
{
while true
do
	USER_NAME=$USER
	PORT=$1
	WORD_DIR=$2
	if [ "$PORT" == "" ]
	then
		PORT=$DEFAULT_PORT
	fi
	LOCAL_IP_STR="`getLocalIP`";
	echo "---------------$LOCAL_IP_STR core文件----------------"
	cd $WORD_DIR;
	ls -lrt core.* backup/core.* 2>/dev/null | awk -F" " '{cmd="file "$8;cmd|getline v;print v" "$6" "$7}' | awk -F" " '{print $15" "$16" "$1" "$14}' | sed s/\'//g | sed 's/: / /g'
	LOCAL_SERVER_LIST=`grep Server config/ZoneConfig.xml | grep "type=" | grep "name=" | grep "ip=\"$LOCAL_IP_STR\"" | awk -F'"' '{print $6}'`
	IS_ZONE_SERVER=false
	for server in $LOCAL_SERVER_LIST
	do
		if [ "$server" == "ZoneServer" ]
		then
			IS_ZONE_SERVER=true
		fi
	done
	if [ "$IS_ZONE_SERVER" == "false" ]
	then
		break;
	fi
	DIST_IP_LIST=`grep Server config/ZoneConfig.xml | grep "name=" | grep -v "ip=\"$LOCAL_IP_STR\"" | awk -F'"' '{print $8}' | sort | uniq`
	if [ "$DIST_IP_LIST" == "" ]
	then
		break;
	fi
	cd ~;
	for ipstr in $DIST_IP_LIST
	do
		$SSH_COMMAND -p $PORT $USER_NAME@$ipstr "cd $WORD_DIR; ./tools/AutoTools.sh checkcore $1 $2";
	done
	break;
done
}


function checkAssertLog()
{
	if [ "$1" == "day" ]
	then
		grep "ASSERT FAIL" ~/log/*server.log.`date +"%y%m%d"`*
	else
		grep "ASSERT FAIL" ~/log/*server.log
	fi
}

function rebuildAdmin()
{
	ROOT_PATH="admin/release"
	svn up;
	if [ $? -ne 0 ]
	then
		echo "svn up命令执行失败!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		exit 1
	fi
	cmake .;
	if [ $? -ne 0 ]
	then
		echo "cmake .命令执行失败!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		exit 1
	fi
	make -j4 AdminServer;
	if [ $? -ne 0 ]
	then
		echo "make -j4命令执行失败!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		exit 1
	fi
	mkdir -p admin;
	cd admin;
	rm -rf release;
	cd ..;
	mkdir -p $ROOT_PATH;
	mkdir -p $ROOT_PATH/AdminServer;
	echo "...导出Admin..."
	cp -rf AdminServer/AdminServer $ROOT_PATH/AdminServer/;
	cp -rf AdminServer/AdminConfig.xml.example $ROOT_PATH/AdminServer/;
	cp -rf AdminServer/data $ROOT_PATH/AdminServer/;
	cp -rf ThirdParty/gperftools-2.1/lib/libprofiler.so.0 $ROOT_PATH/
	cp -rf ThirdParty/gperftools-2.1/lib/libtcmalloc.so.4 $ROOT_PATH/
	cp -rf ThirdParty/gperftools-2.1/lib/libunwind.so.7 $ROOT_PATH/
	cp ThirdParty/curl-4.2.0/lib/libcares.so.2 $ROOT_PATH/libcares.so.2
	cp ThirdParty/curl-4.2.0/lib/libcurl.so.4 $ROOT_PATH/libcurl.so.4
	cp ThirdParty/openssl-1.0.1/lib/libcrypto.so.1.0.0 $ROOT_PATH/libcrypto.so.1.0.0
	cp ThirdParty/openssl-1.0.1/lib/libssl.so.1.0.0 $ROOT_PATH/libssl.so.1.0.0
	cp ThirdParty/gperftools-2.1/bin/pprof $ROOT_PATH/pprof
	cp -rf /usr/local/lib/libiconv.so.2 $ROOT_PATH/
	cp -rf Run.sh $ROOT_PATH/
	cp -rf log.sh $ROOT_PATH/
	cp -rf tools $ROOT_PATH/
	cp -rf sql $ROOT_PATH/
	svn info > $ROOT_PATH/version.txt;

	cd admin/
	mv release release_`date +%y_%m_%d_%H_%M_%S`
	cd ..;
	echo "...admin版本制作完成";
}

function rebuildPlatform()
{
	ROOT_PATH="platform/release"
	svn up;
	if [ $? -ne 0 ]
	then
		echo "svn up命令执行失败!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		exit 1
	fi
	cmake .;
	if [ $? -ne 0 ]
	then
		echo "cmake .命令执行失败!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		exit 1
	fi
	make -j4 PlatformServer;
	if [ $? -ne 0 ]
	then
		echo "make -j4命令执行失败!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		exit 1
	fi
	mkdir -p platform;
	cd platform;
	rm -rf release;
	cd ..;
	mkdir -p $ROOT_PATH;
	mkdir -p $ROOT_PATH/PlatformServer;
	echo "...导出Platform..."
	cp -rf PlatformServer/PlatformServer $ROOT_PATH/PlatformServer/;
	cp -rf PlatformServer/PlatformConfig.xml.example $ROOT_PATH/PlatformServer/;
	cp -rf PlatformServer/QQGameConfig.xml $ROOT_PATH/PlatformServer/;
	cp -rf PlatformServer/data $ROOT_PATH/PlatformServer/;
	cp -rf ThirdParty/gperftools-2.1/lib/libprofiler.so.0 $ROOT_PATH/
	cp -rf ThirdParty/gperftools-2.1/lib/libtcmalloc.so.4 $ROOT_PATH/
	cp -rf ThirdParty/gperftools-2.1/lib/libunwind.so.7 $ROOT_PATH/
	cp ThirdParty/curl-4.2.0/lib/libcares.so.2 $ROOT_PATH/libcares.so.2
	cp ThirdParty/curl-4.2.0/lib/libcurl.so.4 $ROOT_PATH/libcurl.so.4
	cp ThirdParty/openssl-1.0.1/lib/libcrypto.so.1.0.0 $ROOT_PATH/libcrypto.so.1.0.0
	cp ThirdParty/openssl-1.0.1/lib/libssl.so.1.0.0 $ROOT_PATH/libssl.so.1.0.0
	cp ThirdParty/gperftools-2.1/bin/pprof $ROOT_PATH/pprof
	cp -rf /usr/local/lib/libiconv.so.2 $ROOT_PATH/
	cp -rf Run.sh $ROOT_PATH/
	cp -rf log.sh $ROOT_PATH/
	cp -rf tools $ROOT_PATH/
	cp -rf sql $ROOT_PATH/
	svn info > $ROOT_PATH/version.txt;

	cd platform/
	mv release release_`date +%y_%m_%d_%H_%M_%S`
	cd ..;
	echo "...monitor版本制作完成";
}

function rebuildMonitor()
{
	ROOT_PATH="monitor/release"
	svn up;
	if [ $? -ne 0 ]
	then
		echo "svn up命令执行失败!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		exit 1
	fi
	cmake .;
	if [ $? -ne 0 ]
	then
		echo "cmake .命令执行失败!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		exit 1
	fi
	make -j4 MonitorServer;
	if [ $? -ne 0 ]
	then
		echo "make -j4命令执行失败!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		exit 1
	fi
	mkdir -p monitor;
	cd monitor;
	rm -rf release;
	cd ..;
	mkdir -p $ROOT_PATH;
	mkdir -p $ROOT_PATH/MonitorServer;
	echo "...导出Monitor..."
	cp -rf MonitorServer/MonitorServer $ROOT_PATH/MonitorServer/;
	cp -rf MonitorServer/MonitorConfig.xml.example $ROOT_PATH/MonitorServer/;
	cp -rf MonitorServer/data $ROOT_PATH/MonitorServer/;
	cp -rf ThirdParty/gperftools-2.1/lib/libprofiler.so.0 $ROOT_PATH/
	cp -rf ThirdParty/gperftools-2.1/lib/libtcmalloc.so.4 $ROOT_PATH/
	cp -rf ThirdParty/gperftools-2.1/lib/libunwind.so.7 $ROOT_PATH/
	cp ThirdParty/curl-4.2.0/lib/libcares.so.2 $ROOT_PATH/libcares.so.2
	cp ThirdParty/curl-4.2.0/lib/libcurl.so.4 $ROOT_PATH/libcurl.so.4
	cp ThirdParty/openssl-1.0.1/lib/libcrypto.so.1.0.0 $ROOT_PATH/libcrypto.so.1.0.0
	cp ThirdParty/openssl-1.0.1/lib/libssl.so.1.0.0 $ROOT_PATH/libssl.so.1.0.0
	cp ThirdParty/gperftools-2.1/bin/pprof $ROOT_PATH/pprof
	cp -rf /usr/local/lib/libiconv.so.2 $ROOT_PATH/
	cp -rf Run.sh $ROOT_PATH/
	cp -rf log.sh $ROOT_PATH/
	cp -rf tools $ROOT_PATH/
	cp -rf sql $ROOT_PATH/
	svn info > $ROOT_PATH/version.txt;

	cd monitor/
	mv release release_`date +%y_%m_%d_%H_%M_%S`
	cd ..;
	echo "...monitor版本制作完成";
}


function rebuildRelease()
{
	svn up;
	if [ $? -ne 0 ]
	then
		echo "svn up命令执行失败!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		exit 1
	fi
	cmake .;
	if [ $? -ne 0 ]
	then
		echo "cmake .命令执行失败!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		exit 1
	fi
	make -j4;
	if [ $? -ne 0 ]
	then
		echo "cmake .命令执行失败!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		exit 1
	fi

	mkdir -p server

	RELEASE_PATH=server/release
	RELEASE_BIN_LIST="ZoneServer DataServer SessionServer LogicServer AIServer GatewayServer InfoServer BlobTool"
	rm -rf $RELEASE_PATH
	echo "...创建目录..."
	mkdir -p $RELEASE_PATH;
	for binFileName in $RELEASE_BIN_LIST
	do
		mkdir -p $RELEASE_PATH/$binFileName;
	done

	echo "...导出 sql..."
	svn export sql $RELEASE_PATH/sql;
	echo "...导出 config..."
	svn export config $RELEASE_PATH/config;
	echo "...导出 data..."
	svn export data $RELEASE_PATH/data;
	echo "...导出 Script..."
	svn export Script $RELEASE_PATH/Script;
	echo "...导出 tools..."
	svn export tools $RELEASE_PATH/tools;

	for binFileName in $RELEASE_BIN_LIST
	do
		echo "...导出$binFileName..."
		cp $binFileName/$binFileName $RELEASE_PATH/$binFileName/;
	done

	echo "...导出ThirdParty..."
	cp ThirdParty/gperftools-2.1/lib/libprofiler.so.0 $RELEASE_PATH/
	cp ThirdParty/gperftools-2.1/lib/libtcmalloc.so.4 $RELEASE_PATH/
	cp ThirdParty/gperftools-2.1/lib/libunwind.so.7 $RELEASE_PATH/
	cp ThirdParty/curl-4.2.0/lib/libcares.so.2 $RELEASE_PATH/libcares.so.2
	cp ThirdParty/curl-4.2.0/lib/libcurl.so.4 $RELEASE_PATH/libcurl.so.4
	cp ThirdParty/openssl-1.0.1/lib/libcrypto.so.1.0.0 $RELEASE_PATH/libcrypto.so.1.0.0
	cp ThirdParty/openssl-1.0.1/lib/libssl.so.1.0.0 $RELEASE_PATH/libssl.so.1.0.0
	cp ThirdParty/gperftools-2.1/bin/pprof $RELEASE_PATH/pprof
	cp /usr/local/lib/libiconv.so.2 $RELEASE_PATH/
	cp Run.sh $RELEASE_PATH/
	cp log.sh $RELEASE_PATH/
	cp BlobTool/BlobConfig.xml.example $RELEASE_PATH/BlobTool/
	svn info > $RELEASE_PATH/version.txt;

	cd server/
	mv release release_`date +%y_%m_%d_%H_%M_%S`
	cd ..;
	echo "...版本制作完成";
}

#生成正式官方版本
function buildOfficalVersion()
{
	svn up;
	if [ $? -ne 0 ]
	then
		echo "svn up命令执行失败!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		exit 1
	fi
	cmake .;
	if [ $? -ne 0 ]
	then
		echo "cmake .命令执行失败!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		exit 1
	fi
	make -j4;
	if [ $? -ne 0 ]
	then
		echo "cmake .命令执行失败!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		exit 1
	fi

	mkdir -p yunwei

	RELEASE_FILE_NAME=release_`date +%y_%m_%d_%H_%M_%S`
	RELEASE_PATH=yunwei/release
	RELEASE_BIN_LIST="ZoneServer DataServer SessionServer LogicServer AIServer GatewayServer InfoServer BlobTool"
	rm -rf $RELEASE_PATH
	echo "...创建目录"
	mkdir -p $RELEASE_PATH;
	for binFileName in $RELEASE_BIN_LIST
	do
		mkdir -p $RELEASE_PATH/$binFileName;
	done

	echo "...导出 sql..."
	svn export sql $RELEASE_PATH/sql;
	echo "...导出 config..."
	svn export config $RELEASE_PATH/config;
	echo "...导出 data..."
	svn export data $RELEASE_PATH/data;
	echo "...导出 Script..."
	svn export Script $RELEASE_PATH/Script;
	echo "...导出 tools..."
	svn export tools $RELEASE_PATH/tools;

	for binFileName in $RELEASE_BIN_LIST
	do
		echo "...导出$binFileName..."
		cp $binFileName/$binFileName $RELEASE_PATH/$binFileName/;
		strip $RELEASE_PATH/$binFileName/$binFileName
	done

	echo "...导出ThirdParty..."
	cp ThirdParty/gperftools-2.1/lib/libprofiler.so.0 $RELEASE_PATH/
	cp ThirdParty/gperftools-2.1/lib/libtcmalloc.so.4 $RELEASE_PATH/
	cp ThirdParty/gperftools-2.1/lib/libunwind.so.7 $RELEASE_PATH/
	cp ThirdParty/curl-4.2.0/lib/libcares.so.2 $RELEASE_PATH/libcares.so.2
	cp ThirdParty/curl-4.2.0/lib/libcurl.so.4 $RELEASE_PATH/libcurl.so.4
	cp ThirdParty/openssl-1.0.1/lib/libcrypto.so.1.0.0 $RELEASE_PATH/libcrypto.so.1.0.0
	cp ThirdParty/openssl-1.0.1/lib/libssl.so.1.0.0 $RELEASE_PATH/libssl.so.1.0.0
	cp ThirdParty/gperftools-2.1/bin/pprof $RELEASE_PATH/pprof
	cp /usr/local/lib/libiconv.so.2 $RELEASE_PATH/
	cp Run.sh $RELEASE_PATH/
	cp log.sh $RELEASE_PATH/
	cp BlobTool/BlobConfig.xml.example $RELEASE_PATH/BlobTool/
	svn info > $RELEASE_PATH/version.txt;

	cd yunwei/
	mv release $RELEASE_FILE_NAME
	tar -zcvf $RELEASE_FILE_NAME.tar $RELEASE_FILE_NAME
	cd ..;
	echo "...版本制作完成";
}

function distRelease()
{
while true
do
	USER_NAME=$USER
	PORT=$1
	if [ "$PORT" == "" ]
	then
		PORT=$DEFAULT_PORT
	fi
	LOCAL_IP_STR="`getLocalIP`";
	OLD_FILE_NAME="release_old_`date +%y_%m_%d_%H_%M_%S`";
	DIST_IP_LIST=`grep Server config/ZoneConfig.xml | grep "name=" | grep -v "ip=\"$LOCAL_IP_STR\"" | awk -F'"' '{print $8}' | sort | uniq`
	if [ "$DIST_IP_LIST" == "" ]
	then
		echo "不是分布式部署"
		break;
	fi
	cd ~;
	cp -rf ~/release ~/release_new;
	for ipstr in $DIST_IP_LIST
	do
		echo "---------------$ipstr 开始同步----------------"
		$SSH_COMMAND -p $PORT $USER_NAME@$ipstr "source ~/.bash_profile;\
		cd ~;\
		mkdir log;\
		mkdir data;\
		cp -rf release release_new;"
		rsync -ravtzp --delete --progress -e "$SSH_COMMAND -p$PORT" ~/release_new/ $USER_NAME@$ipstr:/home/$USER_NAME/release_new/;
		$SSH_COMMAND -p $PORT $USER_NAME@$ipstr "source ~/.bash_profile;\
		cd ~;\
		mv ~/release $OLD_FILE_NAME;\
		mv ~/release_new ~/release;\
		cd ~/release;\
		chmod u+x tools/AutoTools.sh;\
		chmod u+x log.sh;\
		chmod u+x Run.sh;\
		./tools/AutoTools.sh updatedbconfig;"
		echo "-------------$ipstr 服务器资源同步完成-------"
	done
	rm -rf ~/release_new;
	break;
done
}

function getLocalIP()
{
	LOCAL_IP_LIST=`/sbin/ifconfig | grep "inet addr" | cut -f 2 -d ":" | cut -f 1 -d " "`
	#确定配置中使用的本地IP
	for ipstr in $LOCAL_IP_LIST
	do
		COUNT=`grep Server config/ZoneConfig.xml | grep "type=" | grep "name=" | grep "ip=\"$ipstr\"" | wc -l`
		if [ $COUNT -gt 0 ]
		then
			echo "$ipstr"
			break;
		fi
	done
}

function distConfig()
{
while true
do
	USER_NAME=$USER
	PORT=$1
	if [ "$PORT" == "" ]
	then
		PORT=$DEFAULT_PORT
	fi
	LOCAL_IP_STR="`getLocalIP`";
	DIST_IP_LIST=`grep Server config/ZoneConfig.xml | grep "name=" | grep -v "ip=\"$LOCAL_IP_STR\"" | awk -F'"' '{print $8}' | sort | uniq`
	if [ "$DIST_IP_LIST" == "" ]
	then
		echo "不是分布式部署"
		break;
	fi
	cd ~;
	for ipstr in $DIST_IP_LIST
	do
		echo "---------------$ipstr 开始更新----------------"
		rsync -ravtzp --delete --progress -e "$SSH_COMMAND -p$PORT" ~/release/config/ZoneConfig.xml $USER_NAME@$ipstr:/home/$USER_NAME/release/config/ZoneConfig.xml;
		echo "-------------$ipstr 服务器配置更新完成-------"
	done
	break;
done
}

function showDistConfig()
{
	USER_NAME=$USER
	PORT=$1
	if [ "$PORT" == "" ]
	then
		PORT=$DEFAULT_PORT
	fi
	DIST_IP_LIST=`grep Server config/ZoneConfig.xml | grep "type=" | grep "name=" | grep "ip=" | awk -F'"' '{print $8}' | sort | uniq`
	IP_NUM=0
	for ipstr in $DIST_IP_LIST
	do
		IP_NUM=$(($((IP_NUM))+1))
		echo -n "$IP_NUM : "
		IP_LIST=`$SSH_COMMAND -p $PORT $USER_NAME@$ipstr "/sbin/ifconfig | grep -v '127.0.0.1' | grep 'inet addr' | cut -f 2 -d ':' | cut -f 1 -d ' '"`
		IP_LIST_COUNT=`echo $IP_LIST | wc -w`
		if [ $IP_LIST_COUNT -lt 2 ]
		then
			echo $IP_LIST;
		else
			#去掉局域网地址
			echo $IP_LIST | sed -e 's/192.168\.[0-9]*\.[0-9]*//g' | sed -e 's/10\.[0-9]*\.[0-9]*\.[0-9]*//g' | sed -e 's/172\.(1[6-9])|(2[0-9])|(3[0-1])\.[0-9]*\.[0-9]*//g' | cut -d ' ' -f 1
		fi
		grep Server config/ZoneConfig.xml | grep "type=" | grep "name=" | grep "ip=\"$ipstr\"" | awk -F'"' '{print "	"$6" id="$2}'
	done
}

function showQQDistConfig()
{
	USER_NAME=$USER
	PORT=$1
	if [ "$PORT" == "" ]
	then
		PORT=$DEFAULT_PORT
	fi
	DIST_IP_LIST=`grep Server config/ZoneConfig.xml | grep "type=" | grep "name=" | grep "ip=" | awk -F'"' '{print $8}' | sort | uniq`
	IP_NUM=0
	for ipstr in $DIST_IP_LIST
	do
		IP_NUM=$(($((IP_NUM))+1))
		echo "$IP_NUM : $ipstr"
		grep Server config/ZoneConfig.xml | grep "type=" | grep "name=" | grep "ip=\"$ipstr\"" | awk -F'"' '{print "	"$6" id="$2}'
	done
}

function releaseZone()
{
	USER_NAME=$1
	IP_STR=$2
	PORT=$3
	RELEASE_FILE_NAME=$4
	cd server;
	if [ ! -e $RELEASE_FILE_NAME ]
	then
		echo "$RELEASE_FILE_NAME 文件不存在!!!!!!!!"
		exit
	fi
	oldFileName="release_old_`date +%y_%m_%d_%H_%M_%S`";
	START_SEC=`date +%s`
	$SSH_COMMAND -p $PORT $USER_NAME@$IP_STR "source ~/.bash_profile;\
	cd ~;\
	cp -rf release release_new;"
	rsync -ravtzp --delete --exclude=data/run --exclude=config/ZoneConfig.xml --exclude=sql/database.py --exclude=tools/InstallConfig.sh --progress -e "$SSH_COMMAND -p$PORT" ./$RELEASE_FILE_NAME/ $USER_NAME@$IP_STR:/home/$USER_NAME/release_new/;
	$SSH_COMMAND -p $PORT $USER_NAME@$IP_STR "source ~/.bash_profile;\
	cd ~;\
	mv ~/release $oldFileName;\
	mv ~/release_new ~/release
	cd ~/release;\
	chmod u+x tools/AutoTools.sh;\
	chmod u+x log.sh;\
	chmod u+x Run.sh;\
	./tools/AutoTools.sh updatedbconfig;\
	./tools/AutoTools.sh distrelease $PORT;\
	cd ~/release;\
	cd sql;\
	python dbtool.py update;\
	cd ..;\
	./Run.sh;\
	cd ..;\
	echo -n '更新时间：' > readme.txt;\
	date >> readme.txt;\
	echo '版本文件：$RELEASE_FILE_NAME' >> readme.txt"
	echo "服务器版本发布完成";
	END_SEC=`date +%s`
	TOTAL_SEC=$(($((END_SEC)) - $((START_SEC))))
	echo "共用时$TOTAL_SEC秒"
}

function releaseAdmin()
{
	USER_NAME=$1
	IP_STR=$2
	PORT=$3
	RELEASE_FILE_NAME=$4
	cd admin;
	if [ ! -e $RELEASE_FILE_NAME ]
	then
		echo "$RELEASE_FILE_NAME 文件不存在!!!!!!!!"
		exit
	fi
	echo "文件传输可能需要一段时间，请耐心等待"
	START_SEC=`date +%s`
	$SSH_COMMAND -p $PORT $USER_NAME@$IP_STR "source ~/.bash_profile;\
	cd ~;\
	cp -rf release release_new;"
	rsync -ravtzp --delete --exclude=AdminServer/AdminConfig.xml --exclude=sql/gencode.py --exclude=sql/database.py --progress -e "$SSH_COMMAND -p$PORT" ./$RELEASE_FILE_NAME/ $USER_NAME@$IP_STR:/home/$USER_NAME/release_new/;
	oldFileName="release_old_`date +%y_%m_%d_%H_%M_%S`";
	$SSH_COMMAND -p $PORT $USER_NAME@$IP_STR "source ~/.bash_profile;\
	cd ~;\
	mv ~/release $oldFileName;\
	mv ~/release_new ~/release
	cd ~/release;\
	chmod u+x Run.sh;\
	mkdir ~/log;
	cd sql;\
	python dbtool.py updateadmin;\
	cd ~/release;
	./Run.sh admin;\
	cd ..;\
	echo -n '更新时间：' > readme.txt;\
	date >> readme.txt;\
	echo '版本文件：$RELEASE_FILE_NAME' >> readme.txt"
	echo "服务器版本发布完成";
	END_SEC=`date +%s`
	TOTAL_SEC=$(($((END_SEC)) - $((START_SEC))))
	echo "共用时$TOTAL_SEC秒"
}

function releasePlatform()
{
	USER_NAME=$1
	IP_STR=$2
	PORT=$3
	RELEASE_FILE_NAME=$4
	cd platform;
	if [ ! -e $RELEASE_FILE_NAME ]
	then
		echo "$RELEASE_FILE_NAME 文件不存在!!!!!!!!"
		exit
	fi
	echo "文件传输可能需要一段时间，请耐心等待"
	START_SEC=`date +%s`
	$SSH_COMMAND -p $PORT $USER_NAME@$IP_STR "source ~/.bash_profile;\
	cd ~;\
	cp -rf release release_new;"
	rsync -ravtzp --delete --exclude=PlatformServer/PlatformConfig.xml --exclude=sql/gencode.py --exclude=sql/database.py --exclude=tools/AutoRun.sh --exclude=checkDBConn.sh --progress -e "$SSH_COMMAND -p$PORT" ./$RELEASE_FILE_NAME/ $USER_NAME@$IP_STR:/home/$USER_NAME/release_new/;
	oldFileName="release_old_`date +%y_%m_%d_%H_%M_%S`";
	$SSH_COMMAND -p $PORT $USER_NAME@$IP_STR "source ~/.bash_profile;\
	cd ~;\
	mv ~/release $oldFileName;\
	mv ~/release_new ~/release
	cd ~/release;\
	chmod u+x Run.sh;\
	mkdir ~/log;
	cd sql;\
	python dbtool.py updateplatform;\
	cd ~/release;
	./Run.sh platform;\
	cd ..;\
	echo -n '更新时间：' > readme.txt;\
	date >> readme.txt;\
	echo '版本文件：$RELEASE_FILE_NAME' >> readme.txt"
	echo "服务器版本发布完成";
	END_SEC=`date +%s`
	TOTAL_SEC=$(($((END_SEC)) - $((START_SEC))))
	echo "共用时$TOTAL_SEC秒"
}

function releaseMonitor()
{
	USER_NAME=$1
	IP_STR=$2
	PORT=$3
	RELEASE_FILE_NAME=$4
	cd monitor;
	if [ ! -e $RELEASE_FILE_NAME ]
	then
		echo "$RELEASE_FILE_NAME 文件不存在!!!!!!!!"
		exit
	fi
	echo "文件传输可能需要一段时间，请耐心等待"
	START_SEC=`date +%s`
	$SSH_COMMAND -p $PORT $USER_NAME@$IP_STR "source ~/.bash_profile;\
	cd ~;\
	cp -rf release release_new;"
	rsync -ravtzp --delete --exclude=MonitorServer/MonitorConfig.xml --progress -e "$SSH_COMMAND -p$PORT" ./$RELEASE_FILE_NAME/ $USER_NAME@$IP_STR:/home/$USER_NAME/release_new/;
	oldFileName="release_old_`date +%y_%m_%d_%H_%M_%S`";
	$SSH_COMMAND -p $PORT $USER_NAME@$IP_STR "source ~/.bash_profile;\
	cd ~;\
	mv ~/release $oldFileName;\
	mv ~/release_new ~/release
	cd ~/release;\
	chmod u+x Run.sh;\
	mkdir ~/log;
	./Run.sh monitor;\
	cd ..;\
	echo -n '更新时间：' > readme.txt;\
	date >> readme.txt;\
	echo '版本文件：$RELEASE_FILE_NAME' >> readme.txt"
	echo "服务器版本发布完成";
	END_SEC=`date +%s`
	TOTAL_SEC=$(($((END_SEC)) - $((START_SEC))))
	echo "共用时$TOTAL_SEC秒"
}

function releaseClient()
{
	USER_NAME=$1
	IP_STR=$2
	PORT=$3
	RELEASE_FILE_NAME=$4
	DEST_DIR_NAME=$5
	if [ ! -e $RELEASE_FILE_NAME ]
	then
		echo "$RELEASE_FILE_NAME 文件不存在!!!!!!!!"
		exit
	fi
	rm -rf ClientRes_new;
	unzip $RELEASE_FILE_NAME;
	mv `echo $RELEASE_FILE_NAME | sed s/.zip//` ClientRes_new;
	$SSH_COMMAND -p $PORT $USER_NAME@$IP_STR "cd /data/www/wwwroot/ddt/$DEST_DIR_NAME/;\
    mkdir ClientRes;\
    rm -rf ClientRes_new;\
	cp -rf ClientRes ClientRes_new;"
	rsync -ravtzp --delete --progress -e "$SSH_COMMAND -p$PORT" ./ClientRes_new/ $USER_NAME@$IP_STR:/data/www/wwwroot/ddt/$DEST_DIR_NAME/ClientRes_new/;
	rm -rf ./ClientRes_new;
	$SSH_COMMAND -p $PORT $USER_NAME@$IP_STR "cd /data/www/wwwroot/ddt/$DEST_DIR_NAME/;\
	rm -rf ClientRes_old;\
	mv ClientRes_back ClientRes_old;\
	mv ClientRes ClientRes_back;\
	mv ClientRes_new ClientRes;\
	cd ..;\
	echo -n '更新时间：' > version.txt;\
	date >> version.txt;\
	echo '版本文件：$RELEASE_FILE_NAME' >> version.txt"
	echo "客户端资源发布完成";
}

function commitTXVersion()
{
	USER_NAME=$1
	IP_STR=$2
	PORT=$3
	RELEASE_FILE_NAME=$4
	VERSION_DIR=$5
	BRANCH=$6
	cd $VERSION_DIR;
	if [ ! -e $RELEASE_FILE_NAME ]
	then
		echo "$RELEASE_FILE_NAME 文件不存在!!!!!!!!"
		exit
	fi
	oldFileName="release_old_`date +%y_%m_%d_%H_%M_%S`";
	START_SEC=`date +%s`
	$SSH_COMMAND -p $PORT $USER_NAME@$IP_STR "source ~/.bash_profile;\
	cd /data/server/branch;\
	cd $BRANCH;\
	svn up;"
	rsync -ravtzp --delete --force --delete-before --progress --exclude=.svn/ --exclude=Run.sh --exclude=AutoRun.sh --exclude=dist.sh --exclude=zl.txt --exclude=zm.py --exclude=dayData.sh --exclude=uploadLogData.sh -e "$SSH_COMMAND -p$PORT" ./$RELEASE_FILE_NAME/ $USER_NAME@$IP_STR:/data/server/branch/$BRANCH/ 2>&1 | tee rsync.log;
	RM_DIR_LIST=`grep "cannot delete non-empty directory:" rsync.log | awk -F' ' '{print $5}'`
	for dir in $RM_DIR_LIST
	do
		$SSH_COMMAND -p $PORT $USER_NAME@$IP_STR "source ~/.bash_profile;\
		cd /data/server/branch;\
		cd $BRANCH;\
		rm -rf $dir;"
	done
	$SSH_COMMAND -p $PORT $USER_NAME@$IP_STR "source ~/.bash_profile;\
	cd /data/server/branch/;\
	chmod 755 autosvn.sh;\
	./autosvn.sh $BRANCH $RELEASE_FILE_NAME"
	echo "版本$RELEASE_FILE_NAME已经上传到svn服务器"
}

case "$1" in
	"installzone")
		source ./InstallConfig.sh;
		installZone
		initZone
	;;
	"initzone")
		source tools/InstallConfig.sh;
		initZone
	;;
	"updatedbconfig")
		source ./tools/InstallConfig.sh;
		updateDatabaseConfig
	;;
	"zoneconfig")
		source ./tools/InstallConfig.sh;
		updateZoneConfig
	;;
	"rebuildversion")
		source ./tools/InstallConfig.sh;
		rebuildVersion
	;;
	"checkcore")
		checkCore $2 $3
	;;
	"checkassert")
		checkAssertLog $2
	;;
	"backupcore")
		backupCore
	;;
	"rebuildrelease")
		rebuildRelease 2>&1 | tee server/build.log
	;;
	"rebuildadmin")
		rebuildAdmin 2>&1 | tee server/build.log
	;;
	"rebuildplatform")
		rebuildPlatform 2>&1 | tee server/build.log
	;;
	"rebuildmonitor")
		rebuildMonitor 2>&1 | tee server/build.log
	;;
	"releasezone")
		releaseZone $2 $3 $4 $5 2>&1 | tee server/release.log
	;;
	"releaseclient")
		releaseClient $2 $3 $4 $5 $6 2>&1 | tee release.log
	;;
	"releaseadmin")
		releaseAdmin $2 $3 $4 $5 2>&1 | tee server/release.log
	;;
	"releaseplatform")
		releasePlatform $2 $3 $4 $5 2>&1 | tee server/release.log
	;;
	"releasemonitor")
		releaseMonitor $2 $3 $4 $5 2>&1 | tee server/release.log
	;;
	"distrelease")
		distRelease $2
	;;
	"distconfig")
		distConfig $2
	;;
	"showdistconfig")
		showDistConfig $2
	;;
	"showqqdistconfig")
		showQQDistConfig $2
	;;
	"getlocalip")
		getLocalIP
	;;
	"buildofficalversion")
		buildOfficalVersion
	;;
	"committxversion")
		commitTXVersion $2 $3 $4 $5 $6 $7
	;;
esac
