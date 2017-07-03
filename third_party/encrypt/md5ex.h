#ifndef _MD5EX_H
#define _MD5EX_H

class Stream;
bool MD5Data(const void* pData,int size,unsigned char *pMD5 /* 16 Byte*/);
bool MD5Stream(Stream* pStream,unsigned char *pMD5 /* 16 Byte*/);
bool MD5File(const char* pszFile,unsigned char *pMD5 /* 16 Byte*/);
bool MD5String(const char* string,unsigned char* pMD5 /* 16 Byte*/);

#endif

