#ifndef _STRING_TOOL_H_
#define _STRING_TOOL_H_

#include "base/nAny.h"

namespace Tidy
{
	/// @brief An extension of STL's string providing additional functionality that is often availiable in
	///		higher-level languages such as C#, php, Python.
	/// @ref 参考了ext_string实现：http://www.gotroot.ca/ext_string/
	class nStringTool
	{
		public:
			static const std::string WHITE_SPACE;			///< 空白字符 依次为: 空格, 水平制表符, 回车, 换行, 走纸换行, 垂直制表符
			static const int DEFAULT_BUFFER_SIZE = 1024;	///< vformat时默认的缓冲区大小
		public:
			/// @brief 字符串格式化
			static std::string& format(std::string& resultOUT, const char* fmt, ...);
			/// @brief 字符串格式化
			static std::string format(const char* fmt, ...);
			/// @brief 字符串格式化
			/// @param default_size 默认的缓冲区大小,如果该缓冲区大小不够用则会2倍增长直到缓冲区足够
			static std::string& vformat(std::string& resultOUT, const char *fmt, va_list argptr, int default_size = DEFAULT_BUFFER_SIZE);
			/// @brief 字符串格式化
			/// @param default_size 默认的缓冲区大小,如果该缓冲区大小不够用则会2倍增长直到缓冲区足够
			static inline std::string vformat(const char *fmt, va_list argptr, int default_size = DEFAULT_BUFFER_SIZE);

			/// @brief 类似C#的按照下标索引的字符串格式化
			static bool format_index(std::string& resultOUT, const char* fmt, const std::vector<std::string>& argList);
			/// @brief 类似C#的按照下标索引的字符串格式化
			static inline bool format_index(std::string& resultOUT, const char* fmt);
			/// @brief 类似C#的按照下标索引的字符串格式化
			template<typename Arg0>
				static bool format_index(std::string& resultOUT, const char* fmt, Arg0 arg0);
			/// @brief 类似C#的按照下标索引的字符串格式化
			template<typename Arg0, typename Arg1>
				static bool format_index(std::string& resultOUT, const char* fmt, Arg0 arg0, Arg1 arg1);
			/// @brief 类似C#的按照下标索引的字符串格式化
			template<typename Arg0, typename Arg1, typename Arg2>
				static bool format_index(std::string& resultOUT, const char* fmt, Arg0 arg0, Arg1 arg1, Arg2 arg2);
			/// @brief 类似C#的按照下标索引的字符串格式化
			template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
				static bool format_index(std::string& resultOUT, const char* fmt, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3);
			/// @brief 类似C#的按照下标索引的字符串格式化
			template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
				static bool format_index(std::string& resultOUT, const char* fmt, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4);
			/// @brief 类似C#的按照下标索引的字符串格式化
			template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
				static bool format_index(std::string& resultOUT, const char* fmt, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5);
			/// @brief 类似C#的按照下标索引的字符串格式化
			template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
				static bool format_index(std::string& resultOUT, const char* fmt, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6);
			/// @brief 类似C#的按照下标索引的字符串格式化
			template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
				static bool format_index(std::string& resultOUT, const char* fmt, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7);

			/// @brief 将数据转换成字符串格式
			static inline std::string& tostring(std::string& resultOUT, char c);
			/// @brief 将数据转换成字符串格式
			static inline std::string& tostring(std::string& resultOUT, const char* pstr);
			/// @brief 将数据转换成字符串格式
			static inline std::string& tostring(std::string& resultOUT, const std::string& str);
			/// @brief 将数据转换成字符串格式
			static inline std::string& tostring(std::string& resultOUT, bool value);
			/// @brief 将数据转换成字符串格式
			template<typename ValueType>
				static inline std::string& tostring(std::string& resultOUT, ValueType value);

			/// @brief 字符串追加
			/// @return str的引用
			static std::string& append(std::string& str, const char* fmt, ...);
			/// @brief 字符串追加
			/// @return str的引用
			static inline std::string& append(std::string& str, const std::string& cat);
			/// @brief 字符串追加
			/// @return str的引用
			static inline std::string& append(std::string& str, char c);
			/// @brief 字符串追加
			/// @return str的引用
			static inline std::string& append(std::string& str, bool value);
			/// @brief 字符串追加
			/// @return str的引用
			template<typename ValueType>
				static inline std::string& append(std::string& str, ValueType value);

			/// @brief 滤去字符串开头的空白
			static inline std::string trimbegin(std::string& str, const std::string& whiteSpace = WHITE_SPACE);
			/// @brief 滤去字符串结尾的空白
			static inline std::string trimend(std::string& str, const std::string& whiteSpace = WHITE_SPACE);
			/// @brief 滤去字符串两头的空白
			static inline std::string trim(std::string& str, const std::string& whiteSpace = WHITE_SPACE);

			/// @brief 判断字符串中是否包含给定的字符串
			static inline bool contains(const std::string& str, const std::string& tosearch);
			/// @brief 判断字符串中是否包含给定的字符串
			static inline bool contains(const std::string& str, const char* tosearch);
			/// @brief 判断字符串是否以给定的字符串开头
			static inline bool startwith(const std::string& str, const std::string& needle);
			/// @brief 判读字符串是否以给定的字符串结尾
			static inline bool endwith(const std::string& str, const std::string& needle);

			/// @brief 字符串左填充
			static inline std::string& padleft(std::string& str, std::string::size_type size, std::string::value_type paddingchar = ' ');
			/// @brief 字符串右填充
			static inline std::string& padright(std::string& str, std::string::size_type size, std::string::value_type paddingchar = ' ');

			/// @brief 对字符串容器去重
			/// @remark StringListContainer can be std::list<std::string> or std::vector<std::string>
			template <typename StringListContainerFrom, typename StringListContainerTo>
				static void unique(const StringListContainerFrom& strList, StringListContainerTo& resultOUT);
			/// @brief 对字符串容器去重
			/// @remark StringListContainer can be std::list<std::string> or std::vector<std::string>
			template<typename StringListContainer>
				static void unique(StringListContainer& strList);

			/// @brief 将字符串转换为数字。
			/// @param str 要解析的字符串
			/// @param ret 解析的结果
			/// @param end 不为空时将存贮解析的终止位置，失败时将等于str指针的值
			/// @return 是否成功解析
			static inline bool try_parse_int(const char* str, int* ret, char** end = NULL);

			/// @brief 字符串分隔
			/// @param separator 分割字符（ASCII字符）
			template<typename OutContainer>
				static void split(const std::string& str, std::string::value_type separator, OutContainer& resultOUT, size_t resultLimit = (size_t)-1);
			/// @brief 字符串分隔
			/// @param separator 分割字符串（也可以为汉字）
			template<typename OutContainer>
				static void split(const std::string& str, const std::string& separator, OutContainer& resultOUT, size_t resultLimit = (size_t)-1);
			/// @brief 字符串分隔
			/// @param separatorList 分割字符串数组，其中从先向后依次分别可作为分隔符
			/// @remark StringListContainer can be std::list<std::string> or std::vector<std::string>
			template<typename StringListContainer,typename OutContainer>
				static void split(const std::string& str, const StringListContainer& separatorList, OutContainer& resultOUT, size_t resultLimit = (size_t)-1);
			/// @brief 字符串分隔
			/// @param separator 分割字符（ASCII字符）
			template<typename OutContainer>
				static void split(const char* str, std::string::value_type separator, OutContainer& resultOUT, size_t resultLimit = (size_t)-1);
			/// @brief 将字符串按照空白分开，典型的空白包括" \t\r\n\f\v"（依次为: 空格, 水平制表符, 回车, 换行, 走纸换行, 垂直制表符）
			static void split_white_space(const std::string& str, std::vector<std::string>& resultOUT, size_t resultLimit = (size_t)-1);
			/// @brief 按照长度的字符串切分。切分后保证结果中每个子串的长度不大于给定长度，并能保证汉字不会被意外切碎
			/// @remark 是否切分成功
			static bool split_fix_size(const std::string& str, std::string::size_type fix_size, std::vector<std::string>& resultOUT);

			/// @brief 字符串分隔
			/// @param separator 分隔符
			/// @param resultOUT 类似vector<WORD>,提供push_back接口
			template<typename OutContainer>
				static void splitValue(const std::string& str, std::string::value_type separator, OutContainer& resultOUT, size_t resultLimit = static_cast<DWORD>(-1));
			/// @brief 两层字符串分隔
			/// @param outSeparator 外层分隔符
			/// @param internalSeparator 里层分隔符
			/// @param resultOUT 类似list< vector<WORD> >
			template<typename OutContainer>
				static void splitValue(const std::string& str, std::string::value_type outSeparator, std::string::value_type internalSeparator, OutContainer& resultOUT);
			/// @brief 字符串粘结
			template <class InputIterator>
				static inline std::string& join(const std::string &glue, InputIterator first, InputIterator last, std::string& resultOUT);
			/// @brief 字符串粘结
			template <class InputIterator>
				static inline std::string join(const std::string &glue, InputIterator first, InputIterator last);

			/// @brief 将字符串str中所有的from替换为to
			static std::string& replace(std::string& str, const std::string& from, const std::string& to);
			/// @brief 将字符串str中所有的from替换为to
			static std::string& replace(std::string& str, std::string::value_type from, std::string::value_type to);

			/// @brief 将字符串str中所有的to_remove删除，从前到后只扫描一遍
			static inline std::string& erase(std::string& str, const std::string& to_remove);

			/// @brief 将字符串转换为数字，从前向后尽可能多的转换
			static long int integer(const std::string& str);
			/// @brief 将字符串转换为数字，从前向后尽可能多的转换
			/// @return 返回是否全部被转换
			static bool integer(const std::string& str, long int& valueOut);

			/// @brief 重复字符串
			static std::string& repeat(const std::string& str, std::string::size_type times, std::string& resultOUT);
			/// @brief 重复字符串
			static inline std::string repeat(const std::string& str, std::string::size_type times);

			/// @brief 判断给定的字符串是否全部为小写字母
			static bool is_lower(const std::string& str);
			/// @brief 判断给定的字符是否是小写字母
			static inline bool is_lower(char c);
			/// @brief 判断给定的字符串是否全部为大写字母
			static bool is_upper(const std::string& str);
			/// @brief 判断给定的字符是否是大写字母
			static inline bool is_upper(char c);
			/// @brief 判断给定的字符串是否全部为字母
			static bool is_alpha(const std::string& str);
			/// @brief 判断给定的字符是否是字母
			static inline bool is_alpha(char c);
			/// @brief 判断给定的字符串是否全部为数字
			static bool is_numeric(const std::string& str);
			/// @brief 判断给定的字符是否为数字
			static inline bool is_numeric(char c);
			/// @brief 判断给定的字符串是否全为数字或字母
			static bool is_alnum(const std::string& str);
			/// @brief 判断给定的字符是否为数字或者字母
			static inline bool is_alnum(char c);
			/// @brief 判断给定的字符是否是空白字符
			static inline bool is_whitespace(char c);

			/// @brief 将字符串就地转换为小写
			static std::string& tolower(std::string& str);
			/// @brief 将字符串就地转换为小写
			static char* tolower(char* pstr);
			/// @brief 将字符转换为小写
			static inline char tolower(char c);
			/// @brief 将字符串就地转换为大写
			static std::string& toupper(std::string& str);
			/// @brief 将字符传唤为大写
			static inline char toupper(char c);
			/// @brief 就地颠倒给定字符串的大小写
			static std::string& swapcase(std::string& str);
			/// @brief 颠倒给定字符的大小写
			static inline char swapcase(char c);

			/// @brief 得到str中module出现的次数
			static std::string::size_type count(const std::string& str, const std::string &module);
			/// @brief 字符串比较函数
			static inline int compare(const char* str1, const char* str2);
			/// @brief 字符串比较函数
			static inline int compare(const std::string& str1, const char* str2);
			/// @brief 字符串比较函数
			static inline int compare(const std::string& str1, const std::string& str2);
			/// @brief 字符串比较函数
			static inline int compare(const char* str1, const std::string& str2);
			/// @brief 字符比较函数
			static inline int compare(char c1, char c2);
			/// @brief 字符比较函数
			static inline int compare(const std::string& str1, const char* c2, const size_t num);

			/// @brief 不区分大小写的比较函数
			static int icompare(const std::string& str1, const std::string& str2);
			/// @brief 不区分大小写的比较函数
			static inline int icompare(const char* str1, const char* str2);
			/// @brief 不区分大小写的字符比较函数
			static inline int icompare(char c1, char c2);

			/// @brief 字符串自然比较函数
			/// @remark http://sourcefrog.net/projects/natsort/
			static int compare_nature(const char *str1, const char *str2, bool case_sensitive = true);
			/// @brief 字符串自然比较函数
			static inline int compare_nature(const std::string& str1, const std::string& str2, bool case_sensitive = true);

			/// @brief 字符串通配符匹配判断
			/// @remark http://www.codeproject.com/KB/string/wildcmp.aspx
			static bool match_wildcard(const char* str, const char* wildcard);

			/// @brief 判断给定的字符串是否为空
			static inline bool empty(const char* pstr);
			/// @brief 判断给定的字符串是否为空
			static inline bool empty(const std::string& str);
			/// @brief 判断给定的字符是否为空
			static inline bool empty(char c);

			/// @brief 得到字符串的长度
			static inline size_t strlen(const char* pstr);
			/// @brief 得到字符串的长度
			static inline size_t strlen(const std::string& str);
			static inline bool trimEmpty(const char* str);

			/// @brief 判断字符串是否包含前缀prefix
			static inline bool hasPrefix(const std::string& str, const std::string& prefix);
			/// @brief 如果字符串包含前缀prefix则去掉
			static inline std::string stripPrefix(const std::string& str, const std::string& prefix);
			/// @brief 判断字符串是否包含后缀
			static inline bool hasSuffix(const std::string& str, const std::string& suffix);
			/// @brief 如果字符串包含后缀prefix则去掉
			static inline std::string stripSuffix(const std::string& str, const std::string& suffix);


			/// @brief 编辑距离
			/// @define (1) The smallest number of insertions, deletions, and substitutions required to change 
			///		one string or tree into another. (2) A Θ(m × n) algorithm to compute the distance between 
			///		strings, where m and n are the lengths of the strings
			/// @remark 参考	http://www.merriampark.com/ld.htm
			///					http://www.merriampark.com/ldcpp.htm 
			/// 
			/// @other Manhattan Distance 曼哈顿距离
			/// @define The distance between two points measured along axes at right angles. 
			///		In a plane with p1 at (x1, y1) and p2 at (x2, y2), it is |x1 - x2| + |y1 - y2|. 
			/// 
			/// @other Hamming Distance 汉明距离
			/// @define The number of bits which differ between two binary strings.
			///		More formally, the distance between two strings A and B is ∑ | Ai - Bi |. 
			/// 
			/// @other Euclidean Distance 欧几里德距离
			/// @define The straight line distance between two points. 
			///		In a plane with p1 at (x1, y1) and p2 at (x2, y2), it is √((x1 - x2)? + (y1 - y2)?). 
			static int LevenshteinDistance(const std::string& source, const std::string& target);
			static bool convertCharset(const char* inputCharset, const char *inputBuffer, size_t inputLen, const char* outputCharset, char* outputBuffer, size_t outputLen);
			static size_t copyStr(char* dest, const char* src, size_t num);

		private:
			nStringTool(){}
	};

	///< 实现函数------------------------------------------------------------------------------------------------------

	inline std::string nStringTool::vformat(const char *fmt, va_list argptr, int default_size/* = DEFAULT_BUFFER_SIZE */)
	{
		std::string resultOUT;
		return vformat(resultOUT, fmt, argptr, default_size);
	}
	inline bool nStringTool::format_index(std::string& resultOUT, const char* fmt)
	{
		return format_index(resultOUT, fmt, std::vector<std::string>());
	}
	template<typename Arg0>
		bool nStringTool::format_index(std::string& resultOUT, const char* fmt, Arg0 arg0)
		{
			std::vector<std::string> args(1);
			tostring(args[0], arg0);
			return format_index(resultOUT, fmt, args);
		}
	template<typename Arg0, typename Arg1>
		bool nStringTool::format_index(std::string& resultOUT, const char* fmt, Arg0 arg0, Arg1 arg1)
		{
			std::vector<std::string> args(2);
			tostring(args[0], arg0); tostring(args[1], arg1);
			return format_index(resultOUT, fmt, args);
		}
	template<typename Arg0, typename Arg1, typename Arg2>
		bool nStringTool::format_index(std::string& resultOUT, const char* fmt, Arg0 arg0, Arg1 arg1, Arg2 arg2)
		{
			std::vector<std::string> args(3);
			tostring(args[0], arg0); tostring(args[1], arg1); tostring(args[2], arg2);
			return format_index(resultOUT, fmt, args);
		}
	template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
		bool nStringTool::format_index(std::string& resultOUT, const char* fmt, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3)
		{
			std::vector<std::string> args(4);
			tostring(args[0], arg0); tostring(args[1], arg1); tostring(args[2], arg2); tostring(args[3], arg3); 
			return format_index(resultOUT, fmt, args);
		}
	template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		bool nStringTool::format_index(std::string& resultOUT, const char* fmt, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
		{
			std::vector<std::string> args(5);
			tostring(args[0], arg0); tostring(args[1], arg1); tostring(args[2], arg2); tostring(args[3], arg3); tostring(args[4], arg4);
			return format_index(resultOUT, fmt, args);
		}
	template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		bool nStringTool::format_index(std::string& resultOUT, const char* fmt, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
		{
			std::vector<std::string> args(6);
			tostring(args[0], arg0); tostring(args[1], arg1); tostring(args[2], arg2); tostring(args[3], arg3); tostring(args[4], arg4); tostring(args[5], arg5);
			return format_index(resultOUT, fmt, args);
		}
	template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
		bool nStringTool::format_index(std::string& resultOUT, const char* fmt, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6)
		{
			std::vector<std::string> args(7);
			tostring(args[0], arg0); tostring(args[1], arg1); tostring(args[2], arg2); tostring(args[3], arg3); tostring(args[4], arg4); tostring(args[5], arg5); tostring(args[6], arg6);
			return format_index(resultOUT, fmt, args);
		}
	template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
		bool nStringTool::format_index(std::string& resultOUT, const char* fmt, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7)
		{
			std::vector<std::string> args(8);
			tostring(args[0], arg0); tostring(args[1], arg1); tostring(args[2], arg2); tostring(args[3], arg3); tostring(args[4], arg4); tostring(args[5], arg5); tostring(args[6], arg6); tostring(args[7], arg7);
			return format_index(resultOUT, fmt, args);
		}

	inline std::string& nStringTool::tostring(std::string& resultOUT, char c)
	{
		resultOUT = c;
		return resultOUT;
	}
	inline std::string& nStringTool::tostring(std::string& resultOUT, const char* pstr)
	{
		resultOUT = pstr == NULL ? "" : pstr; 
		return resultOUT;
	}
	inline std::string& nStringTool::tostring(std::string& resultOUT, const std::string& str)
	{
		resultOUT = str; 
		return resultOUT;
	}
	inline std::string& nStringTool::tostring(std::string& resultOUT, bool value)
	{
		resultOUT = value ? "true" : "false"; 
		return resultOUT;
	}
	template<typename ValueType>
		inline std::string& nStringTool::tostring(std::string& resultOUT, ValueType value)
		{
			std::stringstream oss;
			oss << value;
			resultOUT = oss.str();
			return resultOUT;
		} 
	inline std::string& nStringTool::append(std::string& str, const std::string& cat)
	{
		str += cat;
		return str;
	}
	inline std::string& nStringTool::append(std::string& str, char c)
	{
		str += c;
		return str;
	}
	inline std::string& nStringTool::append(std::string& str, bool value)
	{
		str += value ? "true" : "false";
		return str;
	}
	template<typename ValueType>
		inline std::string& nStringTool::append(std::string& str, ValueType value)
		{
			std::stringstream oss;
			oss << value;
			str += oss.str();
			return str;
		}
	inline std::string nStringTool::trimbegin(std::string& str, const std::string& whiteSpace/* = WHITE_SPACE*/)
	{
		return str.erase(0,str.find_first_not_of(whiteSpace));
	}
	inline std::string nStringTool::trimend(std::string& str, const std::string& whiteSpace/* = WHITE_SPACE*/)
	{
		return str.erase(str.find_last_not_of(whiteSpace)+1);
	}
	inline std::string nStringTool::trim(std::string& str, const std::string& whiteSpace/* = WHITE_SPACE*/)
	{
		str.erase(str.find_last_not_of(whiteSpace)+1);
		return str.erase(0,str.find_first_not_of(whiteSpace));
	}
	inline bool nStringTool::contains(const std::string& str, const std::string& tosearch)
	{
		return str.find(tosearch) != std::string::npos;
	}
	inline bool nStringTool::contains(const std::string& str, const char* tosearch)
	{
		if(tosearch == NULL)
			return false;
		return str.find(tosearch) != std::string::npos;
	}
	inline bool nStringTool::startwith(const std::string& str, const std::string& needle)
	{
		if(str.size() < needle.size())
			return false;
		return str.compare(0, needle.size(), needle) == 0;
	}
	inline bool nStringTool::endwith(const std::string& str, const std::string& needle)
	{
		if(str.size() < needle.size())
			return false;
		return str.compare(str.size() - needle.size(), needle.size(), needle) == 0;
	}
	inline std::string& nStringTool::padleft(std::string& str, std::string::size_type size, std::string::value_type paddingchar/*  = ' '*/)
	{
		if(size > str.size())
			str.insert(0, size - str.size(), paddingchar);
		return str;
	}
	inline std::string& nStringTool::padright(std::string& str, std::string::size_type size, std::string::value_type paddingchar/*  = ' '*/)
	{
		if(size > str.size())
			str.resize(size, paddingchar);
		return str;
	}
	template <typename StringListContainerFrom, typename StringListContainerTo>
		void nStringTool::unique(const StringListContainerFrom& strList, StringListContainerTo& resultOUT)
		{
			resultOUT.clear();
			resultOUT.reserve(strList.size());
			for(typename StringListContainerFrom::const_iterator it_from = strList.begin();
					it_from != strList.end(); ++it_from)
			{
				bool repeat = false;
				for(typename StringListContainerFrom::const_iterator it_to = resultOUT.begin(); 
						it_to != resultOUT.end(); ++it_to)
				{
					if(it_from->compare(*it_to) == 0)
					{
						repeat = true;
						break;
					}
				}
				if(repeat == false)
					resultOUT.push_back(*it_from);
			}
		}
	template<typename StringListContainer>
		void nStringTool::unique(StringListContainer& strList)
		{
			StringListContainer result;
			nStringTool::unique(strList, result);
			if(strList.size() != result.size())
				strList.assign(result.begin(), result.end());
		}
	template<typename OutContainer>
		void nStringTool::split(const std::string& str, std::string::value_type separator, OutContainer& resultOUT, size_t resultLimit /* = -1*/)
		{
			resultOUT.clear();
			if(resultLimit == 0) 
				return;
			resultOUT.push_back(std::string());
			for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
			{
				if(*it == separator)
				{
					if(resultOUT.back().empty() == false)
					{
						if(resultOUT.size() == resultLimit) 
							return;
						resultOUT.push_back(std::string());
					}
				}
				else
				{
					resultOUT.back().push_back(*it);
				}
			}
			if(resultOUT.back().empty())
				resultOUT.pop_back();
			//assert_debug(resultOUT.size() <= resultLimit);
		}

	template<typename OutContainer>
		void nStringTool::splitValue(const std::string& str, std::string::value_type separator, OutContainer& resultOUT, size_t resultLimit /* = -1*/)
		{
			resultOUT.clear();
			std::string trimStr = str;
			trim(trimStr);
			std::vector<std::string> valueStrVec;
			split(trimStr, separator, valueStrVec);
			for(std::vector<std::string>::iterator iter = valueStrVec.begin(); iter != valueStrVec.end(); ++ iter)
			{
				resultOUT.push_back((typename OutContainer::value_type)(nAny(*iter)));
			}
		}

	template<typename OutContainer>
		void nStringTool::splitValue(const std::string& str, std::string::value_type outSeparator, std::string::value_type internalSeparator, OutContainer& resultOUT)
		{
			typedef typename OutContainer::value_type InternalContainerType;
			resultOUT.clear();
			std::string trimStr = str;
			trim(trimStr);
			std::vector<std::string> outStrVec;
			split(trimStr, outSeparator, outStrVec);
			for(std::vector<std::string>::iterator outIter = outStrVec.begin(); outIter != outStrVec.end(); ++ outIter)
			{
				std::vector<std::string> internalStrVec;
				split(*outIter, internalSeparator, internalStrVec);

				InternalContainerType internalContainer;
				for(std::vector<std::string>::iterator internalIter = internalStrVec.begin(); internalIter != internalStrVec.end(); ++ internalIter)
				{
					internalContainer.push_back((typename InternalContainerType::value_type)(nAny(*internalIter)));
				}
				resultOUT.push_back(internalContainer);
			}
		}

	template<typename OutContainer>
		void nStringTool::split(const std::string& str, const std::string& separator, OutContainer& resultOUT, size_t resultLimit/* = -1*/)
		{
			resultOUT.clear();
			if(resultLimit == 0) 
				return;
			if(separator.empty())
			{
				resultOUT.push_back(str);
				return;
			}
			std::string::size_type begin = 0, index;
			for(;;)
			{
				index = str.find(separator, begin);
				if(index == std::string::npos)
					break;
				size_t len = index - begin;
				if(len != 0)
				{
					resultOUT.push_back(str.substr(begin, len));
					if(resultOUT.size() == resultLimit)
						return;
				}
				begin = index + separator.size();
			}
			if(begin < str.size() && resultOUT.size() < resultLimit)
				resultOUT.push_back(str.substr(begin));
			//assert_debug(resultOUT.size() <= resultLimit);
		}
	template<typename StringListContainer,typename OutContainer>
		void nStringTool::split(const std::string& str, const StringListContainer& separatorList, OutContainer& resultOUT, size_t resultLimit /* = -1*/)
		{
			typedef std::vector<std::string> container;

			resultOUT.clear();
			if(resultLimit == 0) 
				return;
			// 分割模板去重
			std::vector<std::string> splitSeparatorList;
			nStringTool::unique(separatorList, splitSeparatorList);
			if(separatorList.empty())
			{
				resultOUT.push_back(str);
				return;
			}

			// 进行分割操作
			std::string::size_type begin = 0;
			for(std::string::size_type i = 0; i < str.size(); i++)
			{
				for(container::const_iterator it = splitSeparatorList.begin(); it != splitSeparatorList.end(); ++it)
				{
					if(it->compare(0, it->size(), str, i, it->size()) == 0)
					{
						if(i != begin)
						{
							resultOUT.push_back(str.substr(begin, i - begin));
							if(resultOUT.size() == resultLimit)
								return;
						}
						begin = i + it->size();
						i = begin - 1;// -1是为了抵消外层for循环后面的i++
						break;
					}
				}
			}
			if(begin < str.size() && resultOUT.size() < resultLimit)
				resultOUT.push_back(str.substr(begin));
			//assert_debug(resultOUT.size() <= resultLimit);
		}
	template<typename OutContainer>
		void nStringTool::split(const char* str, std::string::value_type separator, OutContainer& resultOUT, size_t resultLimit /* = -1*/)
		{
			std::string temp = str;
			nStringTool::split(temp, separator, resultOUT, resultLimit);
		}
	template <class InputIterator>
		inline std::string& nStringTool::join(const std::string &glue, InputIterator first, InputIterator last, std::string& resultOUT)
		{
			resultOUT.clear();
			if(first == last)
				return resultOUT;
			for (; first != last; first++)
			{
				resultOUT.append(*first);
				resultOUT.append(glue);
			}
			resultOUT.erase(resultOUT.size() - glue.size());
			return resultOUT;
		}
	template <class InputIterator>
		inline std::string nStringTool::join(const std::string &glue, InputIterator first, InputIterator last)
		{
			std::string resultOUT;
			return join(glue, first, last, resultOUT);
		}
	inline std::string& nStringTool::erase(std::string& str, const std::string& to_remove)
	{
		return replace(str, to_remove, "");
	}
	inline std::string nStringTool::repeat(const std::string& str, std::string::size_type times)
	{
		std::string resultOUT;
		return repeat(str, times, resultOUT);
	}
	inline bool nStringTool::is_lower(char c)
	{
		return c >= 'a' && c <= 'z';
	}
	inline bool nStringTool::is_upper(char c)
	{
		return c >= 'A' && c <= 'Z';
	}
	inline bool nStringTool::is_alpha(char c)
	{
		return is_lower(c) || is_upper(c);
	}
	inline bool nStringTool::is_numeric(char c)
	{
		return c >= '0' && c <= '9';
	}
	inline bool nStringTool::is_alnum(char c)
	{
		return is_alpha(c) || is_numeric(c);
	}
	inline bool nStringTool::is_whitespace(char c)
	{
		return WHITE_SPACE.find(c) != std::string::npos;
	}
	inline char nStringTool::tolower(char c)
	{
		return is_upper(c) ? c - 'A' + 'a' : c; 
	}
	inline char nStringTool::toupper(char c)
	{
		return is_lower(c) ? c - 'a' + 'A' : c;
	}
	inline char nStringTool::swapcase(char c)
	{
		if(is_upper(c)) 
			return c - 'A' + 'a';
		if(is_lower(c)) 
			return c - 'a' + 'A';
		return c;
	}
	inline int nStringTool::compare(const char* str1, const char* str2)
	{
		if(str1 == NULL)
		{
			if(str2 == NULL) 
				return 0; 
			else	
				return -1;
		}
		else
		{
			if(str2 == NULL) 
				return 1; 
			else 
				return strcmp(str1, str2);
		}
	}
	inline int nStringTool::compare(const std::string& str1, const char* str2)
	{
		return str1.compare(str2);
	}
	inline int nStringTool::compare(const std::string& str1, const std::string& str2)
	{
		return str1.compare(str2);
	}
	inline int nStringTool::compare(const char* str1, const std::string& str2)
	{
		return -compare(str2, str1);
	}
	inline int nStringTool::compare(char c1, char c2)
	{
		if(c1 > c2)
			return 1;
		else if(c1 < c2)
			return -1;
		else
			return 0;
	}
	inline int nStringTool::compare(const std::string& str1, const char* c2, const size_t num)
	{
		return str1.compare(0, str1.size(), c2, num);
	}
	inline int nStringTool::icompare(const char* str1, const char* str2)
	{
		if(str1 == str2)
			return 0;
		if(str1 == NULL)
			return -1;
		if(str2 == NULL)
			return 1;
//#ifdef unix
		return strcasecmp(str1, str2);
//#else
//		return _stricmp(str1, str2);
//#endif
	}
	inline int nStringTool::icompare(char c1, char c2)
	{
		if(c1 == c2)
			return 0;
		if(is_alpha(c1))
			c1 = tolower(c1);
		if(is_alpha(c2))
			c2 = tolower(c2);
		if(c1 > c2)
			return 1;
		else// if(c1 < c2)
			return -1;
	}
	inline int nStringTool::compare_nature(const std::string& str1, const std::string& str2, bool case_sensitive /* = true*/)
	{
		return compare_nature(str1.c_str(), str2.c_str(), case_sensitive);
	}

	inline bool nStringTool::empty(const char* pstr)
	{
		return pstr == NULL || pstr[0] == '\0';
	}
	inline bool nStringTool::empty(const std::string& str)
	{
		return str.empty();
	}
	inline bool nStringTool::empty(char c)
	{
		return c == '\0';
	}
	inline size_t nStringTool::strlen(const char* pstr)
	{
		return empty(pstr)? 0 : ::strlen(pstr);
	}
	inline size_t nStringTool::strlen(const std::string& str)
	{
		return str.size();
	}

	inline bool nStringTool::trimEmpty(const char* pstr)
	{
		std::string str(pstr);
		nStringTool::trim(str);
		return str.empty();
	}

	inline bool nStringTool::try_parse_int(const char* str, int* ret, char** end /* = NULL*/)
	{
		char* p = NULL;
		long int n = 0;
		if(str == NULL || ret == NULL)
			goto L_ErrorEnd;	
		n = strtol(str, &p, 0);
		if(n == LONG_MAX || n == LONG_MIN)
		{
			if(errno == ERANGE)
				goto L_ErrorEnd;
		}
		else if(n == 0)
		{
			if(p == str)
				goto L_ErrorEnd;
		}
		if(end != NULL)
			*end = p;
		*ret = n;
		return true;
L_ErrorEnd:
		if(end != NULL)
			*end = const_cast<char*>(str);
		if(ret != NULL)
			*ret = 0;
		return false;
	}
};

#endif
