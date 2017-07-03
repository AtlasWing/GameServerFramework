#ifndef _N_BASER_MANAGER_H_
#define _N_BASER_MANAGER_H_

#include "base/nMisc.h" 
#include "protocol/baseCommand.h"
#include "base/nServerConn.h"
#include "game/nTable.h"
#include "game/nTableManager.h"
#include "protocol/Proto/UserData.pb.h"
#include "game/nUser.h"

class EventHandler
{

};

struct EventArgs
{
    DWORD lparams;
    DWORD rparams;
    const void * request_msg;
    void * reply_msg;
};

typedef void (EventHandler::* Event)(void * _sender,EventArgs *e); 
#define  NewEventHandler(classAndFunc) reinterpret_cast<Event>(&classAndFunc);

namespace Game
{
    class nBaseManager
    {
        public:
            DISALLOW_COPY_AND_ASSIGN(nBaseManager);
            
            nBaseManager() 
            {
            }
            virtual ~nBaseManager()
            {
            }
            
        public:
            template<typename T, typename userT> static T* getInstance(userT * owner = NULL )
            {
                T * _ins = new T(owner);
                ((nBaseManager*)_ins)->initManager();
                return _ins;
            }
            virtual bool initManager()  = 0;
            virtual bool finalManager() = 0;
            virtual bool doTick()       = 0;
            virtual void roleLevelUp(int currentLevel, int lastLevel){};
            virtual bool parseLoginData(protocol::user::LoginSerializeBinary& serializeBinary) { return true; }
            virtual bool parseProtoCommand(Tidy::nServerConn* serverConn, const protocol::NullProtoCommand* nullCommand, const DWORD len) 
            {
                return true;
            }

    };
};

#endif
