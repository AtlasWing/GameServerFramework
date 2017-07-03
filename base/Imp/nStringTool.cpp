#include "base/nStringTool.h"

#include "base/Tidy.h"
// 干掉win下对max和min宏的定义
#undef max
#undef min

namespace Tidy
{
	const std::string nStringTool::WHITE_SPACE = " \t\r\n\f\v";	///< 空白字符 依次为: 空格, 水平制表符, 回车, 换行, 走纸换行, 垂直制表符

	std::string& nStringTool::format(std::string& resultOUT, const char* fmt, ...)
	{
		va_list vl;
		va_start(vl, fmt);
		vformat(resultOUT, fmt, vl);
		va_end(vl);
		return resultOUT;
	}
	std::string nStringTool::format(const char* fmt, ...)
	{
		va_list vl;
		va_start(vl, fmt);
		std::string resultOUT;
		vformat(resultOUT, fmt, vl);
		va_end(vl);
		return resultOUT;
	}
	std::string& nStringTool::vformat(std::string& resultOUT, const char *fmt, va_list argptr, int default_size /*= DEFAULT_BUFFER_SIZE*/)
	{
		std::vector<char> buffer(default_size, 0);
		for(;;)
		{
			int len = vsnprintf(&(buffer[0]), buffer.size(), fmt, argptr);
/*#ifdef unix
#else
			// MSDN: formats and writes up to count characters of the given data to the memory
			// pointed to by buffer and appends a terminating null.
			//int len = vsnprintf_s(&(buffer[0]), buffer.size(), buffer.size() - 1, fmt, argptr);
#endif
*/
			if(len == -1)
			{
				buffer.resize(buffer.size() * 2, 0);
				continue;
			}
			if(len == static_cast<int>(buffer.size()))
				buffer.push_back(0);
			break;
		}
		resultOUT.assign(&(buffer[0]));
		return resultOUT;
	}
	bool nStringTool::format_index(std::string& resultOUT, const char* fmt, const std::vector<std::string>& argList)
	{
		// 解析状态机的状态
		enum FormatState
		{
			FS_NORMAL,
			FS_R_BRACKE,
			FS_L_BRACKE,
			FS_NUMBER,
		};
#ifndef unix
#pragma warning(push)
#pragma warning(disable: 4127) // C4127: 条件表达式是常数
#endif

		// 出错处理代码
#ifdef _DEBUG
#define FS_ERROR_MESSAGE(msg) \
		do{\
			resultOUT = (msg);\
			return false;\
		} while(0)
#else
#define FS_ERROR_MESSAGE(msg) \
		do{\
			resultOUT.clear();\
			return false;\
		} while(0)
#endif

		// 预处理
		resultOUT.clear();
		if(fmt == NULL || fmt[0] == 0)
			return true;
		// 正式解析
		const char* p = fmt;
		FormatState state = FS_NORMAL;
		std::string index;
		for(;;)
		{
			const char c = *p++;
			if(c == 0)
			{
				if(state == FS_NORMAL)
					break; // OK 算法执行完毕
				else
					FS_ERROR_MESSAGE("ERROR！格式化字符末尾可能有残缺，不能正确解析");
			}
			switch(state)
			{
				case FS_NORMAL:
					switch(c)
					{
						case '}':
							state = FS_R_BRACKE;
							break;
						case '{':
							state = FS_L_BRACKE;
							break;
						default:
							resultOUT += c;
							break;
					}
					break;
				case FS_R_BRACKE:
					if(c == '}')
					{
						state = FS_NORMAL;
						resultOUT += '}';
					}
					else
					{
						FS_ERROR_MESSAGE("ERROR！在格式化字符串中发现不成对的\'}\'字符");
					}
					break;
				case FS_L_BRACKE:
					if(c == '{')
					{
						resultOUT += '}';
						state = FS_NORMAL;
					}
					else if(is_numeric(c))
					{
						index = c;
						state = FS_NUMBER;
					}
					else
					{
						FS_ERROR_MESSAGE("ERROR！在格式化字符串中发现\'{\'后的非数字字符");
					}
					break;
				case FS_NUMBER:
					if(c == '}')
					{
						long int idx = 0;
						if(integer(index, idx))
						{
							if(idx >= 0 && static_cast<size_t>(idx) < argList.size())
							{
								resultOUT += argList[idx];
								state = FS_NORMAL;
							}
							else
							{
								FS_ERROR_MESSAGE("ERROR！在格式化字符串中的参数下标超出参数列表范围");
							}
						}
						else
						{
							FS_ERROR_MESSAGE("ERROR！在格式化字符串中的参数下标不能正确转换为数字");
						}
					}
					else if(is_numeric(c))
					{
						index += c;
					}
					else
					{
						FS_ERROR_MESSAGE("ERROR！在格式化字符串中发现\'{\'后的非数字字符");
					}
					break;
				default:
					assert_debug(false, "");//所有状态签名已经包含，程序不应该执行到此处
					break;
			}
		}
		return true;

#undef FS_ERROR_MESSAGE

#ifndef unix
#pragma warning(pop)
#endif
	}
	std::string& nStringTool::append(std::string& str, const char* fmt, ...)
	{
		va_list vl;
		va_start(vl, fmt);
		std::string temp;
		vformat(temp, fmt, vl);
		va_end(vl);
		str += temp;
		return str;
	}

	void nStringTool::split_white_space(const std::string& str, std::vector<std::string>& resultOUT, size_t resultLimit/* = -1*/)
	{
		std::vector<std::string> white_space;
		white_space.push_back(" ");
		white_space.push_back("\t");
		white_space.push_back("\r");
		white_space.push_back("\n");
		white_space.push_back("\f");
		white_space.push_back("\v");
		split(str, white_space, resultOUT, resultLimit);
	}
	bool nStringTool::split_fix_size(const std::string& str, std::string::size_type fix_size, std::vector<std::string>& resultOUT)
	{
		resultOUT.clear();
		if(fix_size == 0)//长度为0时无法处理
			return false;
		else if(fix_size == 1)// 当长度为1时不能处理汉字
		{
			for(std::string::const_iterator iter = str.begin(); iter != str.end(); ++iter)
			{
				if(*iter < 0)
					return false;
			}
		}
		else if(fix_size >= str.size())//长度足够大时不必要处理
		{
			resultOUT.push_back(str);
			return true;
		}
		resultOUT.reserve(str.size() / fix_size + 1);

		bool fullword = true;// 是否是完整字的标志，用于处理完整的汉字
		std::string::const_iterator it = str.begin();
		std::string::const_iterator last = it;
		for (; it != str.end(); ++it)
		{
			if(*it >= 0)
				fullword = true;
			else
				fullword = !fullword;
			if (it - last == static_cast<int>(fix_size))
			{
				// 发现分界处有汉字断字现象则回退一个字节
				if(fullword && *it < 0)
				{
					--it;
					fullword = false;
				}
				resultOUT.push_back(std::string(last, it));
				last = it;
			}
		}

		if (last != it)
			resultOUT.push_back(std::string(last, it));
		return true;
	}
	std::string& nStringTool::replace(std::string& str, const std::string& from, const std::string& to)
	{
		if(from.empty())
			return str;
		std::string::size_type lastpos = 0, thispos;
		while ((thispos = str.find(from, lastpos)) != std::string::npos)
		{
			str.replace(thispos, from.size(), to);
			lastpos = thispos + to.size();
		}
		return str;
	}
	std::string& nStringTool::replace(std::string& str, std::string::value_type from, std::string::value_type to)
	{
		for (std::string::iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i == from)
			{
				*i = to;
			}
		}
		return str;
	}
	long int nStringTool::integer(const std::string& str)
	{
		long int retval = 0;
		integer(str, retval);
		return retval;
	}

	bool nStringTool::integer(const std::string& str, long int& valueOut)
	{
		valueOut = 0;
		if(str.empty())
			return false;

		bool neg = false;
		bool error = false;
		for (std::string::const_iterator i = str.begin(); i != str.end(); i++)
		{
			if (i == str.begin())
			{
				if (*i == '-')
				{
					neg = true;
					continue;
				}
				else if (*i == '+' || *i == ' ')
					continue;
			}
			if (*i >= '0' && *i <= '9')
			{
				valueOut *= 10;
				valueOut += *i - '0';
			}
			else
			{
				error = true;
				break;
			}
		}
		if(neg)
			valueOut = -valueOut;
		return !error;
	}

	std::string& nStringTool::repeat(const std::string& str, std::string::size_type times, std::string& resultOUT)
	{
		resultOUT.clear();
		for (std::string::size_type i = 0; i < times; i++)
		{
			resultOUT.append(str);
		}
		return resultOUT;
	}

	std::string& nStringTool::tolower(std::string& str)
	{
		for (std::string::iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i >= 'A' && *i <= 'Z')
				*i = (*i) + ('a' - 'A');
		}
		return str;
	}
	char* nStringTool::tolower(char* pstr)
	{
		if (!pstr)
			return pstr;
		char* s = pstr;
		for(; *s; ++s)
		{
			if (*s >= 'A' && *s <= 'Z')
				*s = (*s) + ('a' - 'A');
		}
		return pstr;
	}
	std::string& nStringTool::toupper(std::string& str)
	{
		for (std::string::iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i >= 'a' && *i <= 'z')
				*i = (*i) - ('a' - 'A');
		}
		return str;
	}
	std::string& nStringTool::swapcase(std::string& str)
	{
		for (std::string::iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i >= 'A' && *i <= 'Z')
				*i += ('a' - 'A');
			else if (*i >= 'a' && *i <= 'z')
				*i -= ('a' - 'A');
		}
		return str;
	}

	bool nStringTool::is_lower(const std::string& str)
	{
		if (str.empty())
			return false;

		for (std::string::const_iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i < 'a' || *i < 'z')
				return false;
		}

		return true;
	}

	bool nStringTool::is_upper(const std::string& str)
	{
		if (str.empty())
			return false;

		for (std::string::const_iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i < 'A' || *i > 'Z')
				return false;
		}

		return true;
	}

	bool nStringTool::is_alpha(const std::string& str)
	{
		if (str.empty())
			return false;

		for (std::string::const_iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i < 'A' || (*i > 'Z' && (*i < 'a' || *i > 'z')))
				return false;
		}
		return true;
	}

	bool nStringTool::is_numeric(const std::string& str)
	{
		if (str.empty())
			return false;

		for (std::string::const_iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i < '0' || *i > '9')
				return false;
		}
		return true;
	}
	bool nStringTool::is_alnum(const std::string& str)
	{
		if (str.empty())
			return false;

		for (std::string::const_iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i < 'A' || *i > 'Z')
				if (*i < '0' || *i > '9')
					if (*i < 'a' || *i > 'z')
						return false;
		}
		return true;
	}

	std::string::size_type nStringTool::count(const std::string& str, const std::string &module)
	{
		std::string::size_type count = 0, last = 0, cur = 0;

		while ((cur = str.find(module, last)) != std::string::npos)
		{
			count++;
			last = cur + module.size();
		}
		return count;
	}

	int nStringTool::icompare(const std::string& str1, const std::string& str2)
	{
		if(str1.data() == str2.data())
			return 0;
		int ret = (int)str1.size() - (int)str2.size();
		if(ret != 0)
			return ret;
		std::string::const_iterator it1 = str1.begin();
		std::string::const_iterator it2 = str2.begin();
		for(; it1 < str1.end()/* && it2 < str2.end()*/; ++it1, ++it2)
		{
			if(*it1 == *it2)
				continue;
			char c1 = *it1;
			char c2 = *it2;
			if(c1 >= 'A' && c1 <= 'Z')
			{
				if(c2 >= 'a' && c2 <= 'z')
				{
					if(c1 - 'A' == c2 - 'a')
						continue;
				}
			}
			else if(c1 >= 'a' && c1 <= 'z')
			{
				if(c2 >= 'A' && c2 <= 'Z')
				{
					if(c1 - 'a' == c2 - 'A')
						continue;
				}
			}
			return c1 - c2;
		}
		return 0;
	}

	namespace detail
	{
		static int compare_right(const char *a, const char *b)
		{
			int bias = 0;

			/* The longest run of digits wins.  That aside, the greatest
			   value wins, but we can't know that it will until we've scanned
			   both numbers to know that they have the same magnitude, so we
			   remember it in BIAS. */
			for (;; a++, b++) 
			{
				if (!nStringTool::is_numeric(*a)  &&  !nStringTool::is_numeric(*b))
					return bias;
				else if (!nStringTool::is_numeric(*a))
					return -1;
				else if (!nStringTool::is_numeric(*b))
					return +1;
				else if (*a < *b)
				{
					if (!bias)
						bias = -1;
				} 
				else if (*a > *b) 
				{
					if (!bias)
						bias = +1;
				}
				else if (!*a  &&  !*b)
					return bias;
			}
		}


		static int compare_left(const char *a, const char *b)
		{
			/* Compare two left-aligned numbers: the first to have a
			   different value wins. */
			for (;; a++, b++) 
			{
				if (!nStringTool::is_numeric(*a)  &&  !nStringTool::is_numeric(*b))
					return 0;
				else if (!nStringTool::is_numeric(*a))
					return -1;
				else if (!nStringTool::is_numeric(*b))
					return +1;
				else if (*a < *b)
					return -1;
				else if (*a > *b)
					return +1;
			}
		}
	}

	int compare_nature(const char *str1, const char *str2, bool case_sensitive /*= true*/) 
	{
		if(str1 == str2)
			return 0;
		if(str1 == NULL)
			return -1;
		else if(str2 == NULL)
			return 1;

		int ai(0), bi(0);
		char ca, cb;
		int fractional, result;
		for(;;)
		{
			ca = str1[ai]; cb = str2[bi];

			/* skip over leading spaces or zeros */
			while (nStringTool::is_whitespace(ca))
				ca = str1[++ai];

			while (nStringTool::is_whitespace(cb))
				cb = str2[++bi];

			/* process run of digits */
			if (nStringTool::is_numeric(ca)  &&  nStringTool::is_numeric(cb)) 
			{
				fractional = (ca == '0' || cb == '0');

				if (fractional) 
				{
					if ((result = detail::compare_left(str1+ai, str2+bi)) != 0)
						return result;
				}
				else 
				{
					if ((result = detail::compare_right(str1+ai, str2+bi)) != 0)
						return result;
				}
			}

			if (!ca && !cb)
			{
				/* The strings compare the same.  Perhaps the caller
				   will want to call strcmp to break the tie. */
				return 0;
			}
			if(!case_sensitive)
			{
				ca = toupper(ca);
				cb = toupper(cb);
			}
			if (ca < cb)
				return -1;
			else if (ca > cb)
				return +1;

			++ai; ++bi;
		}
	}
	bool nStringTool::match_wildcard(const char* str, const char* wildcard)
	{
		if(str == NULL || wildcard == NULL)
			return false;
		if(str == wildcard)
			return true;
		// Written by Jack Handy - jakkhandy@hotmail.com
		const char *cp = NULL, *mp = NULL;
		while ((*str) && (*wildcard != '*')) 
		{
			if ((*wildcard != *str) && (*wildcard != '?')) 
				return false;
			wildcard++;
			str++;
		}
		while (*str) 
		{
			if (*wildcard == '*') 
			{
				if (!*++wildcard) 
					return true;
				mp = wildcard;
				cp = str+1;
			} 
			else if ((*wildcard == *str) || (*wildcard == '?')) 
			{
				wildcard++;
				str++;
			}
			else 
			{
				wildcard = mp;
				str = cp++;
			}
		}

		while (*wildcard == '*') 
		{
			wildcard++;
		}
		return (*wildcard) ? false : true;
	}

	bool nStringTool::hasPrefix(const std::string& str, const std::string& prefix)
	{
		return str.size() >= prefix.size() &&
			str.compare(0, prefix.size(), prefix) == 0;
	}

	std::string nStringTool::stripPrefix(const std::string& str, const std::string& prefix)
	{
		if (hasPrefix(str, prefix)) 
		{
			return str.substr(prefix.size());
		} 
		else 
		{
			return str;
		}
	}

	bool nStringTool::hasSuffix(const std::string& str, const std::string& suffix)
	{
		return str.size() >= suffix.size() &&
			str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
	}

	std::string nStringTool::stripSuffix(const std::string& str, const std::string& suffix)
	{
		if (hasSuffix(str, suffix)) 
		{
			return str.substr(0, str.size() - suffix.size());
		} 
		else 
		{
			return str;
		}
	}

	int nStringTool::LevenshteinDistance(const std::string& source, const std::string& target) 
	{
		// Step 1
		const int n = static_cast<int>(source.length());
		const int m = static_cast<int>(target.length());
		if (n == 0) 
			return m;
		if (m == 0)
			return n;

		// Good form to declare a TYPEDEF
		typedef std::vector< std::vector<int> > Tmatrix; 

		Tmatrix matrix(n+1);

		// Size the vectors in the 2.nd dimension. Unfortunately C++ doesn't
		// allow for allocation on declaration of 2.nd dimension of vec of vec
		for (int i = 0; i <= n; i++) 
			matrix[i].resize(m+1);

		// Step 2
		for (int i = 0; i <= n; i++) 
			matrix[i][0]=i;
		for (int j = 0; j <= m; j++) 
			matrix[0][j]=j;

		// Step 3
		for (int i = 1; i <= n; i++) 
		{
			const char s_i = source[i-1];
			// Step 4
			for (int j = 1; j <= m; j++) 
			{
				const char t_j = target[j-1];
				// Step 5
				int cost;
				if (s_i == t_j) 
					cost = 0;
				else 
					cost = 1;

				// Step 6
				const int above = matrix[i-1][j];
				const int left = matrix[i][j-1];
				const int diag = matrix[i-1][j-1];
				int cell = std::min( above + 1, std::min(left + 1, diag + cost));

				// Step 6A: Cover transposition, in addition to deletion,
				// insertion and substitution. This step is taken from:
				// Berghel, Hal ; Roach, David : "An Extension of Ukkonen's 
				// Enhanced Dynamic Programming ASM Algorithm"
				// (http://www.acm.org/~hlb/publications/asm/asm.html)
				if (i>2 && j>2) 
				{
					int trans = matrix[i-2][j-2]+1;
					if (source[i-2]!=t_j) trans++;
					if (s_i!=target[j-2]) trans++;
					if (cell>trans) cell=trans;
				}
				matrix[i][j]=cell;
			}
		}
		// Step 7
		return matrix[n][m];
	}

	bool nStringTool::convertCharset(const char* inputCharset, const char *inputBuffer, size_t inputLen, const char* outputCharset, char* outputBuffer, size_t outputLen)
	{
		iconv_t handle = iconv_open(outputCharset, inputCharset);
		if(handle  == (iconv_t)-1)
		{
			return false;
		}
		if((int)iconv(handle, (char**)&inputBuffer,  &inputLen, &outputBuffer, &outputLen) == -1)
		{
			iconv_close(handle);
			return false;
		}
		if(inputLen != 0)
		{
			iconv_close(handle);
			return false;
		}

		iconv_close(handle);
		return true;
	}

	size_t nStringTool::copyStr(char* dest, const char* src, size_t num)
	{
		size_t i = 0;
		while(i < num && src[i] != '\0')
		{
			dest[i] = src[i];
			++ i;
		}
		if(i == num)
		{
			dest[num - 1] = '\0';
			return num;
		}
		else
		{
			dest[i] = '\0';
		}

		return i;
	}
};
