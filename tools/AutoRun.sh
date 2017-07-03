#!/bin/sh

#set -x
#kill already run AutoRun.sh
#pkill AutoRun.sh -u `whoami`

SSH_USERNAME=$USER
SSH_PORT=59878

#禁止启动多个自动启动脚本
tempPid=$$
tempName=`basename $0`
tempOperType=`ps x | awk -v p=$tempPid -v n=$tempName '$1==p {print $(NF-1)}'`
ps x | awk -v p=$tempPid -v n=$tempName -v o=$tempOperType '$1!=p && $(NF-1)==o && $NF~n {system("kill -9 "$1)}'

ulimit -c unlimited

PRG="$0"
execDir=`dirname "$PRG"`
cd $execDir
cd ..

LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

#确定配置中使用的本地IP
LOCAL_IP_STR=`./tools/AutoTools.sh getlocalip`
#本机服务器去重
LOCAL_SERVER_LIST=`grep Server config/ZoneConfig.xml | grep "type=" | grep "name=" | grep "ip=\"$LOCAL_IP_STR\"" | awk -F'"' '{print $6}' | sort | uniq`
DIST_IP_LIST=`grep Server config/ZoneConfig.xml | grep "name=" | grep -v "ip=\"$LOCAL_IP_STR\"" | awk -F'"' '{print $8}' | sort | uniq`
LOCAL_IP_STR_LIST=`grep "ip=\"$LOCAL_IP_STR\"" config/ZoneConfig.xml`

function mainLoop()
{
while test -f tools/AutoRun.sh
do
    PROGRESS_LIST=`ps x`
	#手动重启时禁用自动重启
	COUNT=`echo "$PROGRESS_LIST" | grep "/Run\.sh" | grep -v grep | wc -l`
	if [ $COUNT -gt 0 ]
	then
		sleep 1;
		continue;
	fi

	for server in $LOCAL_SERVER_LIST
	do
		SERVER_COUNT=`echo "$LOCAL_IP_STR_LIST" | grep "name=\"$server\"" | wc -l`
		RUNNING_COUNT=`echo "$PROGRESS_LIST" | grep "$server/$server -id=" | sed -e '/make/d' -e '/CMakeFiles/d' -e '/grep/d' | wc -l`
		if [ $RUNNING_COUNT -lt $SERVER_COUNT ]
		then
			echo "./$server/$server -d"
			./$server/$server -d
		fi
	done
    COUNT=`ls -l |grep "core*" | wc -l`
	if [ $COUNT -ge 10 ]
	then
        break
	fi
	sleep 5
done
}

function dist()
{
	IS_ZONE_SERVER=false
	for server in $LOCAL_SERVER_LIST
	do
		if [ "$server" == "ZoneServer" ]
		then
			IS_ZONE_SERVER=true
		fi
	done
	if [ "$IS_ZONE_SERVER" == "true" ]
	then
		for ipstr in $DIST_IP_LIST
		do
			ssh -p $SSH_PORT $SSH_USERNAME@$ipstr "cd ~/release; (./tools/AutoRun.sh &) >/dev/null </dev/null 2>&1;"
		done
	fi
}

dist
mainLoop
