#include "base/Tidy.h"

#include "base/Imp/nArgp.h"

namespace Tidy
{
	/// @brief 参数BUG报告地址
	const char *argp_program_bug_address = "<atlas.youlong@gmail.com>";

	/// @brief 默认参数描述
	static const char navy_args_doc[] = "";
	/// @brief 默认参数描述
	static const char default_doc[] = "this is default argument document.";
	/// @brief 默认可用选项
	static struct argp_option default_options[] =
	{
		{0,	0,	0,	0,	0, 0}
	};
	/// @brief 默认的分析函数,参见info argp_parse;
	error_t parse_opt(int key, char *arg, struct argp_state *state)
	{
		switch (key)
		{
			default:
				if(nArgp::getInstance()._user_parser!=0)
					return nArgp::getInstance()._user_parser(key,arg,state);
				else
					return ARGP_ERR_UNKNOWN;
		}
		return 0;
	}

	nArgp::nArgp()
	{
		_user_parser    = 0;
		_all_options    = 0;

		_argp.children    = 0;
		_argp.help_filter = 0;
		_argp.argp_domain = 0;

		_argp.parser    = parse_opt;
		_argp.args_doc  = navy_args_doc;
		_argp.doc       = default_doc;
		addOptions(default_options);
	}

	nArgp::~nArgp()
	{
		SAFE_DELETE_VEC(_all_options);
	}

	void nArgp::addOptions(const struct argp_option *options)
	{
		if(options==0) return;

		int ucount=0;
		while(options[ucount].name!=0)
			ucount++;

		if(_all_options==0)
		{
			_all_options=new struct argp_option[ucount+1];
			memcpy(_all_options,options,sizeof(argp_option)*(ucount+1));

		}
		else
		{
			int ocount=0;
			while(_all_options[ocount].name!=0)
				ocount++;

			struct argp_option *otemp=_all_options;
			_all_options = new argp_option[ucount+ocount+1];
			if (ocount > 0) 
				memcpy(_all_options,otemp,sizeof(argp_option)*ocount);

			memcpy(_all_options+ocount,options,sizeof(argp_option)*(ucount+1));
			SAFE_DELETE_VEC(otemp);
		}
		_argp.options=_all_options;
	}

	bool nArgp::add(const struct argp_option *options,argsParser func,const char *args_doc,const char *doc)
	{
		if(func!=0)
			_user_parser=func;
		if(options!=0)
			addOptions(options);
		if(args_doc!=0)
			_argp.args_doc=args_doc;
		if(doc!=0)
			_argp.doc=doc;
		return true;
	}

	bool nArgp::parse(int argc ,char *argv[])
	{
		return argp_parse(&_argp, argc, argv, 0, 0, 0) == 0;
	}
};
