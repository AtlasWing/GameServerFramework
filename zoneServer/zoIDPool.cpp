#include "protocol/adminCommand.h"

#include "zoneServer/zoIDPool.h"
#include "zoneServer/zoServer.h"
#include "zoneServer/zoMessageService.h"

zoIDPool::zoIDPool(const DWORD idType, const DWORD initStartID, const DWORD maxPoolNum) :
    _id_type(idType),
    _init_start_id(initStartID),
    _max_pool_num(maxPoolNum),
    _cur_pool_num(0),
    _is_wait_return(false)
{
}

zoIDPool::~zoIDPool()
{
    final();
}

const DWORD zoIDPool::getID() const
{
    return _id_type;
}

const bool zoIDPool::requestID(DWORD *id)
{
    if(_reuse_id_set.empty() == false){
        std::set<DWORD>::const_iterator iter = _reuse_id_set.begin();
        *id = *iter;
        _reuse_id_set.erase(iter);
        -- _cur_pool_num;
        return true;
    }
    if(_id_range_list.empty() == true) {
        //assert_debug(_is_wait_return == true, "没有请求");
        return false;
    }
    IDRange& range = _id_range_list.front();
    *id = range.start;
    -- _cur_pool_num;
    ++ range.start;
    if(range.start > range.end) {
        _id_range_list.pop_front();
    }

    checkRequestPool();
    return true;
}

bool zoIDPool::addIDPool(const WORD num, const protocol::admin::Range* data)
{
    //TODO admin一次返回,所有不分批返回
    assert_debug(_is_wait_return == true, "");
    _is_wait_return = false;
    for(WORD i = 0; i < num; ++ i) {
        if(data[i].start > data[i].end) {
            assert_fail("id错误");
            continue;
        }

        _id_range_list.push_back(IDRange(data[i].start, data[i].end));
        _cur_pool_num += (data[i].end - data[i].start + 1);
    }
    return true;
}

bool zoIDPool::reuseID(const DWORD *idList, const DWORD num)
{
    for(DWORD i = 0; i < num; ++ i){
        if(_reuse_id_set.insert(idList[i]).second == false){
            assert_fail("id重复");
            return false;
        }
        ++ _cur_pool_num;
    }
    return true;
}

void zoIDPool::checkRequestPool()
{
    if(_is_wait_return == true)
    {
        return;
    }

    if(_cur_pool_num > (_max_pool_num/3 + 1))
    {
        return;
    }

    /*
    if(server->messageService->hasAdminServerConn() == false)
    {
        return;
    }

    protocol::admin::S2A_RequestIDRangeCommand send;
    send.idType = _id_type;
    send.idNum = _max_pool_num * 2/3;
    send.initStartID = _init_start_id;
    server->messageService->sendCmdToAdminServer(&send, sizeof(send));
    */

    _is_wait_return = true;
}

void zoIDPool::dealAdminConnClose()
{
    if(_is_wait_return == true) {
        _is_wait_return = false;
    }
}

void zoIDPool::final()
{
    BUFFER_PROTO_CMD(protocol::admin::S2A2S_ReturnIDRangeCommand, send);
    send->idType = this->_id_type;
    send->num = 0;
    for(Tidy::list<IDRange>::iterator iter = _id_range_list.begin(); iter != _id_range_list.end(); ++ iter) {
        send->data[send->num].start = (*iter).start;
        send->data[send->num].end = (*iter).end;
        ++ send->num;
    }
    for(std::set<DWORD>::iterator iter = _reuse_id_set.begin(); iter != _reuse_id_set.end(); ){ 
        send->data[send->num].start = *iter;
        send->data[send->num].end = *iter;
        ++ iter;
        while(iter != _reuse_id_set.end() && ((*iter) == (send->data[send->num].end + 1))){
            send->data[send->num].end = *iter;
            ++ iter;
        }
        ++ send->num;
    }
    server->messageService->sendCmdToAdminServer(send, send->size());
    _id_range_list.clear();
}
