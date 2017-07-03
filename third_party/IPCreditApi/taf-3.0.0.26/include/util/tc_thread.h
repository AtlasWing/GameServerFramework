#ifndef	__TC_THREAD_H_
#define __TC_THREAD_H_

#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include "util/tc_ex.h"
#include "util/tc_monitor.h"

namespace taf
{
/////////////////////////////////////////////////
/** 
 * @file tc_thread.h 
 * @brief  �߳���(�޸���ICEԴ��). 
 *  
 * @author jarodruan@tencent.com  
 */          
/////////////////////////////////////////////////

/**
 * @brief  �߳̿����쳣��
 */
struct TC_ThreadThreadControl_Exception : public TC_Exception
{
    TC_ThreadThreadControl_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_ThreadThreadControl_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_ThreadThreadControl_Exception() throw() {};
};

/**
 * @brief  �߳̿�����
 */
class TC_ThreadControl
{
public:

    /**
	 * @brief  ����, ��ʾ��ǰ���е��̣߳�
	 *  		join��detach�ڲ����ڸö����ϵ���
	*/
	   
    TC_ThreadControl();

    /**
     * @return explicit
     */
    explicit TC_ThreadControl(pthread_t);

    /**
     * @brief  �ȴ���ǰ�߳̽���, �����ڵ�ǰ�߳��ϵ���
     */
    void join();

    /**
     * @brief  detach, �����ڵ�ǰ�߳��ϵ���
     */
    void detach();

    /**
     * @brief  ��ȡ��ǰ�߳�id.
     *
     * @return pthread_t��ǰ�߳�id
     */
    pthread_t id() const;

    /**
	 * @brief  ��Ϣmsʱ��. 
	 *  
     * @param millsecond ��Ϣ��ʱ�䣬����ms����
     */
    static void sleep(long millsecond);

    /**
     * @brief  ������ǰ�߳̿���Ȩ
     */
    static void yield();

private:

    pthread_t _thread;
};

/**
 *
 */
class TC_Runable
{
public:
    virtual ~TC_Runable(){};
    virtual void run() = 0;
};

/**
 * @brief �̻߳���. 
 * �̻߳��࣬�����Զ����̼̳߳��ڸ��࣬ͬʱʵ��run�ӿڼ���, 
 *  
 * ����ͨ��TC_ThreadContorl�����̡߳�
 */
class TC_Thread : public TC_Runable
{
public:

	/**
     * @brief  ���캯��
	 */
	TC_Thread();

	/**
     * @brief  ��������
	 */
	virtual ~TC_Thread(){};

	/**
     * @brief  �߳�����
	 */
	TC_ThreadControl start();

    /**
     * @brief  ��ȡ�߳̿�����.
     *
     * @return ThreadControl
     */
    TC_ThreadControl getThreadControl() const;

    /**
     * @brief  �߳��Ƿ���.
     *
     * @return bool ����true�����򷵻�false
     */
    bool isAlive() const;

	/**
     * @brief  ��ȡ�߳�id.
	 *
	 * @return pthread_t �߳�id
	 */
	pthread_t id() { return _tid; }

protected:

	/**
	 * @brief  ��̬����, �߳����. 
	 *  
	 * @param pThread �̶߳���
	 */
	static void threadEntry(TC_Thread *pThread);

	/**
     * @brief  ����
	 */
    virtual void run() = 0;

protected:
    /**
     * �Ƿ�������
     */
    bool            _running;

    /**
     * �߳�ID
     */
	pthread_t	    _tid;

    /**
     * �߳���
     */
    TC_ThreadLock   _lock;
};

}
#endif

