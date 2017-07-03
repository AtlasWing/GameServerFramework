#ifndef __TAF_ENDPOINT_INFO_H_
#define __TAF_ENDPOINT_INFO_H_

#include "servant/Global.h"
#include "servant/NetworkUtil.h"

using namespace std;

namespace taf
{
/**
 * ��ַ��ϢIP:Port
 */
class EndpointInfo
{
public:
	enum EType { TCP = 1, UDP = 2, };

    /**
     * ���캯��
     */
	EndpointInfo();

    /**
     * ���캯��
     * @param host
     * @param port
     * @param type
     */
	EndpointInfo(const string& host, uint16_t port, EndpointInfo::EType type, int32_t grid,const string & setDivision,int qos);

    /**
     * ��ַ���ַ�������,����set��Ϣ
     *
     * @return string
     */
     const string& descNoSetInfo() const;

    /**
     * ��ַ���ַ�������
     *
     * @return string
     */
	const string & desc() const
    {
        return _desc;
    }

	const string & compareDesc() const
    {
        return _sCompareDesc;
    }

    /**
     * ��ϸ��ַ�ַ�������
     * 
     * @return string 
     */
	const string& fulldesc() const;

    /**
     * ��ȡ������
     *
     * @return const string&
     */
	string host() const;

    /**
     * ��ȡ�˿ں�
     *
     * @return uint16_t
     */
	uint16_t port() const;

    /**
     * ��ȡ·��״̬
     * @return int32_t
     */
	int32_t grid() const;

    /*
     * ��ȡqos��descpֵ
     */
    int32_t qos() const {return _qos;}

    /**
     * ��ȡ������ַ
     *
     * @return const struct sockaddr_in&
     */
	const struct sockaddr_in& addr() const;

    /**
     * ���ض˿�����
     *
     * @return EndpointInfo::EType
     */
	EndpointInfo::EType type() const;

    /**
    *����set������Ϣ
    *
    *@return void
    */
    void setDivision(const string& sSetDivision);

    /**
    *����set������Ϣ
    *
    *@return string
    */
    const string& setDivision() const;

#if 0
    /**
     * ��ֵ����
     */
    EndpointInfo& operator = (const EndpointInfo& r);
#endif

    /**
     * ����
     * @param r
     *
     * @return bool
     */
    bool operator == (const EndpointInfo& r) const;

    /**
    *����,set��Ϣ������Ƚ�
    *@param r
    *
    *@return bool
    */
    bool equalNoSetInfo(const EndpointInfo& r) const;

    /**
     * С��
     * @param r
     *
     * @return bool
     */
    bool operator < (const EndpointInfo& r) const;

protected:

    /**
     * ��ַ���ַ�������
     * @param bWithSetInfo,��ʶ
     * @return string
     */
	string createDesc() const;

    /**
     * ��ϸ��ַ�ַ�������
     * 
     * @return string 
     */
	string createCompareDesc();

private:
    /**
     * ��ַIP
     */
    char _host[16];

    /**
     * �˿ں�
     */
    uint16_t _port;

    /**
     * ·��״̬
     */
    int32_t _grid;

    /*
     * qos��dscpֵ
     */

    int32_t _qos;

    /**
     * ����
     */
	EndpointInfo::EType _type;

   /**
    *set������Ϣ
    */
    string _setDivision;

    /**
     * ��ַ
     */
	struct sockaddr_in _addr;

    string _sCompareDesc;

    /**
     * ��ַ����
     */
    string _desc;

    #if 0
    /**
     * ��ַȫ����
     */
    string _fulldesc;


    /**
    *����set��Ϣ�ĵ�ַ����
    */
    string _descNoSetInfo;
    #endif
};
/////////////////////////////////////////////////////////////////////////////
}
#endif
