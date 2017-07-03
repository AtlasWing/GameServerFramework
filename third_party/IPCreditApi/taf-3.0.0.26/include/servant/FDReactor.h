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
 * ����FD�¼�������ע���handle
 */
class FDReactor : public TC_Thread, public TC_ThreadLock, public TC_HandleBase
{
public:
    enum { R = 0x01, W = 0x02, };

    /**
     * ���캯��
     */
    FDReactor();

    /**
     * ��������
     */
    virtual ~FDReactor();

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
     * @param event
     * @param handle
     */
    void registerAdapter(AdapterProxy* adapter, uint32_t event);

    /**
     * ȡ����ע���handle
     * @param fd
     * @param event
     * @param handle
     */
    void unregisterAdapter(AdapterProxy* adapter, uint32_t event);

    /**
     * ֪ͨ�¼�����
     * @param fd
     */
    void notify(AdapterProxy* adapter);

protected:
    /**
     * ������
     * 
     * @param adapter 
     * @param events 
     */
    void handle(AdapterProxy* adapter, int events);

    /**
     * �����쳣
     * @param fd
     * 
     */
    void handleExcept(AdapterProxy* adapter);

    /**
     * �����¼�
     * @param pi
     */
    void handleInputImp(AdapterProxy* adapter);

    /**
     * ����¼�
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
