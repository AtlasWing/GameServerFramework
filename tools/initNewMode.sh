#!/bin/bash
#set -x

TEMP_MODE_NAME="TemplateModeName"
TEMP_MODE_NAME_UPPER="`echo $TEMP_MODE_NAME| tr a-z A-Z`"

NEW_MODE_NAME="$1"
NEW_MODE_NAME_UPPER="`echo $NEW_MODE_NAME| tr a-z A-Z`"

TEMP_FILE_LIST="`find . ! -name "*.o" | grep $TEMP_MODE_NAME | sort | uniq`"
NEW_FILE_LIST="`find . ! -name "*.o" | grep $TEMP_MODE_NAME | sort | uniq | sed -e s/$TEMP_MODE_NAME/$NEW_MODE_NAME/g`"

if [ "$1" == "" ]
then
	echo "用法 ./tools/initNewMode.sh  [newModeName]"
	exit
fi

echo "新增文件"
for file in $TEMP_FILE_LIST
do
	newFileName="`echo $file | sed -e s/$TEMP_MODE_NAME/$NEW_MODE_NAME/g`"
	if [ -f $newFileName ]
	then
		echo "$newFileName文件已经存在.!!"
		continue
	fi
	cat $file | sed -e "s/$TEMP_MODE_NAME/$NEW_MODE_NAME/g" -e "s/$TEMP_MODE_NAME_UPPER/$NEW_MODE_NAME_UPPER/g" > $newFileName;
	echo "$newFileName"
done

echo "需要自己修改的文件"
FILE_LIST=`find . -name "*.h" -o -name "*.cpp" -o -name "*.pkg" -o -name "*.lua"  -o -name "*.proto" | grep -v $TEMP_MODE_NAME | grep -v ".pb.h" | xargs grep -n $TEMP_MODE_NAME | grep -v "lgScript.cpp" | grep -v ".pb.h" | cut -d ":" -f 1,2 | sort | uniq`
for file in $FILE_LIST
do
	echo "$file"
done
echo "需要自己添加相关地图文件data/map下:*servermap,*.xml,*.nav.xml"
echo "详情参考tools/newGameMode.txt文档"
