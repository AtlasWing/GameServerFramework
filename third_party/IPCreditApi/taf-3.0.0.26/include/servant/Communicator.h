#ifndef __TAF_COMMUNICATOR_H_
#define __TAF_COMMUNICATOR_H_

#include "util/tc_thread.h"
#include "util/tc_config.h"
#include "util/tc_singleton.h"
#include "servant/Global.h"
#include "servant/ServantProxy.h"
#include "servant/ServantProxyFactory.h"
#include "servant/ObjectProxyFactory.h"
#include "servant/AsyncProcThread.h"
#include "servant/CommunicatorEpoll.h"

#define CONFIG_ROOT_PATH "/taf/application/client"

namespace taf
{
////////////////////////////////////////////////////////////////////////
/**
 * �ͻ�������
 */
struct ClientConfig
{
    /**
     * �ͻ���IP��ַ
     */
    static string LocalIp;
    /**
     * �ͻ���ģ������
     */
    static string ModuleName;
    /**
     * �ͻ������е�IP��ַ
     */
    static set<string>  SetLocalIp;
   /**
   *�ͻ����Ƿ��set����
   */
   static bool SetOpen;
   /**
   *�ͻ���set����
   */
   static string SetDivision;

   /**
    * �ͻ��˵�taf�汾��
    */
   static string TafVersion;

};
////////////////////////////////////////////////////////////////////////
/**
 * ͨ����,���ڴ�����ά���ͻ���proxy
 */
class Communicator : public TC_HandleBase, public TC_ThreadRecMutex
{
public:
    /**
     * ���캯��
     */
    Communicator();

    /**
     * �������ù���
     * @param conf
     * @param path
     */
    Communicator(TC_Config& conf, const string& domain = CONFIG_ROOT_PATH);

    /**
     * ����
     * ����ʱ�Զ���������߳�
     */
    ~Communicator();

    /**
     * ���ɴ���
     * @param T
     * @param objectName
     * @param setName ָ��set���õ�setid
     * @return T
     */
    template<class T> T stringToProxy(const string& objectName,const string& setName="")
    {
        T prx = NULL;

        stringToProxy<T>(objectName, prx,setName);

        return prx;
    }

    /**
     * ���ɴ���
     * @param T
     * @param objectName
     * @param setName ָ��set���õ�setid
     * @param proxy
     */
    template<class T> void stringToProxy(const string& objectName, T& proxy,const string& setName="")
    {
        ServantProxy * pServantProxy = getServantProxy(objectName,setName);
        proxy = (typename T::element_type*)(pServantProxy);
    }
public:

    /*
    *��ȡepoll�ĸ���
    */
    inline size_t getEpollNum()
    {
        return _iEpollNum;
    }

    /*
    *��ȡepoll�Ķ���
    */
    inline CommunicatorEpoll * getCommunicatorEpoll(size_t iNum)
    {
        assert(iNum<_iEpollNum);
        return _vpCommunicatorEpoll[iNum];
    }

public:
    /**
     * ��ȡ����
     * @param name
     * @param dft, ȱʡֵ
     * @return string
     */
    string getProperty(const string& name, const string& dft = "");

    /**
     * ��������
     * @param properties
     */
    void setProperty(const map<string, string>& properties);

    /**
     * ����ĳһ������
     * @param name
     * @param value
     */
    void setProperty(const string& name, const string& value);

    /**
     * ��������
     * @param conf
     * @param path
     */
    void setProperty(TC_Config& conf, const string& domain = CONFIG_ROOT_PATH);

    /**
     * �ϱ�ͳ��
     * @return StatReport*
     */
    StatReport * getStatReport();

    /**
     * ���¼�������
     */
    int reloadProperty(string & sResult)
    {
        return 0;
    }

    /**
     * ��ȡobj��Ӧ����ip port�б�  ������÷���,ֻ����ͬ����ķ����ip
     * @param sObjName
     * @return vector<TC_Endpoint>
     */
    vector<TC_Endpoint> getEndpoint(const string & objName);

    /**
     * ��ȡobj��Ӧ����ip port�б� ��������IDC��
     * @param sObjName
     * @return vector<TC_Endpoint>
     */
    vector<TC_Endpoint> getEndpoint4All(const string & objName);

    /**
     * ����
     */
    void terminate();

protected:
    /**
     * ��ʼ��
     */
    void initialize();

    /**
     * �Ƿ�������
     * @return bool
     */
    bool isTerminating();

    /**
     * ��Property��ʼ���ͻ�������
     */
    void initClientConfig();

    /**
     * ��ȡ�������������
     * @return ServantProxyFactoryPtr
     */
    ServantProxyFactory * servantProxyFactory();

    /**
     * ��ȡͨ�ö���
     * @param objectName
     * @param setName ָ��set���õ�setid
     * @return ServantPrx
     */
    ServantProxy * getServantProxy(const string& objectName,const string& setName="");

    /**
     * ����ڲ���Ҫֱ�ӷ���ͨ��������
     */
    friend class AdapterProxy;

    friend class ServantProxy;

    friend class ObjectProxy;

    friend class ServantProxyFactory;

    friend class ObjectProxyFactory;

    friend class AsyncProcThread;

    friend class CommunicatorEpoll;

protected:
    bool _initialized;

    bool _terminating;

    map<string, string> _properties;

    ServantProxyFactoryPtr _servantProxyFactory;

    //�����߳�����
    CommunicatorEpoll * _vpCommunicatorEpoll[MAX_CLIENT_EPOLL_NUM];

    size_t _iEpollNum;

    //StatReport
    StatReport * _pStatReport;

};
////////////////////////////////////////////////////////////////////////
}
#endif
