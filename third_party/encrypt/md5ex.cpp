#ifdef _USE_ENGINE
//#include "../../engine/include/engine.h"
#endif

#include "md5c.h"
#include "md5ex.h"
#include <stdio.h>
#include <string.h>

bool MD5Data(const void* pData,size_t size,unsigned char *pMD5 /* 16 Byte*/)
{
	MD5_CTX context;
	MD5Init (&context);
	MD5Update (&context, (unsigned char*)pData, size);
	MD5Final (pMD5, &context);
	return true;
}

#ifdef _USE_ENGINE

bool MD5Stream(Stream* pStream,unsigned char *pMD5 /* 16 Byte*/)
{
	MD5_CTX context;
	unsigned char buffer[4096];
	size_t size = pStream->getStreamSize();
	MD5Init (&context);
	while(size)
	{
		size_t readSize = getMin(size,(size_t)4096);
		if(!pStream->read(buffer,readSize))
		{
			MD5Final (pMD5, &context);
			return false;
		}
		MD5Update (&context, buffer, readSize);
	}
	MD5Final (pMD5, &context);
	return true;
}

bool MD5File(const char* pszFile,unsigned char *pMD5 /* 16 Byte*/)
{
	FileStream stream;
	if(!stream.open(pszFile,FileStream::Read))
		return false;
	return MD5Stream(&stream,pMD5);
}

#else

bool MD5File(const char* pszFile,unsigned char *pMD5 /* 16 Byte*/)
{
	FILE* fp = fopen(pszFile,"rb");
	if(!fp) return false;

	unsigned char buffer[8192];
	size_t len;
	fseek(fp,0,SEEK_END);
	len = ftell(fp);
	fseek(fp,0,SEEK_SET);

	MD5_CTX context;
	MD5Init (&context);
	while( len )
	{
		size_t readLen = (len < 8192 ? len : 8192);
		if(1 != fread(buffer,readLen,1,fp))
		{
			MD5Final (pMD5, &context);
			return false;
		}
		len -= readLen;
		MD5Update (&context, buffer, readLen);
	}
	MD5Final (pMD5, &context);
	return true;
}

#endif

bool MD5String(const char* string,unsigned char* pMD5 /* 16 Byte*/)
{
	size_t size = strlen(string);
	return MD5Data(string,size,pMD5);
}

