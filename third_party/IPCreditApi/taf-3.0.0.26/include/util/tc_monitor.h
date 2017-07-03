#ifndef _TC_MONITOR_H
#define _TC_MONITOR_H

#include "util/tc_thread_mutex.h"
#include "util/tc_thread_cond.h"

namespace taf
{
/////////////////////////////////////////////////
/** 
 * @file tc_monitor.h 
 * @brief ���������(�޸���ICEԴ��). 
 *  
 * @author jarodruan@tencent.com 
 */           
/////////////////////////////////////////////////
/**
 * @brief �߳������ģ����.
 * ͨ���߳�������ͨ��������ʹ�ã�������ֱ����TC_ThreadMutex��TC_ThreadRecMutex 
 *  
 * ���ཫTC_ThreadMutex/TC_ThreadRecMutex ��TC_ThreadCond��������� 
 */
template <class T, class P>
class TC_Monitor
{
public:

    /**
     * @brief ���������ƶ���
     */
    typedef TC_LockT<TC_Monitor<T, P> > Lock;
    typedef TC_TryLockT<TC_Monitor<T, P> > TryLock;

    /**
     * @brief ���캯��
     */
    TC_Monitor() : _nnotify(0)
    {
    }

    /**
     * @brief ����
     */
    virtual ~TC_Monitor()
    {
    }

    /**
     * @brief ��
     */
    void lock() const
    {
        _mutex.lock();
        _nnotify = 0;
    }

    /**
     * @brief ����, ���������Ĵ���֪ͨ
     */
    void unlock() const
    {
        notifyImpl(_nnotify);
        _mutex.unlock();
    }

    /**
     * @brief ������.
     *
     * @return bool
     */
    bool tryLock() const
    {
        bool result = _mutex.tryLock();
        if(result)
        {
            _nnotify = 0;
        }
        return result;
    }

    /**
     * @brief �ȴ�,��ǰ�����߳������ϵȴ���ֱ���¼�֪ͨ��
     */
    void wait() const
    {
        notifyImpl(_nnotify);

        try
        {
            _cond.wait(_mutex);
        }
        catch(...)
        {
            _nnotify = 0;
            throw;
        }

        _nnotify = 0;
    }

    /**
	 * @brief �ȴ�ʱ��,��ǰ�����߳������ϵȴ���ֱ����ʱ�����¼�֪ͨ
	 *  
	 * @param millsecond �ȴ�ʱ��
     * @return           false:��ʱ��, ture:���¼�����
     */
    bool timedWait(int millsecond) const
    {
        notifyImpl(_nnotify);

        bool rc;

        try
        {
            rc = _cond.timedWait(_mutex, millsecond);
        }
        catch(...)
        {
            _nnotify = 0;
            throw;
        }

        _nnotify = 0;
        return rc;
    }

    /**
	 * @brief ֪ͨĳһ���߳����� 
	 *  
	 * ֪ͨ�ȴ��ڸ�����ĳһ���߳��ѹ��� ,���øú���֮ǰ�������, 
	 *  
	 * �ڽ�����ʱ�������֪ͨ 
     */
    void notify()
    {
        if(_nnotify != -1)
        {
            ++_nnotify;
        }
    }

    /**
	 * @brief ֪ͨ�ȴ��ڸ����ϵ������߳��ѹ�����
	 * ע����øú���ʱ�����Ѿ������.
	 *  
	 * �ú�������ǰ֮�������, �ڽ�����ʱ�������֪ͨ 
     */
    void notifyAll()
    {
        _nnotify = -1;
    }

protected:

    /**
	 * @brief ֪ͨʵ��. 
	 *  
     * @param nnotify �����Ĵ���
     */
    void notifyImpl(int nnotify) const
    {
        if(nnotify != 0)
        {
            if(nnotify == -1)
            {
                _cond.broadcast();
                return;
            }
            else
            {
                while(nnotify > 0)
                {
                    _cond.signal();
                    --nnotify;
                }
            }
        }
    }

private:

	 /** 
	  * @brief noncopyable
	  */
    TC_Monitor(const TC_Monitor&);
    void operator=(const TC_Monitor&);

protected:

	/**
	 * �����Ĵ���
	 */
    mutable int     _nnotify;
    mutable P       _cond;
    T               _mutex;
};

/**
 * ��ͨ�߳���
 */
typedef TC_Monitor<TC_ThreadMutex, TC_ThreadCond> TC_ThreadLock;

/**
 * ѭ����(һ���߳̿��ԼӶ����)
 */
typedef TC_Monitor<TC_ThreadRecMutex, TC_ThreadCond> TC_ThreadRecLock;

}
#endif

