#ifndef __TAF_QUEUE_TIMEOUT_H_
#define __TAF_QUEUE_TIMEOUT_H_

#include "servant/Global.h"
#include "util/tc_thread.h"
#include "util/tc_monitor.h"

namespace taf
{
/**
 * ��ʱ����ͻ��˶����й�������
 */
class QueueTimeout : public TC_Thread, public TC_ThreadLock, public TC_HandleBase
{

public:
    /**
     * ���캯��
     */
    QueueTimeout(Communicator *comm);

    /**
     * ��������
     */
    virtual ~QueueTimeout();

public:
    /**
     * ���̣߳�ѭ�����ʱ��
     */
    virtual void run();

    /**
     * ��������
     */
    virtual void terminate();

	/**
     * ������г�ʱ
     */
    void handleQueueTimeout();

protected:
    bool _terminate;

    Communicator *_comm;
};
////////////////////////////////////////////////////
}
#endif
