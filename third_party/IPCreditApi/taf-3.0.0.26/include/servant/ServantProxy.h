#ifndef _TAF_SERVANT_PROXY_H_
#define _TAF_SERVANT_PROXY_H_

#include "servant/Message.h"
#include "servant/AppProtocol.h"
#include "servant/JceCurrent.h"
#include "servant/EndpointInfo.h"
#include "util/tc_monitor.h"
//#include "util/tc_pack.h"
#include "util/tc_autoptr.h"
#include "servant/CommunicatorEpoll.h"

namespace taf
{

/////////////////////////////////////////////////////////////////////////

//seq 管理的类
class SeqManager
{
#define MAX_UNSIGN_SHORT 0xffff
public:
    struct SeqInfo
    {
        uint16_t next;
        bool free;
    };

    SeqManager(size_t iNum);
    uint16_t get();
    void del(uint16_t iSeq);

private:
    uint16_t _iNum;
    uint16_t _iFree;
    uint16_t _iFreeTail;
    SeqInfo * _p;
};

/*
 * 获取每个线程的私有队列
 * 业务线程跟网络线程的通信队列
 */
struct ReqQueueThread
{
    ReqQueueThread():msgQueue(100000),iSeq(0){}

    ReqInfoQueue msgQueue;
    size_t iSeq;
};

//线程私有数据
class ServantProxyThreadData : public TC_ThreadPool::ThreadData
{
public:
    static TC_ThreadMutex _mutex;  //全局的互斥锁
    static pthread_key_t _key;   //私有线程数据key
    //static size_t _iSeq;
    static SeqManager * _pSeq;

    static int INVALID_GRID_CODE;

    /**
     * 构造函数
     */
    ServantProxyThreadData();

    /**
     * 析构函数
     */
    ~ServantProxyThreadData();

    /**
     * 数据资源释放
     * @param p
     */
    static void destructor(void* p);

    /**
     * 获取线程数据，没有的话会自动创建
     * @return ServantProxyThreadData*
     */
    static ServantProxyThreadData * getData();

public:
    /*
     * 每个业务线程跟epoll通信的队列
     */
    ReqInfoQueue * _pReqQueue[MAX_CLIENT_EPOLL_NUM];
    bool _bQueueInit;
    size_t _iReqQNo;

    /**
     * servant选择eopll的轮训值
     */
    size_t _iNetSeq;

    /**
     * hash属性,客户端每次调用都进行设置
     */
    bool        _bHash;         //是否hash
    int64_t     _iHashCode;     //hash值

    /**
     * 染色信息
     */
    bool        _bDyeing;       //标识当前线程是否需要染色
    string      _dyeingKey;     //染色的key值

    //网络线程发起的请求回到自己的网络线程来处理
    //-1是默认值 不处理 按照轮询规则 其它值是网络线程的id
    int         _netThreadSeq;

    int32_t     _gridCode;      //灰度状态值
    string      _routeKey;      //灰度路由的key值

	/**
	 * 允许客户端设置接口级别的超时时间,包括同步和异步
	 */
	bool        _bHasTimeout;    //是否设置超时间
	int			_iTimeout;       //超时时间

	//是否按负载值调用
	bool 	    _bLoaded;

	char        _szHost[64];    //调用对端地址

    SampleKey   _sampleKey;

	/**
	 * ObjectProxy的数量
	 */
    size_t _iObjectProxyNum;

    /**
     * 所属的ObjectProxy
     * ObjectProxy 的数量
     */
    ObjectProxy ** _ppObjectProxy;
};

//////////////////////////////////////////////////////////////////////////
// 异步回调对象的基类
class ServantProxyCallback : virtual public TC_HandleBase
{
public:
    /**
     * 构造
     */
    ServantProxyCallback();

    /**
     * 析构函数
     */
    virtual ~ServantProxyCallback() {}

    /**
     * 获取类型
     * @return const string&
     */
    virtual const string& getType() { return _type; }

    /**
     * 设置类型
     * @return const string&
     */
    virtual void setType(const string& type) { _type = type; }


    /**
     * 异步请求是否在网络线程处理
     * taf内部用的到 业务不能设置这个值
     * */
    inline void setNetThreadProcess(bool bNetThreadProcess)
    {
        _bNetThreadProcess = bNetThreadProcess;
    }

    inline bool getNetThreadProcess()
    {
        return _bNetThreadProcess;
    }

public:
    /**
     * 异步回调对象实现该方法，进行业务逻辑处理
     * @param msg
     * @return int
     */
    virtual int onDispatch(ReqMessagePtr msg) = 0;

protected:

    /**
     * 同一链路多个cb的时候可以用来区分class类型
     */
    string _type;
    /**
     * 异步请求是否在网络线程处理
     * taf内部用的到 业务不能设置这个值
     * */
    bool _bNetThreadProcess;
};

//////////////////////////////////////////////////////////////////////////
/**
 * ServantProxy的路由器，由业务实现并注册到ServantProxy
 * 完整的切换逻辑如下：
 * -服务器根据灰度状态分组
 * -请求根据当前的用户灰度状态从分组中选择相应的服务器随机发送
 * -如果某台服务器超时超过一定值， 则将这台服务器设置为失效状态并关闭所有连接， 并定时重试
 * -具体超时的判断逻辑请参见taf_check_timeout说明
 */
struct ServantProxyRouter : public TC_HandleBase
{
    /**
     * 析构函数
     */
    virtual ~ServantProxyRouter() {}

    /**
     * 根据key获取用户的灰度状态，由业务实现，
     * 返回INVALID_GRID_CODE则认为无灰度状态
     * @param key
     * @return int
     */
    virtual int getGridByKey(const string& key) = 0;
};

//////////////////////////////////////////////////////////////////////////
/**
 * 1:远程对象的本地代理
 * 2:同名servant在一个通信器中最多只有一个实例
 * 3:防止和用户在Jce中定义的函数名冲突，接口以taf_开头
 */
class EndpointManagerThread;

class ServantProxy : public TC_HandleBase, public TC_ThreadMutex
{
public:
    /**
     * 通过status传递数据时用到的缺省字符串
     */
    static string STATUS_DYED_KEY;  //需要染色的用户ID

    static string STATUS_GRID_KEY;  //需要路由的用户ID

    static string STATUS_GRID_CODE; //目标服务的灰度值

    static string STATUS_SAMPLE_KEY; //stat 采样的信息

    static string STATUS_RESULT_CODE; //处理结果码,wup使用

    static string STATUS_RESULT_DESC; //处理结果描述,wup使用
	static string STATUS_LOADED_VALUE; //服务端负载值

    /**
     * 缺省的同步调用超时时间
     * 超时后不保证消息不会被服务端处理
     */
    enum { DEFAULT_SYNCTIMEOUT = 3000, DEFAULT_ASYNCTIMEOUT=5000};

    /**
     * 构造函数
     * @param op
     */
    ServantProxy(Communicator * pCommunicator, ObjectProxy ** ppObjectProxy, size_t iNum);

    /**
     * 析构函数
     */
    virtual ~ServantProxy();
public:

    /**
     * 获取Object可用服务列表 如果启用set则获取本set的,如果启用分组,只返回同分组的服务端ip
     *  @return void
     */
    void taf_endpoints(vector<EndpointInfo> &activeEndPoint, vector<EndpointInfo> &inactiveEndPoint);

    /**
     * 获取Object可用服务列表 所有的列表
     *  @return void
     */
    void taf_endpointsAll(vector<EndpointInfo> &activeEndPoint, vector<EndpointInfo> &inactiveEndPoint);

    /**
     * 获取Object可用服务列表 根据set名字获取
     *  @return void
     */
    void taf_endpointsBySet(const string & sName,vector<EndpointInfo> &activeEndPoint, vector<EndpointInfo> &inactiveEndPoint);

    /**
     * 获取Object可用服务列表 根据地区名字获取
     *  @return void
     */
    void taf_endpointsByStation(const string & sName,vector<EndpointInfo> &activeEndPoint, vector<EndpointInfo> &inactiveEndPoint);

    /**
     *  获取Object可用服务列表 包括指定归属地
     *  @return vector<TC_Endpoint>
     **/
    vector<TC_Endpoint> taf_endpoints(const std::string & sStation);

    /**
     * 获取Object可用服务列表 包括指定归属地
     *  @return void
     */
    void taf_endpoints(const std::string & sStation, vector<TC_Endpoint> & vecActive, vector<TC_Endpoint> & vecInactive);

    /**
     * 获取Object可用服务列表 如果启用分组,只返回同分组的服务端ip
     * @return vector<TC_Endpoint>
     * 怎么没用 taf 开头-_-!!!!!!!
     */
    vector<TC_Endpoint> getEndpoint();

    /**
     * 获取Object可用服务列表 包括所有IDC
     * @return vector<TC_Endpoint>
     * 怎么没用 taf 开头-_-!!!!!!!
     */
    vector<TC_Endpoint> getEndpoint4All();

public:

    /**
     * 获取通信器
     *
     * @return Communicator*
     */
    Communicator* taf_communicator() { return _pCommunicator; }

    /**
     * 发送测试消息到服务器
     */
    void taf_ping();

    /**
     * 设置同步调用超时时间，对该proxy上所有方法都有效
     * @param msecond
     */
    void taf_timeout(int msecond);

    /**
     * 获取同步调用超时时间，对该proxy上所有方法都有效
     * @return int
     */
    int taf_timeout() const;

    /**
     * 设置连接超时时间
     * @return int
     */
    void taf_connect_timeout(int conTimeout);

    /**
     * 获取所属的Object名称
     * @return string
     */
    string taf_name() const;


    /**
     * 获取最近一次调用的IP地址和端口
     * @return string
     */
    static TC_Endpoint taf_invoke_endpoint();

    /**
     * 设置用户自定义协议
     * @param protocol
     */
    void taf_set_protocol(const ProxyProtocol& protocol);

    /**
    *设置套接字选项
    */
    void taf_set_sockopt(int level, int optname, const void *optval, socklen_t optlen);

    /**
     * hash方法，为保证一段时间内同一个key的消息发送
     * 到相同的服务端，由于服务列表动态变化，所以
     * 不严格保证
     * @param key
     * @return ServantProxy*
     */
    virtual ServantProxy* taf_hash(int64_t key);

	/**
	 * 根据权值进行调用
	 */
	virtual ServantProxy* taf_loaded();
    /**
     * 清除当前的Hash数据
     * 空函数 为了兼容以前的
     * @param key
     * @return ServantProxy*
     */
    void taf_clear_hash();

	/**
	 * 针对客户端调用接口级别的超时时间设置，包括同步和异步调用
	 * 每次接口调用都必须设置，否则取系统默认超时时间
	 *
	 * @param msecond 单位毫秒
	 * @return ServantProxy*
	 * 示例: prxy->taf_set_timeout(3000)->sayHello();
	 */
	virtual ServantProxy* taf_set_timeout(int msecond);

    /**
     * 设置异步调用超时时间，对该proxy上所有方法都有效
     * @param msecond
     */
    void taf_async_timeout(int msecond);

    /**
     * 获取异步调用超时时间，对该proxy上所有方法都有效
     * @return int
     */
    int taf_async_timeout() const;

    /**
     * 设置二级路由器
     * @param router
     */
    virtual void taf_set_router(const ServantProxyRouterPtr& router);

    /**
     * 清除当前的二级路由器
     */
    virtual void taf_del_router();

    /**
     * 用proxy产生一个该object上的序列号
     * @return uint32_t
     */
    virtual uint32_t taf_gen_requestid();

    /**
     * 设置PUSH类消息的响应callback
     * @param cb
     */
    virtual void taf_set_push_callback(const ServantProxyCallbackPtr& cb);

    /**
     * TAF协议同步方法调用
     */
    virtual void taf_invoke(char cPacketType,
                            const string& sFuncName,
                            const vector<char> &buf,
                            const map<string, string>& context,
                            const map<string, string>& status,
                            ResponsePacket& rep);

    /**
     * TAF协议异步方法调用
     */
    virtual void taf_invoke_async(char cPacketType,
                                  const string& sFuncName,
                                  const vector<char> &buf,
                                  const map<string, string>& context,
                                  const map<string, string>& status,
                                  const ServantProxyCallbackPtr& callback);

    /**
     * 普通协议同步远程调用
     */
    virtual void rpc_call(uint32_t requestId, const string& sFuncName,
                          const char* buff, uint32_t len, ResponsePacket& rsp);

    /**
     * 普通协议异步调用
     */
    virtual void rpc_call_async(uint32_t requestId, const string& sFuncName,
                                const char* buff, uint32_t len,
                                const ServantProxyCallbackPtr& callback);

private:
    /**
     * 远程方法调用
     * @param req
     * @return int
     */
    void invoke(ReqMessage * msg);

    /**
     * 远程方法调用返回
     * @param req
     * @return int
     */
    void finished(ReqMessage * msg);

    /**
     * 选取一个网络线程对应的信息
     * @param pSptd
     * @return void
     */
    void selectNetThreadInfo(ServantProxyThreadData * pSptd, ObjectProxy * & pObjProxy, ReqInfoQueue * & pReqQ);
	/**
	 * 检查是否需要设置染色消息
	 * @param  req
	 */
	void checkDye(RequestPacket& req);
private:
    friend class ObjectProxy;
    friend class AdapterProxy;

    Communicator * _pCommunicator;

    /**
     * 所属的ObjectProxy
     * ObjectProxy 的数量
     */
    ObjectProxy ** _ppObjectProxy;

	/**
	 * ObjectProxy的数量
	 */
    size_t _iObjectProxyNum;

    /**
     * 业务设置的路由器
     */
    ServantProxyRouterPtr _router;

    /**
     * 同步调用超时(毫秒)
     */
    int _syncTimeout;

    /**
     * 同步调用超时(毫秒)
     */
    int _asyncTimeout;

    /**
     * 唯一id
     */
    uint32_t _uiId;

    /**
     * 获取endpoint对象
     */

    EndpointManagerThread * _endpointInfo;
};
}
#endif
