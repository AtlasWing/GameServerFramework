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
 * 客户端配置
 */
struct ClientConfig
{
    /**
     * 客户端IP地址
     */
    static string LocalIp;
    /**
     * 客户端模块名称
     */
    static string ModuleName;
    /**
     * 客户端所有的IP地址
     */
    static set<string>  SetLocalIp;
   /**
   *客户端是否打开set分组
   */
   static bool SetOpen;
   /**
   *客户端set分组
   */
   static string SetDivision;

   /**
    * 客户端的taf版本号
    */
   static string TafVersion;

};
////////////////////////////////////////////////////////////////////////
/**
 * 通信器,用于创建和维护客户端proxy
 */
class Communicator : public TC_HandleBase, public TC_ThreadRecMutex
{
public:
    /**
     * 构造函数
     */
    Communicator();

    /**
     * 采用配置构造
     * @param conf
     * @param path
     */
    Communicator(TC_Config& conf, const string& domain = CONFIG_ROOT_PATH);

    /**
     * 析够
     * 析够时自动接收相关线程
     */
    ~Communicator();

    /**
     * 生成代理
     * @param T
     * @param objectName
     * @param setName 指定set调用的setid
     * @return T
     */
    template<class T> T stringToProxy(const string& objectName,const string& setName="")
    {
        T prx = NULL;

        stringToProxy<T>(objectName, prx,setName);

        return prx;
    }

    /**
     * 生成代理
     * @param T
     * @param objectName
     * @param setName 指定set调用的setid
     * @param proxy
     */
    template<class T> void stringToProxy(const string& objectName, T& proxy,const string& setName="")
    {
        ServantProxy * pServantProxy = getServantProxy(objectName,setName);
        proxy = (typename T::element_type*)(pServantProxy);
    }
public:

    /*
    *获取epoll的个数
    */
    inline size_t getEpollNum()
    {
        return _iEpollNum;
    }

    /*
    *获取epoll的对象
    */
    inline CommunicatorEpoll * getCommunicatorEpoll(size_t iNum)
    {
        assert(iNum<_iEpollNum);
        return _vpCommunicatorEpoll[iNum];
    }

public:
    /**
     * 获取属性
     * @param name
     * @param dft, 缺省值
     * @return string
     */
    string getProperty(const string& name, const string& dft = "");

    /**
     * 设置属性
     * @param properties
     */
    void setProperty(const map<string, string>& properties);

    /**
     * 设置某一个属性
     * @param name
     * @param value
     */
    void setProperty(const string& name, const string& value);

    /**
     * 设置属性
     * @param conf
     * @param path
     */
    void setProperty(TC_Config& conf, const string& domain = CONFIG_ROOT_PATH);

    /**
     * 上报统计
     * @return StatReport*
     */
    StatReport * getStatReport();

    /**
     * 重新加载属性
     */
    int reloadProperty(string & sResult)
    {
        return 0;
    }

    /**
     * 获取obj对应可用ip port列表  如果启用分组,只返回同分组的服务端ip
     * @param sObjName
     * @return vector<TC_Endpoint>
     */
    vector<TC_Endpoint> getEndpoint(const string & objName);

    /**
     * 获取obj对应可用ip port列表 包括所有IDC的
     * @param sObjName
     * @return vector<TC_Endpoint>
     */
    vector<TC_Endpoint> getEndpoint4All(const string & objName);

    /**
     * 结束
     */
    void terminate();

protected:
    /**
     * 初始化
     */
    void initialize();

    /**
     * 是否析构中
     * @return bool
     */
    bool isTerminating();

    /**
     * 由Property初始化客户端配置
     */
    void initClientConfig();

    /**
     * 获取对象代理生成器
     * @return ServantProxyFactoryPtr
     */
    ServantProxyFactory * servantProxyFactory();

    /**
     * 获取通用对象
     * @param objectName
     * @param setName 指定set调用的setid
     * @return ServantPrx
     */
    ServantProxy * getServantProxy(const string& objectName,const string& setName="");

    /**
     * 框架内部需要直接访问通信器的类
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

    //网络线程数组
    CommunicatorEpoll * _vpCommunicatorEpoll[MAX_CLIENT_EPOLL_NUM];

    size_t _iEpollNum;

    //StatReport
    StatReport * _pStatReport;

};
////////////////////////////////////////////////////////////////////////
}
#endif
