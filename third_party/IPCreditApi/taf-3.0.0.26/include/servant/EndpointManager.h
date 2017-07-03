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

    //从主控请求到数据了 最开始调用主控 要通知
    virtual void doNotify() = 0;

    //初始化接口 设置obj和主控名字
    bool init(const string & sObjName,const string & sLocator,const string& setName="");
protected:

    //刷新主控
    void refreshReg(GetEndpointType type,const string & sName);

private:
    void setEndpoints(const string & sEndpoints, set<EndpointInfo> & setEndpoints);

    //设置obj名字,如果是间接连接，则设置主控代理，并从缓存中加载相应的列表
    void setObjName(const string & sObjName);

    void doEndpoints(const vector<taf::EndpointF>& activeEp, const vector<taf::EndpointF>& inactiveEp,int iRet,bool bSync=false);
    void doEndpointsExp(int iRet,bool bSync = false);

    //主控信息刷新到数据库
    void setEndPointToCache(bool bInactive);

protected:

    //是否直连标识
    bool _bDirectProxy;

    //表示是否接口的请求
    bool _bThread;

    //obj name
    string _sObjName;

	//指定set调用的setid,默认为空，如果有值，则说明是指定set调用
	string _invokeSetId;

	QueryFPrx _queryFPrx;

    Communicator * _pCommunicator;

    //活跃节点列表
    set<EndpointInfo> _activeEndpoints;

    //不活跃节点列表 在管理平台里面还是有的
    set<EndpointInfo> _inactiveEndpoints;

    //数据是否有效。只有请求过主控或者从文件load过来的数据才是有效数据
    //初始化的时候是无效的数据
    bool _valid;

private:


    //是否第一个网络线程
    bool _bFirstNetThread;

	//主控地址
	string _sLocator;
private:

    //请求列表的频率,单位秒
    int _iRefreshInterval;

    //请求失败的间隔时间
    int _iFailInterval;

    //请求回来活跃列表为空的间隔时间
    int _iActiveEmptyInterval;

    //连续请求失败的次数
    int _iMaxFailTimes;

    //达到连续请求失败的次数 以后的时间间隔
    int _iManyFailInterval;

    //请求主控的超时时间
    int _iTimeOut;

    //请求主控的等待时间
    int _iWaitTime;

    //请求的超时时间(绝对时间) 防止请求回调丢了。一直在正在请求状态
    int64_t _iRequestTimeout;

    //是否正在请求
    bool _bRequest;

    //连续失败的次数
    int _iFailTimes;

    //下次更新时间,单位秒
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
	 * 根据请求策略从可用的服务列表选择一个服务节点
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

    //从指定的一组adapter中选取一个有效的
    AdapterProxy * selectFromGroup(ReqMessage * msg);

private:

    //轮询选取一个结点
    AdapterProxy * getNextValidProxy();

    //根据hash值选取一个结点
    AdapterProxy* getHashProxy(int64_t hashCode);

	AdapterProxy* getNextWeightedProxy();
    //更新节点权值区间与节点索引映射表
    void updateWeightIndex(const map<int,size_t>& mLoadedIndex);
private:

    ObjectProxy * _pObjectProxy;

private:
    size_t _iLastRoundPosition;

    //活跃节点
	vector<AdapterProxy*> _vLoadedActiveProxy;
	/**
	 * 节点权值区间与节点索引映射表
	 */
    map<size_t,int>  _mWeightIndex;
	/**
	 * 上次重新映射表的时间
	 */
	int64_t _iLastBuildMapTime;
	/**
	 * 负载值更新频率,单位毫秒
	 */
    int32_t _UpdateLoadedInterval;
private:

    //活跃的结点
    vector<AdapterProxy*> _vActiveProxys;

    //部署的结点 包括活跃的和不活跃的
    map<string,AdapterProxy*> _regProxys;
    vector<AdapterProxy*> _vRegProxys;

    //所有曾经create的结点
    map<string,AdapterProxy*> _allProxys;
    vector<AdapterProxy*> _vAllProxys;
};

//一个type_obj 的对象
class EndpointThread : public QueryEpBase
{

public:
    EndpointThread(Communicator* pComm,const string & sObjName,GetEndpointType type,const string & sSetName,bool bFirstNetThread=false);
	~EndpointThread(){};
public:

    void getEndpoints(vector<EndpointInfo> &activeEndPoint, vector<EndpointInfo> &inactiveEndPoint);
    void getTCEndpoints(vector<TC_Endpoint> &activeEndPoint, vector<TC_Endpoint> &inactiveEndPoint);

    void notifyEndpoints(const set<EndpointInfo> & active,const set<EndpointInfo> & inactive,bool bSync);
    //虚函数 用不到 直接用空函数
    void doNotify()
    {
    }

private:

    //活跃的结点
    vector<EndpointInfo> _vActiveEndPoint;
    vector<TC_Endpoint> _vActiveTCEndPoint;

    //不活跃的结点
    vector<EndpointInfo> _vInactiveEndPoint;
    vector<TC_Endpoint> _vInactiveTCEndPoint;

    TC_ThreadLock _lock;

    string _sName;
    GetEndpointType _type;
};

//对外接口请求的class
class EndpointManagerThread
{
    public:
        EndpointManagerThread(Communicator * pComm,const string & sObjName);
		~EndpointManagerThread();
    public:
        //获取默认的结点
        void getEndpoint(vector<EndpointInfo> &activeEndPoint,
                vector<EndpointInfo> &inactiveEndPoint);

        //获取所有的结点
        void getEndpointByAll(vector<EndpointInfo> &activeEndPoint,
                vector<EndpointInfo> &inactiveEndPoint);

        //根据set获取结点
        void getEndpointBySet(const string sName,
                vector<EndpointInfo> &activeEndPoint,
                vector<EndpointInfo> &inactiveEndPoint);

        //根据地区获取结点
        void getEndpointByStation(const string sName,
                vector<EndpointInfo> &activeEndPoint,
                vector<EndpointInfo> &inactiveEndPoint);


        //获取默认的结点
        void getTCEndpoint( vector<TC_Endpoint> &activeEndPoint,
                vector<TC_Endpoint> &inactiveEndPoint);

        //获取所有的结点
        void getTCEndpointByAll(vector<TC_Endpoint> &activeEndPoint,
                vector<TC_Endpoint> &inactiveEndPoint);

        //根据set获取结点
        void getTCEndpointBySet(const string sName,
                vector<TC_Endpoint> &activeEndPoint,
                vector<TC_Endpoint> &inactiveEndPoint);

        //根据地区获取结点
        void getTCEndpointByStation(const string sName,
                vector<TC_Endpoint> &activeEndPoint,
                vector<TC_Endpoint> &inactiveEndPoint);

    protected:
        EndpointThread * getEndpointThread(GetEndpointType type,const string & sName);
    private:
        Communicator * _pCommunicator;

        //保存对象的map
        map<string,EndpointThread*> _mInfo;

        string _sObjName;

        //lock
        TC_ThreadLock _lock;
};

////////////////////////////////////////////////////////////////////////
}
#endif
