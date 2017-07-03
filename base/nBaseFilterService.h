#ifndef BASE_NBASEFILTERSERVICE_H
#define BASE_NBASEFILTERSERVICE_H

#include "base/nSTL.h"
#include "base/nRWLock.h"

namespace Tidy
{
	class nBaseFilterService
	{
		public:
			nBaseFilterService();
			~nBaseFilterService();

		public:
			bool initService(const char *filterDataFile, const DWORD groupKeyStrSizeInKB);
			void finalService();
			bool reloadData(const char *filterDataFile, const DWORD groupKeyStrSizeInKB);

		public:
			bool checkFilter(const char *str);

		private:
			bool loadData(const char *filterDataFile, const DWORD groupKeyStrSizeInKB);
			bool escapeKeyStr(std::string &str);

		private:
			Tidy::vector<regex_t> _regex_vec;
			bool _is_compiled;
			// 关键字分组的长度限制(K),值越大,效率越高,内存消耗越大,reg_complie越耗时
			DWORD _group_key_str_size_in_kb;
			nRWLock _regex_lock;
#if 0
			Tidy::map<DWORD, Tidy::string> _debug_regex_str_map;
#endif
	};
};
#endif
