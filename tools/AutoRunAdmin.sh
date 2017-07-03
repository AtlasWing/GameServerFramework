#!/bin/sh

#set -x

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

function mainLoop()
{
while true
do
	#手动重启时禁用自动重启
	COUNT=`ps x | grep "/Run\.sh" | grep -v grep | wc -l`
	if [ $COUNT -gt 0 ]
	then
		sleep 1;
		continue;
	fi

    cd ~/release;
	RUNNING_COUNT=`ps x | grep "AdminServer/AdminServer" | sed -e '/make/d' -e '/CMakeFiles/d' -e '/grep/d' | wc -l`
	if [ $RUNNING_COUNT -lt 1 ]
	then
		echo "AdminServer/AdminServer -d"
		./AdminServer/AdminServer -d
	fi
	sleep 5
done
}

mainLoop
