#ifndef _TABLE_BASE_H_
#define _TABLE_BASE_H_
#include "game/nPublicDefine.h"

namespace Game 
{

#define DECLARE_CLASS(className) \
    string className##Name;  \
    static DynamicClass* m_className##dc;

#define IMPLEMENT_CLASS(className)  \
    ClassFactory::GetInstance().registClass(#className, className::createInstance);

#define SYNTHESIZE(classType, varType, varName)\
    public:\
           static void set##varName(void * cp, object value)\
    {\
        classType* tp = (classType*)cp; \
        tp->varName = object_cast<varType>(value); \
    }\
    varType get##varName(void) const \
    {\
        return varName; \
    }

#define SAVE_MEMBER(var, type)\
        m_methodMap.insert(pair<string, setValue>(#var, set##var));\
        m_propertyTypeMap.insert(pair<string, string>(#var, #type));

    typedef void(*setValue)(void * t, object v);
    
    class TableQuery
    {
        private:
            map < int, map<string, string>> table;
            static TableQuery * query;
            static string selectKey;
        private:
            TableQuery() { }
            ~TableQuery()
            {
            }
        public:
            static TableQuery * Select()
            {
                TableQuery * _query = new TableQuery;
                query = _query;
                return query;
            }
            static TableQuery * From(map<int, map<string, string>> table)
            {
                query->table = table;
                return query;
            }

        public:
            map<string, string> * Where(string key, int value)
            {
                map<string, string> * newMap = NULL;
                newMap = selectMap(key, value);
                return newMap;
            }
            map<string, string> * Where(string key1, int value1, string key2, int value2)
            {
                map<string, string> * newMap = NULL;
                int keyhash = value1 * 10000 + value2;
                if (table.find(keyhash) != table.end() && table[keyhash].find(key1) != table[keyhash].end()){
                    if(table[keyhash].find(key2) != table[keyhash].end() && Convert::ToInt(table[keyhash][key2]) == value2){
                        newMap = &table[keyhash];
                    }
                }
                return newMap;
            }
        private:
            map<string, string> * selectMap(string key, int value)
            {
                map<string, string> * newMap = NULL;
                if(table.find(value) != table.end() && table[value].find(key) != table[value].end()){
                    if(Convert::ToInt(table[value][key]) == value){
                        newMap = &table[value];
                    }
                }
                return newMap;
            }
    };

    class TableBase
    {
        protected:
            DECLARE_CLASS(TableBase)
        public:
                string Name;
                static void* createInstance(string content = "") 
                { 
                    return new TableBase(); 
                }
                virtual void registProperty() {}
                virtual void display() {}
                virtual bool initTableParam(){return true;}
                map<string, setValue> m_methodMap;
                map<string, string> m_propertyTypeMap;
                std::map<int,TableBase*> tablePointerMap;
        public:
                TableBase() 
                {
                };
                virtual ~TableBase()
                {
                    m_methodMap.clear();
                    m_propertyTypeMap.clear();
                }
        public:
                std::string ID;
                std::string FilePath;
        public:
                void Loaded(std::string name);
                template<typename T> const T* Select(int key) const
                {
                    if(tablePointerMap.empty() == true){
                        return NULL;
                    }
                    map<int, TableBase*>::const_iterator it = tablePointerMap.find(key); 
                    if(it == tablePointerMap.end()){
                        return NULL;
                    }
                    return reinterpret_cast<T*>(it->second);
                }
                template<typename T> const T * Select(int key1, int key2) const
                {
                    int keyhash = key1 * 10000 + key2;
                    return Select<T>(keyhash);    
                }

                void LoadTable(std::string tableName, std::map<int, std::map<std::string, std::string>> tableMap)
                {
                    string name = tableName;
                    for(map<int, map<string, string>>::const_iterator it = tableMap.begin();it != tableMap.end(); ++it){
                        map<string, string> element = it->second;
                        TableBase *sourceObj = (TableBase*)ClassFactory::GetInstance().CreateInstance(name);
                        sourceObj->registProperty();
                        for(map<string, string>::iterator itr = m_propertyTypeMap.begin();itr != m_propertyTypeMap.end(); ++itr){
                            if(element.find(itr->first) == element.end()){
                                continue;
                            }
                            sourceObj->convertTypeSetValue(sourceObj->m_methodMap[itr->first], element[itr->first], itr->second);
                        }
                        tablePointerMap.insert(std::pair<int,TableBase*>(it->first,sourceObj));
                    }
                }

                template<typename T> vector<const T*>* GetTable() const
                {
                    vector<const T*> *list = new vector<const T*>();

                    for(map<int, TableBase*>::const_iterator it = tablePointerMap.begin();it != tablePointerMap.end(); ++it){
                        list->push_back(static_cast<const T*>(it->second));
                    }

                    return list;
                }

        protected:
                void convertTypeSetValue(setValue &set, string value, string type);
                void processSelectResult(map<string, string> * configItem);

                virtual std::string GetKey1()
                {
                    return "ID";
                }
                virtual std::string GetKey2()
                {
                    return "ID";
                }

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

    };

    typedef void (TableBase::* TableLoaded)(void * args);
#define  EventDelegate(classAndFunc) reinterpret_cast<Event>(&classAndFunc)

}

#endif // end of TableBase


