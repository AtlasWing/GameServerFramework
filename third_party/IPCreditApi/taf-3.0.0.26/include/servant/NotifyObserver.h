#ifndef __TAF_NOTIFY_OBSERVER_H_
#define __TAF_NOTIFY_OBSERVER_H_

#include "servant/Global.h"
#include "servant/JceCurrent.h"
#include "util/tc_singleton.h"

namespace taf
{

class BaseNotify;

/**
 * ȫ�ֽ��չ��������֪ͨ����ע��Ķ���
 */
class NotifyObserver : public TC_Singleton<NotifyObserver>, public TC_ThreadRecMutex
{
public:
    /**
     * ���캯��
     */
    NotifyObserver();

    /**
     * ��������
     */
    ~NotifyObserver();

public:

    /**
     * ���չ�������
     * @param command
     * @param current
     * @return string
     */
    string notify(const string& command, JceCurrentPtr current);

public:
    /**
     * ע��ǰ�������������Normal֮ǰ����
     * @param command
     * @param obj
     */
    void registerPrefix(const string& command, BaseNotify* obj);

    /**
     * ע��ǰ�ô�����
     * @param command
     * @param obj
     */
    void unregisterPrefix(const string& command, BaseNotify* obj);

    /**
     * ע����ͨ��������������ֱ��˳�򲻱�֤
     * @param command
     * @param obj
     */
    void registerNotify(const string& command, BaseNotify* obj);

    /**
     * ע����ͨ����Ϣ������
     * @param command
     * @param obj
     */
    void unregisterNotify(const string& command, BaseNotify* obj);

    /**
     * չʾ��ע���������������˳�򲻱�֤
     */
    string viewRegisterCommand();

protected:
    /**
     * ע�����
     * @param command
     * @param obj
     * @param target
     */
    void registerObject(const string& command, BaseNotify* obj, map<string, set<BaseNotify*> >& target);

    /**
     * ȡ������ע��
     * @param command
     * @param obj
     * @param target
     */
    void unregisterObject(const string& command, BaseNotify* obj, map<string, set<BaseNotify*> >& target);

protected:

    /**
     * ǰ�ô�����
     */
    map<string, set<BaseNotify*> > _prefix;

    /**
     * ��ͨ������
     */
    map<string, set<BaseNotify*> > _notifys;
};

/////////////////////////////////////////////////////////////////
}

#endif
