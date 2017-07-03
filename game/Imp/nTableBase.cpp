#include "game/nTableBase.h"
#include "game/nSpecialType.h"
#include "game/nAlgebra.h"

namespace Game 
{
    TableQuery * TableQuery::query = NULL;

    void TableBase::Loaded(std::string name)
    {
        TiXmlDocument doc(this->FilePath.c_str());
        bool loadOkay = doc.LoadFile();
        if(!loadOkay){
            assert(0);
        }
        TiXmlNode* node = 0;
        TiXmlElement* rootElem = 0;
        TiXmlElement* itemElement = 0;
        rootElem = doc.RootElement();
        TiXmlElement* sysElement = rootElem->FirstChild()->ToElement();
        std::map<int, std::map<std::string, std::string>> tableMap;
        for(TiXmlNode *item = sysElement->FirstChild("ConfigItem"); item != NULL; item = item->NextSibling("ConfigItem")){
            map<string, string> node_map;
            for(TiXmlAttribute * attr = item->ToElement()->FirstAttribute(); attr != NULL; attr = attr->Next()){
                string name(attr->Name());
                string value(attr->Value());
                node_map.insert(pair<string, string>(name, value));
            }
            int key = Convert::ToInt(node_map[GetKey1()]);
            if(GetKey1() != GetKey2()){
                key = Convert::ToInt(node_map[GetKey1()]) * 10000 + Convert::ToInt(node_map[GetKey2()]);
            }
            tableMap.insert(pair<int, map<string, string>>(key, node_map));
        }
        LoadTable(name, tableMap);
    }

    void TableBase::convertTypeSetValue(setValue & set, string value, string type)
    {
        if(type == stringType){
            set(this,value);
        }
        else if(type == intType){
            set(this, Convert::ToInt(value));
        }
        else if(type == floatType){
            set(this, Convert::ToFloat(value));
        }
        else if(type == doubleType){
            set(this, Convert::ToDouble(value));
        }
        else{
            //NumericalFormula * spClass =  (NumericalFormula*)ClassFactory::GetInstance().CreateInstance(type,value);
            //NumericalFormula spClass(type,value);
            //set(this, *spClass);
        }
    }

    void TableBase::processSelectResult(map<string, string> *configItem)
    {
        for(map<string, string>::iterator it = m_propertyTypeMap.begin();it != m_propertyTypeMap.end(); ++it){
            if(configItem->find(it->first) == configItem->end()){
                continue;
            }
            if(m_methodMap.find(it->first) == m_methodMap.end()){
                continue;
            }
            convertTypeSetValue(m_methodMap[it->first], (*configItem)[it->first], it->second);
        }
    }
}
