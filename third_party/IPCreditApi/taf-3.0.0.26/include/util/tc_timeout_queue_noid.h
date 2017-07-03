#ifndef _TAF_TIMEOUT_QUEUE_NOID_H_
#define _TAF_TIMEOUT_QUEUE_NOID_H_

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
 * @file tc_timeout_queue_noid.h
 * @brief ��ʱ����
 *
 * @author zhangcunli@tencent.com
 */
/////////////////////////////////////////////////

template<class T>
class TC_TimeoutQueueNoID
{
public:

    struct PtrInfo;
    struct NodeInfo;

    typedef multimap<int64_t,NodeInfo>      time_type;
    typedef list<PtrInfo>                   list_type;

    struct PtrInfo
    {
        T ptr;
        typename time_type::iterator timeIter;
    };

    struct NodeInfo
    {
        typename list_type::iterator listIter;
    };

    /**
	 * @brief ��ʱ���У�ȱʡ5s��ʱ.
	 *
     * @param timeout ��ʱ�趨ʱ��
     * @param size
     */
    TC_TimeoutQueueNoID()
    {
    }

    /**
	 * @brief ȡ�����е�һ������
	 *
     * @param ptr  ȡ������ 
     * @return true  �ɹ� false ʧ��
     */
    bool pop(T & ptr);
    bool pop();

    /**
	 * @brief ��ȡ���е�һ������
	 *
     * @param ptr   ����
     * @return true  �ɹ� false ʧ��
     */
    bool front(T & ptr);

    /**
	 * @brief ������Ϣ������β��.
	 *
     * @param ptr        Ҫ���뵽����β�˵���Ϣ
     * @param timeout    ��ʱʱ�� ����ʱ��
     * @return true  �ɹ� false ʧ��
     */
    bool push(T& ptr, int64_t timeout);

    /**
     * @brief ��ʱɾ������
     */
    void timeout();

    /**
     * @brief ��ʱɾ������
     */
    bool timeout(T & t);

    /**
     * @brief �Ƿ�Ϊ��
     */
    bool empty() const
    {
        return _list.empty();
    }

    /**
     * @brief �����е�����.
     *
     * @return size_t
     */
    size_t size() const { return _list.size(); }

protected:
    time_type                       _time;
    list_type                       _list;
};


template<typename T> bool TC_TimeoutQueueNoID<T>::front(T & t)
{
    if(_list.empty())
    {
        return false;
    }

    PtrInfo & pi = _list.back();

    t = pi.ptr;

    return true;
}

template<typename T> bool TC_TimeoutQueueNoID<T>::pop()
{
    T t;
    return pop(t);
}

template<typename T> bool TC_TimeoutQueueNoID<T>::pop(T & t)
{
    if(_list.empty())
    {
        return false;
    }

    PtrInfo & pi = _list.back();

    t = pi.ptr;

	_time.erase(pi.timeIter);
    _list.pop_back();

    return true;
}


template<typename T> bool TC_TimeoutQueueNoID<T>::push(T& ptr, int64_t timeout)
{
    PtrInfo pi;
    pi.ptr = ptr;

    _list.push_front(pi);

    PtrInfo & pinfo = _list.front();

    NodeInfo stNodeInfo;
    stNodeInfo.listIter = _list.begin();
    pinfo.timeIter = _time.insert(make_pair(timeout,stNodeInfo));

    return true;
}

template<typename T> void TC_TimeoutQueueNoID<T>::timeout()
{
    int64_t iNow = TNOWMS;
    while(true)
    {
        typename time_type::iterator it = _time.begin();

        if(_time.end() == it || it->first>iNow)
            break;

        _list.erase(it->second.listIter);
        _time.erase(it);
    }
}

template<typename T> bool TC_TimeoutQueueNoID<T>::timeout(T & t)
{
    if(_time.empty())
    {
        return false;
    }

    typename time_type::iterator it = _time.begin();
    if(it->first > TNOWMS)
    {
        return false;
    }

    t=it->second.listIter->ptr;

    _list.erase(it->second.listIter);

    _time.erase(it);

    return true;
}
/////////////////////////////////////////////////////////////////
}
#endif
