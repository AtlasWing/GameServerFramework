#include "base/nRegex.h"

namespace Tidy
{
	const int nRegex::REG_UNKNOW(78325);
	const int nRegex::REG_FLAGS(78326);
	const int nRegex::REG_COMP(78327);
	const int nRegex::REG_MATCH(78328);
	const int nRegex::REG_MULTILINE(REG_NEWLINE);
	const int nRegex::REG_DEFAULT(0);

	nRegex::nRegex():
		_err_code(REG_UNKNOW),
		_compiled( false ),
		_matched( false )
	{
	}

	nRegex::~nRegex()
	{
		if(_compiled)
		{
			regfree(&_preg);
		}
	}

	bool nRegex::compile(const char * regex,int flags)
	{
		if(_compiled)
		{
			regfree(&_preg);
			_matched = false;
		}

		if(flags == REG_MULTILINE)
		{
			_err_code = regcomp(&_preg, regex, REG_EXTENDED);
		}
		else if(flags == REG_DEFAULT)
		{
			_err_code=regcomp(&_preg, regex, REG_EXTENDED|REG_NEWLINE);
		}
		else
			_err_code=REG_FLAGS;

		_compiled = (_err_code==0);
		return _compiled;
	}

	bool nRegex::match_mt(const char *s)
	{
		if(s==NULL) 
			return false;

		if(_compiled)
		{
			return (0 == regexec(&_preg,s,32,rgm,0));
		}
		return REG_COMP;
	}

	bool nRegex::match(const char *s)
	{
		if(s==NULL) 
			return false;
		_smatch = s;

		if(_compiled)
		{
			_err_code = regexec(&_preg, s, 32, rgm, 0);
		}
		else
			_err_code = REG_COMP;

		_matched = (_err_code==0);
		return _matched;
	}

	std::string &nRegex::getSub(std::string &s,int sub)
	{
		if(_matched)
		{
			if(sub<32 && sub >= 0 && rgm[sub].rm_so!=-1)
			{
				s = std::string(_smatch,rgm[sub].rm_so,rgm[sub].rm_eo-rgm[sub].rm_so);
			}
			else
				s="";
		}
		else
			_err_code=REG_MATCH;

		return s;
	}

	const std::string & nRegex::getError()
	{
		if(_err_code == REG_UNKNOW)
			_err_str = "unknow error";
		else if(_err_code == REG_FLAGS)
			_err_str = "flags error";
		else if(_err_code == REG_COMP)
			_err_str = "uncompiled error";
		else if(_err_code == REG_MATCH)
			_err_str = "unmatched error";
		else
		{
			char temp[1024];
			regerror(_err_code,&_preg,temp,1023);
			_err_str = temp;
		}
		return _err_str;
	}
};
