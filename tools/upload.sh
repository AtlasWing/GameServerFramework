#!/bin/bash
set -x
FTP_SERVER_NAME=""
FTP_USER_NAME=""
FTP_USER_PASSWORD=""

CUR_DIR=`pwd`
TEMP_FILE_NAME=$CUR_DIR/temp.sh
OLD_FILE_NAME="ClientRes_back_`date +%y_%m_%d_%H_%M_%S`"

echo "cd $CUR_DIR"> $TEMP_FILE_NAME
echo "ftp -n $FTP_SERVER_NAME << ftp_cmd" >> $TEMP_FILE_NAME

echo "user $FTP_USER_NAME $FTP_USER_PASSWORD">> $TEMP_FILE_NAME
echo "bin">> $TEMP_FILE_NAME
echo "rename ClientRes_back $OLD_FILE_NAME" >> $TEMP_FILE_NAME
echo "rename ClientRes ClientRes_back" >> $TEMP_FILE_NAME

function uploadDir()
{
    local dirName="$1"
    echo "mkdir $dirName">> $TEMP_FILE_NAME
    echo "cd $dirName">> $TEMP_FILE_NAME
    echo "lcd $dirName">> $TEMP_FILE_NAME
    cd $dirName
    local FILE_LIST=`ls -l | grep -v '^d' | sed '1d' | awk -F' ' '{print $NF}'`
    for file in $FILE_LIST
    do
        echo "put $file">> $TEMP_FILE_NAME
    done 
    local DIR_LIST=`ls -l | grep '^d' | awk -F' ' '{print $NF}'`
    for dir in $DIR_LIST
    do
         uploadDir $dir
    done
    echo "cd ..">> $TEMP_FILE_NAME
    echo "lcd ../">> $TEMP_FILE_NAME
    cd ..
}

uploadDir ClientRes

echo "bye" >> $TEMP_FILE_NAME
echo "ftp_cmd" >> $TEMP_FILE_NAME
cd ..
chmod 700 $TEMP_FILE_NAME

/bin/bash $TEMP_FILE_NAME
rm -rf $TEMP_FILE_NAME
