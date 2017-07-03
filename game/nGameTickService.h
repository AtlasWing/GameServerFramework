#ifndef GAME_NGAME_TICKSERVICE
#define GAME_NGAME_TICKSERVICE

#include "base/nBaseTickService.h"

namespace Game{
    class nGameTickService : public Tidy::nBaseTickService
    {
        public:
            nGameTickService();
            ~nGameTickService();

        private:
            virtual void run();
    };
};

#endif
