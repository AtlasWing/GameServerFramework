#include "base/nBaseFilterService.h"
#include "base/nStringTool.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	nBaseFilterService::nBaseFilterService():
		_is_compiled(false)
	{
	}

	nBaseFilterService::~nBaseFilterService()
	{
	}

	bool nBaseFilterService::initService(const char *filterDataFile, const DWORD groupKeyStrSizeInKB)
	{
		return loadData(filterDataFile, groupKeyStrSizeInKB);
	}

	bool nBaseFilterService::loadData(const char *filterDataFile, const DWORD groupKeyStrSizeInKB)
	{
		std::ifstream fileStream;
		fileStream.open(filterDataFile);
		if(fileStream.fail() == true){
			assert_fail("打开文件失败");
			return false;
		}
		_group_key_str_size_in_kb = groupKeyStrSizeInKB;
		const DWORD revervedStrSize = (_group_key_str_size_in_kb + 1) * 1024;
		std::string regexStr;
		regexStr.reserve(revervedStrSize);
		regex_t regex;

		char keyWord[1024];
		std::string keyStr;

		int result = REG_NOERROR;
		Tidy::vector<regex_t> regexVec;
		do{
			fileStream.getline(keyWord, sizeof(keyWord));
			while(fileStream.eof() == false && fileStream.fail() == false){
				keyStr.assign(keyWord);
				escapeKeyStr(keyStr);
				if(keyStr.empty() == false){
					if(regexStr.empty() == false){
						regexStr.append("|(");
					}
					else{
						regexStr.append("(");
					}
					regexStr.append(keyStr);
					regexStr.append(")");
					if(regexStr.size() > (revervedStrSize - 1024)){
						break;
					}
				}
				fileStream.getline(keyWord, sizeof(keyWord));
			}

			if(regexStr.empty() == false){
				result = regcomp(&regex, regexStr.c_str(), REG_EXTENDED|REG_NEWLINE);
				if(result != REG_NOERROR){
					char errorInfo[1024];
					regerror(result, &regex, errorInfo, sizeof(errorInfo));
					baseServer->debug("[%d %s]", result, errorInfo);
					assert_fail("表达式错误");
					return false;
				}
				regexVec.push_back(regex);
#if 0
				_debug_regex_str_map[regexVec.size() - 1] = regexStr;
#endif
				regexStr.clear();
			}
			if(fileStream.eof() == true || fileStream.fail() == true){
				baseServer->debug("[%s]", keyWord);
			}
		}while(fileStream.eof() == false && fileStream.fail() == false);

		fileStream.close();

		_regex_lock.wrlock();
		regexVec.swap(_regex_vec);
		_regex_lock.unlock();
		baseServer->debug("屏蔽库[%s] 生成正则数[%lu] ", filterDataFile, regexVec.size());
		if(_is_compiled == true){
			for(DWORD i = 0; i < regexVec.size(); ++ i){
				regfree(&regexVec[i]);
			}
		}
		else{
			_is_compiled = true;
		}

		return true;
	}

	void nBaseFilterService::finalService()
	{
		assert_debug(_is_compiled == true, "没有编译正则");
		for(DWORD i = 0; i < _regex_vec.size(); ++ i){
			regfree(&_regex_vec[i]);
		}
	}

	bool nBaseFilterService::reloadData(const char *filterDataFile, const DWORD groupKeyStrSizeInKB)
	{
		return loadData(filterDataFile, groupKeyStrSizeInKB);
	}

	bool nBaseFilterService::checkFilter(const char *str)
	{
		nScopeRDLock rdLock(_regex_lock);
		int result = 0;
		for(DWORD i = 0; i < _regex_vec.size(); ++ i){
			result = regexec(&_regex_vec[i], str, 0, NULL, 0);
			if(result != REG_NOMATCH){
#if 0
				Tidy::string &str = _debug_regex_str_map[i];
				baseServer->debug("match str=%s", str.c_str());
#endif
				return true;
			}
		}

		return false;
	}

	bool nBaseFilterService::escapeKeyStr(std::string &str)
	{
		if(str.empty() == true){
			return true;
		}
		char escapeCharList[] = {'+', '?', '.', '*', ')', '(', '[', ']', '{', '}', '|', '\\', '^', '$'};
		char *destBuffer = reinterpret_cast<char*>(alloca(str.length() * 2 + 1));
		DWORD size = 0;
		const char *orgData = str.c_str();
		for(DWORD i = 0; i < str.length(); ++ i){
			if(orgData[i] == ' ' || orgData[i] == '\r' || orgData[i] == '\n'){
				continue;
			}
			for(DWORD j = 0; j < sizeof(escapeCharList); ++ j){
				if(escapeCharList[j] == orgData[i]){
					destBuffer[size] = '\\';
					size += 1;
					break;
				}
			}
			destBuffer[size] = orgData[i];
			++ size;
		}
		assert_debug(size <= (str.length() * 2 + 1), "缓存区长度不足");

		str.assign(destBuffer, size);
		return true;
	}
};
