#ifndef __TAF_SERVANT_HANDLE_H
#define __TAF_SERVANT_HANDLE_H

#include <map>
#include <string>
#include <memory>
#include <deque>
#include "util/tc_monitor.h"
#include "util/tc_epoll_server.h"
#include "servant/Servant.h"
#include "servant/StatReport.h"
#include <ucontext.h>

namespace taf
{
//////////////////////////////////////////////////////////////////////////////
/**
 * �������������߳�
 */
class ServantHandle : public TC_EpollServer::Handle
{
public:
    /**
     * ���峣��
     */
    enum
    {
        HEART_BEAT_INTERVAL = 10, /**�������ʱ��**/
    };

    /**
     * ����
     */
    ServantHandle();

    /**
     * ����
     */
    ~ServantHandle();

protected:
    /**
     * �̳߳�ʼ��
     */
    void initialize();

    /**
     * �߼�����
     * @param stRecvData
     */
    virtual void handle(const TC_EpollServer::tagRecvData &stRecvData);

    /**
     * ��ʱ����
     * @param stRecvData
     */
    virtual void handleTimeout(const TC_EpollServer::tagRecvData &stRecvData);

    /**
     * overload ����
     * @param stRecvData
     */
    virtual void handleOverload(const TC_EpollServer::tagRecvData &stRecvData);

    /**
     * close �¼�����
     * @param stRecvData
     */
    virtual void handleClose(const TC_EpollServer::tagRecvData &stRecvData);

    /**
     * ����Filter����Ϣ
     */
    //virtual void handleFilter();
	/**
	 * handleFilter��ֵĵ�һ���֣������첽���ö���
	 */
	virtual void handleAsyncResponse();

	/**
	 * handleFilter��ֵĵڶ����֣������û���������
	 * ����Ϸ�߼��ɺ���bExpectIdle����
	 */
	virtual void handleCustomMessage(bool bExpectIdle = false);

    /**
     * ����
     */
    virtual void heartbeat();

    /**
     * ���servant��û��resp��Ϣ������
     * @return bool
     */
    virtual bool allFilterIsEmpty();

    /**
     * ����������
     * @param stRecvData
     * @return JceCurrent*
     */
    JceCurrentPtr createCurrent(const TC_EpollServer::tagRecvData &stRecvData);

	/**
     * ����������ʱ�Ĺ�������
     * @param stRecvData
     * @return JceCurrent*
     */
    JceCurrentPtr createCloseCurrent(const TC_EpollServer::tagRecvData &stRecvData);

    /**
     * ����TafЭ��
     *
     * @param current
     */
    void handleTafProtocol(const JceCurrentPtr &current);

    /**
     * �����TafЭ��
     *
     * @param current
     */
    void handleNoTafProtocol(const JceCurrentPtr &current);

    /**
     * ����TAF�µĻҶ��߼�
     *
     * @param current
     */
    bool processGrid(const JceCurrentPtr &current);

    /**
     * ����TAF�µĲ���ͳ���߼�
     *
     * @param current
     */
    void processSample(const JceCurrentPtr &current);

    /**
     * ����TAF�µ�Ⱦɫ�߼�
     *
     * @param current
     */
    bool processDye(const JceCurrentPtr &current, string& dyeingKey);

    /**
     * ���set���úϷ���
     *
     * @param current
     * @return bool ������úϷ�����true��������÷Ƿ��򷵻�false
     */
	bool checkValidSetInvoke(const JceCurrentPtr &current);
protected:

    /**
     * �������
     */
    map<string, ServantPtr> _servants;

    /**
     * ��Ϣ����֪ͨ
     */
    TC_ThreadLock _monitor;
};

typedef TC_AutoPtr<ServantHandle> ServantHandlePtr;
///////////////////////////////////////////////////////////
}
#endif

