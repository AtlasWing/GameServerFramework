#ifndef GAME_NMAPSERVICE_H
#define GAME_NMAPSERVICE_H

#include "base/nXMLParser.h"
#include "base/nManager.h"
#include "base/nBaseMonitorService.h"

#include "protocol/commonType.h"

#include "game/nTable.h"
#include "game/nTableManager.h"
#include "game/nGameServer.h"
#include "game/nBaseMap.h"

namespace Game
{
    template <typename T>
        class nMapService : public Tidy::nManager<true, Tidy::nIndexByID< Tidy::CommonGetIDPolicy<T> > > 
    {   
        private:
            typedef Tidy::nIndexByID< Tidy::CommonGetIDPolicy<T> > IDIndex;

        protected:
            nMapService()
            {
            }
            ~nMapService()
            {
            }

        public:
            bool init()
            {
                std::vector<const Game::SceneTable*>* sceneTableVec = Game::TableManager::GetTable<const Game::SceneTable>();
                if(sceneTableVec == NULL){
                    return true;
                }
                for(std::vector<const Game::SceneTable*>::iterator iter = sceneTableVec->begin(); iter != sceneTableVec->end(); ++ iter){
                    if((*iter)->SceneType != eSceneType_PVP){
                        continue;
                    }
                    T *map = new T((*iter));
                    if(map->getID() == 0){
                        assert_fail("加载地图失败");
                        SAFE_DELETE(map);
                        return false;
                    }
                    if(map->init() == false){
                        assert_fail("加载地图失败[%u]", map->getID());
                        SAFE_DELETE(map);
                        return false;
                    }
                    if(add(map) == false){
                        assert_fail("地图重复添加[%u]", map->getID());
                        SAFE_DELETE(map);
                        return false;
                    }
                }
                return true;
            }
        public:
            /// @brief 根据ID索引获得地图
            T * getMapByID(DWORD id) const
            {   
                return IDIndex::getByID(id);
            }
        public:
            /// @brief 地图回调
            template <class ExecT>
                bool execEveryMap(ExecT &exec)
                {
                    return execAll(exec);
                }
    };
};

#endif
