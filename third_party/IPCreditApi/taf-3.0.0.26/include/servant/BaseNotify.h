#ifndef __TAF_BASE_NOTIFY_H_
#define __TAF_BASE_NOTIFY_H_

#include "servant/Global.h"
#include "util/tc_thread_queue.h"
#include "util/tc_thread_mutex.h"
#include "util/tc_functor.h"
#include "util/tc_loki.h"

//////////////////////////////////////////////////////////////////////
namespace taf
{

/**
 * ��Ҫ���յ���������Ķ���Ӹ�������
 */
class BaseNotify : public TC_ThreadRecMutex, public TC_HandleBase
{
public:
    /**
     * ���캯��
     */
    BaseNotify();

    /**
     * ��������
     */
    virtual ~BaseNotify();

    /**
     * ���չ�������
     * @param command
     * @param params
     * @param current
     * @param result
     * @return bool
     */
    bool notify(const string& command, const string& params, JceCurrentPtr current, string& result);

    /**
     * ��������ĺ�������
     * bool: true(��������֪ͨ����object),false(֪ͨ��ֹ)
     * result: ����������
     */
    typedef TC_Functor<bool, TL::TLMaker<const string&, const string&, string&>::Result> TAdminFunc;

    /**
     * ���Servant��������Ͷ�Ӧ�Ĵ�����
     * @param command
     * @param func
     */
    void addAdminCommandNormal(const string& command, TAdminFunc func);

    /**
     * ��ӹ�������Ͷ�Ӧ�Ĵ�����
     * ��ǰ��Servant����ִ���յ�������
     * @param command
     * @param func
     */
    void addAdminCommandPrefix(const string& command, TAdminFunc func);

protected:
    /**
     * �������
     */
    map<string, TAdminFunc> _procFunctors;
};
/////////////////////////////////////////////////////////////////////
}
#endif
