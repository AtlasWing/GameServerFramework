#ifndef __TAF_ASYNC_PROC_THREAD_H_
#define __TAF_ASYNC_PROC_THREAD_H_

#include "servant/Message.h"
#include "servant/Global.h"

namespace taf
{
/**
 * 异步回调后的业务处理线程，线程数在client配置
 */
class AsyncProcThread : public TC_Thread, public TC_HandleBase, public TC_ThreadLock
{
public:
    /**
     * 构造函数
     * @param proc
     */
    AsyncProcThread();

	/**
     * 析构函数
     * @param proc
     */
	virtual ~AsyncProcThread();

    void push_back(ReqMessage * msg);

    size_t getSize()
    {
        return _msgQueue->size();
    }

    /**
     * 结束处理线程
     */
    void terminate();

    /**
     * 从队列中取消息后执行回调逻辑
     */
    void run();

private:
    /**
     * 是否需要退出
     */
    bool _terminate;

    /**
     * 消息队列的所有者
     */
    ReqInfoQueue * _msgQueue;
};
///////////////////////////////////////////////////////
}
#endif
