#ifndef __TC_TIME_PROVIDER_H_
#define __TC_TIME_PROVIDER_H_

#include <string>
#include <string.h>
#include "util/tc_monitor.h"
#include "util/tc_thread.h"
#include "util/tc_autoptr.h"

#define rdtsc(low,high) \
     __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))

#define TNOW     taf::TC_TimeProvider::getInstance()->getNow()
#define TNOWMS   taf::TC_TimeProvider::getInstance()->getNowMs()

namespace taf
{
/////////////////////////////////////////////////
/**
 * @file tc_timeprovider.h
 * @brief �뼶��΢�ʱ���ṩ��. 
 *  
 * @author jarodruan@tencent.com,skingfan@tencent.com 
 */                    
/////////////////////////////////////////////////
class TC_TimeProvider;

typedef TC_AutoPtr<TC_TimeProvider> TC_TimeProviderPtr;

/**
 * @brief �ṩ�뼶���ʱ��
 */
class TC_TimeProvider : public TC_Thread, public TC_HandleBase
{
public:

    /**
	 * @brief ��ȡʵ��. 
	 *  
     * @return TimeProvider&
     */
    static TC_TimeProvider* getInstance();

    /**
     * @brief ���캯��
     */
    TC_TimeProvider() : _terminate(false),_use_tsc(true),_cpu_cycle(0),_buf_idx(0)
    {
        memset(_t,0,sizeof(_t));
        memset(_tsc,0,sizeof(_tsc));

        struct timeval tv;
        ::gettimeofday(&tv, NULL);
        _t[0] = tv;
        _t[1] = tv;
    }

    /**
     * @brief ������ֹͣ�߳�
     */
    ~TC_TimeProvider(); 

    /**
     * @brief ��ȡʱ��.
     *
     * @return time_t ��ǰʱ��
     */
    time_t getNow()     {  return _t[_buf_idx].tv_sec; }

    /**
     * @brief ��ȡʱ��.
     *
	 * @para timeval 
     * @return void 
     */
    void getNow(timeval * tv);

    /**
     * @brief ��ȡmsʱ��.
     *
	 * @para timeval 
     * @return void 
     */
    int64_t getNowMs();
    
    /**
     * @brief ��ȡcpu��Ƶ.
     *  
     * @return float cpu��Ƶ
     */  

    float cpuMHz();

    /**
     * @brief ����
     */
protected:

    virtual void run();

    static TC_ThreadLock        g_tl;

    static TC_TimeProviderPtr   g_tp;

private:
    void setTsc(timeval& tt);

    void addTimeOffset(timeval& tt,const int &idx);

protected:

    bool    _terminate;

    bool    _use_tsc;

private:
    float           _cpu_cycle; 

    volatile int    _buf_idx;

    timeval         _t[2];

    uint64_t        _tsc[2];  
};

}

#endif


