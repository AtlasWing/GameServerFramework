#ifndef _NREGEX_H_
#define _NREGEX_H_

#include "base/nSys.h"

namespace Tidy
{
	/// @brief	正则表达式类，对regex进行了封装，对于正则表达式请参考man 7 regex.
	// 		本类支持子字符串匹配，但最多支持31个字串
	//			本类非线程安全
	class nRegex
	{
		public:
			/// @brief	自定义标记:支持多行匹配，默认不支持
			static const int REG_MULTILINE;
			/// @brief	自定义标记:默认标记
			static const int REG_DEFAULT;

			nRegex();
			~nRegex();

			/// @brief	正则表达式编译函数
			/// @param	regex 要编译的正则表达式 
			/// @param	flags 编译选项，目前支持#REG_MULTILINE,#REG_DEFAULT,如果你不知道用什么建议用默认值 
			/// @return	编译是否成功 
			bool compile(const char * regex,int flags = REG_DEFAULT);
			/// @brief	开始匹配字符串,在匹配前请保证已经正确编译了正则表达式#compile
			/// @param	str 要匹配的字符串
			/// @return	匹配是否成功 
			bool match(const char *str);
			/// @brief	开始匹配字符串,在匹配前请保证已经正确编译了正则表达式#compile
			//		  	正则表达本身非线程安全,但是这个函数可以多线程使用
			/// @param	str 要匹配的字符串
			/// @return	匹配是否成功 
			bool match_mt(const char *str);
			/// @brief	得到匹配的子字符串,在此之前请保证已经正确得进行匹配#match
			/// @param	str 得到的字符串将放入str中
			/// @param	sub 子字符串的位置。注意匹配的字符串位置为0，其他子字符串以此类推.最大值为31
			/// @return	返回str 
			std::string &getSub(std::string &str,int sub = 0);
			/// @brief	得到错误信息 
			/// @return	当进行#compile或#match时返回false,可以用此得到错误信息
			const std::string & getError();

		private:

			std::string _err_str;           ///< 错误信息存放处
			int _err_code;                  ///< 错误代码
			regex_t _preg;                  ///< 正则表达式句柄
			std::string _smatch;            ///< 要匹配的字符串
			bool _compiled;                 ///< 表达式是否已编译
			bool _matched;                  ///< 是否匹配 
			regmatch_t rgm[32];             ///< 子串匹配位置 


			static const int REG_FLAGS;     ///< 自定义错误代码:标记错误 
			static const int REG_COMP;      ///< 自定义错误代码:未编译错误
			static const int REG_UNKNOW;    ///< 自定义错误代码:未知错误
			static const int REG_MATCH;     ///< 自定义错误代码:未进行匹配错误
	};
};
#endif
