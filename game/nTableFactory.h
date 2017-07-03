#pragma once
#include <string>
#include <map>
#include <vector>

namespace Game 
{
    using namespace std;
    typedef void* (*createClass)(string content);

    class ClassFactory
    {
        private:
            ClassFactory() { };
            ~ClassFactory() { };

        public:
            void* CreateInstance(string className,string content = "")
            {
                map<string, createClass>::const_iterator iter;
                iter = m_classMap.find(className);
                if(iter == m_classMap.end()){
                    return NULL;
                }
                else{
                    return iter->second(content);
                }
            }

            void registClass(string name, createClass method)
            {
                m_classMap.insert(pair<string, createClass>(name, method));
            }

            static ClassFactory & GetInstance()
            {
                static ClassFactory _TableFactory;
                return _TableFactory;
            }
        private:
            map<string, createClass> m_classMap;
    };

    class DynamicClass
    {
        public:
            DynamicClass(string name, createClass method)
            {
                ClassFactory::GetInstance().registClass(name, method);
            }
    };
}


