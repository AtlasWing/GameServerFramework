#ifndef __TC_THREAD_POOL_H_
#define __TC_THREAD_POOL_H_

#include "util/tc_thread.h"
#include "util/tc_thread_queue.h"
#include "util/tc_monitor.h"
#include "util/tc_functor.h"

#include <vector>
#include <set>
#include <iostream>

using namespace std;

namespace taf
{
/////////////////////////////////////////////////
/** 
 * @file tc_thread_pool.h
 * @brief �̳߳���,���loki�Լ�wbl��˼��.
 *  
 * @author  jarodruan@tencent.com
 */         
/////////////////////////////////////////////////
/**
* @brief �߳��쳣
*/
struct TC_ThreadPool_Exception : public TC_Exception
{
	TC_ThreadPool_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_ThreadPool_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_ThreadPool_Exception() throw(){};
};


/**
 * @brief ��ͨ�̳߳���, ��tc_functor, tc_functorwrapper���ʹ��. 
 *  
 * ʹ�÷�ʽ˵��:
 * 1 ����tc_functorwrapper��װһ������
 * 2 ��tc_threadpool�Ե��ý���ִ�� 
 * ����ʾ��������μ�:test/test_tc_thread_pool.cpp
 */
class TC_ThreadPool : public TC_ThreadLock
{
public:

    /**
     * @brief ���캯��
     *
     */
    TC_ThreadPool();

    /**
     * @brief ����, ��ֹͣ�����߳�
     */
    ~TC_ThreadPool();

    /**
	 * @brief ��ʼ��. 
	 *  
     * @param num �����̸߳���
     */
    void init(size_t num);

    /**
     * @brief ��ȡ�̸߳���.
     *
     * @return size_t �̸߳���
     */
    size_t getThreadNum()   { Lock sync(*this); return _jobthread.size(); }

    /**
     * @brief ��ȡ�̳߳ص�������(exec��ӽ�ȥ��).
     *
     * @return size_t �̳߳ص�������
     */
    size_t getJobNum()     { return _jobqueue.size(); }

    /**
     * @brief ֹͣ�����߳�
     */
    void stop();

    /**
     * @brief ���������߳�
     */
    void start();

    /**
	 * @brief ���������̲߳�, ִ�г�ʼ������. 
	 *  
     * @param ParentFunctor
     * @param tf
     */
    template<class ParentFunctor>
    void start(const TC_FunctorWrapper<ParentFunctor> &tf)
    {
        for(size_t i = 0; i < _jobthread.size(); i++)
        {
            _startqueue.push_back(new TC_FunctorWrapper<ParentFunctor>(tf));
        }

        start();
    }

    /**
	 * @brief ��Ӷ����̳߳�ִ�У��ú������Ϸ��أ� 
	 *  	  �̳߳ص��߳�ִ�ж���
	 */
    template<class ParentFunctor>
	void exec(const TC_FunctorWrapper<ParentFunctor> &tf)
    {
        _jobqueue.push_back(new TC_FunctorWrapper<ParentFunctor>(tf));
    }

    /**
     * @brief �ȴ����й���ȫ������(����������, �޿����߳�).
     *
     * @param millsecond �ȴ���ʱ��(ms), -1:��Զ�ȴ�
	 * @return            true, ���й������������ 
	 * 				     false,��ʱ�˳�
     */
    bool waitForAllDone(int millsecond = -1);

public:

    /**
     * @brief �߳����ݻ���,�����̵߳�˽�����ݼ̳��ڸ���
     */
    class ThreadData
    {
    public:
        /**
         * @brief ����
         */
        ThreadData(){};
        /**
         * @brief ����
         */
        virtual ~ThreadData(){};

        /**
		 * @brief ��������. 
		 *  
		 * @param T 
         * @return ThreadData*
         */
        template<typename T>
        static T* makeThreadData()
        {
            return new T;
        }
    };

    /**
	 * @brief �����߳�����. 
	 *  
     * @param p �߳�����
     */
    static void setThreadData(ThreadData *p);

    /**
     * @brief ��ȡ�߳�����.
     *
     * @return ThreadData* �߳�����
     */
    static ThreadData* getThreadData();

    /**
	 * @brief �����߳�����, key��Ҫ�Լ�ά��. 
	 *  
     * @param pkey �߳�˽������key
     * @param p    �߳�ָ�� 
     */
    static void setThreadData(pthread_key_t pkey, ThreadData *p);

    /**
	 * @brief ��ȡ�߳�����, key��Ҫ�Լ�ά��. 
	 *  
     * @param pkey �߳�˽������key
     * @return     ָ���̵߳�ThreadData*ָ��
     */
    static ThreadData* getThreadData(pthread_key_t pkey);

protected:

    /**
	 * @brief �ͷ���Դ. 
	 *  
     * @param p
     */
    static void destructor(void *p);

    /**
     * @brief ��ʼ��key
     */
    class KeyInitialize
    {
    public:
        /**
         * @brief ��ʼ��key
         */
        KeyInitialize()
        {
            int ret = pthread_key_create(&TC_ThreadPool::g_key, TC_ThreadPool::destructor);
            if(ret != 0)
            {
                throw TC_ThreadPool_Exception("[TC_ThreadPool::KeyInitialize] pthread_key_create error", ret);
            }
        }

        /**
         * @brief �ͷ�key
         */
        ~KeyInitialize()
        {
            pthread_key_delete(TC_ThreadPool::g_key);
        }
    };

    /**
     * @brief ��ʼ��key�Ŀ���
     */
    static KeyInitialize g_key_initialize;

    /**
     * @brief ����key
     */
    static pthread_key_t g_key;

protected:
    /**
     * @brief �̳߳��еĹ����߳�
     */
    class ThreadWorker : public TC_Thread
    {
    public:
        /**
		 * @brief �����̹߳��캯��. 
		 *  
         * @param tpool
         */
        ThreadWorker(TC_ThreadPool *tpool);

        /**
         * @brief ֪ͨ�����߳̽���
         */
        void terminate();

    protected:
        /**
         * @brief ����
         */
        virtual void run();

    protected:
        /**
         * �̳߳�ָ��
         */
        TC_ThreadPool   *_tpool;

        /**
         * �Ƿ�����߳�
         */
        bool            _bTerminate;
    };

protected:

    /**
     * @brief ���
     */
    void clear();

    /**
     * @brief ��ȡ����, ���û������, ��ΪNULL.
     *
     * @return TC_FunctorWrapperInterface*
     */
    TC_FunctorWrapperInterface *get(ThreadWorker *ptw);

    /**
     * @brief ��ȡ��������.
     *
     * @return TC_FunctorWrapperInterface*
     */
    TC_FunctorWrapperInterface *get();

    /**
	 * @brief ������һ���߳�. 
	 *  
     * @param ptw
     */
    void idle(ThreadWorker *ptw);

    /**
     * @brief ֪ͨ�ȴ�����������ϵĹ����߳�����
     */
    void notifyT();

    /**
     * @brief �Ƿ������.
     *
     * @return bool
     */
    bool finish();

    /**
     * @brief �߳��˳�ʱ����
     */
    void exit();

    friend class ThreadWorker;
protected:

    /**
     * �������
     */
    TC_ThreadQueue<TC_FunctorWrapperInterface*> _jobqueue;

    /**
     * ��������
     */
    TC_ThreadQueue<TC_FunctorWrapperInterface*> _startqueue;

    /**
     * �����߳�
     */
    std::vector<ThreadWorker*>                  _jobthread;

    /**
     * ��æ�߳�
     */
    std::set<ThreadWorker*>                     _busthread;

    /**
     * ������е���
     */
    TC_ThreadLock                               _tmutex;

	/**
	 * �Ƿ���������ִ�����
	 */
	bool                                        _bAllDone;
};

}
#endif

