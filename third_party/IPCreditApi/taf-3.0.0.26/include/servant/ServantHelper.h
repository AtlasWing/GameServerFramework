#ifndef __TAF_SERVANT_HELPER_H
#define __TAF_SERVANT_HELPER_H

#include <iostream>
#include <map>
#include <vector>
#include "util/tc_autoptr.h"
#include "util/tc_singleton.h"
#include "servant/Servant.h"

namespace taf
{

/**
 * Servant
 */
class ServantHelperCreation : public TC_HandleBase
{
public:
    virtual ServantPtr create(const string &s) = 0;
};
typedef TC_AutoPtr<ServantHelperCreation> ServantHelperCreationPtr;

/**
 * Servant
 */
template<class T>
struct ServantCreation : public ServantHelperCreation
{
    ServantPtr create(const string &s) { T *p = new T; p->setName(s); return p; }
};

//////////////////////////////////////////////////////////////////////////////
//
/**
 * Servant����
 */
class ServantHelperManager : public TC_Singleton<ServantHelperManager>
{
public:

    ServantHelperManager() : _isDyeing(false)
    {
    }

    /**
     * ���Servant
     * @param T
     * @param id
     */
    template<typename T>
    void addServant(const string &id,bool check = false)
    {
        if(check && _servant_adapter.end() == _servant_adapter.find(id))
        {
            cerr<<"[TAF]ServantHelperManager::addServant "<< id <<" not find adapter.(maybe not conf in the web)"<<endl;
			throw runtime_error("[TAF]ServantHelperManager::addServant " + id + " not find adapter.(maybe not conf in the web)");
        }
        _servant_creator[id] = new ServantCreation<T>();
    }

    /**
     * ����Servant
     * @param id
     *
     * @return ServantPtr
     */
    ServantPtr create(const string &sAdapter);

    /**
     * ���Adapter��Servant
     * @param sAdapter
     * @param sServant
     */
    void setAdapterServant(const string &sAdapter, const string &sServant);

    /**
     * ����adapter���ƻ�ȡServant����
     * @param sAdapter
     * @return string
     */
    string getAdapterServant(const string &sAdapter)
    {
        if(_adapter_servant.find(sAdapter) != _adapter_servant.end())
        {
            return _adapter_servant[sAdapter];
        }
        return "(NO TAF PROTOCOL)";
    }

    /**
     * ����servant���ƻ�ȡadapter����
     * @param sServant
     * @return string
     */
    string getServantAdapter(const string& sServant)
    {
        if(_servant_adapter.find(sServant) != _servant_adapter.end())
        {
            return _servant_adapter[sServant];
        }
        return "";
    }

    /**
     * ��ȡAdapter/Servant��Ӧ��
     * @return map<string, string>
     */
    map<string, string> getAdapterServant() {return _adapter_servant;}

    /**
     * ����Ⱦɫ��Ϣ
     * @param params: notify���������
     * @param sDyeingKey:      �û�����
     * @param sDyeingServant:  ��������
     * @param sDyeingInterface:�ӿ�����
     * @return string: ���ý��
     */
    bool setDyeing(const string & sDyeingKey, const string & sDyeingServant, const string & sDyeingInterface);

    /**
     * �Ƿ���Ⱦɫ������
     * @param sKey:      �û�����
     * @param sServant:  ��������
     * @param sInterface:�ӿ�����
     * @return string: ���ý��
     */
    bool isDyeingReq(const string & sKey, const string & sServant, const string & sInterface);

    /**
     * �Ƿ����Ѿ���Ⱦɫ
     */
    bool isDyeing() {return _isDyeing;}

    /**
     * ����·����
     * @param sServant
     * @param router
     * @param router
     */
    bool setRouter(const string & sServant,ServantProxyRouterPtr router);

    /**
     * ����key��ȡ�û��ĻҶ�״̬
     * ����INVALID_GRID_CODE����Ϊ�޻Ҷ�״̬
     * @param sServant
     * @param key
     * @return int
     */
    int getGridByKey(const string & sServant,const string & sKey);


protected:

    /**
     * Servant������
     */
    map<string, ServantHelperCreationPtr>   _servant_creator;

    /**
     * Adapter������Servant(Adapter����:servant����)
     */
    map<string, string>                     _adapter_servant;

    /**
     * Adapter������Servant(Servant����:Adapter����)
     */
    map<string, string>                     _servant_adapter;

protected:

    /**
     * ��
     */
    TC_ThreadMutex _mutex;

    /**
     * �Ƿ�Ⱦɫ
     */
    bool   _isDyeing;

    /**
     * Ⱦɫ�û�����
     */
    string  _sDyeingKey;

    /**
     * Ⱦɫ��servant����
     */
    string  _sDyeingServant;

    /**
     * Ⱦɫ�Ľӿ�
     */
    string _sDyeingInterface;

protected:

    /**
     * ����˻Ҷ�·��
     */
    map<string,ServantProxyRouterPtr> _mRouter;


};

}

#endif

