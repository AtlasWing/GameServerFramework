#ifndef __TAF_MESSAGE_H_
#define __TAF_MESSAGE_H_

#include "servant/Global.h"
//#include "servant/EndpointInfo.h"
#include "util/tc_autoptr.h"
#include "util/tc_monitor.h"
#include "util/tc_loop_queue.h"

namespace taf
{

/**
 * 超时一定比率后进行切换
 * 设置超时检查参数
 * 计算到某台服务器的超时率, 如果连续超时次数或者超时比例超过阀值
 * 默认60s内, 超时调用次数>=2, 超时比率0.5或者连续超时次数>5,
 * 则失效
 * 服务失效后, 请求将尽可能的切换到其他可能的服务器, 并每隔tryTimeInterval尝试一次, 如果成功则认为恢复
 * 如果其他服务器都失效, 则随机选择一台尝试
 * 如果是灰度状态的服务, 服务失效后如果请求切换, 也只会转发到相同灰度状态的服务
 * @uint16_t minTimeoutInvoke, 计算的最小的超时次数, 默认2次(在checkTimeoutInterval时间内超过了minTimeoutInvoke, 才计算超时)
 * @uint32_t frequenceFailInvoke, 连续失败次数
 * @uint32_t checkTimeoutInterval, 统计时间间隔, (默认60s, 不能小于30s)
 * @float radio, 超时比例 > 该值则认为超时了 ( 0.1<=radio<=1.0 )
 * @uint32_t tryTimeInterval, 重试时间间隔
 */
struct CheckTimeoutInfo
{
    uint16_t minTimeoutInvoke;

    uint32_t checkTimeoutInterval;

    uint32_t frequenceFailInvoke;

    uint32_t minFrequenceFailTime;

    float radio;

    uint32_t tryTimeInterval;

    CheckTimeoutInfo() : minTimeoutInvoke(2), checkTimeoutInterval(60), frequenceFailInvoke(5), minFrequenceFailTime(5), radio(0.5), tryTimeInterval(30) {}
};

//stat采样信息(用于调用链时序分析)
struct SampleKey
{
    SampleKey() : _root(true) , _unid(""), _depth(0), _width(0), _parentWidth(0) {}

    void init()
    {
        _root = true;
        _unid.clear();
        _depth = 0;
        _width = 0;
        _parentWidth = 0;
    }

    //是否根节点 在根节点产生唯一id
    bool _root;

    //唯一id
    string _unid;

    //深度
    int _depth;

    //广度
    int _width;

    //父节点广度值
    int _parentWidth;
};

/**
 * 用于同步调用时的条件变量
 */
struct ReqMonitor : public TC_ThreadLock {};//, public TC_HandleBase {};

//typedef TC_AutoPtr<ReqMonitor> ReqMonitorPtr;

#define IS_MSG_TYPE(m, t) ((m & t) != 0)
#define SET_MSG_TYPE(m, t) do { (m |= t); } while (0);
#define CLR_MSG_TYPE(m, t) do { (m &=~t); } while (0);

#define FUNC_LEN  31

struct ReqMessage : public TC_HandleBase
{
    ReqMessage()
    {
        eStatus = ReqMessage::REQ_REQ;
        eType = SYNC_CALL;
        bFromRpc = false;

        callback = NULL;
		proxy = NULL;
		pObjectProxy = NULL;
        pMonitor = NULL;
		bMonitorFin = false;

        iBeginTime = 0;
        iEndTime = 0;
        bHash = false;
        iHashCode = 0;

		adapter = NULL;
        bDyeing = false;
        bPush = false;
    }
    //调用类型
    enum CallType
    {
        SYNC_CALL = 1, //同步
        ASYNC_CALL,    //异步
        ONE_WAY,       //单向
        THREAD_EXIT    //线程退出的标识
    };

    //请求状态
    enum ReqStatus
    {
        REQ_REQ = 0,    //状态正常,正在请求中
        REQ_RSP,        //请求已经发出去
        REQ_TIME,       //请求超时
        REQ_BUSY,       //请求队列满了
        REQ_EXC         //客户端请求异常
    };

    ~ReqMessage()
    {
        if(NULL != pMonitor)
            delete pMonitor;
    }

    void init(CallType eCallType,ObjectProxy * pObj,const string & sFuncName)
    {
        eStatus = ReqMessage::REQ_REQ;
        eType = eCallType;
        bFromRpc = false;

        callback = NULL;
        proxy = NULL;
        pObjectProxy = pObj;

        sReqData.clear();
        pMonitor = NULL;
        bMonitorFin = false;

        iBeginTime = 0;
        iEndTime = 0;
        bHash = false;
        iHashCode = 0;
        adapter = NULL;

        bDyeing = false;

        bPush = false;
    }


    ReqStatus                   eStatus;        //调用的状态
    CallType                    eType;          //调用类型
    bool                        bFromRpc;       //是否是第三方协议的rcp_call，缺省为false
    ServantProxyCallbackPtr     callback;       //异步调用时的回调对象
    ServantProxy *              proxy;
    ObjectProxy *               pObjectProxy;   //调用端的proxy对象
    RequestPacket               request;    //请求消息体
    ResponsePacket              response;   //响应消息体

    string                      sReqData;       //请求消息体

    ReqMonitor *                pMonitor;        //用于同步的monitor
    bool                        bMonitorFin;    //同步请求timewait是否结束
    int64_t                     iBeginTime;     //请求时间
    int64_t                     iEndTime;       //完成时间
    bool                        bHash;          //是否hash调用
    int64_t                     iHashCode;      //hash值，用户保证一个用户的请求发送到同一个server(不严格保证)
    AdapterProxy *              adapter;  //调用的adapter

    bool                        bDyeing;        //是否需要染色
    string                      sDyeingKey;     //染色key

    bool                        bPush;          //push back 消息

    SampleKey                   sampleKey;      //采样信息

};

typedef TC_AutoPtr<ReqMessage>  ReqMessagePtr;
typedef TC_LoopQueue<ReqMessage*,1000>  ReqInfoQueue;


#if 0

/**
 * Proxy端的请求消息结构
 */
struct ReqMessage : public TC_HandleBase//, public TC_ThreadPool::ThreadData
{
    /**
     * 超时的时候调用(异步调用超时)
     *
     * @param ptr
     */
    static void timeout(ReqMessagePtr& ptr);

    //调用类型
    enum CallType
    {
        SYNC_CALL = 1, //同步
        ASYNC_CALL,    //异步
        ONE_WAY,       //单向
    };

    /**
     * 构造函数
     */
    ReqMessage(Communicator *comm);

    /**
     * 析构函数
     */
    ~ReqMessage();

public:
    /**
     * 判断消息类型
     *
     * @return bool
     */
    bool is_message_type(uint32_t msg, uint32_t type);
    /**
     *
     * @param msg
     * @param type
     */
    void set_message_type(uint32_t& msg, uint32_t type);
    /**
     *
     * @param msg
     * @param type
     */
    void clr_message_type(uint32_t& msg, uint32_t type);

protected:
    //不能赋值
    ReqMessage& operator=(const ReqMessage &);

protected:
    Communicator*               comm;       //通信器

public:
    ReqMessage::CallType        type;       //调用类型
    ServantPrx::element_type*   proxy;      //调用端的proxy对象
    ReqMonitor*                 monitor;    //用于同步的monitor
    AdapterProxy*               adapter;    //哪台服务器发送的
    RequestPacket               request;    //请求消息体
    ResponsePacket              response;   //响应消息体
    ServantProxyCallbackPtr     callback;   //异步调用时的回调对象
    timeval                     begtime;    //请求时间
    timeval                     endtime;    //完成时间
    int64_t                     hashCode;   //hash值，用户保证一个用户的请求发送到同一个server(不严格保证)
    bool                        fromRpc;    //是否是第三方协议的rcp_call，缺省为false

};
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif
