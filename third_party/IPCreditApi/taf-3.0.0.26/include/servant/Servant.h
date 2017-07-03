#ifndef __TAF_SERVANT_H_
#define __TAF_SERVANT_H_

#include "util/tc_autoptr.h"
#include "util/tc_epoll_server.h"
#include "servant/ServantProxy.h"
#include "servant/JceCurrent.h"
#include "servant/BaseNotify.h"
#include "servant/BusF.h"

namespace taf
{

class BaseNotify;

/**
 * ÿ������Ļ���
 */
class Servant : public BaseNotify
{
public:
    /**
     * ���캯��
     */
    Servant();

    /**
     * ��������
     */
    ~Servant();

    /**
     * ��������
     * @param name
     */
    void setName(const string &name);

    /**
     * ����
     * @return string
     */
    string getName() const;

    /**
     * ����������Handle
     * @param handle 
     */
    void setHandle(TC_EpollServer::Handle* handle);

    /**
     * ��ȡ������Handle
     * @return HandlePtr& 
     */
    TC_EpollServer::Handle* getHandle();

    /**
     * ��ʼ��
     * @return int
     */
    virtual void initialize() = 0;

    /**
     * �˳�
     */
    virtual void destroy() = 0;

public:
    /**
     * �ַ��յ�������
     * @param sRecv
     * @param sFuncName
     * @param sSend
     * @return int
     */
    virtual int dispatch(JceCurrentPtr current, vector<char> &buffer);

    /**
     * tafЭ�飬�ַ�����������
     * @param sRecv
     * @param sFuncName
     * @param sSend
     * @return int
     */
    virtual int onDispatch(JceCurrentPtr current, vector<char> &buffer) { return -1; }

public:
    /**
     * ��ͨЭ�������û�з�����������ҪDispatch
     * @param current 
     * @param buffer 
     * @return int 
     */
    virtual int doRequest(JceCurrentPtr current, vector<char> &buffer) { return -1; }

    /**
     * ��Ϊ�ͻ��˷�������serverʱ���ɹ����ص���Ӧ�ӿ�
     * @param resp 
     * @return int 
     */
    virtual int doResponse(ReqMessagePtr resp) { return -1; }

    /**
     * ��Ϊ�ͻ��˷�������serverʱ�����������쳣����Ӧ�ӿ�
     * @param resp 
     * @return int 
     */
    virtual int doResponseException(ReqMessagePtr resp) { return -1; }

    /**
     * ��Ϊ�ͻ��˷�������serverʱ�����respû���ҵ�request,����Ӧ�ýӿ�
     * ����:push��Ϣ���߳�ʱ�����˳ٵ�����Ӧ
     * @param resp 
     * @return int 
     */
    virtual int doResponseNoRequest(ReqMessagePtr resp) { return -1; }

    /**
     * ÿ��handle�����Ѻ󶼻���ã�ҵ�����ͨ���������߳��е���handle��notify
     * ʵ�������߳��д����������ݵĹ��ܣ����綨ʱ�����������������첽��Ӧ;
	 * [һ�㶼��Ҫ���ҵ�����еĶ���ʹ�ã����п��Է�װ��ServantImp������] 
	 *  
	 * ���ڲ���bExpectIdle�� 
	 * ��ϷforrestliuҪ����룬����Ϸ�߼���ϱȽϽ��ܣ�һ��ҵ���ù�ע��ֵ���ɺ��ԡ� 
	 *  
	 * bExpectIdleΪtrueʱ����ѭ��Adapterʱ���õ� 
	 * bExpectIdleΪfalseʱ���ڴ���Adapter�ϵ�����ʱ���õ� 
     *
     * doCustomMessage() ������������Ϊ�˼����ϰ汾�������ô������ĺ���
     * ���ڲ��������ʹ������Ķ������
     */
    virtual int doCustomMessage(bool bExpectIdle) { return -1; }
    virtual int doCustomMessage() { return -1; }


	 /**
     * �ͻ��˹ر�����ʱ�Ĵ���
	 * @param current 
	 * @return int  
     */
	virtual int doClose(JceCurrentPtr current){ return -1; }

    /**
     * �����Ӧ�����ݶ���
     * @return TC_ThreadQueue<ReqMessagePtr>& 
     */
    TC_ThreadQueue<ReqMessagePtr>& getResponseQueue();

public:


    /**
     * ��������·���߼�
     * @param current
     * @param routekey
     * @return int 0�ɹ�·��
     */
    virtual int doGridRouter(JceCurrentPtr current, string & routekey);


protected:
    /**
     * ����
     */
    string _name;

    /**
     * ������Handle
     */
    TC_EpollServer::Handle* _handle;

    /**
     * �첽��Ӧ����
     */
    TC_ThreadQueue<ReqMessagePtr> _asyncResponseQueue;
};

typedef TC_AutoPtr<Servant> ServantPtr;
//////////////////////////////////////////////////////////////////////
//
//
//
//
//

/**
 * ���߳�ȫ�첽�е�callback����ҵ�����е�callback��Ҫ������̳�
 */
class ServantCallback : public ServantProxyCallback
{
public:
    /**
     * ���캯����type��������ͬһ��·�ϵĶ���cb��������
     * @param type 
     * @param servant 
     * @param current 
     */
    ServantCallback(const string& type, const ServantPtr& servant, const JceCurrentPtr& current);

    /**
     * callback����Ӧ�ӿ� 
     * @param msg 
     * @return int 
     */
    virtual int onDispatch(ReqMessagePtr msg);

    /**
     * �������ʱ������servant
     * @return const ServantPtr& 
     */
    const ServantPtr& getServant();

    /**
     * �������������
     * @return const JceCurrentPtr& 
     */
    const JceCurrentPtr& getCurrent();

protected:
    ServantPtr _servant;

    JceCurrentPtr _current;
};

//////////////////////////////////////////////////////////////////////

//�߳�˽������
class CallbackThreadData : public TC_ThreadPool::ThreadData
{
public:
    static TC_ThreadMutex _mutex;  //ȫ�ֵĻ�����
    static pthread_key_t _key;   //˽���߳�����key

    /**
     * ���캯��
     */
    CallbackThreadData();

    /**
     * ��������
     */
    ~CallbackThreadData() {}

    /**
     * ������Դ�ͷ�
     * @param p
     */
    static void destructor(void* p);

    /**
     * ��ȡ�߳����ݣ�û�еĻ����Զ�����
     * @return CallbackThreadData*
     */
    static CallbackThreadData * getData();

public:
    void setResponseContext(const map<std::string, std::string> & context);
    bool getContextValid(){return _contextValid;}
    map<std::string, std::string> & getResponseContext();
    void delResponseContext();

private:
    bool _contextValid;
    map<std::string, std::string> _responseContext;
};

//////////////////////////////////////////////////////////////////////
/**
 * ���������·�� ���ص�callback
 */
class RouterCallback : public ServantCallback
{
public:
    RouterCallback(const string& type, const ServantPtr& servant, const JceCurrentPtr& current);
    virtual int onDispatch(ReqMessagePtr msg);
};

typedef TC_AutoPtr<ServantCallback> ServantCallbackPtr; 
//////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////
#endif
