#include "base/nZlib.h"
#include "base/nZip.h"
#include "base/nByteBuffer.h"

namespace Tidy
{
	DWORD getMaxZipBufferSize(const DWORD len)
	{
		DWORD zlibSize = (DWORD)unzip_size(len);
		DWORD snappySize = snappy::MaxCompressedLength(len);
		if(zlibSize > snappySize){
			return zlibSize;
		}
		return snappySize;
	}

	bool zip(eZipMethod method, const char *inputData, const size_t inputLen, char *outputData, size_t *outputLen)
	{
		if(method == eZipMethod_None){
			if(inputLen > (*outputLen)){
				assert_fail("[%u, %u]", (*outputLen), inputLen);
				return false;
			}
			memcpy(outputData, inputData, inputLen);
			*outputLen = inputLen;
			return true;
		}
		else if(method == eZipMethod_ZLIB){
			if((*outputLen) < (DWORD)(unzip_size(inputLen))){
				assert_fail("[%u, %u]", (*outputLen), (DWORD)unzip_size(inputLen));
				return false;
			}
			uLong zipLen = *outputLen;
			int retCode = Tidy::zlib_compress((Bytef*)outputData, &zipLen, (const Bytef *)inputData, inputLen);
			if(retCode != Z_OK){
				assert_fail("压缩失败[%d]", retCode);
				return false;
			}
			*outputLen = zipLen;
			return true;
		}
		else if(method == eZipMethod_Snappy){
			if((*outputLen) < snappy::MaxCompressedLength(inputLen)){
				assert_fail("[%u, %u]", (*outputLen), snappy::MaxCompressedLength(inputLen));
				return false;
			}
			snappy::RawCompress(inputData, inputLen, outputData, outputLen);
			return true;
		}

		assert_fail("无法处理的压缩方式[%d]", method);
		return false;
	}

	bool unZip(eZipMethod method, const char *inputData, const size_t inputLen, char *outputData, size_t *outputLen)
	{
		if(method == eZipMethod_None){
			if(inputLen > (*outputLen)){
				assert_fail("[%u, %u]", (*outputLen), inputLen);
				return false;
			}
			memcpy(outputData, inputData, inputLen);
			*outputLen = inputLen;
			return true;
		}
		else if(method == eZipMethod_ZLIB){
			uLongf unzipLen = *outputLen;
			int retCode = zlib_uncompress((Bytef*)outputData, &unzipLen, (const Bytef*)inputData, inputLen);
			if(retCode != Z_OK && retCode != Z_STREAM_END){
				assert_fail("解压缩失败[%d]", retCode);
				return false;
			}
			*outputLen = unzipLen;
			return true;
		}
		else if(method == eZipMethod_Snappy){
			size_t unzipLen = 0;
			if(snappy::GetUncompressedLength(inputData, inputLen, &unzipLen) == false){
				return false;
			}
			if(unzipLen > (*outputLen)){
				assert_fail("缓冲区不足[%u,%u]", unzipLen, (*outputLen));
				return false;
			}
			snappy::RawUncompress(inputData, inputLen, outputData);
			*outputLen = unzipLen;
			return true;
		}

		assert_fail("无法处理的压缩方式[%d]", method);
		return false;
	}
};

