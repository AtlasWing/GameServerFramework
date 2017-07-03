#ifndef __TAF_COMMUNICATOR_FACTORY_H_
#define __TAF_COMMUNICATOR_FACTORY_H_

#include "servant/Global.h"
#include "servant/Communicator.h"

namespace taf
{
/**
 * ����CommunicatorPtr����
 */
    
class CommunicatorFactory : public TC_Singleton<CommunicatorFactory>, public TC_HandleBase, public TC_ThreadRecMutex
{
public:
    /**
     * ���캯��
     * @param comm
     */
    CommunicatorFactory(){};

    /**
     * ����
     */
    ~CommunicatorFactory(){};

    /**
     * ��ȡCommunicatorPtr����
     * @param name
     * @return ServantPrx
     */
    CommunicatorPtr getCommunicator(const string& name = "default")
    {
        TC_LockT<TC_ThreadRecMutex> lock(*this);

        map<string, CommunicatorPtr>::iterator it = _mComms.find(name);

        if (it == _mComms.end())
        {
            _mComms[name] = new Communicator();

            it = _mComms.find(name);
        }
        return it->second;
    }
    
     /**
     * ��ȡCommunicatorPtr���� 
     * @param conf 
     * @param name
     * @return ServantPrx
     */
    CommunicatorPtr getCommunicator(TC_Config& conf, const string& name = "default")
    {
        TC_LockT<TC_ThreadRecMutex> lock(*this);

        map<string, CommunicatorPtr>::iterator it = _mComms.find(name);

        if (it == _mComms.end())
        {
            _mComms[name] = new Communicator(conf);

            it = _mComms.find(name);

            return it->second;
        }

        string s = "";

        it->second->setProperty(conf);

        it->second->reloadProperty(s);

        return it->second;
    }

private:
  
    /**
     * �Ѵ����Ķ���
     */
    map<string, CommunicatorPtr> _mComms;
};
//////////////////////////////////////////////////////
}

#endif
