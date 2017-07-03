#ifndef ZONESERVER_ZOMAPSERVICE_H_
#define ZONESERVER_ZOMAPSERVICE_H_

#include "game/nMapService.h"
class zoMap;
class zoUser;

class zoMapService : public Game::nMapService<zoMap>
{
    public:
        zoMapService();
        ~zoMapService();

    public:
        bool initService();
        bool finalService();

};
#endif
