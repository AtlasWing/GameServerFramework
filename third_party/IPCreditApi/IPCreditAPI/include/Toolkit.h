#ifndef TOOLKIT
#define TOOLKIT

#include <string>
using namespace std;

/*struct IPInfo
{
	uint32_t country;
	uint32_t province;
	uint32_t city;
	uint32_t district;
	uint32_t extend;
	uint32_t oper;
	uint32_t nettype;
	unsigned char apn;
	unsigned char flable;
};*/


class Toolkit
{
public:
	
	//本机字节序到字符串的转换
	static void Ip2Str(uint32_t ip,char str[]);


	//字符串到D段IP的转换
	static unsigned s2ipD(const std::string& sip); 


	//字符串到C段IP的转换
	static unsigned s2ipC(const std::string& sip); 

	//字符串到B段IP的转换
	static unsigned s2ipB(const std::string& sip); 
};
#endif
