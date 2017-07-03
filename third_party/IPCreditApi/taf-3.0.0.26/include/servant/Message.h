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
 * ��ʱһ�����ʺ�����л�
 * ���ó�ʱ������
 * ���㵽ĳ̨�������ĳ�ʱ��, ���������ʱ�������߳�ʱ����������ֵ
 * Ĭ��60s��, ��ʱ���ô���>=2, ��ʱ����0.5����������ʱ����>5,
 * ��ʧЧ
 * ����ʧЧ��, ���󽫾����ܵ��л����������ܵķ�����, ��ÿ��tryTimeInterval����һ��, ����ɹ�����Ϊ�ָ�
 * ���������������ʧЧ, �����ѡ��һ̨����
 * ����ǻҶ�״̬�ķ���, ����ʧЧ����������л�, Ҳֻ��ת������ͬ�Ҷ�״̬�ķ���
 * @uint16_t minTimeoutInvoke, �������С�ĳ�ʱ����, Ĭ��2��(��checkTimeoutIntervalʱ���ڳ�����minTimeoutInvoke, �ż��㳬ʱ)
 * @uint32_t frequenceFailInvoke, ����ʧ�ܴ���
 * @uint32_t checkTimeoutInterval, ͳ��ʱ����, (Ĭ��60s, ����С��30s)
 * @float radio, ��ʱ���� > ��ֵ����Ϊ��ʱ�� ( 0.1<=radio<=1.0 )
 * @uint32_t tryTimeInterval, ����ʱ����
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

//stat������Ϣ(���ڵ�����ʱ�����)
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

    //�Ƿ���ڵ� �ڸ��ڵ����Ψһid
    bool _root;

    //Ψһid
    string _unid;

    //���
    int _depth;

    //���
    int _width;

    //���ڵ���ֵ
    int _parentWidth;
};

/**
 * ����ͬ������ʱ����������
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
    //��������
    enum CallType
    {
        SYNC_CALL = 1, //ͬ��
        ASYNC_CALL,    //�첽
        ONE_WAY,       //����
        THREAD_EXIT    //�߳��˳��ı�ʶ
    };

    //����״̬
    enum ReqStatus
    {
        REQ_REQ = 0,    //״̬����,����������
        REQ_RSP,        //�����Ѿ�����ȥ
        REQ_TIME,       //����ʱ
        REQ_BUSY,       //�����������
        REQ_EXC         //�ͻ��������쳣
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


    ReqStatus                   eStatus;        //���õ�״̬
    CallType                    eType;          //��������
    bool                        bFromRpc;       //�Ƿ��ǵ�����Э���rcp_call��ȱʡΪfalse
    ServantProxyCallbackPtr     callback;       //�첽����ʱ�Ļص�����
    ServantProxy *              proxy;
    ObjectProxy *               pObjectProxy;   //���ö˵�proxy����
    RequestPacket               request;    //������Ϣ��
    ResponsePacket              response;   //��Ӧ��Ϣ��

    string                      sReqData;       //������Ϣ��

    ReqMonitor *                pMonitor;        //����ͬ����monitor
    bool                        bMonitorFin;    //ͬ������timewait�Ƿ����
    int64_t                     iBeginTime;     //����ʱ��
    int64_t                     iEndTime;       //���ʱ��
    bool                        bHash;          //�Ƿ�hash����
    int64_t                     iHashCode;      //hashֵ���û���֤һ���û��������͵�ͬһ��server(���ϸ�֤)
    AdapterProxy *              adapter;  //���õ�adapter

    bool                        bDyeing;        //�Ƿ���ҪȾɫ
    string                      sDyeingKey;     //Ⱦɫkey

    bool                        bPush;          //push back ��Ϣ

    SampleKey                   sampleKey;      //������Ϣ

};

typedef TC_AutoPtr<ReqMessage>  ReqMessagePtr;
typedef TC_LoopQueue<ReqMessage*,1000>  ReqInfoQueue;


#if 0

/**
 * Proxy�˵�������Ϣ�ṹ
 */
struct ReqMessage : public TC_HandleBase//, public TC_ThreadPool::ThreadData
{
    /**
     * ��ʱ��ʱ�����(�첽���ó�ʱ)
     *
     * @param ptr
     */
    static void timeout(ReqMessagePtr& ptr);

    //��������
    enum CallType
    {
        SYNC_CALL = 1, //ͬ��
        ASYNC_CALL,    //�첽
        ONE_WAY,       //����
    };

    /**
     * ���캯��
     */
    ReqMessage(Communicator *comm);

    /**
     * ��������
     */
    ~ReqMessage();

public:
    /**
     * �ж���Ϣ����
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
    //���ܸ�ֵ
    ReqMessage& operator=(const ReqMessage &);

protected:
    Communicator*               comm;       //ͨ����

public:
    ReqMessage::CallType        type;       //��������
    ServantPrx::element_type*   proxy;      //���ö˵�proxy����
    ReqMonitor*                 monitor;    //����ͬ����monitor
    AdapterProxy*               adapter;    //��̨���������͵�
    RequestPacket               request;    //������Ϣ��
    ResponsePacket              response;   //��Ӧ��Ϣ��
    ServantProxyCallbackPtr     callback;   //�첽����ʱ�Ļص�����
    timeval                     begtime;    //����ʱ��
    timeval                     endtime;    //���ʱ��
    int64_t                     hashCode;   //hashֵ���û���֤һ���û��������͵�ͬһ��server(���ϸ�֤)
    bool                        fromRpc;    //�Ƿ��ǵ�����Э���rcp_call��ȱʡΪfalse

};
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif
