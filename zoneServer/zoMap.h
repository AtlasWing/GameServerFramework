#ifndef _ZONESERVER_ZOMAP_H
#define _ZONESERVER_ZOMAP_H

#include "game/nBaseMap.h"
#include "game/nTable.h"
#include "game/nTableManager.h"

class zoMap : public Game::nBaseMap
{
    public:
        zoMap(const Game::SceneTable *sceneData);
        ~zoMap();
};


#endif
