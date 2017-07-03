#ifndef _N_ENCODING_H_
#define _N_ENCODING_H_

#include "base/Tidy.h"

namespace Tidy
{
	/// @brief	从fromEncode转换为toEncode，支持的编码iconv -l查看
	class EncodingConvert
	{
		public:
			DISALLOW_COPY_AND_ASSIGN(EncodingConvert);
			EncodingConvert(const char* fromEncode, const char* toEncode);
			~EncodingConvert();
			/// @brief	编码转换
			int convert(char* src, size_t srcLen, char* dest, size_t destLen);
		private:
			/// @brief	转换句柄
			iconv_t conv;
	};

	/// @brief	封装编码转换
	class nEncoding
	{
		public:
			/// @brief	编码转换
			static int UTF8ToGB2312(char *src, size_t srclen, char *dest, size_t destlen) 
			{
				EncodingConvert gb2312ToUtf8("utf-8", "gb2312");
				return gb2312ToUtf8.convert(src, srclen, dest, destlen);
			}
			static int UTF8ToGB2312(const char *src, size_t srclen, char *dest, size_t destlen) 
			{
				EncodingConvert gb2312ToUtf8("utf-8", "gb2312");
				return gb2312ToUtf8.convert(const_cast<char*>(src), srclen, dest, destlen);
			}
			/// @brief	编码转换
			static int GB2312ToUTF8(char *src, size_t srclen, char *dest, size_t destlen) 
			{
				EncodingConvert utf8ToGB2312("gb2312", "utf-8");
				return utf8ToGB2312.convert(src, srclen, dest, destlen);
			}
			static int GB2312ToUTF8(const char *src, size_t srclen, char *dest, size_t destlen) 
			{
				EncodingConvert utf8ToGB2312("gb2312", "utf-8");
				return utf8ToGB2312.convert(const_cast<char*>(src), srclen, dest, destlen);
			}

			static void GB2312ToUTF8( const std::string& src, std::string& dst )
			{
				char buffer[src.size() << 1];
                memset(buffer, 0, src.size() << 1);
				GB2312ToUTF8( src.c_str(), src.size(), buffer, src.size() << 1 );
				dst.append( buffer );
			}

			static void UTF8ToGB2312( const std::string& src, std::string& dst )
			{
				char buffer[src.size() << 1];
                memset(buffer, 0, src.size() << 1);
				UTF8ToGB2312( src.c_str(), src.size(), buffer, src.size() << 1 );
				dst.append( buffer );
			}

		public:
			// 把Message格式转化成lua的表的数据
			static bool toLua(const google::protobuf::Message &message, std::string *buffer, const DWORD indentLevel=0);
			static bool toLua(const char *fileName, std::string *buffer);

		public:
			/// 以下为处理URL转码的函数
			/// @brief 转为16进制
			static BYTE toHex(const BYTE &x);
			/// @brief 转回字符串
			static BYTE fromHex(const BYTE &x);
			/// @brief 转为URL编码
			static std::string urlEncode(const char *sIn, const DWORD len);
			/// @brief 将URL编码转回字符串
			static std::string urlDecode(const char *sIn, const DWORD len);
			static bool isNumber(const char* cstring);

		private:
			//以下函数是toLua(const char*,std::string*)的辅助函数
			static DWORD getCommandNumbers(const TiXmlElement* rootElementNode, const TiXmlElement* currentElementNode, bool isDisplayName);
			static std::string getLuaTableString(const TiXmlElement* rootElementNode, const TiXmlElement* currentElementNode, bool isDisplayName);
			static bool isExistSameNameElementNode(const TiXmlElement* rootElementNode, const TiXmlElement* currentElementNode);
			static bool isFirstElementChildNode(const TiXmlElement* rootElementNode, const TiXmlElement* currentElementNode);
			static void getBaseAttributeAndElementChildNode(const TiXmlElement* rootElementNode, const TiXmlElement* currentElementNode, std::string& result, const DWORD& size);
			static DWORD getAutoIncrementNum(const TiXmlElement* rootElementNode, const TiXmlElement* currentElementNode);
	};
};
#endif
