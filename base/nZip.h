#ifndef BASE_NZIP_H
#define BASE_NZIP_H

#include <snappy.h>

#include "base/nSys.h"
#include "base/nZlib.h"

namespace Tidy{
	// 压缩方法
	enum eZipMethod{
		eZipMethod_None	=	0,
		eZipMethod_ZLIB	=	1,
		eZipMethod_Snappy	=	2,
		eZipMethod_Max
	};
	extern DWORD getMaxZipBufferSize(const DWORD len);
	extern bool zip(eZipMethod method, const char *inputData, const size_t inputLen, char *output, size_t *outputLen);
	extern bool unZip(eZipMethod method, const char *inputData, const size_t inputLen, char *output, size_t *outputLen);
};
#endif

