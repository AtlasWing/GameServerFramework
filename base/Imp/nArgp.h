#ifndef BASE_IMP_NARGPH_
#define BASE_IMP_NARGPH_

#include "base/nParamMap.h"
#include "base/nSingleton.h"

namespace Tidy
{
	/// @brief	参数分析函数定义
	typedef error_t(*argsParser)(int key, char *arg, struct argp_state *state) ;

	class nArgp : public nSingleton<nArgp>
	{
		private:
			friend error_t parse_opt(int, char *, struct argp_state *);
			friend class nSingleton<nArgp>;
			nArgp();
			~nArgp();

		public:
			/// @brief	添加自己的参数选项和分析器，及参数文档，如果省略用默认值
			/// @param	options 自己的参数选项
			/// @param	func 自己参数选项的分析函数
			/// @param	args_doc 参数选项详细文档
			/// @param	doc 必要参数选项文档
			/// @return	始终返回true
			bool add(const struct argp_option *options=0,argsParser func=0,const char *args_doc=0,const char *doc=0);
			/// @brief 添加参数选项
			/// @param options 要添加的参数选项数组
			void addOptions(const struct argp_option *options);
			/// @brief 参数分析
			/// @param argc 参数个数
			/// @param argv 参数列表
			/// @return 是否成功
			bool parse(int argc ,char *argv[]);

		protected:
			/// @brief	参数分析数据结构
			struct argp _argp;
			/// @brief	用户定义的参数分析函数
			argsParser _user_parser;
			/// @brief	所有参数选项指针
			struct argp_option *_all_options;
	};
};

#endif
