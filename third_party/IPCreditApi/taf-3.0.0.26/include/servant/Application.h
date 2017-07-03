#ifndef __TAF_APPLICATION_H
#define __TAF_APPLICATION_H

#include <iostream>
#include <set>
#include <signal.h>

#include "util/tc_autoptr.h"
#include "util/tc_config.h"
#include "util/tc_epoll_server.h"
#include "servant/BaseNotify.h"
#include "servant/ServantHelper.h"
#include "servant/ServantHandle.h"
#include "servant/StatReport.h"
#include "servant/CommunicatorFactory.h"
#include "log/taf_logger.h"
#include "config/taf_config.h"
#include "notify/taf_notify.h"

namespace taf
{

/**
 * ���¶������ÿ��֧�ֵ�����
 */
#define TAF_CMD_LOAD_CONFIG         "taf.loadconfig"        //����������, ��ȡ��������: taf.loadconfig filename
#define TAF_CMD_SET_LOG_LEVEL       "taf.setloglevel"       //���ù�����־�ĵȼ�: taf.setloglevel [NONE, ERROR, WARN, DEBUG]
#define TAF_CMD_VIEW_STATUS         "taf.viewstatus"        //�鿴����״̬
#define TAF_CMD_VIEW_VERSION        "taf.viewversion"       //�鿴�������TAF�İ汾
#define TAF_CMD_CONNECTIONS         "taf.connection"        //�鿴��ǰ�������
#define TAF_CMD_LOAD_PROPERTY       "taf.loadproperty"      //ʹ�����ļ���property��Ϣ��Ч
#define TAF_CMD_VIEW_ADMIN_COMMANDS "taf.help"              //�����鿴����֧�ֵĹ�������
#define TAF_CMD_SET_DYEING          "taf.setdyeing"         //����Ⱦɫ��Ϣ: taf.setdyeing key servant [interface]
#define TAF_CMD_CLOSE_COUT          "taf.closecout"         //�����Ƿ����ر�cout\cin\cerr: taf.openthreadcontext yes/NO ������������Ч
#define TAF_CMD_SET_DAYLOG_LEVEL       "taf.enabledaylog"      //���ð�����־�Ƿ����: taf.enabledaylog [remote|local]|[logname]|[true|false]

#define TAF_CMD_CLOSE_CORE         "taf.closecore"        //���÷����core limit:  taf.setlimit [yes|no]
/**
 * ֪ͨ��Ϣ��notify����, չʾ��ҳ����
 */
//�ϱ���ͨ��Ϣ
#define TAF_NOTIFY_NORMAL(info)     {TafRemoteNotify::getInstance()->notify(NOTIFYNORMAL, info);}

//�ϱ�������Ϣ
#define TAF_NOTIFY_WARN(info)       {TafRemoteNotify::getInstance()->notify(NOTIFYWARN, info);}

//�ϱ�������Ϣ
#define TAF_NOTIFY_ERROR(info)      {TafRemoteNotify::getInstance()->notify(NOTIFYERROR, info);}

//����������node ���adapter�ֱ��ϱ�
#define TAF_KEEPALIVE(adapter)      {TafNodeFHelper::getInstance()->keepAlive(adapter);}

//����TAF�汾��node
#define TAF_REPORTVERSION(x)        {TafNodeFHelper::getInstance()->reportVersion(TAF_VERSION);}

/**
 * ���ǰ�õ��������
 * ������Normal����֮ǰִ��
 * ���ǰ�÷���֮��˳��ȷ��
 */
#define TAF_ADD_ADMIN_CMD_PREFIX(c,f) \
    do { addAdminCommandPrefix(string(c), TAdminFunc(this, &f)); } while (0);

/**
 * ���Normal�������
 * ������ǰ�÷������ִ��
 * ���Normal����֮��˳��ȷ��
 */
#define TAF_ADD_ADMIN_CMD_NORMAL(c,f) \
    do { addAdminCommandNormal(string(c), TAdminFunc(this, &f)); } while (0);

/**
 * ���������Ϣ
 */
struct ServerConfig
{
    static std::string Application;         //Ӧ������
    static std::string ServerName;          //��������,һ���������ƺ�һ�����������ʶ
    static std::string BasePath;            //Ӧ�ó���·�������ڱ���Զ��ϵͳ���õı���Ŀ¼
    static std::string DataPath;            //Ӧ�ó�������·�����ڱ�����ͨ�����ļ�
    static std::string LocalIp;             //����IP
    static std::string LogPath;             //log·��
    static int         LogSize;             //log��С(�ֽ�)
    static int         LogNum;              //log����()
	static std::string LogLevel;			//log��־����
    static std::string Local;               //�����׽���
    static std::string Node;                //����node��ַ
    static std::string Log;                 //��־���ĵ�ַ
    static std::string Config;              //�������ĵ�ַ
    static std::string Notify;              //��Ϣ֪ͨ����
    static std::string ConfigFile;          //��������ļ�·��
    static int         ReportFlow;          //�Ƿ������ϱ����нӿ�stat���� 0���ϱ� 1�ϱ�(���ڷ�tafЭ���������ͳ��)
    static int         IsCheckSet;          //�Ƿ�԰���set������ý��кϷ��Լ�� 0,����飬1���
};

/**
 * ����Ļ���
 */
class Application : public BaseNotify
{
public:
    /**
     * Ӧ�ù���
     */
    Application();

    /**
     * Ӧ������
     */
    virtual ~Application();

    /**
     * ��ʼ��
     * @param argv
     */
    void main(int argc, char *argv[]);

    /**
     * ����
     */
    void waitForShutdown();

public:
    /**
     * ��ȡ�����ļ�
     *
     * @return TC_Config&
     */
    static TC_Config& getConfig();

    /**
     * ��ȡͨ����
     *
     * @return CommunicatorPtr&
     */
    static CommunicatorPtr& getCommunicator();

    /**
     * ��ȡ����Server����
     *
     * @return TC_EpollServerPtr&
     */
    static TC_EpollServerPtr& getEpollServer();

    /**
     *  ��ֹӦ��
     */
    static void terminate();

	/**
     * ���Config
     * @param filename
     */
    bool addConfig(const string &filename);

    /**
     * ���Ӧ�ü���Config
     * @param filename
     */
    bool addAppConfig(const string &filename);

protected:
    /**
     * ��ʼ��, ֻ����̵���һ��
     */
    virtual void initialize() = 0;

    /**
     * ����, ����ֻ�����һ��
     */
    virtual void destroyApp() = 0;

    /**
     * ����������õ�����
     * ������ɺ����֪ͨServant
     * @param filename
     */
    bool cmdLoadConfig(const string& command, const string& params, string& result);

    /**
     * ���ù�����־�ȼ�
     * @param command
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdSetLogLevel(const string& command, const string& params, string& result);

    bool cmdCloseCoreDump(const string& command, const string& params, string& result);

    /**
     * ���ð�����־�Ƿ���Ч
     * @param command
     * @param params [remote|local]|[logname]|[true|false]
     * @param result
     *
     * @return bool
     */
    bool cmdEnableDayLog(const string& command, const string& params, string& result);

    /**
     * �鿴����״̬
     * @param command
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdViewStatus(const string& command, const string& params, string& result);

    /**
     * �鿴����״̬
     * @param command
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdConnections(const string& command, const string& params, string& result);

    /**
     * �鿴����İ汾
     * @param command
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdViewVersion(const string& command, const string& params, string& result);

    /**
     * ʹ�����ļ���property��Ϣ��Ч
     * @param command
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdLoadProperty(const string& command, const string& params, string& result);

    /**
     *�鿴����֧�ֵĹ�������
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdViewAdminCommands(const string& command, const string& params, string& result);

    /**
     * ����Ⱦɫ��Ϣ
     * @param command
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdSetDyeing(const string& command, const string& params, string& result);


      /**
     * �����Ƿ�ر�stdcout/stderr/stdin ���������ܲ���Ч
     * @param command
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdCloseCout(const string& command, const string& params, string& result);

protected:
    /**
     * ΪAdapter�󶨶�Ӧ��handle����
     * ȱʡʵ����ServantHandle����
     * @param adapter
     */
    virtual void setHandle(TC_EpollServer::BindAdapterPtr& adapter);

    /**
     * ���Servant
     * @param T
     * @param id
     */
    template<typename T> void addServant(const string &id)
    {
        ServantHelperManager::getInstance()->addServant<T>(id,true);
    }

    /**
     * ��tafЭ��server������Servant��Э�������
     * @param protocol
     * @param servant
     */
    void addServantProtocol(const string& servant, const TC_EpollServer::protocol_functor& protocol);

protected:
    /**
     * ��ȡ������Ϣ
     */
    void initializeClient();

    /**
     * ���
     * @param os
     */
    void outClient(ostream &os);

    /**
     * ��ʼ��servant
     */
    void initializeServer();

    /**
     * ���
     * @param os
     */
    void outServer(ostream &os);

    /**
     * ������е�adapter
     * @param os
     */
    void outAllAdapter(ostream &os);

    /**
     * ���
     * @param os
     */
    void outAdapter(ostream &os, const string &v, TC_EpollServer::BindAdapterPtr lsPtr);

    /**
     * ���������ļ�
     */
    void parseConfig(int argc, char *argv[]);

     /**
     * ����ipȨ��allow deny ����
     */
    TC_EpollServer::BindAdapter::EOrder parseOrder(const string &s);

    /**
     * ��server���õ�Adapter�Ͷ���
     */
    void bindAdapter(vector<TC_EpollServer::BindAdapterPtr>& adapters);

	/*
	* �ָ�BUS����
	*/
	//void BusConnectRecover();

    /**
     * @param servant
     * @param sPrefix
     */
    void checkServantNameValid(const string& servant, const string& sPrefix);

    /**
     * ����ȱʡֵ
     * @param s
     *
     * @return string
     */
    string toDefault(const string &s, const string &sDefault);

	/**
	 * ��ȡ�����set������Ϣ,setname.setarea.setgroup
	 *
	 * @return string û�а�set�����򷵻ؿ�""
	 */
	 string setDivision(void);

protected:
    /**
     * �����ļ�
     */
    static TC_Config _conf;

    /**
     * ����
     */
    static TC_EpollServerPtr _epollServer;

    /**
     * ͨ����
     */
    static CommunicatorPtr _communicator;
};
////////////////////////////////////////////////////////////////////
}

#endif

