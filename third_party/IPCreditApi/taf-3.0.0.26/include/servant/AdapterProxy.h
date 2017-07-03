#ifndef __TAF_ADAPTER_PROXY_H_
#define __TAF_ADAPTER_PROXY_H_

#include "util/tc_timeout_queue_new.h"
#include "util/tc_timeout_queue_map.h"
#include "servant/Global.h"
#include "servant/EndpointInfo.h"
#include "servant/ObjectProxy.h"
#include "servant/Transceiver.h"
#include "servant/Message.h"
#include "servant/StatReport.h"
#include <queue>

#define ADAPTER_QUEUE_LEN 1000

namespace taf
{
/**
 * ÿ��Adapter��Ӧһ��Endpoint��Ҳ����һ������˿�
 */
class AdapterProxy
{
public:
    /**
     * ���캯��
     * @param ep
     * @param op
     */
    AdapterProxy(ObjectProxy * pObjectProxy, const EndpointInfo &ep,Communicator* pCom);

    /**
     * ��������
     */
    ~AdapterProxy();

    /**
     * ����server�˶��󷽷�
     * @param req
     * @return int
     */
	int invoke(ReqMessage * msg);

    /**
     * ��������
     * ���ͼ�ѹ������
     * @param req
     * @return
     */
    void doInvoke();

    /**
     * server�˷�������
     * @param req
     * @return int
     */
    void finishInvoke(ResponsePacket &rsp);

    void finishInvoke(ReqMessage * msg);

    /**
     * �˿��Ƿ���Ч,������ȫ��ʧЧʱ����false
     * @bForceConnect : �Ƿ�ǿ�Ʒ�������,Ϊtrueʱ����״̬�����жϾͷ�������
     * @return bool
     */
    bool checkActive(bool bForceConnect = false);

	inline bool isConnTimeout() { return _bConnTimeout; }
    void setConTimeout(bool bConTimeout);

	inline bool isConnExc() { return _bConnExc;}
	inline int ConnExcCnt() const { return _connExcCnt;}

    int getConTimeout();

	/**
	 * ������ס�յ�״̬�Ƿ�Ϊactive,�����ǳ����⵼�£�������Ϊֹͣ�����Ƿ������ߣ�����Ϊ��inactive��
	 * @return bool
	 */
	inline bool isActiveInReg() {return _activeStateInReg;}

	inline void setActiveInReg(bool bActive) {_activeStateInReg = bActive;}
	inline size_t loaded() { return _loaded;}

    //inline void setRetrying(bool retrying) { _retrying = retrying;}

	/**
	 * ��¼�����쳣��
	 * @param bExc �����Ƿ�����쳣
	 */
	void addConnExc(bool bExc);
    /**
     * ���ӵ�����
     * @param bTimeout, �Ƿ��ǳ�ʱ��
     */
    void finishInvoke(bool bTimeout);

    /**
     * ��ȡ�˿���Ϣ
     * @return const EndpointInfo&
     */
    const EndpointInfo & endpoint() const;

    /**
     * �Ӷ�����ȡһ������
     * @param trans
     * @return bool
     */
	bool sendRequest();

    /**
     * ��ȡ����
     *
     * @return Transceiver*
     */
    inline Transceiver* trans() { return _pTrans; }

	/**
	 * �����Ҫ�첽���ӷ�����ô�ٴ��������
	 */
	bool finishConnect();

    /**
     * ����ʱ
     */
    void doTimeout();

    /**
     * ����stat
     */
    void doStat(map<StatMicMsgHead, StatMicMsgBody> & mStatMicMsg);

    void sample(ReqMessage * msg);

    inline CommunicatorEpoll * getEpoll()
    {
        return _pCommunicatorEpoll;
    }

    ObjectProxy * getObjProxy();

protected:

    /**
     * ���÷������Ч״̬
     * @param value
     */
    void setActive(bool value);

    /**
     * ���ν��
     */
    void setInactive();

    /**
     * �Ӷ����л�ȡһ������
     * @param req
     * @return bool
     */
	bool popRequest(ReqMessage * req);

    /**
     * ����
     * @return TransceiverPtr
     */
    Transceiver* doReconnect();

private:

	void initStatHead();

	void stat(ReqMessage * msg);
    void merge(const StatMicMsgBody& inBody, StatMicMsgBody& outBody);
private:

    TC_TimeoutQueueNew<ReqMessage*>*   _pTimeoutQueue;

    ObjectProxy*                    _pObjectProxy;

    CommunicatorEpoll*              _pCommunicatorEpoll;

    Transceiver*                    _pTrans;

	Communicator*					_pCommunicator;

    EndpointInfo					_endpoint;

    int32_t							_checkTransInterval;

    uint32_t						_timeoutInvoke;

	uint32_t						_bConnTimeout;

    uint32_t						_totalInvoke;

    uint32_t						_frequenceFailInvoke;

    time_t                          _nextFinishInvokeTime;

    time_t                          _nextRetryTime;

    time_t							_lastCheckTransTime;

    time_t                          _frequenceFailTime;

    bool							_activeStatus;

    //bool                            _retrying;

	//�ýڵ������ص�״̬�Ƿ���active��,�����Ϻ���Ϊֹͣ������Ϊ��inactive��
	bool							_activeStateInReg;

	/**
	 * �����쳣����,��ʱ��Ƭ��Χ��(Ŀǰ��1����)����5���쳣����Ϊ����ʧЧ
	 */
    uint32_t _connExcCnt;
	bool _bConnExc;


	//�ýڵ����˸���ֵ
	size_t _loaded;

    uint32_t                        _iMaxSampleCount;

    //����ģ������ʱ��ͼ�Ĳ�������
    int                             _iSampleRate;

    //stat��Ϣ
    //key�Ǻ�������
    map<string,StatMicMsgBody>		_mStatBody;

    //stat��ͷ��Ϣ
	StatMicMsgHead					_mStatHead;

    //sample����Ϣ
    map<string,vector<StatSampleMsg> >     _mSample;
};
////////////////////////////////////////////////////////////////////
}
#endif
