#ifndef _DT_GM_COMMAND_H_
#define _DT_GM_COMMAND_H_

#include "game/nGMCommand.h"

class dtGMCommand : public Game::nGMCommand
{
    public:
        virtual void registerCmdFunc(); 
    protected:
        bool handleFetchMail(std::string param);
};

#endif
