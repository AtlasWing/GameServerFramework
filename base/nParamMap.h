#ifndef BASE_N_PARAM_MAP_H_
#define BASE_N_PARAM_MAP_H_

#include "base/nStringTool.h"
#include "base/nSTL.h"
#include "base/nAny.h"

namespace Tidy
{
	class nParamMap
	{
		public:
			nParamMap();

		public:
			const nAny &get(const std::string &key);
			void set(const std::string &key, const nAny &value);
			void erase(const std::string &key);
			bool find(const std::string &key);
			const nAny& operator[] (const std::string &key) const;
			void dump(std::ostream &out) const;
			/// @brief	解析名之对字串
			/// @param	cmdLine	需要解析的字串
			/// @return	解析到的名之对的个数
			DWORD parseCmdLine(const std::string &cmdLine);
			/// @brief	解析名之对字串
			/// @param	cmdLine	需要解析的字串
			/// @return	解析到的名之对的个数
			DWORD parseCmdLine(const char *cmdLine);
			DWORD parse(const std::string &str, const std::string::value_type outSeparator, const std::string::value_type internalSeparator);
			void clear();
			std::string concat(const char separator) const;
			void swap(nParamMap &paramMap);

		public:
			template<typename ExecT>
				bool execAllData(ExecT &exec) const;
			template<typename ExecT>
				bool execAllData(ExecT &exec);

		private:
			struct key_hash 
				: public std::unary_function<const std::string, size_t>
			{
				/// @brief	重载括号运算符
				/// @param	x 被转换的字符串
				/// @return	hash值
				size_t operator()(const std::string &x) const
				{
					//转化字符串为小写
					return _hash(x.c_str());
				}
				__gnu_cxx::hash<const char *> _hash;
			};

			/// @brief	判断两个字符串是否相等的函数
			struct key_equal : public std::binary_function<const std::string, const std::string, bool>
		{
			bool operator()(const std::string &s1, const std::string &s2) const
			{
				return strcasecmp(s1.c_str(), s2.c_str()) == 0;
			}
		};

		private:
			typedef std::map<std::string, nAny> ParamMap;
			typedef ParamMap::iterator ParamMap_Iter;
			typedef ParamMap::const_iterator ParamMap_ConstIter;
			ParamMap _param_map;
	};

	template<typename ExecT>
		bool nParamMap::execAllData(ExecT &exe) const
		{
			for(ParamMap_ConstIter iter = _param_map.begin(); iter != _param_map.end(); ++ iter){
				if(exe.exec(iter->first, iter->second) == false){
					return false;
				}
			}
			return true;
		}
	template<typename ExecT>
		bool nParamMap::execAllData(ExecT &exe)
		{
			for(ParamMap_Iter iter = _param_map.begin(); iter != _param_map.end(); ++ iter){
				if(exe.exec(iter->first, iter->second) == false){
					return false;
				}
			}
			return true;
		}
};

#endif
