#ifndef __TAF_ENDPOINT_MANAGER_H_
#define __TAF_ENDPOINT_MANAGER_H_

#include "servant/EndpointInfo.h"
#include "servant/EndpointF.h"
#include "servant/QueryF.h"
#include "servant/AppProtocol.h"

namespace taf
{

enum  GetEndpointType
{
    E_DEFAULT = 0,
    E_ALL = 1,
    E_SET = 2,
    E_STATION = 3
};

class QueryEpBase : public QueryFPrxCallback
{
public:
    QueryEpBase(Communicator * pComm,bool bFirstNetThread,bool bThread);
	virtual ~QueryEpBase(){};
    void callback_findObjectById4All(taf::Int32 ret,
        const vector<taf::EndpointF>& activeEp,
        const vector<taf::EndpointF>& inactiveEp);

    void callback_findObjectById4All_exception(taf::Int32 ret);

    void callback_findObjectById4Any(taf::Int32 ret,
        const vector<taf::EndpointF>& activeEp,
        const vector<taf::EndpointF>& inactiveEp);

    void callback_findObjectById4Any_exception(taf::Int32 ret);

    void callback_findObjectByIdInSameGroup(taf::Int32 ret,
            const vector<taf::EndpointF>& activeEp,
            const vector<taf::EndpointF>& inactiveEp);

    void callback_findObjectByIdInSameGroup_exception(taf::Int32 ret);

    void callback_findObjectByIdInSameSet(taf::Int32 ret,
            const vector<taf::EndpointF>& activeEp,
            const vector<taf::EndpointF>& inactiveEp);

    void callback_findObjectByIdInSameSet_exception(taf::Int32 ret);

    void callback_findObjectByIdInSameStation(taf::Int32 ret,
            const vector<taf::EndpointF>& activeEp,
            const vector<taf::EndpointF>& inactiveEp);

    void callback_findObjectByIdInSameStation_exception(taf::Int32 ret);

public:

    virtual void notifyEndpoints(const set<EndpointInfo> & active,const set<EndpointInfo> & inactive,bool bSync) = 0;

    //���������������� �ʼ�������� Ҫ֪ͨ
    virtual void doNotify() = 0;

    //��ʼ���ӿ� ����obj����������
    bool init(const string & sObjName,const string & sLocator,const string& setName="");
protected:

    //ˢ������
    void refreshReg(GetEndpointType type,const string & sName);

private:
    void setEndpoints(const string & sEndpoints, set<EndpointInfo> & setEndpoints);

    //����obj����,����Ǽ�����ӣ����������ش������ӻ����м�����Ӧ���б�
    void setObjName(const string & sObjName);

    void doEndpoints(const vector<taf::EndpointF>& activeEp, const vector<taf::EndpointF>& inactiveEp,int iRet,bool bSync=false);
    void doEndpointsExp(int iRet,bool bSync = false);

    //������Ϣˢ�µ����ݿ�
    void setEndPointToCache(bool bInactive);

protected:

    //�Ƿ�ֱ����ʶ
    bool _bDirectProxy;

    //��ʾ�Ƿ�ӿڵ�����
    bool _bThread;

    //obj name
    string _sObjName;

	//ָ��set���õ�setid,Ĭ��Ϊ�գ������ֵ����˵����ָ��set����
	string _invokeSetId;

	QueryFPrx _queryFPrx;

    Communicator * _pCommunicator;

    //��Ծ�ڵ��б�
    set<EndpointInfo> _activeEndpoints;

    //����Ծ�ڵ��б� �ڹ���ƽ̨���滹���е�
    set<EndpointInfo> _inactiveEndpoints;

    //�����Ƿ���Ч��ֻ����������ػ��ߴ��ļ�load���������ݲ�����Ч����
    //��ʼ����ʱ������Ч������
    bool _valid;

private:


    //�Ƿ��һ�������߳�
    bool _bFirstNetThread;

	//���ص�ַ
	string _sLocator;
private:

    //�����б��Ƶ��,��λ��
    int _iRefreshInterval;

    //����ʧ�ܵļ��ʱ��
    int _iFailInterval;

    //���������Ծ�б�Ϊ�յļ��ʱ��
    int _iActiveEmptyInterval;

    //��������ʧ�ܵĴ���
    int _iMaxFailTimes;

    //�ﵽ��������ʧ�ܵĴ��� �Ժ��ʱ����
    int _iManyFailInterval;

    //�������صĳ�ʱʱ��
    int _iTimeOut;

    //�������صĵȴ�ʱ��
    int _iWaitTime;

    //����ĳ�ʱʱ��(����ʱ��) ��ֹ����ص����ˡ�һֱ����������״̬
    int64_t _iRequestTimeout;

    //�Ƿ���������
    bool _bRequest;

    //����ʧ�ܵĴ���
    int _iFailTimes;

    //�´θ���ʱ��,��λ��
    int64_t _iRefreshTime;

    //int64_t _lastCacheEndpointTime;
};

class EndpointManager : public QueryEpBase
{

public:
    EndpointManager(ObjectProxy* pObjectProxy,Communicator* pComm,const string & sObjName,bool bFirstNetThread,const string& setName="");

    virtual ~EndpointManager();

    //void init(const string& sObjName);

	/**
	 * ����������Դӿ��õķ����б�ѡ��һ������ڵ�
	 */
    bool selectAdapterProxy(ReqMessage * msg,AdapterProxy * & pAdapterProxy);
    const vector<AdapterProxy*> & getAdapters()
    {
        return _vAllProxys;
    }

public:

    void notifyEndpoints(const set<EndpointInfo> & active,const set<EndpointInfo> & inactive,bool bSync=false);
    void doNotify();

public:

    //��ָ����һ��adapter��ѡȡһ����Ч��
    AdapterProxy * selectFromGroup(ReqMessage * msg);

private:

    //��ѯѡȡһ�����
    AdapterProxy * getNextValidProxy();

    //����hashֵѡȡһ�����
    AdapterProxy* getHashProxy(int64_t hashCode);

	AdapterProxy* getNextWeightedProxy();
    //���½ڵ�Ȩֵ������ڵ�����ӳ���
    void updateWeightIndex(const map<int,size_t>& mLoadedIndex);
private:

    ObjectProxy * _pObjectProxy;

private:
    size_t _iLastRoundPosition;

    //��Ծ�ڵ�
	vector<AdapterProxy*> _vLoadedActiveProxy;
	/**
	 * �ڵ�Ȩֵ������ڵ�����ӳ���
	 */
    map<size_t,int>  _mWeightIndex;
	/**
	 * �ϴ�����ӳ����ʱ��
	 */
	int64_t _iLastBuildMapTime;
	/**
	 * ����ֵ����Ƶ��,��λ����
	 */
    int32_t _UpdateLoadedInterval;
private:

    //��Ծ�Ľ��
    vector<AdapterProxy*> _vActiveProxys;

    //����Ľ�� ������Ծ�ĺͲ���Ծ��
    map<string,AdapterProxy*> _regProxys;
    vector<AdapterProxy*> _vRegProxys;

    //��������create�Ľ��
    map<string,AdapterProxy*> _allProxys;
    vector<AdapterProxy*> _vAllProxys;
};

//һ��type_obj �Ķ���
class EndpointThread : public QueryEpBase
{

public:
    EndpointThread(Communicator* pComm,const string & sObjName,GetEndpointType type,const string & sSetName,bool bFirstNetThread=false);
	~EndpointThread(){};
public:

    void getEndpoints(vector<EndpointInfo> &activeEndPoint, vector<EndpointInfo> &inactiveEndPoint);
    void getTCEndpoints(vector<TC_Endpoint> &activeEndPoint, vector<TC_Endpoint> &inactiveEndPoint);

    void notifyEndpoints(const set<EndpointInfo> & active,const set<EndpointInfo> & inactive,bool bSync);
    //�麯�� �ò��� ֱ���ÿպ���
    void doNotify()
    {
    }

private:

    //��Ծ�Ľ��
    vector<EndpointInfo> _vActiveEndPoint;
    vector<TC_Endpoint> _vActiveTCEndPoint;

    //����Ծ�Ľ��
    vector<EndpointInfo> _vInactiveEndPoint;
    vector<TC_Endpoint> _vInactiveTCEndPoint;

    TC_ThreadLock _lock;

    string _sName;
    GetEndpointType _type;
};

//����ӿ������class
class EndpointManagerThread
{
    public:
        EndpointManagerThread(Communicator * pComm,const string & sObjName);
		~EndpointManagerThread();
    public:
        //��ȡĬ�ϵĽ��
        void getEndpoint(vector<EndpointInfo> &activeEndPoint,
                vector<EndpointInfo> &inactiveEndPoint);

        //��ȡ���еĽ��
        void getEndpointByAll(vector<EndpointInfo> &activeEndPoint,
                vector<EndpointInfo> &inactiveEndPoint);

        //����set��ȡ���
        void getEndpointBySet(const string sName,
                vector<EndpointInfo> &activeEndPoint,
                vector<EndpointInfo> &inactiveEndPoint);

        //���ݵ�����ȡ���
        void getEndpointByStation(const string sName,
                vector<EndpointInfo> &activeEndPoint,
                vector<EndpointInfo> &inactiveEndPoint);


        //��ȡĬ�ϵĽ��
        void getTCEndpoint( vector<TC_Endpoint> &activeEndPoint,
                vector<TC_Endpoint> &inactiveEndPoint);

        //��ȡ���еĽ��
        void getTCEndpointByAll(vector<TC_Endpoint> &activeEndPoint,
                vector<TC_Endpoint> &inactiveEndPoint);

        //����set��ȡ���
        void getTCEndpointBySet(const string sName,
                vector<TC_Endpoint> &activeEndPoint,
                vector<TC_Endpoint> &inactiveEndPoint);

        //���ݵ�����ȡ���
        void getTCEndpointByStation(const string sName,
                vector<TC_Endpoint> &activeEndPoint,
                vector<TC_Endpoint> &inactiveEndPoint);

    protected:
        EndpointThread * getEndpointThread(GetEndpointType type,const string & sName);
    private:
        Communicator * _pCommunicator;

        //��������map
        map<string,EndpointThread*> _mInfo;

        string _sObjName;

        //lock
        TC_ThreadLock _lock;
};

////////////////////////////////////////////////////////////////////////
}
#endif
