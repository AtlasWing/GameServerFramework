#ifndef __TAF_LOGGER_H__
#define __TAF_LOGGER_H__

#include "util/tc_logger.h"
#include "util/tc_file.h"
#include "util/tc_singleton.h"
#include "servant/Global.h"
#include "log/Log.h"

#define DYEING_DIR "taf_dyeing"
#define DYEING_FILE "dyeing"

namespace taf
{

/**
 * LOG�Ŀ�˵��:
 * �����뿴test/log�Ĵ���
 * 1 ѭ����־����LOG->error(), LOG->debug()
 * 2 ѭ����־���ϴ���������
 * 3 ������־����DLOG, FDLOG����¼
 * 4 ������־Ҳ���Բ��ϴ���Զ�̷�����:DLOG("")->disableRemote();
 * 5 ������־���Ըı�ÿ��һ���ļ��ķ�ʽ:
 *   DLOG("abc3")->setFormat("%Y%m%d%H");
 *   ÿ��Сʱһ���ļ�
 */

/*****************************************************************************
ʵ�ַ�ʽ˵��(ֻ���ܰ�ʱ�����־, ��д��taflog):
    1 �Զ���ʱ����־��WriteT��:RemoteTimeWriteT
    2 ��RemoteTimeWriteT����, д�뵽Զ��
    3 ����Զ����־��:typedef TC_Logger<RemoteTimeWriteT, TC_RollByTime> RemoteTimeLogger;
    4 Ϊ�˱�֤Զ�̵�д��־Ҳ���ڵ����̴߳���,���¶��屾�ذ�����־��
    5 �Զ���ʱ����־��WriteT��:TimeWriteT
    6 ��TimeWriteT���а���RemoteTimeLogger����
    7 ��TimeWriteT���д�������, д�뱾���ļ���, ͬʱд�뵽RemoteTimeLogger������
    8 RemoteTimeLogger����RemoteTimeWriteT������, �첽д�뵽Զ��
    9 �Ӷ������ļ�д��Զ��д����һ���߳���.
*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////

class RollWriteT
{
public:
    RollWriteT();
    ~RollWriteT();

    void operator()(ostream &of, const deque<pair<int, string> > &ds);

    void setDyeingLogInfo(const string &sApp, const string &sServer, const string & sLogPath,
            int iMaxSize, int iMaxNum, const CommunicatorPtr &comm, const string & sLogObj);

protected:

    TC_RollLogger *_pDyeingRollLogger;

    static int  _iDyeingThread;

    string _sApp;
    string _sServer;
    string _sLogPath;
    int _iMaxSize;
    int _iMaxNum;

    /**
     * ȾɫԶ�̹�����־����
     */
    LogPrx                _logPrx;


};


/**
 * ������־������, ����
 * ѭ����־����������������, ��֤�κεط�������ʹ��
 * ���ö��������Ժ�, ��ֱ��cout����
 */
class TafRollLogger : public TC_Singleton<TafRollLogger, CreateUsingNew, PhoneixLifetime>
{
public:
		enum
		{
			NONE_LOG    = 1,	/**���е�log����д*/
			ERROR_LOG   = 2,	/**д����log*/
			WARN_LOG    = 3,	/**д����,����log*/
			DEBUG_LOG   = 4,	/**д����,����,����log*/
			INFO_LOG    = 5		/**д����,����,����,Info log*/
		};
public:
    typedef TC_Logger<RollWriteT, TC_RollBySize> RollLogger;

    /**
     * ���ñ�����Ϣ
     * @param app, ҵ������
     * @param server, ��������
     * @param logpath, ��־·��
     * @param iMaxSize, �ļ�����С,�ֽ�
     * @param iMaxNum, �ļ������
     */
    void setLogInfo(const string &sApp, const string &sServer, const string &sLogpath, int iMaxSize = 1024*1024*50, int iMaxNum = 10, const CommunicatorPtr &comm=NULL, const string &sLogObj="");

    /**
     * ����ͬ��д��־
     *
     * @param bSync
     */
    void sync(bool bSync = true);

    /**
     * ��ȡѭ����־
     *
     * @return RollLogger
     */
    RollLogger *logger()          { return &_logger; }

    /**
     * Ⱦɫ��־�Ƿ�����
     * @param bEnable
     */
    void enableDyeing(bool bEnable, const string& sDyeingKey = "");

protected:

    /**
     * Ӧ��
     */
    string                  _sApp;

    /**
     * ��������
     */
    string                  _sServer;

    /**
     * ��־·��
     */
    string                  _sLogpath;

    /**
     * ѭ����־
     */
    RollLogger           _logger;

    /**
     * �����߳���
     */
    TC_LoggerThreadGroup    _local;

};

///////////////////////////////////////////////////////////////////////////////////////
//
/**
 * д��־�߳�
 * ��д������־��Զ�̷ֿ�����ͬ���߳�
 * ��Ϊ��������, �������������ĵ���
 */
class TafLoggerThread : public TC_Singleton<TafLoggerThread, CreateUsingNew, PhoneixLifetime>
{
public:
    /**
     * ���캯��
     */
    TafLoggerThread();

    /**
     * ��������
     */
    ~TafLoggerThread();

    /**
     * ����д��־�߳�
     */
    TC_LoggerThreadGroup* local();

    /**
     * Զ��д��־�߳�
     *
     * @return TC_LoggerThreadGroup*
     */
    TC_LoggerThreadGroup* remote();

protected:

    /**
     * �����߳���
     */
    TC_LoggerThreadGroup    _local;

    /**
     * Զ��д�߳���
     */
    TC_LoggerThreadGroup    _remote;
};

///////////////////////////////////////////////////////////////////////////////////////
class TimeWriteT;

/**
 * Զ�̵�Logд������
 */
class RemoteTimeWriteT
{
public:
	RemoteTimeWriteT();
    ~RemoteTimeWriteT();

    /**
     * ���캯��
     */
    void setTimeWriteT(TimeWriteT *pTimeWrite);

    /**
     * �������
     * @param of
     * @param buffer
     */
    void operator()(ostream &of, const deque<pair<int, string> > &buffer);

protected:
    /**
     * ͬ����Զ��
     */
    void sync2remote(const vector<string> &buffer);

    /**
     * Ⱦɫ��־ͬ����Զ��
     */
    void sync2remoteDyeing(const vector<string> &buffer);

protected:
    /**
     * ָ��
     */
    TimeWriteT          *_pTimeWrite;

};

////////////////////////////////////////////////////////////////////////////
/**
 * дLogger
 */
class TimeWriteT
{
public:
    typedef TC_Logger<RemoteTimeWriteT, TC_RollByTime> RemoteTimeLogger;

    typedef TC_Logger<TC_DefaultWriteT, TC_RollByTime> DyeingTimeLogger;

    /**
     * ����
     */
    TimeWriteT();

    /**
     * ����
     */
    ~TimeWriteT();

    /**
     * ���û�����Ϣ
     * @param app, Ӧ������
     * @param server, ��������
     * @param file, ��־�ļ���
     * @param sFormat, ��ʽ
     * @param setdivision,set����
     * @param sLogType,��־��¼����
     */
    void setLogInfo(const LogPrx &logPrx, const string &sApp, const string &sServer, const string &sFile, const string &sLogpath, const string &sFormat,const string& setdivision="",const string& sLogType="");

    /**
     * ���ô���
     * @param logPrx ������Ϣ
     */
    void setLogPrx(const LogPrx &logPrx);

    /**
     * Զ����־���ܴ򿪻�ر�
     * @param bEnable
     */
    void enableRemote(bool bEnable)         { _bRemote = bEnable; }

    /**
     * ������־���ܹ��ܴ򿪻�ر�
     * @param bEnable
     */
    void enableLocal(bool bEnable);

    /**
     * Ⱦɫ��־���ܴ򿪻�ر�
     * @param bEnable
     */
    void enableDyeing (bool bEnable, const string& sDyeingKey = "");


	/**
	 * @brief ��־�ļ����Ƿ��.log��׺
	 * @param bEnable
	 */
	void enableSufix(bool bEnable=true){_bHasSufix = bEnable;}
	/**
	 * @brief �Ƿ�����������־�ļ���������ҵ����صı�ʶ
	 * @param bEnable
	 */
	void enablePrefix(bool bEnable=true){_bHasAppNamePrefix = bEnable;}
	/**
	 * @brief ��־�ļ������û��Զ����ַ��������ַ�������ӷ���Ĭ����"_"
	 * @param str
	 */
	void setFileNameConcatStr(const string& str) {_sConcatStr = str;}

	/**
	 * @brief ��������ӵ���־����֮��ķָ����Ĭ����"|"
	 * @param str
	 */
	void setSeparator(const string& str) {_sSepar = str;}

	/**
	 * @brief ��������ں�ʱ��֮���Ƿ���Ҫ��������[],��Щͳ������������Ĭ�ϲ���
	 * @param bEnable
	 */
	void enableSqareWrapper(bool bEnable) {_bHasSquareBracket = bEnable;}


    /**
     * ����ʱ���ʽ("%Y%m%d")
     * @param sFormat
     */
    void setFormat(const string &sFormat)   { _sFormat = sFormat;}

    /**
     * �������
     * @param of
     * @param buffer
     */
    void operator()(ostream &of, const deque<pair<int, string> > &buffer);

protected:

    /**
     * ��Ԫ
     */
    friend class RemoteTimeWriteT;

    /**
     * ��¼�����ļ�
     * @param buffer
     */
    void writeError(const vector<string> &buffer);

    /**
     * ��¼�����ļ�
     * @param buffer
     */
    void writeError(const deque<pair<int, string> > &buffer);

    /**
     * ��ʼ��logger
     */
    void initError();

    /**
     * ��ʼ��Ⱦɫ��־
     */
    void initDyeingLog();

protected:

    /**
     * Զ��ʱ����־
     */
    RemoteTimeLogger    *_pRemoteTimeLogger;

    /**
     * ���ع���
     */
    bool                _bLocal;

    /**
     * Զ�̹���
     */
    bool                _bRemote;

    /**
     * Զ�̷�����
     */
    LogPrx              _logPrx;

    /**
     * app����
     */
    string              _sApp;

    /**
     * ��������
     */
    string              _sServer;

    /**
     * ��־�ļ�����
     */
    string              _sFile;

    /**
     * ʱ���ʽ
     */
    string              _sFormat;

    /**
     * �����ļ�
     */
    string              _sFilePath;

    /**
     * �����ļ�
     */
    TC_DayLogger        _elogger;

    /**
     * ȱʡдģʽ
     */
    TC_DefaultWriteT    _wt;

    /**
     * Ⱦɫ��־
     */
    static int                _bDyeing;

    /**
     * Ⱦɫ��־Ŀ¼·��
     */
    string              _sDyeingFilePath;

    /**
     * Զ��ʱ����־
     */
    DyeingTimeLogger    *_pDyeingTimeLogger;

    /**
     * set������Ϣ
     */
    string                  _sSetDivision;

	/**
	 * ��־�ļ����Ƿ��.log��׺
	 */
	bool					_bHasSufix;
	/**
	 * �Ƿ�����������־�ļ���������ҵ����صı�ʶ
	 */
	bool 					_bHasAppNamePrefix;

	/**
	 * ��־�ļ������û��Զ����ַ��������ַ�������ӷ���Ĭ����"_"
	 */
	string 					_sConcatStr;
	/**
	 * �ָ���
	 */
	 string        _sSepar;
	/**
	 * ���ڲ����Ƿ����[]
	 */
	 bool 		  _bHasSquareBracket;

	 /*
	  * ������־�ļ�¼����,��ʽΪTafLogType.toString()����ֵ,���������TafLogType�����ֵΪ""
	  */
	 string       _sLogType;
};

////////////////////////////////////////////////////////////////////////////
/**
 * Զ����־������, ����
 */
class TafTimeLogger : public TC_HandleBase
                    , public TC_ThreadLock
                    , public TC_Singleton<TafTimeLogger, CreateUsingNew, DefaultLifetime>
{
public:

    //���尴ʱ���������־
    typedef TC_Logger<TimeWriteT, TC_RollByTime> TimeLogger;

    /**
     * ����
     */
    TafTimeLogger();

    /**
     * ����
     */
    ~TafTimeLogger();

    /**
     * ���ñ�����Ϣ
     * @param comm, ͨ����
     * @param obj, ��־��������
     * @param app, ҵ������
     * @param server, ��������
     * @param logpath, ��־·��
     */
    void setLogInfo(const CommunicatorPtr &comm, const string &obj, const string &sApp, const string &sServer, const string &sLogpath,const string& setdivision="");

	/**
     * ��ʼ������ʱ���ʽ("%Y%m%d")
     * ��Ҫ��̬�޸�, �̲߳���ȫ
     * �������Ҫ, ��ʼ����ֱ���޸�
     * @param sFormat, �ļ�����, Ϊ�ձ�ʾȱʡ��ʱ����־
     */
	void initFormat(const string &sFile, const string &sFormat,const TafLogTypePtr& logTypePtr=NULL);
	void initFormat(const string &sApp, const string &sServer,const string &sFile, const string &sFormat,const TafLogTypePtr& logTypePtr=NULL);
    /**
     * ��ʼ������ʱ���ʽ("%Y%m%d")
     * ��Ҫ��̬�޸�, �̲߳���ȫ
     * �������Ҫ, ��ʼ����ֱ���޸�
     * @param sFormat, �ļ�����, Ϊ�ձ�ʾȱʡ��ʱ����־
     * @param frequency ֧��ÿ������/Сʱ/���ӣ����TC_logger.h�й���TafLogByDay,TafLogByHour,TafLogByMinute������
     *
     * �÷�: ������Сʱ��¼��־
     * initFormat<TafLogByHour>("logfile",TafLogByHour::FORMAT,2);
     */
    template<class TafLogType>
    void initFormatWithType(const string &sFile, const string &sFormat,size_t frequency)
    {
        TafLogTypePtr logTypePtr = new TafLogType(sFormat,frequency);
        initFormat(sFile,sFormat,logTypePtr);
    }

    template<class TafLogType>
    void initFormatWithType(const string &sApp, const string &sServer,const string &sFile, const string &sFormat,size_t frequency)
    {
        TafLogTypePtr logTypePtr = new TafLogType(sFormat,frequency);
        initFormat(sApp,sServer,sFile,sFormat,logTypePtr);
    }
    /**
     * ��ȡʱ����־
     * @param file
     */
    TimeLogger *logger(const string &sFile = "");

    /**
     * ��ȡʱ����־
     * @param app, ҵ������
     * @param server, ��������
     * @param file
     */
    TimeLogger *logger(const string &sApp, const string &sServer,const string &sFile = "");

    /**
     * ͬ��д����ʱ����־(Զ����־һ�����첽д��, �޷�����)
     * @param bSync
     */
    void sync(const string &sFile, bool bSync);

    /**
     * Զ��ʱ����־
     * @param sFile, �ļ�����, Ϊ�ձ�ʾȱʡ��ʱ����־
     * @param bEnable
     */
    void enableRemote(const string &sFile, bool bEnable);

    /**
     * Զ��ʱ����־
     * @param sApp,Ӧ������
     * @param sServer,��������
     * @param sFile, �ļ�����, Ϊ�ձ�ʾȱʡ��ʱ����־
     * @param bEnable
     */
	void enableRemoteEx(const string &sApp, const string &sServer,const string &sFile, bool bEnable);
    /**
     * ����ʱ����־
     * @param sFile,�ļ�����, Ϊ�ձ�ʾȱʡ��ʱ����־
     * @param bEnable
     */
    void enableLocal(const string &sFile, bool bEnable);
    /**
     * ����ʱ����־
     * @param sApp,Ӧ������
     * @param sServer,��������
     * @param sFile, �ļ�����, Ϊ�ձ�ʾȱʡ��ʱ����־
     * @param bEnable
     */
	void enableLocalEx(const string &sApp, const string &sServer,const string &sFile, bool bEnable);

	/**
	 * @brief ��־�ļ����Ƿ��.log��׺,Ӱ��ȫ����־�ļ�
	 * @param bEnable
	 */
	void enableSufix(bool bEnable=true){_bHasSufix = bEnable;}
	/**
	 * @brief �Ƿ�����������־�ļ���������ҵ����صı�ʶ,Ӱ��ȫ����־�ļ�
	 * @param bEnable
	 */
	void enablePrefix(bool bEnable=true){_bHasAppNamePrefix = bEnable;}
	/**
	 * @brief ��־�ļ������û��Զ����ַ��������ַ�������ӷ���Ĭ����"_",Ӱ��ȫ����־�ļ�
	 * @param str
	 */
	void setFileNameConcatStr(const string& str) {_sConcatStr = str;}

	/**
	 * @brief ��������ӵ���־����֮��ķָ����Ĭ����"|",Ӱ��ȫ����־�ļ�
	 * @param str
	 */
	void setSeparator(const string& str) {_sSepar = str;}

	/**
	 * @brief ��������ں�ʱ��֮���Ƿ���Ҫ��������[],��Щͳ������������Ĭ�ϲ���,Ӱ��ȫ����־�ļ�
	 * @param bEnable
	 */
	void enableSqareWrapper(bool bEnable) {_bHasSquareBracket = bEnable;}
    /**
	 * @brief �Ƿ����������־�ļ���Ӱ��ȫ����־�ļ�
	 * @param bEnable
     */
    void enableLocalLog(bool bEnable) {_bLocal = bEnable;}
	/**
	 * @brief �Ƿ����Զ����־�ļ�,Ӱ��ȫ����־�ļ�
	 * @param bEnable
	 */
	void enableRemoteLog(bool bEnable) {_bRemote = bEnable;}
protected:

    /**
     * ��ʼ��ʱ����־
     * @param pTimeLogger
     * @param sFile
     * @param sFormat
     * @param frequence, ÿ������/Сʱ/����,��λ����
     */
    void initTimeLogger(TimeLogger *pTimeLogger, const string &sFile, const string &sFormat,const TafLogTypePtr& logTypePtr=NULL);

    /**
     * ��ʼ��ʱ����־
     * @param pTimeLogger
     * @param sApp
     * @param sServer
     * @param sFile
     * @param sFormat
     * @param frequence, ÿ������/Сʱ/����,��λ����
     */

    void initTimeLogger(TimeLogger *pTimeLogger,const string &sApp, const string &sServer, const string &sFile, const string &sFormat,const TafLogTypePtr& logTypePtr=NULL);

protected:

    /**
     * ͨ����
     */
    CommunicatorPtr         _comm;

    /**
     * Զ�̷�����
     */
    LogPrx                  _logPrx;

    /**
     * Ӧ��
     */
    string                  _sApp;

    /**
     * ��������
     */
    string                  _sServer;

    /**
     * ��־·��
     */
    string                  _sLogpath;

    /**
     * ȱʡ������־
     */
    TimeLogger              *_pDefaultLogger;

    /**
     * Զ����־
     */
    map<string, TimeLogger*>    _loggers;
    /**
     * set������Ϣ
     */
    string                  _sSetDivision;

	/**
	 * �Ƿ��.log��׺
	 */
	bool					_bHasSufix;
	/**
	 * �Ƿ�����������־�ļ���������ҵ����صı�ʶ
	 */
	bool 					_bHasAppNamePrefix;

	/**
	 * ��־�ļ������û��Զ����ַ��������ַ�������ӷ���Ĭ����"_"
	 */
	string 					_sConcatStr;
	/**
	 * �ָ���
	 */
	string        _sSepar;
	/**
	 * ���ڲ����Ƿ����[]
	 */
	bool 		  _bHasSquareBracket;
    /**
     * �Ƿ����������־
     */
	bool         _bLocal;
    /**
     * �Ƿ����Զ����־
     */
	bool        _bRemote;
};

/**
 * Ⱦɫ�����࣬����ʱ�ر�
 */
class TafDyeingSwitch
{
public:
    /**
     * ���캯����Ĭ�ϲ���Ⱦɫ��־
     */
    TafDyeingSwitch()
    :_bNeedDyeing(false)
    {
    }

    /**
     * �����������ر��Ѵ򿪵�Ⱦɫ��־
     */
    ~TafDyeingSwitch()
	{
		if(_bNeedDyeing)
		{
			TafRollLogger::getInstance()->enableDyeing(false);

			ServantProxyThreadData * td = ServantProxyThreadData::getData();
            assert(NULL != td);
			if (td)
			{
	            td->_bDyeing = false;
				td->_dyeingKey = "";
			}
		}
	}

	/**
	 * ��ȡȾɫ��key
	 *
	 * @author coonzhang (2011/8/19)
	 * @param key
	 * @return bool
	 */
	static bool getDyeingKey(string & sDyeingkey)
	{
		ServantProxyThreadData * td = ServantProxyThreadData::getData();
        assert(NULL != td);

		if (td && td->_bDyeing == true)
		{
			sDyeingkey = td->_dyeingKey;
			return true;
		}
		return false;
	}

    /**
     * ����Ⱦɫ��־
     */
    void enableDyeing(const string & sDyeingKey = "")
	{
		TafRollLogger::getInstance()->enableDyeing(true);

		ServantProxyThreadData * td = ServantProxyThreadData::getData();
        assert(NULL != td);
		if(td)
		{
	        td->_bDyeing	= true;
			td->_dyeingKey = sDyeingKey;
		}
		_bNeedDyeing = true;
        _sDyeingKey = sDyeingKey;
	}

protected:
    bool _bNeedDyeing;
    string _sDyeingKey;
};

/**
 * ѭ����־
 */
#define LOG             (TafRollLogger::getInstance()->logger())

/**
 * @brief ������ѭ����־��
 *
 * @param level ��־�ȼ�,TafRollLogger::INFO_LOG,TafRollLogger::DEBUG_LOG,TafRollLogger::WARN_LOG,TafRollLogger::ERROR_LOG
 * @msg ��־�������,����<<�ض�������ӵ����,�� "Demo begin" << " testing !" <<endl;
 *
 * @�÷�:
 *       ��׼�������ʽ: cout << "I have " << vApple.size() << " apples!"<<endl;
 *       ��ܺ귽ʽ:     LOGMSG(TafRollLogger::INFO_LOG,"I have " << vApple.size() << " apples!"<<endl);
 */
#define LOGMSG(level,msg...) do{ if(LOG->IsNeedLog(level)) LOG->log(level)<<msg;}while(0)

/**
 * @brief ������ѭ����־
 *
 * @msg ��־�������,����<<�ض�������ӵ����,�� "Demo begin" << " testing !" <<endl;
 *
 * @�÷�:
 *       ��׼�������ʽ: cout << "I have " << vApple.size() << " apples!"<<endl;
 *       ��ܺ귽ʽ:     TLOGINFO("I have " << vApple.size() << " apples!"<<endl);
 */
#define TLOGINFO(msg...)    LOGMSG(TafRollLogger::INFO_LOG,msg)
#define TLOGDEBUG(msg...)   LOGMSG(TafRollLogger::DEBUG_LOG,msg)
#define TLOGWARN(msg...)    LOGMSG(TafRollLogger::WARN_LOG,msg)
#define TLOGERROR(msg...)   LOGMSG(TafRollLogger::ERROR_LOG,msg)

/**
 * ������־
 */
#define DLOG            (TafTimeLogger::getInstance()->logger()->any())
#define FDLOG(x)        (TafTimeLogger::getInstance()->logger(x)->any())
#define FFDLOG(x,y,z)   (TafTimeLogger::getInstance()->logger(x,y,z)->any())

/**
 *  ������־�ֲ�ʹ�ܿ��أ���Ե�����־�ļ�����ʹ�ܣ��������а�����־���ǰ����
 */
#define TENREMOTE_FDLOG(swith,sApp,sServer,sFile) (TafTimeLogger::getInstance()->enableRemoteEx(sApp,sServer,sFile,swith))
#define TENLOCAL_FDLOG(swith,sApp,sServer,sFile) (TafTimeLogger::getInstance()->enableLocalEx(sApp,sServer,sFile,swith))

/**
 * ������־ȫ��ʹ�ܿ��أ��������а�����־���ǰ����
 */
#define TENREMOTE(swith) (TafTimeLogger::getInstance()->enableRemoteLog(swith))
#define TENLOCAL(swith)  (TafTimeLogger::getInstance()->enableLocalLog(swith))
}

#endif

