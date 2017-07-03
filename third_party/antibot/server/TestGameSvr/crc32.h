#ifndef __CRC1_H__
#define __CRC1_H__

//#include "stdafx.h" 

class CCrc32
{
public:
	CCrc32();
	~CCrc32();
    static unsigned int Crc32(unsigned char *buf, unsigned int nsize);
    static void CalcCrc32(unsigned char byte, unsigned int &dwCrc32);
private:
};


#endif