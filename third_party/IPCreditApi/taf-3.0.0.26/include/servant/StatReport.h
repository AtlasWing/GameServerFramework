#ifndef __TAF_STAT_REPORT_H_
#define __TAF_STAT_REPORT_H_

#include "util/tc_thread.h"
#include "servant/PropertyReport.h"
#include "servant/StatF.h"
#include "servant/PropertyF.h"

/*
    �����ϱ�ʹ�÷�ʽ:
    StatReport report;
    report->setReportInfo( pPtr,strMasterName,strMasterIp, 5 );
    vector<int> v;
    v.push_back(1000);
    v.push_back(1001);
    v.push_back(1002);
    v.push_back(1003);
    v.push_back(1004);

    PropertyReportPtr srPtr = report->createPropertyReport("test", PropertyReport::sum(), PropertyReport::dist(v));
    srPtr->report(i);
*/

namespace taf
{

/**
 * ״̬�ϱ���, �ϱ�����Ϣ����:
 * 1 ģ�����õ���Ϣ
 * 2 ҵ���Զ��������ͳ��
 */
struct StatSampleMsgHead
{
    string slaveName;
    string interfaceName;
    string ip;
    bool operator <(const StatSampleMsgHead& m)const
    {
        if(slaveName != m.slaveName)
        {
            return slaveName < m.slaveName;
        }
        if(interfaceName != m.interfaceName)
        {
            return interfaceName < m.interfaceName;
        }
        return ip < m.ip;
    }
};
class StatReport : public TC_HandleBase, public TC_Thread, public TC_ThreadLock
{
public:
    typedef  map<StatMicMsgHead, StatMicMsgBody>        MapStatMicMsg;
    typedef  map<StatPropMsgHead, StatPropMsgBody>      MapStatPropMsg;
    typedef  multimap<StatSampleMsgHead,StatSampleMsg>  MMapStatSampleMsg;
	typedef  TC_LoopQueue<MapStatMicMsg*>         	     stat_queue;

    enum StatResult
    {
        STAT_SUCC       = 0,
        STAT_TIMEOUT    = 1,
        STAT_EXCE       = 2,
    };
public:
    /**
     * ���캯��
     */
    StatReport(size_t iEpollNum=0);

    /**
     * ��������
     */
    ~StatReport();

    /**
     * ��ʼ��
     * @param statPrx, ģ�����÷�������ַ
     * @param propertyPrx, �û��Զ������Է�������ַ
     * @param strModuleName, ģ����
     * @param strModuleIp, ģ��ip
     * @param iReportInterval, �ϱ������λ��
     * @param iMaxReporSizeһ������ϱ������ȡ� ��udp��������8k��MTU����1472�йأ��ݶ�ȡֵ��Χ[500-1400]
     * @param iSampleRate, ��������1/1000
     * @param iMaxSampleCount, ��������
     */
    void setReportInfo(const StatFPrx& statPrx,
                       const PropertyFPrx& propertyPrx,
                       const string& strModuleName,
                       const string& strModuleIp,
                       const string& strSetDivision,
                       int iReportInterval = 60,
                       int iSampleRate = 1000,
                       unsigned int iMaxSampleCount = 100,
                       int iMaxReportSize = 1400);

    /**
     * ����ģ����������
     * @param strModuleName, ����ģ��. һ�����app.servername ����:Comm.BindServer
     * @param setdivision,����set��Ϣ,=>>MTT.s.s
     * @param strInterfaceName,�����ӿ�.һ��Ϊ������
     * @param strModuleIp,����ip
     * @param shSlavePort,����port
     * @param eResult,�ɹ�STAT_SUCC����ʱ STAT_TIMEOUT���쳣STAT_EXC.
     * @param iSptime,��ʱ
     * @param iReturnValue,����ֵ
     * @param bFromClient,�ӿͻ��˲ɼ� false�ӷ���˲ɼ�
     * ��
     */
    void report(const string& strModuleName,
     			const string& setdivision,
                const string& strInterfaceName,
                const string& strModuleIp,
                uint16_t iPort,
                StatResult eResult,
                int  iSptime,
                int  iReturnValue = 0,
                bool bFromClient = true);

     /**
     * ����ģ����������
     * @param strMasterName     �����÷�������
     * @param strMasterIp       �����÷��ĵ�ַ
     * @param strSlaveName      �����÷�������
     * @param strSlaveIp        �����÷��ĵ�ַ
     * @param iSlavePort        �����÷��Ķ˿�
     * @param strInterfaceName  �����ӿ�.һ��Ϊ������
     * @param eResult           �ɹ�STAT_SUCC����ʱ STAT_TIMEOUT���쳣STAT_EXC.
     * @param iSptime           ��ʱ(��λ����)
     * @param iReturnValue      ����ֵ
     */
    void report(const string& strMasterName,
                const string& strMasterIp,
                const string& strSlaveName,
                const string& strSlaveIp,
                uint16_t iSlavePort,
                const string& strInterfaceName,
                StatResult eResult,
                int  iSptime,
                int  iReturnValue = 0);

    /**
     * �������ֻ�ȡ�����ϱ�����
     * @param sProperty
     *
     * @return PropertyReportPtr
     */
    PropertyReportPtr getPropertyReport(const string& strProperty)
    {
         Lock lock(*this);
         if(_mStatPropMsg.find(strProperty) != _mStatPropMsg.end())
         {
             return _mStatPropMsg[strProperty];
         }

         return NULL;
    }

    /**
     * ���������ϱ�����
     * @param strProperty
     * @param t1
     *
     * @return PropertyReportPtr
     */
    template<typename T1>
    PropertyReportPtr createPropertyReport(const string& strProperty, T1 t1)
    {
        Lock lock(*this);

        if(_mStatPropMsg.find(strProperty) != _mStatPropMsg.end())
        {
            return _mStatPropMsg[strProperty];
        }

         PropertyReportPtr srPtr = new PropertyReportImp<typename TL::TLMaker<T1>::Result>(t1);

         _mStatPropMsg[strProperty] = srPtr;

         return srPtr;
    }

    /**
     * ���������ϱ�����
     * @param strProperty
     * @param t1
     * @param t2
     *
     * @return PropertyReportPtr
     */
    template<typename T1, typename T2>
    PropertyReportPtr createPropertyReport(const string& strProperty, T1 t1, T2 t2)
    {
        Lock lock(*this);

        if(_mStatPropMsg.find(strProperty) != _mStatPropMsg.end())
        {
            return _mStatPropMsg[strProperty];
        }
        PropertyReportPtr srPtr = new PropertyReportImp<typename TL::TLMaker<T1, T2>::Result>(t1, t2);

        _mStatPropMsg[strProperty] = srPtr;

        return srPtr;
    };

    /**
     * ���������ϱ�����
     * @param strProperty
     * @param t1
     * @param t2
     * @param t3
     *
     * @return PropertyReportPtr
     */
    template<typename T1, typename T2, typename T3>
    PropertyReportPtr createPropertyReport(const string& strProperty, T1 t1, T2 t2, T3 t3)
    {
        Lock lock(*this);

        if(_mStatPropMsg.find(strProperty) != _mStatPropMsg.end())
        {
            return _mStatPropMsg[strProperty];
        }
        PropertyReportPtr srPtr = new PropertyReportImp<typename TL::TLMaker<T1, T2, T3>::Result>(t1, t2, t3);

        _mStatPropMsg[strProperty] = srPtr;

        return srPtr;
    };

    /**
     * ���������ϱ�����
     * @param strProperty
     * @param t1
     * @param t2
     * @param t3
     * @param t4
     *
     * @return PropertyReportPtr
     */
    template<typename T1, typename T2, typename T3, typename T4>
    PropertyReportPtr createPropertyReport(const string& strProperty, T1 t1, T2 t2, T3 t3, T4 t4)
    {
        Lock lock(*this);

        if(_mStatPropMsg.find(strProperty) != _mStatPropMsg.end())
        {
            return _mStatPropMsg[strProperty];
        }
        PropertyReportPtr srPtr = new PropertyReportImp<typename TL::TLMaker<T1, T2, T3, T4>::Result>(t1, t2, t3, t4);

        _mStatPropMsg[strProperty] = srPtr;

        return srPtr;
    };

    /**
     * ���������ϱ�����
     * @param strProperty
     * @param t1
     * @param t2
     * @param t3
     * @param t4
     * @param t5
     *
     * @return PropertyReportPtr
     */
    template<typename T1, typename T2, typename T3, typename T4, typename T5>
    PropertyReportPtr createPropertyReport(const string& strProperty, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
    {
        Lock lock(*this);

        if(_mStatPropMsg.find(strProperty) != _mStatPropMsg.end())
        {
            return _mStatPropMsg[strProperty];
        }
        PropertyReportPtr srPtr = new PropertyReportImp<typename TL::TLMaker<T1, T2, T3, T4, T5>::Result>(t1, t2, t3, t4, t5);

        _mStatPropMsg[strProperty] = srPtr;

        return srPtr;
    };

    /**
     * ���������ϱ�����
     * @param strProperty
     * @param t1
     * @param t2
     * @param t3
     * @param t4
     * @param t5
     * @param t6
     *
     * @return PropertyReportPtr
     */
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    PropertyReportPtr createPropertyReport(const string& strProperty, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
    {
        Lock lock(*this);

        if(_mStatPropMsg.find(strProperty) != _mStatPropMsg.end())
        {
            return _mStatPropMsg[strProperty];
        }
        PropertyReportPtr srPtr = new PropertyReportImp<typename TL::TLMaker<T1, T2, T3, T4, T5, T6>::Result>(t1, t2, t3, t4, t5, t6);

        _mStatPropMsg[strProperty] = srPtr;

        return srPtr;
    };

    /**
     * ���������ϱ�����
     * @param strProperty
     * @param t1
     * @param t2
     * @param t3
     * @param t4
     * @param t5
     * @param t6
     * @param t7
     *
     * @return PropertyReportPtr
     */
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    PropertyReportPtr createPropertyReport(const string& strProperty, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7)
    {
        Lock lock(*this);

        if(_mStatPropMsg.find(strProperty) != _mStatPropMsg.end())
        {
            return _mStatPropMsg[strProperty];
        }
        PropertyReportPtr srPtr = new PropertyReportImp<typename TL::TLMaker<T1, T2, T3, T4, T5, T6, T7>::Result>(t1, t2, t3, t4, t5, t6, t7);

        _mStatPropMsg[strProperty] = srPtr;

        return srPtr;
    };

    /**
     * ���������ϱ�����
     * @param strProperty
     * @param t1
     * @param t2
     * @param t3
     * @param t4
     * @param t5
     * @param t6
     * @param t7
     * @param t8
     *
     * @return PropertyReportPtr
     */
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    PropertyReportPtr createPropertyReport(const string& strProperty, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8)
    {
        Lock lock(*this);

        if(_mStatPropMsg.find(strProperty) != _mStatPropMsg.end())
        {
            return _mStatPropMsg[strProperty];
        }
        PropertyReportPtr srPtr = new PropertyReportImp<typename TL::TLMaker<T1, T2, T3, T4, T5, T6, T7, T8>::Result>(t1, t2, t3, t4, t5, t6, t7, t8);

        _mStatPropMsg[strProperty] = srPtr;

        return srPtr;
    };

    /**
     * ���������ϱ�����
     * @param strProperty
     * @param t1
     * @param t2
     * @param t3
     * @param t4
     * @param t5
     * @param t6
     * @param t7
     * @param t8
     * @param t9
     *
     * @return PropertyReportPtr
     */
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
    PropertyReportPtr createPropertyReport(const string& strProperty, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9)
    {
        Lock lock(*this);

        if(_mStatPropMsg.find(strProperty) != _mStatPropMsg.end())
        {
            return _mStatPropMsg[strProperty];
        }

        PropertyReportPtr srPtr = new PropertyReportImp<typename TL::TLMaker<T1, T2, T3, T4, T5, T6, T7, T8, T9>::Result>(t1, t2, t3, t4, t5, t6, t7, t8, t9);

        _mStatPropMsg[strProperty] = srPtr;

        return srPtr;
    };

    /**
     * ���������ϱ�����
     * @param strProperty
     * @param t1
     * @param t2
     * @param t3
     * @param t4
     * @param t5
     * @param t6
     * @param t7
     * @param t8
     * @param t9
     * @param t10
     *
     * @return PropertyReportPtr
     */
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
    PropertyReportPtr createPropertyReport(const string& strProperty, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10)
    {
        Lock lock(*this);

        if(_mStatPropMsg.find(strProperty) != _mStatPropMsg.end())
        {
            return _mStatPropMsg[strProperty];
        }

        PropertyReportPtr srPtr = new PropertyReportImp<typename TL::TLMaker<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>::Result>(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);

        _mStatPropMsg[strProperty] = srPtr;

        return srPtr;
    };

public:
    void report(size_t iSeq,MapStatMicMsg * pmStatMicMsg);

public:

    /*
    * ��ȡstat����
    */
    StatFPrx getStatPrx() {return _statPrx; }

    /*
    * ����
    */
    void doSample(const string& strSlaveName,
                      const string& strInterfaceName,
                      const string& strSlaveIp,
                      map<string, string> &status);
    /*
    * ����id
    */
    string sampleUnid();

    /**
     * ���ӹ�עʱ���.  ���÷�ʽaddStatInterv(5)
     * @param strTimePoint  ʱ�������
     * @return
     */
    void addStatInterv(int  iInterv);

    /**
     * ���ù�עʱ���.
     * @return
     */
    void resetStatInterv();

    /**
     *
     * @param body
     * @param time
     * @return void
     */
    void getIntervCount(int time,StatMicMsgBody& body);
    /**
     * �����߳�
     */
    void terminate();

    /**
     * ��ѯ����
     */
    virtual void run();
public:
    /**
     * ���Ƴ���
     * @param str
     * @param limitlen
     *
     * @return string
     */
    static string trimAndLimitStr(const string& str, uint32_t limitlen);
	/*
	* Author :johnson
	* func: taf.s.1 =>> MTT s  1
	*/
	static bool divison2SetInfo(const string& str, vector<string>& vtSetInfo);

	/*
	* Author :johnson
	* func: taf.xxxServer =>> xxxServer
	*/
	static string getServerName(string sModuleName);
private:

    /**
     *
     * @param head
     * @param body
     * @param msg
     * @param bFromClient
     */
    void submit(StatMicMsgHead& head, StatMicMsgBody& body, bool bFromClient);

    /**
     * �ϱ�ģ��������Ϣ  Mic = module interval call
     * @param msg
     * @param bFromClient  �Ƿ�Ϊ�ͻ����ϱ�
     * @return int
     */
    int reportMicMsg(MapStatMicMsg &msg, bool bFromClient);

    /**
     * �ϱ�������Ϣ  Prop = property
     * @return int
     */
    int reportPropMsg();

    /**
     * stat ����
     */
    int reportSampleMsg();


    //�ϲ�����MicMsg
    void addMicMsg(MapStatMicMsg & old,MapStatMicMsg & add);


private:
    time_t              _tTime;

    int                 _iReportInterval;

    int                 _iMaxReportSize;

    bool                _bTerminate;

    string              _strModuleName;

    string              _strSetName; // �����ϱ�����ʾ����set���������ϱ�����ʾ����set��

    string              _strSetArea; //

    string              _strSetID; //

    string              _strIp;

    int                 _iSampleRate; //����ģ������ʱ��ͼ�Ĳ�������

    unsigned int        _iMaxSampleCount; //1����������������

    StatFPrx            _statPrx;

    MapStatMicMsg       _mStatMicMsgClient;

    MapStatMicMsg       _mStatMicMsgServer;

    MMapStatSampleMsg   _mmStatSampleMsg;

    vector<int>         _vTimePoint;

    PropertyFPrx        _propertyPrx;

    map<string, PropertyReportPtr>          _mStatPropMsg;
private:

    size_t _iEpollNum;
    vector<stat_queue*>   vStatMsg;

private:
    //map<string,*objectProxy>  _mObjectProxy; //����stat���ݵ�objectProxy��ҵ��ʹ�á�

};
///////////////////////////////////////////////////////////
}

#endif
