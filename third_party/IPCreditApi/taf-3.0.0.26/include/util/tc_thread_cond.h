#ifndef _TC_THREAD_COND_H
#define _TC_THREAD_COND_H

#include <sys/time.h>
#include <cerrno>
#include <iostream>
#include "util/tc_ex.h"

namespace taf
{
/////////////////////////////////////////////////
/**
 * @file tc_thread_cond.h 
 * @brief ����������(�޸���ICEԴ��). 
 *  
 * @author  jarodruan@tencent.com,skingfan@tencent.com   
 */             
/////////////////////////////////////////////////
class TC_ThreadMutex;

/**
 *  @brief �߳������쳣��
 */
struct TC_ThreadCond_Exception : public TC_Exception
{
    TC_ThreadCond_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_ThreadCond_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_ThreadCond_Exception() throw() {};
};

/**
 *  @brief �߳��ź�������, ����������������ȴ��źŷ���
 *  
 *  ��TC_ThreadMutex��TC_ThreadRecMutex���ʹ��,
 *  
 *  ͨ����ֱ��ʹ�ã�����ʹ��TC_ThreadLock/TC_ThreadRecLock;
 */
class TC_ThreadCond
{
public:

    /**
     *  @brief ���캯��
     */
    TC_ThreadCond();

    /**
     *  @brief ��������
     */
    ~TC_ThreadCond();

    /**
     *  @brief �����ź�, �ȴ��ڸ������ϵ�һ���̻߳���
     */
    void signal();

    /**
     *  @brief �ȴ��ڸ������������̶߳�����
     */
    void broadcast();

    /**
     *  @brief ��ȡ���Եȴ�ʱ��
     */
    timespec abstime(int millsecond) const;

    /**
	 *  @brief �����Ƶȴ�.
	 *  
     * @param M
     */
    template<typename Mutex>
    void wait(const Mutex& mutex) const
    {
        int c = mutex.count();
        int rc = pthread_cond_wait(&_cond, &mutex._mutex);
        mutex.count(c);
        if(rc != 0)
        {
            throw TC_ThreadCond_Exception("[TC_ThreadCond::wait] pthread_cond_wait error", errno);
        }
    }

    /**
	 * @brief �ȴ�ʱ��. 
	 *  
	 * @param M 
     * @return bool, false��ʾ��ʱ, true:��ʾ���¼�����
     */
    template<typename Mutex>
    bool timedWait(const Mutex& mutex, int millsecond) const
    {
        int c = mutex.count();

        timespec ts = abstime(millsecond);

        int rc = pthread_cond_timedwait(&_cond, &mutex._mutex, &ts);

        mutex.count(c);

        if(rc != 0)
        {
            if(rc != ETIMEDOUT)
            {
                throw TC_ThreadCond_Exception("[TC_ThreadCond::timedWait] pthread_cond_timedwait error", errno);
            }

            return false;
        }
        return true;
    }

protected:
    // Not implemented; prevents accidental use.
    TC_ThreadCond(const TC_ThreadCond&);
    TC_ThreadCond& operator=(const TC_ThreadCond&);

private:

    /**
     * �߳�����
     */
    mutable pthread_cond_t _cond;

};

}

#endif

