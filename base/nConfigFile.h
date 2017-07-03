#ifndef _N_CONFIG_FILE_H_
#define _N_CONFIG_FILE_H_

#include "base/nXMLParser.h"

namespace Tidy
{
	/// @brief	配置文件解析器
	class nConfigFile
	{
		public:
			/// @param	filename 配置文件名字
			explicit nConfigFile(const char *filename);
			~nConfigFile();

		public:
			/// @brief	开始解析配置文件
			/// @param	name 使用者自己参数的定义节点名字
			/// @return	解析是否成功
			bool parse(const char *name, Tidy::nParamMap *configMap);
			/// @brief	全局解析函数
			/// @param	node 全局配置节点
			/// @return	解析是否成功
			bool parseShare(Tidy::nParamMap *configMap);
			/// @brief	开始解析配置文件
			/// @param	name 使用者自己参数的定义节点名字
			/// @param	ignore_false 是否忽略错误
			/// @return	解析是否成功
			bool parseNode(const char *name,bool ignore_false, Tidy::nParamMap *configMap);

		private:
			/// @brief	普通参数解析,只是简单的把参数放入global容器中
			/// @param	node 要解析的节点
			/// @return	解析是否成功
			bool parseNormal(Tidy::nParamMap *configMap);

		private:
			/// @brief	xml解析器
			nXMLParser _parser;
			/// @brief	配置文件名称
			std::string _filename;
	};
};
#endif
