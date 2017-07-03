#ifndef _TABLEREGISTER_H_
#define _TABLEREGISTER_H_
#include "base/nAssert.h"

#include "game/nTableBase.h"
#include "game/nTable.h"
#include "game/nTableManager.h"
#include "game/nPublicDefine.h"
#include "game/nAlgebra.h"

class TableRegister
{
    public:
        static bool RegisterAllTable(std::vector<std::string> exceptNameList)
        {
            TiXmlDocument doc("config/TableConfig.xml");
            bool loadOkay = doc.LoadFile();
            if(!loadOkay){
                assert(0);
                return false;
            }
            Game::RegisterTableCreateInstance();
            Game::RegisterSpecialType();
            TiXmlNode* node = 0;
            TiXmlElement* rootElem = 0;
            TiXmlElement* itemElement = 0;
            rootElem = doc.RootElement();
            bool needContinue = false;
            for(TiXmlNode * item = rootElem->FirstChild("Resource"); item != NULL; item = item->NextSibling("Resource")){
                std::string tableName = item->ToElement()->Attribute("Name");
                for(WORD i = 0; i < exceptNameList.size(); ++ i){
                    if(tableName == exceptNameList[i]){
                       needContinue = true; 
                    }
                }
                if(needContinue == true){
                    needContinue = false;
                    continue;
                }
                Game::TableBase * table = (Game::TableBase*)Game::ClassFactory::GetInstance().CreateInstance(tableName);
                if(table == NULL){
                    assert_fail("该表格不存在[%s]", tableName.c_str());
                    return false;
                }
                table->registProperty();
                Game::TableManager::AfxGet().RegisterTable(tableName, table, "config/Table/" + tableName + ".tbl");
                if(table->initTableParam() == false){
                    return false;
                }
            }
            return true;
        }
};

#endif
