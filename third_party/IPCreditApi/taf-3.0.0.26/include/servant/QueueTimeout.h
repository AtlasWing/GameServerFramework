#ifndef __TAF_QUEUE_TIMEOUT_H_
#define __TAF_QUEUE_TIMEOUT_H_

#include "servant/Global.h"
#include "util/tc_thread.h"
#include "util/tc_monitor.h"

namespace taf
{
/**
 * 定时处理客户端队列中过期数据
 */
class QueueTimeout : public TC_Thread, public TC_ThreadLock, public TC_HandleBase
{

public:
    /**
     * 构造函数
     */
    QueueTimeout(Communicator *comm);

    /**
     * 析构函数
     */
    virtual ~QueueTimeout();

public:
    /**
     * 主线程，循环检查时间
     */
    virtual void run();

    /**
     * 结束处理
     */
    virtual void terminate();

	/**
     * 处理队列超时
     */
    void handleQueueTimeout();

protected:
    bool _terminate;

    Communicator *_comm;
};
////////////////////////////////////////////////////
}
#endif
