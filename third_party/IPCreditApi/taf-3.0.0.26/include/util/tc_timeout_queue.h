#ifndef _TAF_TIMEOUT_QUEUE_H_
#define _TAF_TIMEOUT_QUEUE_H_

#include <deque>
//#include <map>
#include <ext/hash_map>
#include <iostream>
#include <list>
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
class TC_TimeoutQueue: public TC_ThreadMutex, public TC_HandleBase
{
public:

    struct PtrInfo;

    struct NodeInfo;

   // typedef map<uint32_t, PtrInfo> data_type;
    typedef hash_map<uint32_t, PtrInfo> data_type;

    typedef list<NodeInfo>         time_type;

    typedef TC_Functor<void, TL::TYPELIST_1(T&)> data_functor;

    struct PtrInfo
    {
        T ptr;

        typename time_type::iterator timeIter;
    };

    struct NodeInfo
    {
        bool hasPoped;

        int64_t createTime;

        typename data_type::iterator dataIter;
    };

    /**
	 * @brief ��ʱ���У�ȱʡ5s��ʱ. 
	 *  
     * @param timeout ��ʱ�趨ʱ��
     * @param size
     */
    TC_TimeoutQueue(int timeout = 5*1000,size_t size = 100 ) : _uniqId(0), _timeout(timeout) 
    {
        _firstNoPopIter=_time.end();
        _data.resize(size);
    }

    /**
     * @brief  �����ö��е���һ��ID
     */
    uint32_t generateId();

    /**
	 * @brief ��ȡָ��id������. 
	 *  
     * @param id ָ�������ݵ�id
     * @return T ָ��id������
     */
    T get(uint32_t uniqId, bool bErase = true);

    /**
	 * @brief ɾ��. 
	 *  
	 * @param uniqId Ҫɾ�������ݵ�id
     * @return T     ��ɾ��������
     */
    T erase(uint32_t uniqId);

    /**
	 * @brief ������Ϣ������β��. 
	 *  
     * @param ptr        Ҫ���뵽����β�˵���Ϣ
     * @return uint32_t id��
     */
    bool push(T& ptr, uint32_t uniqId);

    /**
     * @brief ��ʱɾ������
     */
    void timeout();

    /**
	 * @brief ɾ����ʱ�����ݣ�����df������������ 
     */
    void timeout(data_functor &df);

    /**
	 * @brief ȡ������ͷ������Ϣ. 
	 *  
     * @return T ����ͷ������Ϣ
     */
    T pop();

    /**
     * @brief ȡ������ͷ������Ϣ(����һ��copy).
     * 
     * @param t 
     */
    bool pop(T &t);

    /**
	 * @brief ��������. 
	 *  
     * @param q
     * @return bool
     */
    bool swap(deque<T> &q);

    /**
	 * @brief ���ó�ʱʱ��(����).
	 *  
     * @param timeout
     */
    void setTimeout(int timeout) { _timeout = timeout; }

    /**
     * @brief �����е�����.
     * 
     * @return size_t
     */
    size_t size() const { TC_LockT<TC_ThreadMutex> lock(*this); return _data.size(); }

protected:
    uint32_t                        _uniqId;
    time_t                          _timeout;
    data_type                       _data;
    time_type                       _time;
    typename time_type::iterator    _firstNoPopIter;
};

template<typename T> T TC_TimeoutQueue<T>::get(uint32_t uniqId, bool bErase)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    typename data_type::iterator it = _data.find(uniqId);

    if(it == _data.end())
    {
        return NULL;
    }

    T ptr = it->second.ptr;

    if(bErase)
    {
        if(_firstNoPopIter == it->second.timeIter)
        {
            ++_firstNoPopIter;
        }
        _time.erase(it->second.timeIter);
        _data.erase(it);
    }

    return ptr;    
}

template<typename T> uint32_t TC_TimeoutQueue<T>::generateId()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    while (++_uniqId == 0);

    return _uniqId;
}

template<typename T> bool TC_TimeoutQueue<T>::push(T& ptr, uint32_t uniqId)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    PtrInfo pi;

    pi.ptr = ptr;

    pair<typename data_type::iterator, bool> result;
   
    result = _data.insert(make_pair(uniqId, pi));

    if (result.second == false) return false;

    typename data_type::iterator it = result.first;

    NodeInfo ni;

    struct timeval tv;
    TC_TimeProvider::getInstance()->getNow(&tv);

    ni.createTime = tv.tv_sec * (int64_t)1000 + tv.tv_usec/1000;

    ni.dataIter = it;

    ni.hasPoped = false;

    _time.push_back(ni);

    typename time_type::iterator tmp = _time.end();

    --tmp;

    it->second.timeIter = tmp;

    if (_firstNoPopIter == _time.end())
    {
        _firstNoPopIter = tmp;
    }

    return true;
}

template<typename T> void TC_TimeoutQueue<T>::timeout()
{
    struct timeval tv;
    TC_TimeProvider::getInstance()->getNow(&tv);

    while(true)
    {
        TC_LockT<TC_ThreadMutex> lock(*this);

        typename time_type::iterator it = _time.begin();

        if(it != _time.end() && tv.tv_sec*(int64_t)1000+tv.tv_usec/1000-it->createTime>_timeout)
        {
            _data.erase(it->dataIter);

            if(_firstNoPopIter == it)
            {
                ++_firstNoPopIter;
            }
            _time.erase(it);
        }
        else
        {
            break;
        }
    }
}

template<typename T> void TC_TimeoutQueue<T>::timeout(data_functor &df)
{
    struct timeval tv;
    TC_TimeProvider::getInstance()->getNow(&tv);
    while(true)
    {
        T ptr;

        {
            TC_LockT<TC_ThreadMutex> lock(*this);

            typename time_type::iterator it = _time.begin();

            if(it != _time.end() && tv.tv_sec*(int64_t)1000+tv.tv_usec/1000 - it->createTime > _timeout)
            {
                ptr = (*it->dataIter).second.ptr;

                _data.erase(it->dataIter);

                if(_firstNoPopIter == it)
                {
                    _firstNoPopIter++;
                }
                _time.erase(it);
            }
            else
            {
                break;
            }
        }

        try { df(ptr); } catch(...) { }
    }
}

template<typename T> T TC_TimeoutQueue<T>::erase(uint32_t uniqId)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    typename data_type::iterator it = _data.find(uniqId);

    if(it == _data.end())
    {
        return NULL;
    }

    T ptr = it->second.ptr;

    if(_firstNoPopIter == it->second.timeIter)
    {
        _firstNoPopIter++;
    }
    _time.erase(it->second.timeIter);

    _data.erase(it);

    return ptr;    
}

template<typename T> T TC_TimeoutQueue<T>::pop()
{
    T ptr;

    return pop(ptr) ? ptr : NULL;
}

template<typename T> bool TC_TimeoutQueue<T>::pop(T &ptr)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    if(_time.empty())
    {
        return false;
    }

    typename time_type::iterator it = _time.begin();

    if (it->hasPoped == true)
    {
        it = _firstNoPopIter;
    }

    if (it == _time.end())
    {
        return false;
    }

    assert(it->hasPoped == false);

    ptr = it->dataIter->second.ptr;

    it->hasPoped = true;

    _firstNoPopIter = it;

    ++_firstNoPopIter;

    return true;
}

template<typename T> bool TC_TimeoutQueue<T>::swap(deque<T> &q)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    if(_time.empty())
    {
        return false;
    }

    typename time_type::iterator it = _time.begin();

    while(it != _time.end())
    {
    
        if (it->hasPoped == true)
        {
            it = _firstNoPopIter;
        }

        if (it == _time.end())
        {
            break;
        }
    
        assert(it->hasPoped == false);
    
        T ptr = it->dataIter->second.ptr;
    
        it->hasPoped = true;
    
        _firstNoPopIter = it;
    
        ++_firstNoPopIter;

        q.push_back(ptr);

        ++it;
    }

    if(q.empty())
    {
        return false;
    }

    return true;
}
/////////////////////////////////////////////////////////////////
}
#endif
