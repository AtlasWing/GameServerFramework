#include "base/nDBConnection.h"
#include "base/nDBConnectionPool.h"

#include "protocol/serverRunCommand.h"

#include "zoneServer/zoTaskConn.h"
#include "zoneServer/zoServer.h"

zoTaskConn::zoTaskConn(const int sock, const struct sockaddr *addr, const Tidy::SocketPortData* listenPortData) : 
	Tidy::nTaskConnQueue(sock, addr, listenPortData), 
	_respond_server_num(0), 
	_load_depend_list(true), 
	_sync_state(eServerState_WaitOtherStart)
{
}

zoTaskConn::~zoTaskConn()
{
}

void zoTaskConn::addNotifyOtherRespond(Tidy::nServerConn* conn)
{
    ++ _respond_server_num;
    assert_debug(_respond_server_num <= _depend_server_list.size(), "");
}

void zoTaskConn::setServerEntity(const protocol::ServerRun::ServerEntity* entity)
{
    _server_entity = entity;
    setServerConnInfo(entity->serverID, entity->serverType, getServerTypeString(entity->serverType));
}

const protocol::ServerRun::ServerEntity* zoTaskConn::getServerEntity() const
{
    return _server_entity;
}

void zoTaskConn::setSyncState(const zoTaskServerState state)
{
    _sync_state = state;
}

BYTE zoTaskConn::getSyncState() const
{
    return _sync_state;
}

bool zoTaskConn::isInSyncState(const zoTaskServerState state)
{
    return _sync_state == state;
}

bool zoTaskConn::checkDependStart()
{
    if(_load_depend_list == true) {
        loadDependList();
        _load_depend_list = false;
    }

    Tidy::nServerConn* taskConn = NULL;
    for(Tidy::vector<const protocol::ServerRun::ServerEntity *>::iterator iter = _depend_server_list.begin(); iter != _depend_server_list.end(); ++ iter)
    {
        taskConn = server->getTaskConnByID((*iter)->serverID);
        if(taskConn == NULL)
        {
            server->info("[%u, %s] 等待: [%u, %s] ", this->getID(), this->getName(), (*iter)->serverID, (*iter)->pstrName);
            return false;
        }
    }

    return notifyDependServer();
}

bool zoTaskConn::notifyDependServer()
{
    //设置定时器,超时就重新等待
    _respond_server_num = 0;
    _sync_wait_depend_respond_timer.reset(8000, server->time());

    protocol::ServerRun::NotifyOtherWaitStartCommand send;
    memcpy(&send.serverEntity, getServerEntity(), sizeof(send.serverEntity));
    send.serverID = this->getID();

    Tidy::nServerConn* taskConn = NULL;
    for(Tidy::vector<const protocol::ServerRun::ServerEntity *>::iterator iter = _depend_server_list.begin(); iter != _depend_server_list.end(); ++ iter)
    {
		assert_debug((*iter)->serverType != ZONESERVER, "[%u]", (*iter)->serverType);
        taskConn = server->getTaskConnByID((*iter)->serverID);
        if(taskConn == NULL) {
            //宕机有可能
            //assert_fail("前面已经检查");
            return false;
        }
        if(taskConn->sendCmd(&send, sizeof(send)) == false)
        {
            return false;
        }
    }

    return true;
}

bool zoTaskConn::notifyMeInit()
{
    protocol::ServerRun::ServerNotifyMeInitCommand send;
    memcpy(&send.serverInfo, _server_entity, sizeof(*_server_entity));
    send.adjustTime = Tidy::nTime::adjustTime;
    return sendTaskCmd(&send, sizeof(send));
}

bool zoTaskConn::notifyMeStart()
{
    Tidy::vector<protocol::ServerRun::ServerEntity> serverEntityVec;
    server->getAllTaskConnEntity(serverEntityVec);

    BUFFER_PROTO_CMD(protocol::ServerRun::ServerNotifyMeStartCommand, send);
    send->num = serverEntityVec.size();
    memcpy(send->data, &serverEntityVec[0], send->num * sizeof(send->data[0]));
    for(int i=0; i < send->num; ++i){
        server->debug("将我[%d,%s]启动的消息发送给:[%d,%d,%s]", this->getID(), this->getName(), serverEntityVec[i].serverID, serverEntityVec[i].serverType, serverEntityVec[i].pstrName);
    }
    return sendTaskCmd(send, send->size());
}

bool zoTaskConn::checkDependServerRespond()
{
    // TODO 正常情况下是等于,在服务器不断重启时,可能出现小于的情况
    if(_depend_server_list.size() <= _respond_server_num)
    {
        return true;
    }

    if(_sync_wait_depend_respond_timer.isReady(server->time()) == true)
    {
        notifyDependServer();
    }

    return false;
}

namespace
{
};
void zoTaskConn::loadDependList() const
{
    server->getDependServerEntityList(this->getServerType(), &_depend_server_list);
}
