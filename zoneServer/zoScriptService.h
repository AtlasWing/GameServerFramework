#ifndef INFOSERVER_INSCRIPT_SERVICE_H
#define INFOSERVER_INSCRIPT_SERVICE_H

#include "base/nLuaState.h"

class zoScriptService : public Tidy::nLuaState
{
    public:
        zoScriptService();
        ~zoScriptService();

    public:
        bool initService();
        void finalService();
};
#endif

