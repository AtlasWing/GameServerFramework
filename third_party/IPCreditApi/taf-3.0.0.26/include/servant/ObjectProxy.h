#ifndef __TAF_OBJECT_PROXY_H_
#define __TAF_OBJECT_PROXY_H_

#include "servant/Communicator.h"
#include "servant/Message.h"
#include "servant/AdapterProxy.h"
#include "servant/EndpointInfo.h"
#include "servant/EndpointF.h"
#include "servant/AppProtocol.h"
#include "servant/Global.h"
#include "util/tc_timeout_queue_noid.h"

namespace taf
{
class EndpointManager;

struct SocketOpt
{
	int level;

	int optname;

	const void *optval;

	socklen_t optlen;
};

///////////////////////////////////////////////////////////////////
/**
 * ÿ��objectname�ڽ��̿ռ�Ψһ��һ��objectproxy
 * �����շ�����Ϣ����
 */
class ObjectProxy : public TC_HandleBase, public TC_ThreadMutex //public BaseProxy
{
public:
    /**
     * ���캯��
     * @param comm
     * @param name
     * @param setName ָ��set���õ�setid
     */
    ObjectProxy(CommunicatorEpoll * pCommunicatorEpoll, const string & sObjectProxyName,const string& setName="");

    /**
     * ��������
     */
    ~ObjectProxy();

    /**
     * ��ʼ����Ϣ���е�
     */
    void initialize();

    /**
     * ��ȡservantproxy
     */
    ServantProxy * getServantProxy();

    /**
     * ����servantproxy
     */
    void setServantProxy(ServantProxy * pServantProxy);

    /**
     * ��������
     * @param req
     * @return int
     */
    void invoke(ReqMessage *  msg);

    /**
     * ���ͻ�����obj������
     */
    void doInvoke();

    /**
     * ��ȡ���ӳ�ʱʱ��
     * @return int
     */
    int getConTimeout()
    {
        return _conTimeout;
    }

    /**
     * �������ӳ�ʱʱ��
     * @return int
     */
    void setConTimeout(int conTimeout)
    {
        _conTimeout = conTimeout;
    }

    /**
     * ��ȡobject����
     * @return const string&
     */
    const string & name() const;

    /**
     * ��ʱ���Ի�ȡ������
     * @return CheckTimeoutInfo&
     */
    CheckTimeoutInfo& checkTimeoutInfo();

    /**
     * ����Э�������
     * @return UserProtocol&
     */
    void setProxyProtocol(const ProxyProtocol& protocol);

    /**
     * ��ȡЭ�������
     * @return ProxyProtocol&
     */
    ProxyProtocol& getProxyProtocol();

    /**
    *�����׽ӿ�ѡ��
    */
    void setSocketOpt(int level, int optname, const void *optval, socklen_t optlen);

     /**
     *��ȡ�׽���ѡ��
     */
     vector<SocketOpt>& getSocketOpt();

    /**
     * ����ˢ�·����б�ļ��
     * @param msecond
     */
    void refreshEndpointInterval(int msecond);

     /**
     * ���û�������б�ļ��
     * @param msecond
     */
    void cacheEndpointInterval(int msecond);

    /**
     * ����locator
     * @param
     * @return AdapterPrx
     */
    int loadLocator();

    /**
     * ����PUSH����Ϣ��callback����
     * @param cb
     */
    void setPushCallbacks(const ServantProxyCallbackPtr& cb);

    /**
     * ��ȡPUSH����Ϣ��callback����
     */
    inline ServantProxyCallbackPtr & getPushCallback()
    {
        return _pPushCallback;
    }

    uint32_t generateId()
    {
        _uiId ++;
        if(_uiId == 0)
            ++_uiId;
        return _uiId;
    }

    /**
     * ��ȡ���е�adapter
     */
    const vector<AdapterProxy*> & getAdapters() const;

    /**
     * ���Obj�Ķ��г�ʱ
     */
    void doTimeout();

    /**
     * Obj�Ķ��еĳ���
     */
    size_t timeQSize()
    {
        return _reqInfoTimeQ.size();
    }
	/**
	 * �жϴ�obj�Ƿ��߰�set����������̣������ֱ����ʽ����ʹ�����������set�ģ�Ҳ����Ϊ�ǰ�set������õ�
	 */
	bool IsInvokeBySet() const;

	/**
	 * ��ȡ��set������õ�set����
	 */
	const string& getInvokeSetName() const;
public:

    int timeout() const
    {
        return _timeout;
    }

    void timeout(int msec)
    {
        //�������첽��ʱʱ�䲻��С��100����
        if (msec >= 100)
            _timeout = msec;
        else
            _timeout = 100;
    }

    inline CommunicatorEpoll * getCommunicatorEpoll()
    {
        return _pCommunicatorEpoll;
    }

protected:

	/**
	 * ���������쳣
	 *
	 */
	void doInvokeException(ReqMessage * msg);

private:

    uint32_t _uiId;

    string _name;

	//��set������õ�set����
	string _invokeSetId;

	//�Ƿ��ǰ�set�������
	bool  _bInvokeBySet;

    string _locator;

    int32_t _timeout;

    int32_t _maxTransNum;

    string _queryFObjectName;

    bool _isDirectProxy;

    bool _serverHasGrid;

    int32_t _refreshEndpointInterval;

    time_t _lastCacheEndpointTime;

    int32_t _cacheEndpointInterval;

    int32_t _roundStartIndex;

    int _conTimeout;

    CheckTimeoutInfo _checkTimeoutInfo;

    ProxyProtocol _proxyProtocol;

    vector<SocketOpt> _SocketOpts;

    //push��Ϣ callback
    ServantProxyCallbackPtr _pPushCallback;

    //��������
    EndpointManager * _pEndpointManger;

    CommunicatorEpoll * _pCommunicatorEpoll;

    TC_TimeoutQueueNoID<ReqMessage *> _reqInfoTimeQ;

    ServantProxy * _pServantProxy;

};
///////////////////////////////////////////////////////////////////////////////////
}
#endif
