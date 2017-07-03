#include "base/nXMLParser.h"
#include "base/Tidy.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	nXMLParser::nXMLParser():
		_cur_node(NULL)
	{
	}

	nXMLParser::~nXMLParser()
	{
		//_doc.Clear();
	}
	bool nXMLParser::loadFile(const char *xmlFile)
	{
		if(!_doc.LoadFile(xmlFile))
		{
			baseServer->error("加载:%s失败,错误描述:%s",xmlFile,_doc.ErrorDesc());
			return false;
		}
		return true;
	}
	bool nXMLParser::parseString(const char* xmlString)
	{
		_doc.Parse(xmlString);
		if( _doc.Error() )
		{
			baseServer->error("解析XML字符串失败,错误描述:%s,%s",xmlString,_doc.ErrorDesc());
			return false;
		}
		return true;
	}

	bool nXMLParser::getCurrentNodeString(std::string& nodeString)
	{
		if(!_cur_node)
		{
			return false;
		}
		nodeString<<*_cur_node;
		return true;
	}

	void nXMLParser::freeFile()
	{
		_doc.Clear();
		_cur_node = NULL;
	}
	void nXMLParser::printFile()
	{
		_doc.Print();
	}
#if 0
	bool nXMLParser::execEveryNode(const char *parentNode)
	{
		TiXmlNode* node = _doc.FirstChild(parentNode);
		TiXmlElement *ele = NULL;
		if(node)
		{
			ele = node->ToElement();
			if(ele)
			{
				node = ele->FirstChildElement();
				if(node)
				{
					ele = node->ToElement();
					if(ele)
					{
						node = ele->FirstChildElement("zoneserver");
						if(node)
						{
							ele = node->ToElement();
							if(ele)
							{
								baseServer->debug("[%s]",ele->Value());
								baseServer->debug("[%s]",ele->Attribute("port"));
								baseServer->debug("[%s]",ele->GetText());
							}
							else
							{
								baseServer->debug("找不到节点1:[%s]",parentNode);
							}
						}
						else
						{
							baseServer->debug("找不到节点3:[%s]",parentNode);
						}
					}
				}
				//baseServer->debug("[%s]",ele->Attribute("global"));
			}
			//baseServer->debug("[%s]",node->GetDocument()->Print());
			//baseServer->debug("[%s]",node->GetDocument()->Print());
		}
		else
		{
			baseServer->debug("找不到节点:[%s]",parentNode);
		}
		return true;
	}
	bool nXMLParser::execEveryElement(const char *node)
	{
		return true;
	}
#endif

	bool nXMLParser::setChildNode()
	{
		_up_node_stack.push_back(_cur_node);
		_cur_node = _cur_node->FirstChildElement();
		if(!_cur_node)
		{
			//baseServer->debug("没有子节点");
			return false;
		}
		return true;
	}

	bool nXMLParser::setChildNode(const char* name)
	{
		_up_node_stack.push_back(_cur_node);
		_cur_node = _cur_node->FirstChildElement(name);
		if(!_cur_node)
		{
			return false;
		}
		return true;
	}

	bool nXMLParser::setCurNode(const char *name,bool needlog)
	{
		_cur_node = _doc.FirstChildElement(name);
		if(!_cur_node)
		{
			if(needlog)
			{
				baseServer->error("找不到节点:%s",name);
			}
			return false;
		}

		_up_node_stack.clear();
		return true;
	}
	bool nXMLParser::setCurNode(const char *name1, const char *name2,bool needlog)
	{
		if(!this->setCurNode(name1,needlog))
		{
			return false;
		}
		_up_node_stack.push_back(_cur_node);

		_cur_node = _cur_node->FirstChildElement(name2);
		if(!_cur_node)
		{
			if(needlog)
			{
				baseServer->error("找不到节点:%s",name2);
			}
			return false;
		}
		return true;
	}
	bool nXMLParser::setCurNode(const char *name1, const char *name2, const char *name3,bool needlog)
	{
		if(!this->setCurNode(name1,name2,needlog))
		{
			return false;
		}
		_up_node_stack.push_back(_cur_node);

		_cur_node = _cur_node->FirstChildElement(name3);
		if(!_cur_node)
		{
			if(needlog)
			{
				baseServer->error("找不到节点:%s",name3);
			}
			return false;
		}
		return true;
	}
	bool nXMLParser::setCurNode(const char *name1, const char *name2, const char *name3, const char *name4,bool needlog)
	{
		if(!this->setCurNode(name1,name2,name3,needlog))
		{
			return false;
		}
		_up_node_stack.push_back(_cur_node);

		_cur_node = _cur_node->FirstChildElement(name4);
		if(!_cur_node)
		{
			if(needlog)
			{
				baseServer->error("找不到节点:%s",name4);
			}
			return false;
		}
		return true;
	}
	bool nXMLParser::setCurNode(const char *name1, const char *name2, const char *name3, const char *name4, const char *name5,bool needlog)
	{
		if(!this->setCurNode(name1,name2,name3,name4,needlog))
		{
			return false;
		}
		_up_node_stack.push_back(_cur_node);

		_cur_node = _cur_node->FirstChildElement(name5);
		if(!_cur_node)
		{
			if(needlog)
			{
				baseServer->error("找不到节点:%s",name5);
			}
			return false;
		}
		return true;
	}
	bool nXMLParser::setNextNode()
	{
		if(!_cur_node)
		{
			return false;
		}
		_cur_node = _cur_node->NextSiblingElement();
		if(!_cur_node)
		{
			return false;
		}
		return true;
	}
	bool nXMLParser::setUpNode()
	{
		if(_up_node_stack.empty())
		{
			return false;
		}
		_cur_node = _up_node_stack.back();
		_up_node_stack.pop_back();
		return true;
	}
	bool nXMLParser::setUpNode(const char *nodeName)
	{
		for(Tidy::list<TiXmlElement*>::iterator iter=_up_node_stack.end(); iter != _up_node_stack.begin(); )
		{
			if((*iter)->ValueTStr() == nodeName)
			{
				_cur_node = *iter;
				_up_node_stack.pop_back();
				return true;
			}
			-- iter;
			assert_debug(_up_node_stack.empty() == false, "");
			_up_node_stack.pop_back();
		}

		return false;
	}
	bool nXMLParser::isNoneNode()
	{
		return _cur_node == NULL;
	}
	bool nXMLParser::getNodeAttrInt(const char *attrName, int *prop) const
	{
		if(!attrName || !prop || !_cur_node) {
			return false;
		}
		switch(_cur_node->QueryIntAttribute(attrName,(int *)prop))
		{
			case TIXML_NO_ATTRIBUTE:
				{
					baseServer->error("属性不存在:[%s]",attrName);
					return false;
				}
				break;
			case TIXML_WRONG_TYPE:
				{
					baseServer->error("属性类型错误:[%s]",attrName);
					return false;
				}
				break;
			case TIXML_SUCCESS:
				{
					return true;
				}
				break;
			default:
				break;
		}
		return false;
	}

	bool nXMLParser::setNodeAttrInt(const char *attrName, int prop)
	{
		if(!attrName || !prop || !_cur_node) 
		{
			return false;
		}
		_cur_node->SetAttribute(attrName, prop);
		return true;
	}

	bool nXMLParser::getNodeAttrInt(const char *attrName, int *prop, const int &defaultValue) const
	{
		if(!attrName || !prop || !_cur_node) 
		{
			return false;
		}
		switch(_cur_node->QueryIntAttribute(attrName,(int *)prop))
		{
			case TIXML_NO_ATTRIBUTE:
				{
					(*prop) = defaultValue;
					return true;
				}
				break;
			case TIXML_WRONG_TYPE:
				{
					baseServer->error("属性类型错误:[%s]",attrName);
					return false;
				}
				break;
			default:
				break;
		}
		return true;
	}

	bool nXMLParser::getNodeAttrDouble(const char *attrName, void *prop, const bool needLog) const
	{
		if(!attrName || !prop || !_cur_node) {
			if(needLog == true){
				baseServer->error("接口数据错误");
			}
			return false;
		}
		switch(_cur_node->QueryDoubleAttribute(attrName,(double *)prop))
		{
			case TIXML_NO_ATTRIBUTE:
				{
					if(needLog == true){
						baseServer->error("属性不存在:[%s]",attrName);
					}
					return false;
				}
				break;
			case TIXML_WRONG_TYPE:
				{
					if(needLog == true){
						baseServer->error("属性类型错误:[%s]",attrName);
					}
					return false;
				}
				break;
			case TIXML_SUCCESS:
				{
					return true;
				}
				break;
			default:
				break;
		}

		return false;
	}

	bool nXMLParser::getNodeAttrFloat(const char *attrName, void *prop, const bool needLog) const
	{
		double propValue = 0.0;
		const bool result = getNodeAttrDouble(attrName, &propValue, needLog);
		if(result == true) {
			*(reinterpret_cast<float*>(prop)) = static_cast<float>(propValue);
		}
		return result;
	}

	bool nXMLParser::getNodeAttrStr(const char *attrName,std::string *prop,bool needLog) const
	{
		if(!attrName) {
			return false;
		}
		if(_cur_node->QueryStringAttribute(attrName, prop) == TIXML_NO_ATTRIBUTE){
			if(needLog) {
				baseServer->error("属性不存在:[%s]",attrName);
			}
			return false;
		}

		return true;
	}

	bool nXMLParser::getNodeAttrStr(const char *attrName,void *prop,int propSize,bool needLog) const
	{
		if(!attrName || !prop || !_cur_node){
			return false;
		}
		const char *tmp = _cur_node->Attribute(attrName);
		if(!tmp) {
			if(needLog) {
				baseServer->error("属性不存在:[%s]",attrName);
			}
			return false;
		}
		copyStr((char *)prop,tmp,propSize);
		return true;
	}

	bool nXMLParser::getNodeContentDouble(double *content) const
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
		*content = atof(tmp);
		return true;
	}

	bool nXMLParser::getNodeContentFloat(float *content) const
	{
		double contentValue = 0.0;
		const bool result = getNodeContentDouble(&contentValue);
		if(result == true) {
			*content = static_cast<float>(contentValue);
		}
		return result;
	}

	bool nXMLParser::getNodeContentStr(void *content, int contentSize) const
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
		copyStr((char *)content,tmp,contentSize);
		return true;
	}

	const char* nXMLParser::getNodeContentStr() const
	{
		if(!_cur_node) 
		{
			return NULL;
		}
		return _cur_node->GetText();
	}
	bool nXMLParser::setNodeContentStr(const std::string &content)
	{
		if( content.empty() || !_cur_node) 
		{
			return false;
		}
		const char *tmp = _cur_node->GetText();
		TiXmlText text(content);
		if(!tmp)
		{
			if(_cur_node->InsertEndChild(text))
				return true;
			else
				return false;
		}
		TiXmlNode* child = _cur_node->FirstChild();
		if( child->Type() != static_cast<int>(TiXmlNode::TINYXML_TEXT) )
		{
			return false;
		}
		if( !_cur_node->ReplaceChild(child, text) )
		{
			return false;
		}
		return true;
	}

	const char *nXMLParser::getNodeName() const
	{
		if(!_cur_node) 
		{
			return NULL;
		}
		return _cur_node->Value();
	}

	bool nXMLParser::isAtNode(const char* nodeName) const
	{
		return _cur_node ? (_cur_node->ValueTStr() == nodeName) : false;
	}
};
