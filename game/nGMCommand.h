#ifndef _N_GM_COMMAND_H_
#define _N_GM_COMMAND_H_

#include "base/nParamMap.h"

namespace Game
{
    enum eGMLevel
    {
        eGMLevel_None   = 0,    ///< None 
        eGMLevel_Normal = 1,    ///< 普通GM指令 
        eGMLevel_Super  = 2,    ///< 超级GM指令 
        eGMLevel_Robot  = 3,    ///< Robot指令 
        eGMLevel_Test   = 4,    ///< 玩家测试GM指令
        eGMLevel_Debug  = 5,    ///< 程序调试GM指令 
        eGMLevel_MAX
    };

    inline bool isGM(const BYTE gmLevel)
    {
        return gmLevel == Game::eGMLevel_Normal || gmLevel == Game::eGMLevel_Super;
    }

    class nGMCommand
    {
        public:
            typedef bool(nGMCommand::*processFunc)(std::string cmdParam);
            #define NewCommandHandler(classAndFunc) reinterpret_cast<processFunc>(&classAndFunc);
            ~nGMCommand()
            {
            }

        public:
            const char* getName() const
            {
                return name;
            }

            const char* getDesc() const
            {
                return desc.c_str();
            }

            const char* getAuthor() const
            {
                return author.c_str();
            }
            virtual void registerCmdFunc() = 0;
            template<typename T> static T* createInstance(void * owner = NULL )
            {   
                T * _ins = new T();
                ((nGMCommand*)_ins)->registerCmdFunc();
                ((nGMCommand*)_ins)->Owner = owner;
                return _ins;
            }   

        public:
            ///< @brief 描述
            std::string desc;
            ///< @brief 作者
            std::string author;
            ///< @brief 权限
            DWORD level;
            ///< @brief 服务器类型
            DWORD serverType;
        public:
            char name[MAX_NAMESIZE];
        protected:
            std::map<int,processFunc> _map_ProcessFunc;
            void * Owner;
    };


};

#endif

