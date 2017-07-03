#!/bin/sh
#name:AutoUpload (自动打包上传)

#要打包的文件所在的目录
LOG_DIR=~/warDataLog
#备份的地址
BACK_UP_DIR=~/backupWarDataLog/
#要发送到平台的地址
PLATFROM_URL=xxx@118.26.201.230::xxx
source ~/server/tools/InstallConfig.sh

# 如果目录为空则创建目录
if [ ! -d $LOG_DIR ]
then
    mkdir $LOG_DIR
fi
cd $LOG_DIR
# 打包的文件名
PACKAGE_FILE_NAME="doudoutang`date +%s`-$zoneNum.tar"

ALL_DATA=`find . -name "*csv.*" | awk -F"/" '{print $2}'`
for line in $ALL_DATA
do
    #判断该文件是否被系统使用
    /usr/sbin/lsof $line
    #echo "$IF_USED" 打印是否被使用
    if [ $? -eq 0 ]
    then
        echo "该文件正在被使用不能打包"
    else
        chmod +x $line
        tar -uvPf $PACKAGE_FILE_NAME $line
        mv $line $BACK_UP_DIR
    fi
done

# 备份文件
if [ ! -d $BACK_UP_DIR ]
then
    mkdir $BACK_UP_DIR
fi
cp $PACKAGE_FILE_NAME $BACK_UP_DIR 

# 开始传输打包文件
rsync -avz $PACKAGE_FILE_NAME $PLATFROM_URL
if [ $? != 0 ]
then
    echo "传输失败"
else
    echo "传输成功"
    rm $PACKAGE_FILE_NAME
fi
