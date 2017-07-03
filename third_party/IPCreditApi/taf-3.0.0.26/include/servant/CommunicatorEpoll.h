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
 * ����FD�¼�������ע���handle
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
        //֪ͨFD��Ϣ
        FDInfo stFDInfo;
        //֪ͨfd
        TC_Socket notify;
        int eventFd;
        //�Ƿ���Ч
        bool bValid;
    };

    /**
     * ���캯��
     */
    CommunicatorEpoll(Communicator * pCommunicator, size_t _netThreadSeq);

    /**
     * ��������
     */
    virtual ~CommunicatorEpoll();

public:
    /**
     * ��ȡ Communicator
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
     * ��ȡ��epoll�Ĵ������
     */
    ObjectProxy * getObjectProxy(const string & sObjectProxyName,const string& setName="");


    //�ж��Ƿ��ǵ�һ�������߳� ����д�����ʱ���õ�
    bool isFirstNetThread()
    {
        return (0 == _netThreadSeq);
    }

public:
    /**
     * ѭ�����������¼�
     */
    virtual void run();

    /**
     * ��ֹ����
     */
    void terminate();

public:
    ////////////////////////���º�������������ص���////////////////////////////
    /**
     * ע��fd��Ӧ�Ĵ���handle
     * @param fd
     * @param info �¼�ָ��
     * @param event
     * @param handle
     */
    void addFd(int fd,FDInfo * info, uint32_t events);

    /**
     * ȡ����ע���handle
     * @param fd
     * @param info �¼�ָ��
     * @param event
     * @param handle
     */
    void delFd(int fd,FDInfo * info, uint32_t events);

    /**
     * ֪ͨ�¼�����
     * @param fd
     */
    void notify(size_t iSeq,ReqInfoQueue * pReqQueue);
    void notifyDel(size_t iSeq);

    /**
     * ���ݼ��뵽�첽�̶߳�������
     * @return
     */
    void pushAsyncThreadQueue(ReqMessage * msg);

protected:
    /**
     * ������
     *
     * @param pFDInfo
     * @param events
     */
    void handle(FDInfo * pFDInfo, uint32_t events);

    /**
     * �����¼�
     * @param pi
     */
    void handleInputImp(Transceiver * pTransceiver);

    /**
     * ����¼�
     * @param pi
     */
    void handleOutputImp(Transceiver * pTransceiver);

protected:

    /**
     * ����ʱ
     * @param pi
     */
    void doTimeout();

    /**
     * ����stat
     * @param pi
     */
    void doStat();


protected:
    //ͨ����
    Communicator * _pCommunicator;

    NotifyInfo _notify[MAX_THREAD_NUM];

    TC_Socket _shutdown;

    bool _terminate;

    TC_Epoller _ep;

    size_t _notifyNum;

    //�´μ�鳬ʱ��ʱ��
    int64_t _iNextTime;

    int64_t _iNextStatTime;

    //objproxy �Ĵ���
    ObjectProxyFactory * _pObjectProxyFactory;

    //�첽�߳�
    AsyncProcThread * _vpAsyncThread[MAX_THREAD_NUM];

    size_t _iAsyncThreadNum;
    //��ѯ��seq
    size_t _iAsyncSeq;

    //�����̵߳�id��
    size_t _netThreadSeq;

    //ͳ���ϱ��Ķ���
    PropertyReport * _pReportAsyncQueue;
};

/////////////////////////////////////////////////////////////////////////////////////
}

#endif
