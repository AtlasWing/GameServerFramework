#ifndef __TAF_OBJECT_PROXY_FACTORY_H_
#define __TAF_OBJECT_PROXY_FACTORY_H_

#include "servant/Communicator.h"
#include "servant/ObjectProxy.h"
#include <vector>

namespace taf
{
/**
 * ��ȡObjectProxy����
 * ÿ��objectname�ڽ��̿ռ�Ψһ��Ӧһ��objectproxy
 * object proxy �̰߳�ȫ
 *
 */
#define MAX_OBJ_NUM 100000

class ObjectProxyFactory : public TC_HandleBase, public TC_ThreadRecMutex
{
public:
    /**
     * ���캯��
     * @param cm
     */
    ObjectProxyFactory(CommunicatorEpoll * pCommunicatorEpoll);

    /**
     * ��ȡ�������
     * @param sObjectProxyName
     * @param setName ָ��set���õ�setid
     *
     * @return ObjectPrx
     */
    ObjectProxy * getObjectProxy(const string& sObjectProxyName,const string& setName="");

    /**
     *
     */
    ~ObjectProxyFactory();

    /**
     * ��ȡ���ж���ĸ�����Ϊ�˲���������map
     */
    inline size_t getObjNum()
    {
        return _iObjNum;
    }

    /**
     * ������� ��ȡ����obj����Ϊ�˲���������map
     */
    inline ObjectProxy * getObjectProxy(size_t iNum)
    {
        assert(iNum<_iObjNum);
        return _vpObjectProxys[iNum];
    }

    /**
     * ���ж���������locator��Ϣ
     */
    int loadObjectLocator();

private:
    /**
     * ͨ���� epool �߳�
     */
    CommunicatorEpoll * _pCommunicatorEpoll;

    /**
     * �����Ѵ�����objectproxy
     */
    map<string,ObjectProxy*> _objectProxys;

    /**
     * �����Ѿ�������obj ȡ��ʱ����Բ��ü���
     */
    ObjectProxy * _vpObjectProxys[MAX_OBJ_NUM];

    /*
     *�����Ѿ�����obj������
     */
    size_t _iObjNum;
};
/////////////////////////////////////////////////////
}
#endif
