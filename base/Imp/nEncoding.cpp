#include <google/protobuf/descriptor.h>
#include <google/protobuf/text_format.h>

#include "base/nEncoding.h"

namespace Tidy
{
	EncodingConvert::EncodingConvert(const char* fromEncode, const char* toEncode)
	{
		this->conv = iconv_open(toEncode,fromEncode);
	}

	EncodingConvert::~EncodingConvert()
	{
		iconv_close(this->conv);
	}

	int EncodingConvert::convert(char* src, size_t srcLen, char* dest, size_t destLen)
	{
		return iconv(conv, &src, &srcLen, &dest, &destLen);
	}

	bool nEncoding::toLua(const google::protobuf::Message &message, std::string *output, DWORD indentLevel)
	{
		const ::google::protobuf::Reflection::Reflection *ref = message.GetReflection();
		const google::protobuf::Descriptor *messageDes = message.GetDescriptor();
		std::vector<const ::google::protobuf::FieldDescriptor*> fieldList;
		//ref->ListFields(message, &fieldList);

		for (int i = 0; i < messageDes->field_count(); i++) {
			const google::protobuf::FieldDescriptor* field = messageDes->field(i);
			if (field->is_repeated()) {
				fieldList.push_back(field);
			} else {
				if (ref->HasField(message, field)) {
					fieldList.push_back(field);
				}
			}
		}

		output->append("\n");
		output->append(indentLevel, '\t');
		output->append("{\n");

		++ indentLevel;
		for(DWORD i = 0; i < fieldList.size(); ++ i){
			const ::google::protobuf::FieldDescriptor *desc = fieldList[i];

			output->append(indentLevel, '\t');
			output->append(desc->name().c_str());
			output->append(" = ");
			if(desc->is_repeated() == true){
				output->append("\n");
				output->append(indentLevel, '\t');
				output->append("{");

				++ indentLevel;
				DWORD size = ref->FieldSize(message, desc);
				for(DWORD i = 0; i < size; ++ i){
					if(desc->cpp_type() == ::google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE){
						const ::google::protobuf::Message &subMessage = ref->GetRepeatedMessage(message, desc, i);
						toLua(subMessage, output, indentLevel);
					}
					else{
						std::string value;
						if(desc->type() == ::google::protobuf::FieldDescriptor::TYPE_BYTES){
							value = ref->GetString(message, desc);
							output->append("{");
							output->append(value.begin(), value.end());
							output->append("}");
						}
						else{
							google::protobuf::TextFormat::PrintFieldValueToString(message, desc, i, &value);
							output->append(value);
						}
					}
					output->append(",");
				}
				-- indentLevel;

				output->append("\n");
				output->append(indentLevel, '\t');
				output->append("}");
			}
			else if(desc->cpp_type() == ::google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE){
				const ::google::protobuf::Message &subMessage = ref->GetMessage(message, desc);
				toLua(subMessage, output, indentLevel);
			}
			else{
				std::string value;
				if(desc->type() == ::google::protobuf::FieldDescriptor::TYPE_BYTES){
					value = ref->GetString(message, desc);
					output->append("{");
					output->append(value.begin(), value.end());
					output->append("}");
				}
				else{
					google::protobuf::TextFormat::PrintFieldValueToString(message, desc, i, &value);
					output->append(value);
				}
			}

			output->append(",\n");
		}

		-- indentLevel;

		output->append(indentLevel, '\t');
		output->append("}");
		return true;
	}

	//查询cur节点是否是同一层次中同名的第一个节点，是返回true，否则返回false
	bool nEncoding::isFirstElementChildNode(const TiXmlElement *rootElementNode, const TiXmlElement *currentElementNode)
	{
		if(currentElementNode == rootElementNode){
			return true;
		}

		for(const TiXmlElement *iter = currentElementNode->Parent()->FirstChildElement(); iter != currentElementNode; iter = iter->NextSiblingElement()){
			if(iter->ValueStr() == currentElementNode->ValueStr()){
				return false;
			}
		}

		return true;
	}

	//查询cur节点所在层次中是否存在与之同名的节点,是返回true，否则返回false
	bool nEncoding::isExistSameNameElementNode(const TiXmlElement *rootElementNode, const TiXmlElement *currentElementNode)
	{
		if(currentElementNode == rootElementNode){
			return false;
		}

		for(const TiXmlElement *iter = currentElementNode->Parent()->FirstChildElement(); iter != NULL; iter = iter->NextSiblingElement()){
			if((iter != currentElementNode) && (iter->ValueStr() == currentElementNode->ValueStr())){
				return true;
			}
		}

		return false;
	}

	//查询cur节点是所在层次中同名节点的第几个
	DWORD nEncoding::getAutoIncrementNum(const TiXmlElement *rootElementNode, const TiXmlElement *currentElementNode)
	{
		DWORD num = 1;
		if(currentElementNode == rootElementNode){
			return num;
		}

		for(const TiXmlElement *iter = currentElementNode->Parent()->FirstChildElement(); iter != NULL; iter = iter->NextSiblingElement()){
			if(iter == currentElementNode){
				break;
			}
			else if(iter->ValueStr() == currentElementNode->ValueStr()){
				++ num;
			}
		}

		return num;
	}


	//生成基本的属性与孩子部分
	void nEncoding::getBaseAttributeAndElementChildNode(const TiXmlElement *rootElementNode, const TiXmlElement *currentElementNode, std::string& result, const DWORD& size)
	{ 
		for(const TiXmlAttribute *iter = currentElementNode->FirstAttribute(); iter != NULL; iter = iter->Next()){
			if(strcmp(iter->Name(), "key") != 0 && strcmp(iter->Name(), "nodeNameSuffix") != 0 && strcmp(iter->Name(), "nodeRepeated") != 0){
				result.append(1, '\n');
				result.append(size + 4, ' ');
				result.append(iter->Name());
				result.append(" = ");
				const char* value = currentElementNode->Attribute(iter->Name());
				if(isNumber(value) == false){
					if(strcmp(value, "autoIncrement") == 0){
						std::stringstream ss;
						ss << getAutoIncrementNum(rootElementNode, currentElementNode);
						result.append(ss.str());
					}
					else{
						result.append(1, '\"');
						result.append(value);
						result.append(1, '\"');
					}
				}
				else{
					result.append(value);
				}
				result.append(1, ',');
			}
		}    

		const char* text = currentElementNode->GetText();
		if(text != NULL){
			result.append(1, '\n');
			result.append(size + 4, ' ');
			const TiXmlAttribute *firstAttribute = currentElementNode->FirstAttribute();
			if((firstAttribute != NULL && strcmp(firstAttribute->Name(), "key") != 0 && strcmp(firstAttribute->Name(), "nodeNameSuffix") != 0 && strcmp(firstAttribute->Name(), "nodeRepeated") != 0) || (currentElementNode->FirstChildElement() != NULL)){
				result.append("innertext = ");
			}
			if(isNumber(text) == false){
				result.append(1, '\"');
				result.append(text);
				result.append(1, '\"');
			}
			else{
				result.append(text);
			}
			result.append(1, ',');
		}

		for(const TiXmlElement *iter = currentElementNode->FirstChildElement(); iter != NULL; iter = iter->NextSiblingElement()){
			if(isFirstElementChildNode(rootElementNode, iter) == true){
				result.append(getLuaTableString(rootElementNode, iter, true));
			}
		}
	}

	/*计算currentElementNode节点生成的table string.当currentElementNode是同一层次的首个节点的时候:若isDisplayName为ture,生成带名字的string,否则生成不带名字的string;当currentElementNode不是同一层次的首个节点的时候,生成不带名字的string*/                
	std::string nEncoding::getLuaTableString(const TiXmlElement *rootElementNode, const TiXmlElement *currentElementNode, bool isDisplayName)
	{
		std::string result;
		if(currentElementNode != rootElementNode){
			result.append(1, '\n');
		}
		DWORD size = getCommandNumbers(rootElementNode, currentElementNode, isDisplayName);
		/*同一层次首个节点且isDisplayName为true;需要带名字输出*/
		if((isFirstElementChildNode(rootElementNode, currentElementNode) == true) && (isDisplayName == true)){
			result.append(size, ' ');
			result.append(currentElementNode->ValueStr());
			if(currentElementNode == rootElementNode && currentElementNode->Attribute("nodeNameSuffix") != NULL){
				const char *value = currentElementNode->Attribute(currentElementNode->Attribute("nodeNameSuffix"));
				assert_debug(value != NULL, "");
				result.append(1, '_');
				result.append(value);
			}   
			result.append(" =");
			if((currentElementNode->Attribute("nodeRepeated") == NULL) && (isExistSameNameElementNode(rootElementNode, currentElementNode) == false) && (currentElementNode->FirstAttribute() == NULL) && (currentElementNode->FirstChildElement() == NULL) && (currentElementNode->GetText() != NULL)){
				result.append(1, ' ');
				if(isNumber(currentElementNode->GetText()) == false){
					result.append(1, '\"');
					result.append(currentElementNode->GetText());
					result.append(1, '\"');
				}
				else{
					result.append(currentElementNode->GetText());
				}
				if(currentElementNode != rootElementNode){
					result.append(1, ',');
				}
			}
			else{
				result.append(1, '\n');
				result.append(size, ' ');
				result.append(1, '{');
				/*同一层次存在多个同名节点或者需要显示key所指示的索引时*/
				if((isExistSameNameElementNode(rootElementNode, currentElementNode) == true) || (currentElementNode->Attribute("key") != NULL || currentElementNode->Attribute("nodeRepeated") != NULL)) {
					result.append(getLuaTableString(rootElementNode, currentElementNode, false));
					for(const TiXmlElement *iter = currentElementNode->NextSiblingElement(); iter != NULL; iter = iter->NextSiblingElement()){
						if(iter->ValueStr() == currentElementNode->ValueStr()){
							result.append(getLuaTableString(rootElementNode, iter, true));
						}
					}
					result.append(1, '\n');
					result.append(size, ' ');
					result.append(1, '}');
					if(currentElementNode != rootElementNode){
						result.append(1, ',');
					}
				}
				else{
					getBaseAttributeAndElementChildNode(rootElementNode, currentElementNode, result, size);
					result.append(1, '\n');
					result.append(size, ' ');
					result.append(1, '}');
					if(currentElementNode != rootElementNode){
						result.append(1, ',');
					}
				}
			}
		}
		/*不是同一层次第一个节点或者虽然是第一个节点但isDisplayName为false;统一情况:都不需要带名字输出*/
		else{
			if(currentElementNode->Attribute("key") != NULL){
				result.append(size, ' ');
				result.append(1, '[');
				const char* value = currentElementNode->Attribute(currentElementNode->Attribute("key"));
				assert_debug(value != NULL, "");
				if(isNumber(value) == false){
					if(strcmp(value, "autoIncrement") == 0){
						std::stringstream ss;
						ss << getAutoIncrementNum(rootElementNode, currentElementNode);
						result.append(ss.str());
					}
					else{
						result.append(1, '\"');
						result.append(value);
						result.append(1, '\"');
					}
				}
				else{
					result.append(value);
				}
				result.append(1, ']');
				result.append(" =");
				if((currentElementNode->FirstAttribute() == NULL) && (currentElementNode->FirstChildElement() == NULL) && (currentElementNode->GetText() != NULL)){
					result.append(1, ' ');
					if(isNumber(currentElementNode->GetText()) == false){
						result.append(1, '\"');
						result.append(currentElementNode->GetText());
						result.append(1, '\"');
					}
					else{
						result.append(currentElementNode->GetText());
					}
					if(currentElementNode != rootElementNode){
						result.append(1, ',');
					}
				}
				else{
					result.append(1, '\n');
					result.append(size, ' ');
					result.append(1, '{');
					getBaseAttributeAndElementChildNode(rootElementNode, currentElementNode, result, size);
					result.append(1, '\n');
					result.append(size, ' ');
					result.append(1, '}');
					if(currentElementNode != rootElementNode){
						result.append(1, ',');
					}
				}
			}
			else{
				if(((currentElementNode->FirstAttribute() == NULL) || (currentElementNode->Attribute("nodeRepeated") != NULL && currentElementNode->FirstAttribute() == currentElementNode->LastAttribute())) && (currentElementNode->FirstChildElement() == NULL) && (currentElementNode->GetText() != NULL)){
					result.append(size, ' ');
					if(isNumber(currentElementNode->GetText()) == false){
						result.append(1, '\"');
						result.append(currentElementNode->GetText());
						result.append(1, '\"');
					}
					else{
						result.append(currentElementNode->GetText());
					}
					if(currentElementNode != rootElementNode){
						result.append(1, ',');
					}
				}
				else{
					result.append(size, ' ');
					result.append(1, '{');
					getBaseAttributeAndElementChildNode(rootElementNode, currentElementNode, result, size);
					result.append(1, '\n');
					result.append(size, ' ');
					result.append(1, '}');
					if(currentElementNode != rootElementNode){
						result.append(1, ',');
					}
				}
			}
		}

		return result;
	}

	/*给定xml文件名字fileName,生成对应的lua table string,存在buffer指向的内存区*/
	bool nEncoding::toLua(const char* fileName, std::string *buffer)
	{
		TiXmlDocument doc(fileName);

		if(doc.LoadFile() == false){
			assert_fail("读取xml文件失败[%s][%u,%s] 位置[%d,%d]", fileName, doc.ErrorId(), doc.ErrorDesc(), doc.ErrorRow(), doc.ErrorCol());
			return false;
		}

		TiXmlElement *rootElement = doc.RootElement();
		*buffer = getLuaTableString(rootElement, rootElement, true);
		return true;
	}

	//计算生成lua table时开头需要的空格数目
	DWORD nEncoding::getCommandNumbers(const TiXmlElement *rootElementNode, const TiXmlElement *currentElementNode, bool isDisplayName)
	{
		if(currentElementNode == rootElementNode){
			if(isDisplayName == true){
				return 0;
			}
			else{
				return 4;
			}
		}

		DWORD result = 0;
		result += 4;
		if((isFirstElementChildNode(rootElementNode, currentElementNode) == false) || (isDisplayName == false)){
			result += 4;
		}
		for(const TiXmlElement *iter = currentElementNode->Parent()->ToElement();iter != rootElementNode;iter = iter->Parent()->ToElement()){
			result += 4;
			if(isExistSameNameElementNode(rootElementNode, iter) == true || iter->Attribute("nodeRepeated") != NULL || iter->Attribute("key") != NULL){
				result += 4;
			}
		}

		return result;
	}

	/*查看是否可以转换成数值数据,可以返回true,否则返回false*/
	bool nEncoding::isNumber(const char* cstring)
	{
		if(strcmp(cstring, "true") == 0 || strcmp(cstring, "false") == 0){
			return true;
		}
		DWORD pointNumber = 0;
		DWORD start = ((cstring[0] == '+') || (cstring[0] == '-')) ? 1 : 0;
		for(DWORD i = start; i < strlen(cstring); i ++){
			if((cstring[i] < '0') || (cstring[i] > '9')){
				if(cstring[i] == '.'){
					++ pointNumber;
				}
				else{
					return false;
				}
			}
		}

		if(pointNumber > 1){
			return false;
		}

		return true;
	}

	BYTE nEncoding::toHex(const BYTE &x)
	{
		return (x > 9) ? (x -10 + 'A'): (x + '0');
	}

	BYTE nEncoding::fromHex(const BYTE &x)
	{
		return (x >= '0' && x <= '9') ? (x-'0') : (x-'A'+10);
	}

	std::string nEncoding::urlEncode(const char *sIn, const DWORD len)
	{
        const BYTE *data = reinterpret_cast<const BYTE*>(sIn);
		std::string sOut;
        sOut.reserve(len);
		for( size_t ix = 0; ix < len; ix++ ) {
            const BYTE character = data[ix];
			if( character == '-' || character == '_' || character == '.' || (character >= '0' && character <= '9') || (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z')) {     
                sOut.push_back(character);
			}
			else {
                sOut.push_back('%');
                sOut.push_back(toHex(character/16));
                sOut.push_back(toHex(character%16));
			}
		}
		return sOut;
	};

	std::string nEncoding::urlDecode(const char *sIn, const DWORD len)
	{
		std::string sOut;
		sOut.reserve(len);
		for( size_t ix = 0; ix < len; ix++ )
		{
			BYTE ch = 0;
			if(sIn[ix]=='%') {
				ch = (fromHex(sIn[ix+1])<<4);
				ch |= fromHex(sIn[ix+2]);
				ix += 2;
			}
			else if(sIn[ix] == '+') {
				ch = ' ';
			}
			else {
				ch = sIn[ix];
			}
			sOut += (char)ch;
		}
		return sOut;
	}
};
