#ifndef __TAF_GLOBAL_H_
#define __TAF_GLOBAL_H_

#include <inttypes.h>
#include <iostream>
#include <sstream>
#include <map>
#include <list>
#include <deque>
#include <vector>
#include <memory>
#include <netinet/in.h>
#include <errno.h>
#include <iomanip>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#include "util/tc_clientsocket.h"
#include "util/tc_autoptr.h"
#include "util/tc_common.h"
#include "util/tc_logger.h"
#include "util/tc_thread_mutex.h"
#include "util/tc_functor.h"
#include "util/tc_loki.h"
#include "jce/RequestF.h"
#include "servant/BaseF.h"

using namespace std;

namespace taf
{
#define MAX_CLIENT_EPOLL_NUM 16
class Communicator;
class AdapterProxy;
class ServantProxy;
class ServantProxyCallback;
class ServantProxyRouter;
class ObjectProxy;
//class BaseProxy;
class JceCurrent;
class FDReactor;
//class QueueTimeout;
//class TMReactor;
//class FDEventHandle;
class Transceiver;
//class TransceiverHandle;
//class ReqMessage;
class StatFProxy;
class StatReport;
class ServantProxyFactory;
class ObjectProxyFactory;
//class AdapterProxyFactory;
//class TMEventHandle;
class AsyncProcThread;
//class AsyncProcThreadRunner;
//class ThreadContextCallback;
//class PrivateContextThread;

typedef TC_AutoPtr<Communicator> CommunicatorPtr;
//typedef TC_AutoPtr<AdapterProxy> AdapterPrx;
typedef TC_AutoPtr<ServantProxy> ServantPrx;
typedef TC_AutoPtr<ServantProxyCallback> ServantProxyCallbackPtr;
//typedef TC_AutoPtr<ThreadContextCallback> ThreadContextCallbackPtr;
typedef TC_AutoPtr<ServantProxyRouter> ServantProxyRouterPtr;
typedef TC_AutoPtr<ObjectProxy> ObjectPrx;
typedef TC_AutoPtr<JceCurrent> JceCurrentPtr;
typedef TC_AutoPtr<StatFProxy> StatFPrx;
typedef TC_AutoPtr<StatReport> StatReportPtr;
//typedef TC_AutoPtr<BaseProxy> BasePrx;
typedef TC_AutoPtr<FDReactor> FDReactorPtr;
//typedef TC_AutoPtr<QueueTimeout> QueueTimeoutPtr;
//typedef TC_AutoPtr<TMReactor> TMReactorPtr;
//typedef TC_AutoPtr<FDEventHandle> FDEventHandlePtr;
//typedef TC_AutoPtr<Transceiver> TransceiverPtr;
//typedef TC_AutoPtr<TransceiverHandle> TransceiverHandlePtr;
//typedef TC_AutoPtr<ReqMessage> ReqMessagePtr;
typedef TC_AutoPtr<ServantProxyFactory> ServantProxyFactoryPtr;
typedef TC_AutoPtr<ObjectProxyFactory> ObjectProxyFactoryPtr;
//typedef TC_AutoPtr<AdapterProxyFactory> AdapterProxyFactoryPtr;
//typedef TC_AutoPtr<TMEventHandle> TMEventHandlePtr;
typedef TC_AutoPtr<AsyncProcThread> AsyncProcThreadPtr;
//typedef TC_AutoPtr<AsyncProcThreadRunner> AsyncProcThreadRunnerPtr;
//typedef TC_AutoPtr<PrivateContextThread> PrivateContextThreadPtr;

//////////////////////////////////////////////////////////////
// ����TAF������õ��쳣����
struct TafException : public TC_Exception
{
    TafException(const string &buffer) : TC_Exception(buffer){};
    TafException(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TafException() throw(){};
    /**
     * ���ݷ���ֵ�׳��쳣
     * @param ret
     * @param desc
     */
    static void throwException(int ret, const string& desc = "");
};

////////////////////////////////////////////////////////////////
// ���������쳣
/**
 * ���������쳣
 */
struct TafNetConnectException : public TafException
{
    TafNetConnectException(const string &buffer) : TafException(buffer){};
    TafNetConnectException(const string &buffer, int err) : TafException(buffer, err){};
    ~TafNetConnectException() throw(){};
};
/**
 * ���Ӷ�ʧ
 */
struct TafNetConnectLostException : public TafException
{
    TafNetConnectLostException(const string &buffer) : TafException(buffer){};
    TafNetConnectLostException(const string &buffer, int err) : TafException(buffer, err){};
    ~TafNetConnectLostException() throw(){};
};
/**
 * Socket�쳣
 */
struct TafNetSocketException : public TafException
{
    TafNetSocketException(const string &buffer) : TafException(buffer){};
    TafNetSocketException(const string &buffer, int err) : TafException(buffer, err){};
    ~TafNetSocketException() throw(){};
};
/**
 * Proxy�����쳣
 */
struct TafProxyDecodeException : public TafException
{
    TafProxyDecodeException(const string &buffer) : TafException(buffer){};
    TafProxyDecodeException(const string &buffer, int err) : TafException(buffer, err){};
    ~TafProxyDecodeException() throw(){};
};
/**
 * Proxy�����쳣
 */
struct TafProxyEncodeException : public TafException
{
    TafProxyEncodeException(const string &buffer) : TafException(buffer){};
    TafProxyEncodeException(const string &buffer, int err) : TafException(buffer, err){};
    ~TafProxyEncodeException() throw(){};
};
/**
 * Server�����쳣
 */
struct TafServerEncodeException : public TafException
{
    TafServerEncodeException(const string &buffer) : TafException(buffer){};
    TafServerEncodeException(const string &buffer, int err) : TafException(buffer, err){};
    ~TafServerEncodeException() throw(){};
};
/**
 * Server�����쳣
 */
struct TafServerDecodeException : public TafException
{
    TafServerDecodeException(const string &buffer) : TafException(buffer){};
    TafServerDecodeException(const string &buffer, int err) : TafException(buffer, err){};
    ~TafServerDecodeException() throw(){};
};
/**
 * Server�޺����쳣
 */
struct TafServerNoFuncException : public TafException
{
    TafServerNoFuncException(const string &buffer) : TafException(buffer){};
    TafServerNoFuncException(const string &buffer, int err) : TafException(buffer, err){};
    ~TafServerNoFuncException() throw(){};
};
/**
 * Server�޶����쳣
 */
struct TafServerNoServantException : public TafException
{
    TafServerNoServantException(const string &buffer) : TafException(buffer){};
    TafServerNoServantException(const string &buffer, int err) : TafException(buffer, err){};
    ~TafServerNoServantException() throw(){};
};
/**
 * ��Ϣ�ڷ���˶��г�ʱ
 */
struct TafServerQueueTimeoutException : public TafException
{
    TafServerQueueTimeoutException(const string &buffer) : TafException(buffer){};
    TafServerQueueTimeoutException(const string &buffer, int err) : TafException(buffer, err){};
    ~TafServerQueueTimeoutException() throw(){};
};
/**
 * ����˷��ص�δֵ֪
 */
struct TafServerUnknownException : public TafException
{
    TafServerUnknownException(const string &buffer) : TafException(buffer){};
    TafServerUnknownException(const string &buffer, int err) : TafException(buffer, err){};
    ~TafServerUnknownException() throw(){};
};
/**
 * ͬ�����ó�ʱ�쳣
 */
struct TafSyncCallTimeoutException  : public TafException
{
    TafSyncCallTimeoutException (const string &buffer) : TafException(buffer){};
    TafSyncCallTimeoutException (const string &buffer, int err) : TafException(buffer, err){};
    ~TafSyncCallTimeoutException () throw(){};
};
/**
 * ���� Registry ����
 */
struct TafRegistryException : public TafException
{
    TafRegistryException(const string &buffer) : TafException(buffer){};
    TafRegistryException(const string &buffer, int err) : TafException(buffer, err){};
    ~TafRegistryException() throw(){};
};
/**
 * ���������Grid״̬
 */
struct TafServerResetGridException : public TafException
{
    TafServerResetGridException(const string &buffer) : TafException(buffer){};
    TafServerResetGridException(const string &buffer, int err) : TafException(buffer, err){};
    ~TafServerResetGridException() throw(){};
};
/**
 * �ͻ��˶�������
 */
struct TafClientQueueException : public TafException
{
    TafClientQueueException(const string &buffer) : TafException(buffer){};
    TafClientQueueException(const string &buffer, int err) : TafException(buffer, err){};
    ~TafClientQueueException() throw(){};
};
///////////////////////////////////////////////////////////////////
}
#endif
