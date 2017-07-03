#ifndef __TAF_OBJECT_PROXY_FACTORY_H_
#define __TAF_OBJECT_PROXY_FACTORY_H_

#include "servant/Communicator.h"
#include "servant/ObjectProxy.h"
#include <vector>

namespace taf
{
/**
 * 获取ObjectProxy对象
 * 每个objectname在进程空间唯一对应一个objectproxy
 * object proxy 线程安全
 *
 */
#define MAX_OBJ_NUM 100000

class ObjectProxyFactory : public TC_HandleBase, public TC_ThreadRecMutex
{
public:
    /**
     * 构造函数
     * @param cm
     */
    ObjectProxyFactory(CommunicatorEpoll * pCommunicatorEpoll);

    /**
     * 获取对象代理
     * @param sObjectProxyName
     * @param setName 指定set调用的setid
     *
     * @return ObjectPrx
     */
    ObjectProxy * getObjectProxy(const string& sObjectProxyName,const string& setName="");

    /**
     *
     */
    ~ObjectProxyFactory();

    /**
     * 获取所有对象的个数，为了不加锁不用map
     */
    inline size_t getObjNum()
    {
        return _iObjNum;
    }

    /**
     * 根据序号 获取所有obj对象，为了不加锁不用map
     */
    inline ObjectProxy * getObjectProxy(size_t iNum)
    {
        assert(iNum<_iObjNum);
        return _vpObjectProxys[iNum];
    }

    /**
     * 所有对象代理加载locator信息
     */
    int loadObjectLocator();

private:
    /**
     * 通信器 epool 线程
     */
    CommunicatorEpoll * _pCommunicatorEpoll;

    /**
     * 保存已创建的objectproxy
     */
    map<string,ObjectProxy*> _objectProxys;

    /**
     * 保存已经创建的obj 取的时候可以不用加锁
     */
    ObjectProxy * _vpObjectProxys[MAX_OBJ_NUM];

    /*
     *保存已经创建obj的数量
     */
    size_t _iObjNum;
};
/////////////////////////////////////////////////////
}
#endif
