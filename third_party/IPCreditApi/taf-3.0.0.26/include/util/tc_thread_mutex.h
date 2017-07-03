#ifndef __TC_THREAD_MUTEX_H
#define __TC_THREAD_MUTEX_H

#include "util/tc_lock.h"

namespace taf
{
/////////////////////////////////////////////////
/** 
 * @file tc_thread_mutex.h 
 * @brief �߳���������(�޸���ICEԴ��). 
 *  
 * @author jarodruan@tencent.com  
 */
             
/////////////////////////////////////////////////
class TC_ThreadCond;

/**
 * @brief �̻߳������
 */
struct TC_ThreadMutex_Exception : public TC_Lock_Exception
{
    TC_ThreadMutex_Exception(const string &buffer) : TC_Lock_Exception(buffer){};
    TC_ThreadMutex_Exception(const string &buffer, int err) : TC_Lock_Exception(buffer, err){};
    ~TC_ThreadMutex_Exception() throw() {};
};

/**
* @brief �߳��� . 
*  
* �����ظ���������ͬһ���̲߳������ظ����� 
*  
* ͨ����ֱ��ʹ�ã���TC_Monitor���ʹ�ã���TC_ThreadLock; 
*/
class TC_ThreadMutex
{
public:

    TC_ThreadMutex();
    virtual ~TC_ThreadMutex();

    /**
     * @brief ����
     */
    void lock() const;

    /**
     * @brief ������
     * 
     * @return bool
     */
    bool tryLock() const;

    /**
     * @brief ����
     */
    void unlock() const;

    /**
	 * @brief ���������unlock�Ƿ������� 
	 *  	  ��TC_Monitorʹ�õ� ��Զ����true
     * @return bool
     */
    bool willUnlock() const { return true;}

protected:

    // noncopyable
    TC_ThreadMutex(const TC_ThreadMutex&);
    void operator=(const TC_ThreadMutex&);

	/**
     * @brief ����
	 */
    int count() const;

    /**
     * @brief ����
	 */
    void count(int c) const;

    friend class TC_ThreadCond;

protected:
    mutable pthread_mutex_t _mutex;

};

/**
* @brief �߳�����. 
*  
* �����߳̿�ʵ��
**/
class TC_ThreadRecMutex
{
public:

    /**
    * @brief ���캯��
    */
    TC_ThreadRecMutex();

    /**
    * @brief ��������
    */
    virtual ~TC_ThreadRecMutex();

    /**
	* @brief ��, ����pthread_mutex_lock. 
	*  
    * return : ����pthread_mutex_lock�ķ���ֵ
    */
    int lock() const;

    /**
	* @brief ����, pthread_mutex_unlock. 
	*  
    * return : ����pthread_mutex_lock�ķ���ֵ
    */
    int unlock() const;

    /**
	* @brief ������, ʧ���׳��쳣. 
	*  
    * return : true, �ɹ���; false �����߳��Ѿ�����
    */
    bool tryLock() const;

    /**
     * @brief ���������unlock�Ƿ�����, ��TC_Monitorʹ�õ�
     * 
     * @return bool
     */
    bool willUnlock() const;
protected:

	/**
     * @brief ��Ԫ��
     */
    friend class TC_ThreadCond;

	/**
     * @brief ����
	 */
    int count() const;

    /**
     * @brief ����
	 */
    void count(int c) const;

private:
    /**
    ������
    */
    mutable pthread_mutex_t _mutex;
	mutable int _count;
};

}

#endif

