#ifndef GAME_EVENT_H_
#define GAME_EVENT_H_

#include "base/nAssert.h"

enum eServerEventID
{
    eServerEventID_User = 0,
    eServerEventID_Script = 7 * 1024,
    eServerEventID_Max
};

enum eUserEventType
{
    eUserEventType_UseItem        = 0,
    eUserEventType_GetItem        = 1,
    eUserEventType_RemoveItem     = 2,
    eUserEventType_UnlockFunction = 3,
    eUserEventType_Max
};

#endif
