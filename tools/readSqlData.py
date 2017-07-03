#!/usr/bin/python
# -*- coding: utf-8 -*-

import struct 
import os 
import sys 

def updateData(fileName) :
	fileHandle = open(fileName, 'r+b')
	inputSqlListIndex = fileHandle.read(1)
	inputSqlListIndex = struct.unpack('B', inputSqlListIndex)[0]
	ouputSqlListIndex = (inputSqlListIndex + 1) % 2
	print "ouputSqlListIndex=%u" % (ouputSqlListIndex)
	fileSize = os.path.getsize(fileName)
	print "fileSize=%u" % fileSize
	listHeadFormat = '=2I'
	listHeadFormatSize = struct.calcsize(listHeadFormat)
	sqlDataHeadFormat = '=B6IQI'
	sqlDataHeadFormatSize = struct.calcsize(sqlDataHeadFormat)
	binaryDataFormat = 'I'
	binaryDataFormatSize = struct.calcsize(binaryDataFormat)
	for i in range(2):
		fileHandle.seek(1 + ouputSqlListIndex * (fileSize-1)/2)
		ouputSqlListIndex = (ouputSqlListIndex + 1)%2

		listHead = fileHandle.read(listHeadFormatSize)
		listHead = struct.unpack(listHeadFormat, listHead)
		dataStartPos = listHead[0]
		dataEndPos = listHead[1]
		print("dataStartPos=%u dataEndPos=%u" % (dataStartPos, dataEndPos))
		while dataStartPos < dataEndPos:
			fileHandle.seek(dataStartPos, 1)
			binaryDataLen = fileHandle.read(binaryDataFormatSize)
			binaryDataLen = struct.unpack(binaryDataFormat, binaryDataLen)[0]
			sqlBinaryData = fileHandle.read(binaryDataLen)
			sqlDataFormat = sqlDataHeadFormat + str(binaryDataLen - sqlDataHeadFormatSize) + 's'
			sqlData = struct.unpack(sqlDataFormat, sqlBinaryData)
			print("version=%u id=%u connectionID=%u sqlLen=%u sql=%s" % (sqlData[0], sqlData[1], sqlData[2], sqlData[8], sqlData[9]))
			sql = sqlData[9]
			assert(sqlData[8] == 0 or len(sqlData[9]) == sqlData[8])
			# 在这里加条件判断删除sql或者修改sql
			if sql == "condition" :
				overwriteData = list(sqlData)
				# 把sql长度置为0会不执行这个sql
				overwriteData[8] = 0
				#可以修改sql内容,比如: overwriteData[9] = UPDATE... overwriteData[8]=len(overwriteData[9])
				overwriteData = struct.pack(sqlDataFormat, *overwriteData)
				fileHandle.seek(-binaryDataLen, 1)
				fileHandle.write(overwriteData)
			dataStartPos = dataStartPos + binaryDataFormatSize + binaryDataLen

if __name__ == "__main__":
	if len(sys.argv) != 2:
		print "指定文件名"
	else :
		fileName = sys.argv[1]
		updateData(fileName)
