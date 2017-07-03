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

//seq �������
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
 * ��ȡÿ���̵߳�˽�ж���
 * ҵ���̸߳������̵߳�ͨ�Ŷ���
 */
struct ReqQueueThread
{
    ReqQueueThread():msgQueue(100000),iSeq(0){}

    ReqInfoQueue msgQueue;
    size_t iSeq;
};

//�߳�˽������
class ServantProxyThreadData : public TC_ThreadPool::ThreadData
{
public:
    static TC_ThreadMutex _mutex;  //ȫ�ֵĻ�����
    static pthread_key_t _key;   //˽���߳�����key
    //static size_t _iSeq;
    static SeqManager * _pSeq;

    static int INVALID_GRID_CODE;

    /**
     * ���캯��
     */
    ServantProxyThreadData();

    /**
     * ��������
     */
    ~ServantProxyThreadData();

    /**
     * ������Դ�ͷ�
     * @param p
     */
    static void destructor(void* p);

    /**
     * ��ȡ�߳����ݣ�û�еĻ����Զ�����
     * @return ServantProxyThreadData*
     */
    static ServantProxyThreadData * getData();

public:
    /*
     * ÿ��ҵ���̸߳�epollͨ�ŵĶ���
     */
    ReqInfoQueue * _pReqQueue[MAX_CLIENT_EPOLL_NUM];
    bool _bQueueInit;
    size_t _iReqQNo;

    /**
     * servantѡ��eopll����ѵֵ
     */
    size_t _iNetSeq;

    /**
     * hash����,�ͻ���ÿ�ε��ö���������
     */
    bool        _bHash;         //�Ƿ�hash
    int64_t     _iHashCode;     //hashֵ

    /**
     * Ⱦɫ��Ϣ
     */
    bool        _bDyeing;       //��ʶ��ǰ�߳��Ƿ���ҪȾɫ
    string      _dyeingKey;     //Ⱦɫ��keyֵ

    //�����̷߳��������ص��Լ��������߳�������
    //-1��Ĭ��ֵ ������ ������ѯ���� ����ֵ�������̵߳�id
    int         _netThreadSeq;

    int32_t     _gridCode;      //�Ҷ�״ֵ̬
    string      _routeKey;      //�Ҷ�·�ɵ�keyֵ

	/**
	 * ����ͻ������ýӿڼ���ĳ�ʱʱ��,����ͬ�����첽
	 */
	bool        _bHasTimeout;    //�Ƿ����ó�ʱ��
	int			_iTimeout;       //��ʱʱ��

	//�Ƿ񰴸���ֵ����
	bool 	    _bLoaded;

	char        _szHost[64];    //���öԶ˵�ַ

    SampleKey   _sampleKey;

	/**
	 * ObjectProxy������
	 */
    size_t _iObjectProxyNum;

    /**
     * ������ObjectProxy
     * ObjectProxy ������
     */
    ObjectProxy ** _ppObjectProxy;
};

//////////////////////////////////////////////////////////////////////////
// �첽�ص�����Ļ���
class ServantProxyCallback : virtual public TC_HandleBase
{
public:
    /**
     * ����
     */
    ServantProxyCallback();

    /**
     * ��������
     */
    virtual ~ServantProxyCallback() {}

    /**
     * ��ȡ����
     * @return const string&
     */
    virtual const string& getType() { return _type; }

    /**
     * ��������
     * @return const string&
     */
    virtual void setType(const string& type) { _type = type; }


    /**
     * �첽�����Ƿ��������̴߳���
     * taf�ڲ��õĵ� ҵ�����������ֵ
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
     * �첽�ص�����ʵ�ָ÷���������ҵ���߼�����
     * @param msg
     * @return int
     */
    virtual int onDispatch(ReqMessagePtr msg) = 0;

protected:

    /**
     * ͬһ��·���cb��ʱ�������������class����
     */
    string _type;
    /**
     * �첽�����Ƿ��������̴߳���
     * taf�ڲ��õĵ� ҵ�����������ֵ
     * */
    bool _bNetThreadProcess;
};

//////////////////////////////////////////////////////////////////////////
/**
 * ServantProxy��·��������ҵ��ʵ�ֲ�ע�ᵽServantProxy
 * �������л��߼����£�
 * -���������ݻҶ�״̬����
 * -������ݵ�ǰ���û��Ҷ�״̬�ӷ�����ѡ����Ӧ�ķ������������
 * -���ĳ̨��������ʱ����һ��ֵ�� ����̨����������ΪʧЧ״̬���ر��������ӣ� ����ʱ����
 * -���峬ʱ���ж��߼���μ�taf_check_timeout˵��
 */
struct ServantProxyRouter : public TC_HandleBase
{
    /**
     * ��������
     */
    virtual ~ServantProxyRouter() {}

    /**
     * ����key��ȡ�û��ĻҶ�״̬����ҵ��ʵ�֣�
     * ����INVALID_GRID_CODE����Ϊ�޻Ҷ�״̬
     * @param key
     * @return int
     */
    virtual int getGridByKey(const string& key) = 0;
};

//////////////////////////////////////////////////////////////////////////
/**
 * 1:Զ�̶���ı��ش���
 * 2:ͬ��servant��һ��ͨ���������ֻ��һ��ʵ��
 * 3:��ֹ���û���Jce�ж���ĺ�������ͻ���ӿ���taf_��ͷ
 */
class EndpointManagerThread;

class ServantProxy : public TC_HandleBase, public TC_ThreadMutex
{
public:
    /**
     * ͨ��status��������ʱ�õ���ȱʡ�ַ���
     */
    static string STATUS_DYED_KEY;  //��ҪȾɫ���û�ID

    static string STATUS_GRID_KEY;  //��Ҫ·�ɵ��û�ID

    static string STATUS_GRID_CODE; //Ŀ�����ĻҶ�ֵ

    static string STATUS_SAMPLE_KEY; //stat ��������Ϣ

    static string STATUS_RESULT_CODE; //��������,wupʹ��

    static string STATUS_RESULT_DESC; //����������,wupʹ��
	static string STATUS_LOADED_VALUE; //����˸���ֵ

    /**
     * ȱʡ��ͬ�����ó�ʱʱ��
     * ��ʱ�󲻱�֤��Ϣ���ᱻ����˴���
     */
    enum { DEFAULT_SYNCTIMEOUT = 3000, DEFAULT_ASYNCTIMEOUT=5000};

    /**
     * ���캯��
     * @param op
     */
    ServantProxy(Communicator * pCommunicator, ObjectProxy ** ppObjectProxy, size_t iNum);

    /**
     * ��������
     */
    virtual ~ServantProxy();
public:

    /**
     * ��ȡObject���÷����б� �������set���ȡ��set��,������÷���,ֻ����ͬ����ķ����ip
     *  @return void
     */
    void taf_endpoints(vector<EndpointInfo> &activeEndPoint, vector<EndpointInfo> &inactiveEndPoint);

    /**
     * ��ȡObject���÷����б� ���е��б�
     *  @return void
     */
    void taf_endpointsAll(vector<EndpointInfo> &activeEndPoint, vector<EndpointInfo> &inactiveEndPoint);

    /**
     * ��ȡObject���÷����б� ����set���ֻ�ȡ
     *  @return void
     */
    void taf_endpointsBySet(const string & sName,vector<EndpointInfo> &activeEndPoint, vector<EndpointInfo> &inactiveEndPoint);

    /**
     * ��ȡObject���÷����б� ���ݵ������ֻ�ȡ
     *  @return void
     */
    void taf_endpointsByStation(const string & sName,vector<EndpointInfo> &activeEndPoint, vector<EndpointInfo> &inactiveEndPoint);

    /**
     *  ��ȡObject���÷����б� ����ָ��������
     *  @return vector<TC_Endpoint>
     **/
    vector<TC_Endpoint> taf_endpoints(const std::string & sStation);

    /**
     * ��ȡObject���÷����б� ����ָ��������
     *  @return void
     */
    void taf_endpoints(const std::string & sStation, vector<TC_Endpoint> & vecActive, vector<TC_Endpoint> & vecInactive);

    /**
     * ��ȡObject���÷����б� ������÷���,ֻ����ͬ����ķ����ip
     * @return vector<TC_Endpoint>
     * ��ôû�� taf ��ͷ-_-!!!!!!!
     */
    vector<TC_Endpoint> getEndpoint();

    /**
     * ��ȡObject���÷����б� ��������IDC
     * @return vector<TC_Endpoint>
     * ��ôû�� taf ��ͷ-_-!!!!!!!
     */
    vector<TC_Endpoint> getEndpoint4All();

public:

    /**
     * ��ȡͨ����
     *
     * @return Communicator*
     */
    Communicator* taf_communicator() { return _pCommunicator; }

    /**
     * ���Ͳ�����Ϣ��������
     */
    void taf_ping();

    /**
     * ����ͬ�����ó�ʱʱ�䣬�Ը�proxy�����з�������Ч
     * @param msecond
     */
    void taf_timeout(int msecond);

    /**
     * ��ȡͬ�����ó�ʱʱ�䣬�Ը�proxy�����з�������Ч
     * @return int
     */
    int taf_timeout() const;

    /**
     * �������ӳ�ʱʱ��
     * @return int
     */
    void taf_connect_timeout(int conTimeout);

    /**
     * ��ȡ������Object����
     * @return string
     */
    string taf_name() const;


    /**
     * ��ȡ���һ�ε��õ�IP��ַ�Ͷ˿�
     * @return string
     */
    static TC_Endpoint taf_invoke_endpoint();

    /**
     * �����û��Զ���Э��
     * @param protocol
     */
    void taf_set_protocol(const ProxyProtocol& protocol);

    /**
    *�����׽���ѡ��
    */
    void taf_set_sockopt(int level, int optname, const void *optval, socklen_t optlen);

    /**
     * hash������Ϊ��֤һ��ʱ����ͬһ��key����Ϣ����
     * ����ͬ�ķ���ˣ����ڷ����б�̬�仯������
     * ���ϸ�֤
     * @param key
     * @return ServantProxy*
     */
    virtual ServantProxy* taf_hash(int64_t key);

	/**
	 * ����Ȩֵ���е���
	 */
	virtual ServantProxy* taf_loaded();
    /**
     * �����ǰ��Hash����
     * �պ��� Ϊ�˼�����ǰ��
     * @param key
     * @return ServantProxy*
     */
    void taf_clear_hash();

	/**
	 * ��Կͻ��˵��ýӿڼ���ĳ�ʱʱ�����ã�����ͬ�����첽����
	 * ÿ�νӿڵ��ö��������ã�����ȡϵͳĬ�ϳ�ʱʱ��
	 *
	 * @param msecond ��λ����
	 * @return ServantProxy*
	 * ʾ��: prxy->taf_set_timeout(3000)->sayHello();
	 */
	virtual ServantProxy* taf_set_timeout(int msecond);

    /**
     * �����첽���ó�ʱʱ�䣬�Ը�proxy�����з�������Ч
     * @param msecond
     */
    void taf_async_timeout(int msecond);

    /**
     * ��ȡ�첽���ó�ʱʱ�䣬�Ը�proxy�����з�������Ч
     * @return int
     */
    int taf_async_timeout() const;

    /**
     * ���ö���·����
     * @param router
     */
    virtual void taf_set_router(const ServantProxyRouterPtr& router);

    /**
     * �����ǰ�Ķ���·����
     */
    virtual void taf_del_router();

    /**
     * ��proxy����һ����object�ϵ����к�
     * @return uint32_t
     */
    virtual uint32_t taf_gen_requestid();

    /**
     * ����PUSH����Ϣ����Ӧcallback
     * @param cb
     */
    virtual void taf_set_push_callback(const ServantProxyCallbackPtr& cb);

    /**
     * TAFЭ��ͬ����������
     */
    virtual void taf_invoke(char cPacketType,
                            const string& sFuncName,
                            const vector<char> &buf,
                            const map<string, string>& context,
                            const map<string, string>& status,
                            ResponsePacket& rep);

    /**
     * TAFЭ���첽��������
     */
    virtual void taf_invoke_async(char cPacketType,
                                  const string& sFuncName,
                                  const vector<char> &buf,
                                  const map<string, string>& context,
                                  const map<string, string>& status,
                                  const ServantProxyCallbackPtr& callback);

    /**
     * ��ͨЭ��ͬ��Զ�̵���
     */
    virtual void rpc_call(uint32_t requestId, const string& sFuncName,
                          const char* buff, uint32_t len, ResponsePacket& rsp);

    /**
     * ��ͨЭ���첽����
     */
    virtual void rpc_call_async(uint32_t requestId, const string& sFuncName,
                                const char* buff, uint32_t len,
                                const ServantProxyCallbackPtr& callback);

private:
    /**
     * Զ�̷�������
     * @param req
     * @return int
     */
    void invoke(ReqMessage * msg);

    /**
     * Զ�̷������÷���
     * @param req
     * @return int
     */
    void finished(ReqMessage * msg);

    /**
     * ѡȡһ�������̶߳�Ӧ����Ϣ
     * @param pSptd
     * @return void
     */
    void selectNetThreadInfo(ServantProxyThreadData * pSptd, ObjectProxy * & pObjProxy, ReqInfoQueue * & pReqQ);
	/**
	 * ����Ƿ���Ҫ����Ⱦɫ��Ϣ
	 * @param  req
	 */
	void checkDye(RequestPacket& req);
private:
    friend class ObjectProxy;
    friend class AdapterProxy;

    Communicator * _pCommunicator;

    /**
     * ������ObjectProxy
     * ObjectProxy ������
     */
    ObjectProxy ** _ppObjectProxy;

	/**
	 * ObjectProxy������
	 */
    size_t _iObjectProxyNum;

    /**
     * ҵ�����õ�·����
     */
    ServantProxyRouterPtr _router;

    /**
     * ͬ�����ó�ʱ(����)
     */
    int _syncTimeout;

    /**
     * ͬ�����ó�ʱ(����)
     */
    int _asyncTimeout;

    /**
     * Ψһid
     */
    uint32_t _uiId;

    /**
     * ��ȡendpoint����
     */

    EndpointManagerThread * _endpointInfo;
};
}
#endif
