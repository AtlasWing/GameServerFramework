#ifndef _TABLEMANAGER_H_
#define _TABLEMANAGER_H_

#include "game/nPublicDefine.h"
#include "game/nTableBase.h"
#include "game/nAlgebra.h"

typedef unsigned int DWORD;
namespace Game 
{

    class TableManager
    {
        public:
            static TableManager& AfxGet()
            {
                static TableManager _instance;
                return _instance;
            }
            TableManager() 
            {
                numerical = new NumericalFormula();
            } 
            ~TableManager() 
            {
                for(ittableMap it = m_tabmap.begin(); it != m_tabmap.end(); ++it){
                    TableBase* tbl = it->second;
                    SAFE_DELETE(tbl);
                }
                m_tabmap.clear();
                SAFE_DELETE(numerical);
            }
            typedef std::map<const std::string, TableBase*> tableMap;
            typedef std::map<const std::string, TableBase*>::const_iterator ittableMap;
            void RegisterTable(const std::string name, TableBase* p, string filePath);
            
            static NumericalFormula *numerical;
            static double getFormulaCalculateResult(string value,double * param, int param_num)
            {
                numerical->Init(value);
                return numerical->GetFormulaCalculateResult(param,param_num);
            }
        public:
            template<typename T> T * GetTableInstance(string tableName)
            {
                if (m_tabmap.find(tableName) != m_tabmap.end()){
                    return static_cast<T*>(m_tabmap[tableName]);
                }
                return NULL;
            }

            template<typename T> static  T*  Select(int ID)
            {
                string name = AfxGet().getTableName(typeid(T).name());
                if(name == ""){
                    return NULL;
                }
                if(AfxGet().m_tabmap.find(name) == AfxGet().m_tabmap.end()){
                    return NULL;
                }
                return AfxGet().m_tabmap[name]->Select<T>(ID);
            }
            template<typename T> static  T*  Select(int ID1, int ID2)
            {
                string name = AfxGet().getTableName(typeid(T).name());
                if(name == ""){
                    return NULL;
                }
                if(AfxGet().m_tabmap.find(name) == AfxGet().m_tabmap.end()){
                    return NULL;
                }
                return AfxGet().m_tabmap[name]->Select<T>(ID1, ID2);
            }
            template<typename T> static vector<T*>* GetTable()
            {
                string name = AfxGet().getTableName(typeid(T).name());
                if(name == ""){
                    return NULL;
                }
                if(AfxGet().m_tabmap.find(name) == AfxGet().m_tabmap.end()){
                    return NULL;
                }
                return AfxGet().m_tabmap[name]->GetTable<T>();
            }
        private: 
            string getTableName(string typeName)
            {
#ifdef _WIN32
                std::vector<string> nameSplit = Split(typeName, "::");
                if(nameSplit.size() == 2){
                    return nameSplit[1];
                }
                return "";
#else
                int start = 0;
                std::string nameLength = "";
                std::string tempStr = typeName;
                tempStr = typeName.substr(6, tempStr.length()); // 前面6位是 N4Game 代表namespace的4个字节的Game
                for(int i = 0; i < tempStr.length(); i++){
                    if(tempStr[i] >= '0' && tempStr[i] <= '9'){
                        nameLength += tempStr[i];
                    }
                    else{
                        start = i;
                        break;
                    }
                }
                if(nameLength == ""){
                    return "";
                }
                int clsNameLength = Convert::ToInt(nameLength);
                return tempStr.substr(start, clsNameLength);
#endif
            }
        private:
            tableMap m_tabmap;
    };

}

#endif
