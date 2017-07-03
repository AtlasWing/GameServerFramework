#ifndef _TAF_TIMEOUT_QUEUE_MAP_H_
#define _TAF_TIMEOUT_QUEUE_MAP_H_

#include <iostream>
#include <cassert>
#include "util/tc_timeprovider.h"

using namespace std;

namespace taf
{
/////////////////////////////////////////////////
/**
 * @file tc_timeout_queue.h
 * @brief ��ʱ����(ģ��Ԫ��ֻ��������ָ��).
 *
 * @author coonzhang@tencent.com
 */
/////////////////////////////////////////////////

//��ʱʱ�侫ȷ��λ��100ms
#define MAX_TIME_OUT    60000

template<class T>
class TC_TimeoutQueueMap
{
public:

    struct PtrInfo;
    struct NodeInfo;
    struct SendInfo;

    struct NodeInfo
    {
        NodeInfo()
        :id(0)
        ,free(true)
        ,noSendNext(0)
        ,noSendPrev(0)
        ,noSendPrevEnd(false)
        ,timeoutNext(0)
        ,timeoutPrev(0)
        ,timeoutPrevEnd(false)
        ,freeNext(0)
        {
        }
        //request id
        uint16_t    id;

        //�Ƿ����
        bool        free;

        bool        hasSend;

        //δ����������һ��id
        uint16_t    noSendNext; 
        //δ��������ǰһ��id
        uint16_t    noSendPrev;
        //���Ϊfalse noSendPrev��ֵ��requestid
        //���Ϊtrue noSendPrev��ֵ��
        bool        noSendPrevEnd;

        //��ʱ�������һ��id
        uint16_t    timeoutNext;
        //��ʱ�����ǰһ��id
        uint16_t    timeoutPrev;
        //���Ϊfalse timeoutPrev��ֵ��requestid
        //���Ϊtrue timeoutPrev��ֵ��timeout�����id
        bool        timeoutPrevEnd;

        //�����������һ��id
        uint16_t    freeNext;

        T           ptr;
    };

    /**
	 * @brief ��ʱ���У�ȱʡ5s��ʱ.
	 *
     * @param timeout ��ʱ�趨ʱ��
     * @param size
     */
    TC_TimeoutQueueMap()
    :_noSendHead(0)
    ,_noSendTail(0)
    ,_noSendSize(0)
    ,_freeHead(1)
    ,_timeoutPtr(0)
    ,_timeoutPtrTime(0)
    {
        _dataSize = 51200;
        _data = NULL;
        _data = new NodeInfo[_dataSize];
        assert(NULL != _data);

        for(uint16_t i = 1; i < _dataSize; ++i)
        {
            _data[i].id = i;
            _data[i].free = true;
            _data[i].noSendNext = 0; 
            _data[i].noSendPrev = 0;
            _data[i].timeoutNext = 0;
            _data[i].timeoutPrev = 0;
            _data[i].freeNext = i+1;
        }
        //���free�����nextҪ�ĳ�0
        _data[_dataSize-1].freeNext = 0;
        _freeHead = 1;
        _freeTail = _dataSize-1;

        _timeoutSize = MAX_TIME_OUT >> 5;
        _timeoutHead = NULL;
        _timeoutHead = new uint16_t[_timeoutSize];

        for(uint16_t i = 0; i < _timeoutSize; ++i)
        {
            _timeoutHead[i] = 0;
        }

        _timeoutPtr = 0;
        _timeoutPtrTime = 0;
    }

    /**
     * @brief  �����ö��е���һ��ID
     */
    uint32_t generateId();

    /**
     * Ҫ���͵������Ƿ�Ϊ��
     */
    bool sendListEmpty()
    {
        return (0 == _noSendHead);
    }

    /**
     * ��ȡҪ���͵�����
     */
    bool getSend(T & t);

    /**
     * ���Ѿ����͵����ݴ�list����ɾ��
     */
    void popSend(bool del = false);

    /**
     *��ȡҪ����list��size
     */
    size_t getSendListSize()
    {
        return _noSendSize;
    }

    /**
	 * @brief ��ȡָ��id������.
	 *
     * @param id ָ�������ݵ�id
     * @param T ָ��id������
     * @return bool get�Ľ��
     */
    bool get(uint16_t uniqId, T & t,bool bErase = true);

    /**
	 * @brief ɾ��.
	 *
	 * @param uniqId Ҫɾ�������ݵ�id
     * @param T     ��ɾ��������
     * @return bool ɾ�����
     */
    bool erase(uint16_t uniqId, T & t);

    /**
	 * @brief ������Ϣ������β��.
	 *
     * @param ptr        Ҫ���뵽����β�˵���Ϣ
     * @param uniqId     ���к�
     * @param timeout    ��ʱʱ��
     * @return true  �ɹ� false ʧ��
     */
    uint16_t push(T& ptr, uint16_t id, int64_t timeout,bool hasSend = true);

    /**
     * @brief ��ʱɾ������
     */
    bool timeout(T & t);

    /**
     * @brief �����е�����.
     *
     * @return size_t
     */
    size_t size() const { return _data.size(); }
private:
    void delFromTimeout(uint16_t id);
    void delFromData(uint16_t id);
    void delFromNoSend(uint16_t id);

protected:
    uint32_t                        _uniqId;

    uint16_t    _dataSize;

    //δ�����б��head
    uint16_t    _noSendHead; 
    uint16_t    _noSendTail;

    uint16_t    _noSendSize;

    uint16_t *  _timeoutHead;
    uint16_t    _timeoutSize;

    uint16_t    _freeHead;
    uint16_t    _freeTail;

    uint16_t    _timeoutPtr;
    int64_t     _timeoutPtrTime;

    NodeInfo *  _data;
};

template<typename T> bool TC_TimeoutQueueMap<T>::getSend(T & t)
{
    //����Ϊ�շ���ʧ��
    if(0 == _noSendHead)
    {
        return false;
    }

    assert(!_data[_noSendHead].hasSend);
    assert(!_data[_noSendHead].free);
    t = _data[_noSendHead].ptr;
    return true;
}


template<typename T> void TC_TimeoutQueueMap<T>::popSend(bool del)
{
    assert(0 != _noSendHead);
    assert(!_data[_noSendHead].hasSend);
    assert(!_data[_noSendHead].free);

    uint16_t popId = _noSendHead;

    //�ó��Ѿ�����
    _data[popId].hasSend = true;

    //��nosend ����ɾ��
    delFromNoSend(popId);

    if(del)
    {
        delFromTimeout(popId);
        delFromData(popId);
    }
}

template<typename T> bool TC_TimeoutQueueMap<T>::get(uint16_t id, T & t, bool bErase)
{
    assert(id < _dataSize);
    if(id >= _dataSize)
    {
        return false;
    }

    if(_data[id].free)
    {
        return false;
    }

    t = _data[id].ptr;

    if(bErase)
    {
        delFromTimeout(id);
        delFromData(id);
    }

    return true;
}

template<typename T> uint32_t TC_TimeoutQueueMap<T>::generateId()
{
    if(0 == _freeHead)
    {
        return 0;
    }
    assert(0 != _freeTail);

    //��free������һ��
    uint16_t id = _freeHead;
    NodeInfo & node = _data[_freeHead];
    assert(node.free);
    node.free = false;

    //�޸�free����
    _freeHead = node.freeNext;
    if(0 == _freeHead)
    {
        _freeTail = 0;
    }
    //cerr<<"generateId:"<<id<<endl;
    return id;
}

template<typename T> uint16_t TC_TimeoutQueueMap<T>::push(T& ptr, uint16_t id, int64_t timeout, bool hasSend)
{
    //cerr<<"push:"<<id<<endl;
    assert(id < _dataSize);
    if(id >= _dataSize)
    {
        return false;
    }

    if(_data[id].free)
    {
        return false;
    }

    //ʱ������ ��ʼ��
    int64_t timeoutSelf = ( timeout >> 6);
    if(0 == _timeoutPtrTime)
    {
        assert(0 == _timeoutPtr);
        _timeoutPtrTime = (TNOWMS >> 6);
    }

    if(timeoutSelf <= _timeoutPtrTime)
    {
        assert(false);
        return 0;
    }

    //��鳬ʱʱ���Ƿ�Ϸ�
    if((timeoutSelf - _timeoutPtrTime) >= (int64_t)_timeoutSize)
    {
        assert(false);
        return 0;
    }

    //��free������һ��
#if 0
    uint16_t id = _freeHead;
    NodeInfo & node = _data[_freeHead];
    assert(node.free);
    node.ptr = ptr;
    node.hasSend = hasSend;
    node.free = false;

    //�޸�free����
    _freeHead = node.freeNext;
    if(0 == _freeHead)
    {
        _freeTail = 0;
    }
#endif
    NodeInfo & node = _data[id];
    node.ptr = ptr;
    node.hasSend = hasSend;

    //���뵽ʱ����������
    uint16_t timeoutPtr;
    timeoutPtr = (uint16_t)(timeoutSelf - _timeoutPtrTime) + _timeoutPtr;
    timeoutPtr = timeoutPtr % _timeoutSize;

    if(0 == _timeoutHead[timeoutPtr])
    {
        node.timeoutNext = 0;
    }
    else
    {
        uint16_t nextId = _timeoutHead[timeoutPtr];
        assert(_data[nextId].timeoutPrevEnd);
        _data[nextId].timeoutPrev = id;
        _data[nextId].timeoutPrevEnd = false;

        _data[id].timeoutNext = nextId;
    }
    node.timeoutPrevEnd = true;
    node.timeoutPrev = timeoutPtr;
    _timeoutHead[timeoutPtr] = id;
    
    //û�з��ͷŵ�list��������
    if(!hasSend)
    {
        _noSendSize ++;
        if(0 == _noSendTail)
        {
            assert(0 == _noSendHead);
            _noSendHead = id;
            _data[id].noSendPrev = 0;
        }
        else
        {
            assert(0 != _noSendHead);
            _data[_noSendTail].noSendNext = id;
            _data[id].noSendPrev = _noSendTail;
        }
        _data[id].noSendNext = 0;
        _noSendTail = id;
    }

    return id;
}

template<typename T> bool TC_TimeoutQueueMap<T>::timeout(T & t)
{
    if(0 == _timeoutPtrTime)
    {
        return false;
    }

    int64_t nowSelf = TNOWMS >> 6;

    while(true)
    {
        if(_timeoutPtrTime >= nowSelf)
        {
            return false;
        }
        if(0 == _timeoutHead[_timeoutPtr])
        {
            _timeoutPtr++;
            _timeoutPtrTime ++;
            if(_timeoutSize == _timeoutPtr)
            {
                _timeoutPtr = 0;
            }
        }
        else
        {
            uint16_t id = _timeoutHead[_timeoutPtr];
            NodeInfo & node = _data[id];
            t = node.ptr;
            if(!node.hasSend)
            {
                delFromNoSend(id);
            }
            delFromTimeout(id);
            delFromData(id);
            return true;
        }
    }
    return true;
}

template<typename T> bool TC_TimeoutQueueMap<T>::erase(uint16_t id, T & t)
{
    //cerr<<"line:"<<__LINE__<<" erase:"<<id<<endl;
    assert(id < _dataSize);
    if(id >= _dataSize)
    {
        return false;
    }

    if(_data[id].free)
    {
        return false;
    }

    t = _data[id].ptr;
    if(!_data[id].hasSend)
    {
        delFromNoSend(id);
    }
    delFromTimeout(id);
    delFromData(id);

    return true;
}

template<typename T> void TC_TimeoutQueueMap<T>::delFromTimeout(uint16_t delId)
{
    //�ӳ�ʱ����ɾ��
    if(_data[delId].timeoutPrevEnd)
    {
        //cerr<<"line:"<<__LINE__<<endl;
        uint16_t nextId;
        uint16_t timeoutId;
        timeoutId = _data[delId].timeoutPrev;
        nextId = _data[delId].timeoutNext;

        _timeoutHead[timeoutId] = nextId;
        if(0 != nextId)
        {
            _data[nextId].timeoutPrevEnd = true;
            _data[nextId].timeoutPrev = timeoutId;
        }
    }
    else
    {
        //cerr<<"line:"<<__LINE__<<endl;
        uint16_t prevId,nextId;
        prevId = _data[delId].timeoutPrev;
        nextId = _data[delId].timeoutNext;

        assert(0 != prevId);
        _data[prevId].timeoutNext = nextId;
        if(0 != nextId)
        {
            _data[nextId].timeoutPrev = prevId;
        }
    }
}

template<typename T> void TC_TimeoutQueueMap<T>::delFromData(uint16_t id)
{
    //�����������
    _data[id].freeNext = 0;
    if(0 == _freeTail)
    {
        assert(0 == _freeHead);
        _freeHead = id;
    }
    else
    {
        assert(0 != _freeHead);
        _data[_freeTail].freeNext = id;
    }
    _freeTail = id;
    _data[id].free = true;
}

template<typename T> void TC_TimeoutQueueMap<T>::delFromNoSend(uint16_t id)
{
    uint16_t prevId,nextId;
    prevId = _data[id].noSendPrev;
    nextId = _data[id].noSendNext;

    assert(_noSendSize>0);
    _noSendSize --;

    if(0 == prevId)
    {
        _noSendHead = nextId;
    }
    else
    {
        _data[prevId].noSendNext = nextId;
    }

    if(0 == nextId)
    {
        _noSendTail = 0;
    }
    else
    {
        _data[nextId].noSendPrev = prevId;
    }
}
/////////////////////////////////////////////////////////////////
}
#endif
