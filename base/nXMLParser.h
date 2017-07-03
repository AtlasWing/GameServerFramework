#ifndef _NXMLPARSE_H_
#define _NXMLPARSE_H_

#include "base/nSTL.h"
#include "base/Tidy.h"

namespace Tidy
{
	class nXMLParser
	{
		public:
			nXMLParser();
			~nXMLParser();

		public:
			/// @brief	加载一个XML文件
			/// @param	xmlFile XML文件
			/// @return	加载文件是否成功
			bool loadFile(const char *xmlFile);
			/// @brief	解析一个XML字符串
			/// @param	xmlString XML字符串
			/// @return	解析字符串是否成功
			bool parseString(const char* xmlString);
			/// @brief	返回当前节点的字符串
			/// @param	nodeString 返回的字符串
			/// @return	是否成功
			bool getCurrentNodeString(std::string& nodeString);
			/// @brief	释放所有资源
			void freeFile();
			/// @brief	输出文件,用于测试
			void printFile();
			/// @brief	设置到第一个子节点
			/// @return	找到节点返回true,否则返回false
			bool setChildNode();
			/// @brief	找到第一个指定的子节点
			/// @param	name 节点名字
			/// @return	找到节点返回true,否则返回false
			bool setChildNode(const char* name);
			/// @brief	设置到指定节点
			/// @param	name 节点名字
			/// @return	找到节点返回true,否则返回false
			bool setCurNode(const char *name,bool needlog=true);
			/// @brief	设置到指定节点
			/// @param	name1 父节点名字
			/// @param	name2 子节点名字
			/// @return	找到节点返回true,否则返回false
			bool setCurNode(const char *name1, const char *name2,bool needlog=true);
			/// @brief	设置到指定节点
			/// @param	name1 祖父节点名字
			/// @param	name2 父节点名字
			/// @param	name3 子节点名字
			/// @return	找到节点返回true,否则返回false
			bool setCurNode(const char *name1, const char *name2, const char *name3,bool needlog=true);
			/// @brief	设置到指定节点
			/// @param	name1 祖父节点名字
			/// @param	name2 父节点名字
			/// @param	name3 子节点名字
			/// @param	name4 子节点名字
			/// @return	找到节点返回true,否则返回false
			bool setCurNode(const char *name1, const char *name2, const char *name3, const char *name4,bool needlog=true);
			/// @brief	设置到指定节点
			/// @param	name1 祖父节点名字
			/// @param	name2 父节点名字
			/// @param	name3 子节点名字
			/// @param	name4 子节点名字
			/// @param	name5 子节点名字
			/// @return	找到节点返回true,否则返回false
			bool setCurNode(const char *name1, const char *name2, const char *name3, const char *name4, const char *name5,bool needlog=true);
			/// @brief	设置到下一个节点
			/// @return	找到节点返回true,否则返回false
			bool setNextNode();
			/// @brief	返回父节点
			/// @return	找到节点返回true，否则返回false	
			bool setUpNode();
			/// @brief	根据节点名字返回上层节点
			/// @param	nodeName 节点名字
			/// @return	找到节点返回true，否则返回false
			bool setUpNode(const char *nodeName);
			/// @brief	检查是否时空节点
			/// @return	空节点true,否则返回false
			bool isNoneNode();
			/// @brief	得到整型属性值
			/// @param	attrName 属性名称
			/// @param	prop 输出的整数
			/// @return	赋值是否成功
			bool getNodeAttrInt(const char *attrName,int *prop) const;
			/// @brief	设置整形属性值
			/// @param	attrName 属性名称
			/// @param	prop 属性值
			/// @return	赋值是否成功
			bool setNodeAttrInt(const char *attrName, int prop);
			/// @brief	得到整型属性值,模板实现.对于int参数有重载
			/// @param	attrName 属性名称
			/// @param	prop 输出的整数
			/// @return	赋值是否成功
			template<typename T>
				bool getNodeAttrInt(const char *attrName, T *prop) const;
			/// @brief	得到整型属性值
			/// @param	attrName 属性名称
			/// @param	prop 输出的整数
			/// @param	defaultValue 默认值
			/// @return	赋值是否成功
			bool getNodeAttrInt(const char *attrName,int *prop,const int &defaultValue) const;
			/// @brief	得到整型属性值,模板实现.对于int参数有重载
			/// @param	attrName 属性名称
			/// @param	prop 输出的整数
			/// @param	defaultValue 默认值
			/// @return	赋值是否成功
			template<typename T>
				bool getNodeAttrInt(const char *attrName, T *prop,const int &defaultValue) const;
			/// @brief	得到double属性值
			/// @param	attrName 属性名称
			/// @param	prop 输出的整数
			/// @return	赋值是否成功
			bool getNodeAttrDouble(const char *attrName,void *prop, const bool needLog) const;
			/// @brief	得到float属性值
			/// @param	attrName 属性名称
			/// @param	prop 输出的整数
			/// @return	赋值是否成功
			bool getNodeAttrFloat(const char *attrName, void *prop, const bool needLog) const;
			/// @brief	得到字符串属性值
			/// @param	attrName 属性名称
			/// @param	prop 输出的整数
			/// @param	propSize 获得字串的最大字节
			/// @return	赋值是否成功
			bool getNodeAttrStr(const char *attrName,void *prop,int propSize,bool needLog=true) const;
			/// @brief	得到字符串属性值
			/// @param	attrName 属性名称
			/// @param	prop 输出的字符串
			/// @return	赋值是否成功
			bool getNodeAttrStr(const char *attrName,std::string *prop,bool needLog = true) const;

			/// @brief	得到整数内容值
			/// @param	content 输出的整数
			/// @return	赋值是否成功
			template<typename T>
				bool getNodeContentInt(T *content) const;
			/// @brief	得到double内容值
			/// @param	content 输出的整数
			/// @return	赋值是否成功
			bool getNodeContentDouble(double *content) const;
			/// @brief	得到float内容值
			/// @param	content 输出的整数
			/// @return	赋值是否成功
			bool getNodeContentFloat(float *content) const;
			/// @brief	得到字符串内容值
			/// @return	返回的值
			const char* getNodeContentStr() const;
			/// @brief	得到字符串内容值
			/// @param	content 输出的整数
			/// @param	contentSize 获得字串的最大字节
			/// @return	赋值是否成功
			bool getNodeContentStr(void *content, int contentSize) const;
			/// @brief	给当前节点赋内容值
			/// @param	content 输入的字符串值
			/// @return	赋值是否成功
			bool setNodeContentStr(const std::string &content);
			/// @brief	得到节点名字
			/// @return	节点名字
			const char *getNodeName() const;
			/// @brief	判断当前节点是否是在指定的节点
			/// @param	nodeName 比较的节点名字
			/// @return	当前节点和指定名字相同返回true,否则返回false。
			bool isAtNode(const char* nodeName) const;

		private:
			/// @brief	XML解析器
			TiXmlDocument  _doc;
			/// @brief	当前指向的XML元素
			TiXmlElement *_cur_node;
			/// @brief	保存所有的上层节点栈
			Tidy::list<TiXmlElement*> _up_node_stack;
	};

	template<typename T>
		inline bool nXMLParser::getNodeAttrInt(const char *attrName, T *prop) const
		{
			if(!attrName || !prop || !_cur_node) 
			{
				return false;
			}
			int tmp=0;
			if(getNodeAttrInt(attrName, &tmp))
			{
				*prop = (T)tmp;
				return true;
			}

			return false;
		}
	template<typename T>
		inline bool nXMLParser::getNodeAttrInt(const char *attrName, T *prop, const int &defaultValue) const
		{
			if(!attrName || !prop || !_cur_node) 
			{
				return false;
			}
			int tmp=0;
			if(getNodeAttrInt(attrName, &tmp, defaultValue))
			{
				*prop = (T)tmp;
				return true;
			}

			return false;
		}
	template<typename T>
		inline bool nXMLParser::getNodeContentInt(T *content) const
		{
			if(!content || !_cur_node) 
			{
				return false;
			}
			const char *tmp = _cur_node->GetText();
			if(!tmp)
			{
				return false;
			}
			int temp = atoi(tmp);
			(*content) = (T)temp;
			return true;
		}
};
#endif
