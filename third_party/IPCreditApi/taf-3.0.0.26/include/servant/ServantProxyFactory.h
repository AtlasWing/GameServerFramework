#ifndef __TAF_SERVANT_PROXY_FACTORY_H_
#define __TAF_SERVANT_PROXY_FACTORY_H_

#include "servant/Global.h"
#include "servant/Communicator.h"
#include "servant/ServantProxy.h"

namespace taf
{
/**
 * ����ServantProxy����ÿ��object�ڽ��̿ռ�ֻ��һ��ServantProxyʵ��
 */
class ServantProxyFactory : public TC_HandleBase, public TC_ThreadRecMutex
{
public:
    /**
     * ���캯��
     * @param comm
     */
    ServantProxyFactory(Communicator* comm);

    /**
     * ����
     */
    ~ServantProxyFactory();

    /**
     * ��ȡServantProxy����
     * @param name
     * @param setName ָ��set���õ�setid
     * @return ServantPrx
     */
    ServantPrx::element_type* getServantProxy(const string& name,const string& setName="");

private:
    /**
     * ͨ����
     */
    Communicator* _comm;

    /**
     * �Ѵ����Ķ���
     */
    map<string, ServantPrx> _servantProxy;
};
//////////////////////////////////////////////////////
}
#endif
