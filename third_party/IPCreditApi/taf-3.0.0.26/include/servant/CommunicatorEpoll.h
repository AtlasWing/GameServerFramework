#ifndef __TAF__COMMUNICATOR_EPOLL_H_
#define __TAF__COMMUNICATOR_EPOLL_H_

#include "util/tc_thread.h"
#include "util/tc_thread_mutex.h"
#include "util/tc_epoller.h"
#include "util/tc_loop_queue.h"
#include "servant/Message.h"
#include <set>

namespace taf
{

class Communicator;
class ObjectProxy;
class ObjectProxyFactory;
class StatReport;
class PropertyReport;

/**
 * 监听FD事件并触发注册的handle
 */
struct FDInfo
{
    enum
    {
        ET_C_NOTIFY = 1,
        ET_C_NET    = 2,
    };
	FDInfo()
	{
        iSeq = 0;
		fd = -1;
		iType = ET_C_NOTIFY;
		p = NULL;
	}

    size_t iSeq;
    int fd;

    int iType;
    void * p;
};

class CommunicatorEpoll : public TC_Thread ,public TC_ThreadRecMutex
{
#define MAX_ASYNC_THREAD 1024
#define MAX_THREAD_NUM 2048
public:
    struct NotifyInfo
    {
        NotifyInfo()
        {
            bValid = false;
			eventFd = -1;
        }
        //通知FD信息
        FDInfo stFDInfo;
        //通知fd
        TC_Socket notify;
        int eventFd;
        //是否有效
        bool bValid;
    };

    /**
     * 构造函数
     */
    CommunicatorEpoll(Communicator * pCommunicator, size_t _netThreadSeq);

    /**
     * 析构函数
     */
    virtual ~CommunicatorEpoll();

public:
    /**
     * 获取 Communicator
     * @param name
     *
     * @return Communicator *
     */

    inline Communicator * getCommunicator()
    {
        return _pCommunicator;
    }

public:
    /*
     * 获取本epoll的代理对象
     */
    ObjectProxy * getObjectProxy(const string & sObjectProxyName,const string& setName="");


    //判断是否是第一个网络线程 主控写缓存的时候用到
    bool isFirstNetThread()
    {
        return (0 == _netThreadSeq);
    }

public:
    /**
     * 循环监听网络事件
     */
    virtual void run();

    /**
     * 中止监听
     */
    void terminate();

public:
    ////////////////////////以下函数处理网关相关调用////////////////////////////
    /**
     * 注册fd对应的处理handle
     * @param fd
     * @param info 事件指针
     * @param event
     * @param handle
     */
    void addFd(int fd,FDInfo * info, uint32_t events);

    /**
     * 取消已注册的handle
     * @param fd
     * @param info 事件指针
     * @param event
     * @param handle
     */
    void delFd(int fd,FDInfo * info, uint32_t events);

    /**
     * 通知事件过来
     * @param fd
     */
    void notify(size_t iSeq,ReqInfoQueue * pReqQueue);
    void notifyDel(size_t iSeq);

    /**
     * 数据加入到异步线程队列里面
     * @return
     */
    void pushAsyncThreadQueue(ReqMessage * msg);

protected:
    /**
     * 处理函数
     *
     * @param pFDInfo
     * @param events
     */
    void handle(FDInfo * pFDInfo, uint32_t events);

    /**
     * 输入事件
     * @param pi
     */
    void handleInputImp(Transceiver * pTransceiver);

    /**
     * 输出事件
     * @param pi
     */
    void handleOutputImp(Transceiver * pTransceiver);

protected:

    /**
     * 处理超时
     * @param pi
     */
    void doTimeout();

    /**
     * 处理stat
     * @param pi
     */
    void doStat();


protected:
    //通信器
    Communicator * _pCommunicator;

    NotifyInfo _notify[MAX_THREAD_NUM];

    TC_Socket _shutdown;

    bool _terminate;

    TC_Epoller _ep;

    size_t _notifyNum;

    //下次检查超时的时间
    int64_t _iNextTime;

    int64_t _iNextStatTime;

    //objproxy 的创建
    ObjectProxyFactory * _pObjectProxyFactory;

    //异步线程
    AsyncProcThread * _vpAsyncThread[MAX_THREAD_NUM];

    size_t _iAsyncThreadNum;
    //轮询的seq
    size_t _iAsyncSeq;

    //网络线程的id号
    size_t _netThreadSeq;

    //统计上报的对象
    PropertyReport * _pReportAsyncQueue;
};

/////////////////////////////////////////////////////////////////////////////////////
}

#endif
