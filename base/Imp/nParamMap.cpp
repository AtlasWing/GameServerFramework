#include "base/nParamMap.h"

namespace Tidy
{
	nParamMap::nParamMap()
	{
	}

	const nAny& nParamMap::get(const std::string &key)
	{
		return _param_map[key];
	}

	void nParamMap::set(const std::string &key, const nAny &value)
	{
		_param_map[key] = value;
	}

	bool nParamMap::find(const std::string &key)
	{
		ParamMap_ConstIter it = _param_map.find(key);
		if (it != _param_map.end()){
			return true;
		}
		return false;
	}

	const nAny& nParamMap::operator[] (const std::string &key) const
	{
		static nAny nullData;
		ParamMap_ConstIter iter = _param_map.find(key);
		if(iter == _param_map.end()){
			return nullData;
		}
		return iter->second;
	}

	void nParamMap::dump(std::ostream &out) const
	{
		for(ParamMap_ConstIter it = _param_map.begin(); it != _param_map.end(); it++)
			out << it->first << " = " << it->second.getCString() << std::endl;
	}

	DWORD nParamMap::parseCmdLine(const std::string &cmdLine)
	{
		std::vector<std::string> sv;
		nStringTool::split_white_space(cmdLine,sv);
		std::vector<std::string> ssv;
		for(std::vector<std::string>::const_iterator it = sv.begin(); it != sv.end(); ++it)
		{
			ssv.clear();
			nStringTool::split(*it,'=',ssv,2);
			if (ssv.size() == 2)
			{
				_param_map[ssv[0]] = ssv[1];
			}
		}
		return _param_map.size();
	}

	DWORD nParamMap::parseCmdLine(const char *cmdLine)
	{
		if(cmdLine[0] == '\0')
		{
			return 0;
		}
		std::vector<std::string> sv;
		nStringTool::split_white_space(cmdLine,sv);
		for(std::vector<std::string>::const_iterator it = sv.begin(); it != sv.end(); ++it)
		{
			std::vector<std::string> ssv;
			nStringTool::split(*it,'=',ssv,2);
			if (ssv.size() == 2)
			{
				_param_map[ssv[0]] = ssv[1];
			}
		}
		return _param_map.size();
	}

	DWORD nParamMap::parse(const std::string &str, const std::string::value_type outSeparator, const std::string::value_type internalSeparator)
	{
		std::vector< std::vector<std::string> > valueList;
		nStringTool::splitValue(str, outSeparator, internalSeparator, valueList);
		for(DWORD i = 0; i < valueList.size(); ++ i){
			if(valueList[i].size() == 2){
				_param_map.insert(ParamMap::value_type(valueList[i][0], nAny(valueList[i][1])));
			}
			else{
				// 即使为空,也要加进去key,因为有依赖于key的逻辑,比如充值去签名计算
				_param_map.insert(ParamMap::value_type(valueList[i][0], nAny()));
			}

		}
		return _param_map.size();
	}

	void nParamMap::clear()
	{
		_param_map.clear();
	}

	namespace{
		struct ConcatExec{
			std::string _result;
			const char _separator;
			ConcatExec(const char separator) : _separator(separator){
			}
			bool exec(const std::string &key, const nAny &value){
				_result.append(key);
				_result.push_back('=');
				_result.append(value.getCString());
				_result.push_back(_separator);
				return true;
			}
		};
	};
	std::string nParamMap::concat(const char separator) const
	{
		ConcatExec exec(separator);
		execAllData(exec);
		if(exec._result.empty() == false){
			exec._result.erase(exec._result.size() - 1);
		}
		return exec._result;
	}

	void nParamMap::erase(const std::string &key)
	{
		_param_map.erase(key);
	}

	void nParamMap::swap(nParamMap &paramMap)
	{
		_param_map.swap(paramMap._param_map);
	}
};
