#ifndef __TAF_ASYNC_PROC_THREAD_H_
#define __TAF_ASYNC_PROC_THREAD_H_

#include "servant/Message.h"
#include "servant/Global.h"

namespace taf
{
/**
 * �첽�ص����ҵ�����̣߳��߳�����client����
 */
class AsyncProcThread : public TC_Thread, public TC_HandleBase, public TC_ThreadLock
{
public:
    /**
     * ���캯��
     * @param proc
     */
    AsyncProcThread();

	/**
     * ��������
     * @param proc
     */
	virtual ~AsyncProcThread();

    void push_back(ReqMessage * msg);

    size_t getSize()
    {
        return _msgQueue->size();
    }

    /**
     * ���������߳�
     */
    void terminate();

    /**
     * �Ӷ�����ȡ��Ϣ��ִ�лص��߼�
     */
    void run();

private:
    /**
     * �Ƿ���Ҫ�˳�
     */
    bool _terminate;

    /**
     * ��Ϣ���е�������
     */
    ReqInfoQueue * _msgQueue;
};
///////////////////////////////////////////////////////
}
#endif
