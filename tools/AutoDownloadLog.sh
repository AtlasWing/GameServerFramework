#!/bin/sh
#自动下载创建和登录数据的文件并放到相应天的文件夹里
#按日期存放的目录
DATA_DIR="`date +%Y-%m-%d -d -24hour`"
cd /data/www/wwwroot
echo `lftp -u app100714286,fkys6543 ftp-cvmgz00.opencloud.qq.com:53000 -e "mirror $DATA_DIR $DATA_DIR; quit"`
mv $DATA_DIR ./dataLog/
cd ./dataLog/$DATA_DIR
ALL_DATA=`find . -name "*csv" | awk -F"/" '{print $2}'`
for line in $ALL_DATA
do
	iconv -f UTF-8 -t cp936 $line >> "$line.a"
	rm $line
	mv "$line.a" $line
done

