#ifndef __TAF_ADMIN_SERVANT_H
#define __TAF_ADMIN_SERVANT_H

#include "servant/AdminF.h"

namespace taf
{

class JceCurrent;
class Application;

/**
 * ����Servant
 */
class AdminServant : public AdminF
{
public:
    /**
     * ���캯��
     * @param pEpollServer
     */
    AdminServant();

    /**
     * ��������
     */
    virtual ~AdminServant();

    /**
     * ��ʼ��
     * @return int
     */
    virtual void initialize();

    /**
     * �˳�
     */
    virtual void destroy();

    /**
     * �رշ���
     * @param current
     */
    void shutdown(JceCurrentPtr current);

    /**
     * ֪ͨ
     * @param command
     */
    string notify(const string &command, JceCurrentPtr current);

};
////////////////////////////////////////////////////////////////
}

#endif


