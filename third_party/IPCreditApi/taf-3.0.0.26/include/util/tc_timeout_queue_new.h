#ifndef _TAF_TIMEOUT_QUEUE_NEW_H_
#define _TAF_TIMEOUT_QUEUE_NEW_H_

#include <map>
#include <list>
#include <ext/hash_map>
#include <iostream>
#include <cassert>
#include "util/tc_autoptr.h"
#include "util/tc_monitor.h"
#include "util/tc_functor.h"
#include "util/tc_timeprovider.h"

using namespace std;
using namespace __gnu_cxx;

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

template<class T>
class TC_TimeoutQueueNew
{
public:

    struct PtrInfo;
    struct NodeInfo;
    struct SendInfo;

    typedef hash_map<uint32_t, PtrInfo>     data_type;
    typedef multimap<int64_t,NodeInfo>      time_type;
    typedef list<SendInfo>                  send_type;

    typedef TC_Functor<void, TL::TYPELIST_1(T&)> data_functor;

    struct PtrInfo
    {
        T ptr;
        bool hasSend;
        typename time_type::iterator timeIter;
        typename send_type::iterator sendIter;
    };

    struct NodeInfo
    {
        typename data_type::iterator dataIter;
    };

    struct SendInfo
    {
        typename data_type::iterator dataIter;
    };
    /**
	 * @brief ��ʱ���У�ȱʡ5s��ʱ.
	 *
     * @param timeout ��ʱ�趨ʱ��
     * @param size
     */
    TC_TimeoutQueueNew(int timeout = 5*1000,size_t size = 100 ) : _uniqId(0)
    {
        _data.resize(size);
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
        return _send.empty();
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
        return _send.size();
    }

    /**
	 * @brief ��ȡָ��id������.
	 *
     * @param id ָ�������ݵ�id
     * @param T ָ��id������
     * @return bool get�Ľ��
     */
    bool get(uint32_t uniqId, T & t,bool bErase = true);

    /**
	 * @brief ɾ��.
	 *
	 * @param uniqId Ҫɾ�������ݵ�id
     * @param T     ��ɾ��������
     * @return bool ɾ�����
     */
    bool erase(uint32_t uniqId, T & t);

    /**
	 * @brief ������Ϣ������β��.
	 *
     * @param ptr        Ҫ���뵽����β�˵���Ϣ
     * @param uniqId     ���к�
     * @param timeout    ��ʱʱ��
     * @return true  �ɹ� false ʧ��
     */
    bool push(T& ptr, uint32_t uniqId,int64_t timeout,bool hasSend = true);

    /**
     * @brief ��ʱɾ������
     */
    void timeout();

    /**
     * @brief ��ʱɾ������
     */
    bool timeout(T & t);

    /**
	 * @brief ɾ����ʱ�����ݣ�����df������������
     */
    void timeout(data_functor &df);

    /**
     * @brief �����е�����.
     *
     * @return size_t
     */
    size_t size() const { return _data.size(); }

protected:
    uint32_t                        _uniqId;
    data_type                       _data;
    time_type                       _time;
    send_type                       _send;
};

template<typename T> bool TC_TimeoutQueueNew<T>::getSend(T & t)
{
    //����Ϊ�շ���ʧ��
    if(_send.empty())
    {
        return false;
    }

    SendInfo & stSendInfo = _send.back();
    assert(!stSendInfo.dataIter->second.hasSend);
    t = stSendInfo.dataIter->second.ptr;
    return true;
}


template<typename T> void TC_TimeoutQueueNew<T>::popSend(bool del)
{
    assert(!_send.empty());
    SendInfo stSendInfo;
    stSendInfo = _send.back();
    stSendInfo.dataIter->second.hasSend = true;
    if(del)
    {
        _time.erase(stSendInfo.dataIter->second.timeIter);
        _data.erase(stSendInfo.dataIter);
    }
    _send.pop_back();
}

template<typename T> bool TC_TimeoutQueueNew<T>::get(uint32_t uniqId, T & t, bool bErase)
{
    typename data_type::iterator it = _data.find(uniqId);

    if(it == _data.end())
    {
        return false;
    }

    t = it->second.ptr;

    if(bErase)
    {
        _time.erase(it->second.timeIter);
        if(!it->second.hasSend)
        {
            _send.erase(it->second.sendIter);
        }
        _data.erase(it);
    }

    return true;
}

template<typename T> uint32_t TC_TimeoutQueueNew<T>::generateId()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    while (++_uniqId == 0);

    return _uniqId;
}

template<typename T> bool TC_TimeoutQueueNew<T>::push(T& ptr, uint32_t uniqId,int64_t timeout,bool hasSend)
{
    PtrInfo pi;
    pi.ptr = ptr;
    pi.hasSend = hasSend;

    pair<typename data_type::iterator, bool> result;
    result = _data.insert(make_pair(uniqId, pi));
    if (result.second == false) return false;

    NodeInfo stNodeInfo;
    stNodeInfo.dataIter = result.first;
    result.first->second.timeIter = _time.insert(make_pair(timeout,stNodeInfo));

    //û�з��ͷŵ�list��������
    if(!hasSend)
    {
        SendInfo stSendInfo;
        stSendInfo.dataIter = result.first;
        _send.push_front(stSendInfo);
        result.first->second.sendIter = _send.begin();
    }

    return true;
}

template<typename T> void TC_TimeoutQueueNew<T>::timeout()
{
    int64_t iNow = TNOWMS;
    while(true)
    {
        typename time_type::iterator it = _time.begin();

        if(_time.end() == it || it->first>iNow)
            break;

        if(!it->second.dataIter->second.hasSend)
        {
            _send.erase(it->second.dataIter->second.sendIter);
        }
        _data.erase(it->second.dataIter);
        _time.erase(it);
    }
}

template<typename T> bool TC_TimeoutQueueNew<T>::timeout(T & t)
{
    int64_t iNow = TNOWMS;
    if(_time.empty())
        return false;
    typename time_type::iterator it = _time.begin();
    if(it->first>iNow)
        return false;

    t=it->second.dataIter->second.ptr;
    if(!it->second.dataIter->second.hasSend)
    {
        _send.erase(it->second.dataIter->second.sendIter);
    }
    _data.erase(it->second.dataIter);
    _time.erase(it);
    return true;
}

template<typename T> void TC_TimeoutQueueNew<T>::timeout(data_functor &df)
{
    while(true)
    {
        int64_t iNow = TNOWMS;
        T ptr;
        typename time_type::iterator it = _time.begin();
        if(_time.end() == it || it->first>iNow)
            break;

        ptr=it->second->second.ptr;
        if(!it->second.dataIter->second.hasSend)
        {
            _send.erase(it->second.dataIter->second.sendIter);
        }
        _data.erase(it->second.dataIter);
        _time.erase(it);

        try { df(ptr); } catch(...) { }
    }
}

template<typename T> bool TC_TimeoutQueueNew<T>::erase(uint32_t uniqId, T & t)
{
    typename data_type::iterator it = _data.find(uniqId);

    if(it == _data.end())
        return false;

    t = it->second.ptr;
    if(!it->second.hasSend)
    {
        _send.erase(it->second.sendIter);
    }
    _time.erase(it->second.timeIter);
    _data.erase(it);

    return true;
}
/////////////////////////////////////////////////////////////////
}
#endif
