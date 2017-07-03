#ifndef _ZONE_TASK_CONN_H_
#define _ZONE_TASK_CONN_H_

#include "base/nTaskConn.h"

namespace protocol
{
    namespace ServerRun
    {
        struct ServerEntity;
    };
};

class zoTaskConn : public Tidy::nTaskConnQueue
{
    public:
        enum zoTaskServerState
        {
            eServerState_WaitOtherStart,
            eServerState_WaitOtherRet,
            eServerState_WaitMeStart,
            eServerState_OK
        };
    public:
        zoTaskConn(const int sock, const struct sockaddr *addr, const Tidy::SocketPortData* listenPortData);
        virtual ~zoTaskConn();

    public:
        void setSyncState(const zoTaskServerState state);
        BYTE getSyncState() const;
        bool isInSyncState(const zoTaskServerState state);

    public:
        bool checkDependStart();
        bool checkDependServerRespond();
        void addNotifyOtherRespond(Tidy::nServerConn* conn);

    public:
        void setServerEntity(const protocol::ServerRun::ServerEntity* entity);
        const protocol::ServerRun::ServerEntity* getServerEntity() const;

    public:
        bool notifyMeInit();
        bool notifyMeStart();

    private:
        bool notifyDependServer();
        void loadDependList() const;

    private:
        mutable Tidy::vector<const protocol::ServerRun::ServerEntity*> _depend_server_list;
        WORD _respond_server_num;
        mutable bool _load_depend_list;
        const protocol::ServerRun::ServerEntity* _server_entity;

    private:
        Tidy::nTimer _sync_wait_depend_respond_timer;
        BYTE _sync_state;
};

#endif
