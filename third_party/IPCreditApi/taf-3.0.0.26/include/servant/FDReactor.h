#ifndef __TAF_FD_REACTOR_H_
#define __TAF_FD_REACTOR_H_

#include "servant/Global.h"
#include "util/tc_thread.h"
#include "util/tc_thread_mutex.h"
#include "util/tc_epoller.h"
#include <set>

namespace taf
{
/**
 * 监听FD事件并触发注册的handle
 */
class FDReactor : public TC_Thread, public TC_ThreadLock, public TC_HandleBase
{
public:
    enum { R = 0x01, W = 0x02, };

    /**
     * 构造函数
     */
    FDReactor();

    /**
     * 析构函数
     */
    virtual ~FDReactor();

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
     * @param event
     * @param handle
     */
    void registerAdapter(AdapterProxy* adapter, uint32_t event);

    /**
     * 取消已注册的handle
     * @param fd
     * @param event
     * @param handle
     */
    void unregisterAdapter(AdapterProxy* adapter, uint32_t event);

    /**
     * 通知事件过来
     * @param fd
     */
    void notify(AdapterProxy* adapter);

protected:
    /**
     * 处理函数
     * 
     * @param adapter 
     * @param events 
     */
    void handle(AdapterProxy* adapter, int events);

    /**
     * 处理异常
     * @param fd
     * 
     */
    void handleExcept(AdapterProxy* adapter);

    /**
     * 输入事件
     * @param pi
     */
    void handleInputImp(AdapterProxy* adapter);

    /**
     * 输出事件
     * @param pi
     */
    void handleOutputImp(AdapterProxy* adapter);

protected:
    //Communicator *_comm;

    TC_Socket _shutdown;

    bool _terminate;

    TC_Epoller _ep;
};
/////////////////////////////////////////////////////////////////////////////////////
}

#endif
