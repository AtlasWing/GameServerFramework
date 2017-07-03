#include "zoneServer/zoMapService.h"
#include "zoneServer/zoMap.h"
#include "zoneServer/zoUser.h"
#include "zoneServer/Relation/zoRelationUser.h"

zoMapService::zoMapService() :
	Game::nMapService<zoMap>()
{
}

zoMapService::~zoMapService()
{
}

bool zoMapService::initService()
{
    if(Game::nMapService<zoMap>::init() == false){
        return false;
    }
    return true;
}

bool zoMapService::finalService()
{
    return true;
}
