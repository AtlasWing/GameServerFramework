#ifndef __TAF_TRANSCEIVER_H_
#define __TAF_TRANSCEIVER_H_

#include "servant/EndpointInfo.h"
#include "servant/NetworkUtil.h"
#include "servant/CommunicatorEpoll.h"
#include <list>

using namespace std;

namespace taf
{
class AdapterProxy;
/**
 * ���紫����࣬��Ҫ�ṩsend/recv�ӿ�
 */
class Transceiver
{
public:
    /**
     * ����״̬
     */
    enum ConnectStatus
    {
        eUnconnected,
        eConnecting,
        eConnected,
        eBusConnecting,
    };

    enum ReturnStatus
    {
        eRetError = -1,
        eRetOk =0,
        eRetFull=1,
    };

    /**
     * ���캯��
     * @param ep
     * @param fd
     */
    Transceiver(AdapterProxy * pAdapterProxy,const EndpointInfo &ep);

    /**
     *
     *��������
     */
    virtual ~Transceiver();

    /**
     * ��ʼ��FIFO��MMAP
     */
    virtual void init(){};

    /**
     * ��������Ƿ�ʱ
     */
    void checkTimeout();

    /**
     * ���´�������
     */
    void reconnect();

    /**
     * �������ӣ���ʼ��
     */
    void connect();

     /**
     * ��ȡ�ļ�������
     * @return int
     */
    virtual int fd() const
    {
        return _iFd;
    }

	/**
	 * ��ȡ���ļ�������
	 */
	virtual int getrfd() const
    {
        return _iFd;
    }

	/**
	 * ��ȡд�ļ�������
	 */
	virtual int getwfd() const
    {
        return _iFd;
    }

    /**
     * �ر�����
     */
    virtual void close();

    /**
     * �Ƿ���Ч
     * @return bool
     */
	bool isValid() const
    {
        return (_iFd != -1);
    }

    /**
     * �˿���Ϣ
     * @return const EndpointInfo&
     */
	EndpointInfo& getEndpointInfo() { return _ep;}

    /**
     * ��ȡconnect������adapter
     * @return AdapterProxy
     */
    AdapterProxy * getAdapterProxy()
    {
        return _pAdapterProxy;
    }

public:
    /**
     * ���������ж�Send BufferCache�Ƿ��������İ�
     * @return int
     */
	virtual int doRequest();

    /**
     * �����أ��ж�Recv BufferCache�Ƿ��������İ�
     * @param done
     * @return int
     */
	virtual int doResponse(list<ResponsePacket>& done) = 0;

    /**
     * ��fd���淢������
     * ���fd����������,���ش���
     * ������ݷ���һ�룬����������,���سɹ�
     */
    int sendRequest(const char * pData,size_t iSize);

public:
	/**
	 * �ж��Ƿ��Ѿ����ӵ������
	 */
	bool hasConnected() { return isValid() && (_connStatus == eConnected); }

	/**
	 * ���õ�ǰ����̬
	 */
	void setConnected();

    /**
     * ����Bus����
     *
     * @return bool
     */
    bool isConnecting() { return isValid() && (_connStatus == eConnecting); }

    /**
     * ���õ�ǰ����̬
     */
    void setBusConnecting() { _connStatus = eBusConnecting; }

    /**
     * ��������
     *
     * @return bool
     */
    bool isBusConnecting() { return isValid() && (_connStatus == eBusConnecting); }

    /**
     * ��������ʧ��
     *
     */
    void setConnectFailed() { _connStatus = eUnconnected; }

public:

    /**
     * ���緢�ͽӿ�
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag) = 0;

    /**
     * ������սӿ�
     * @param buf
     * @param len
     * @param flag
     *
     * @return int
     */
    virtual int recv(void* buf, uint32_t len, uint32_t flag) = 0;

protected:
    int _iFd;

	EndpointInfo _ep;

    AdapterProxy * _pAdapterProxy;

    string _sendBuffer;

    string _recvBuffer;

	ConnectStatus _connStatus;	    //��ʾ�Ƿ��Ѿ����ӵ������

    //���ӵĳ�ʱʱ��
    int64_t     _conTimeoutTime;

	FDInfo _fdInfo;

};

//////////////////////////////////////////////////////////
/**
 * TCP ����ʵ��
 */
class TcpTransceiver : public Transceiver
{
public:
    /**
     * ���캯��
     * @param ep
     * @param fd
     */
    TcpTransceiver(AdapterProxy * pAdapterProxy, const EndpointInfo &ep);

    /**
     * TCP ����ʵ��
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag);

    /**
     * TCP ����ʵ��
     * @param buf
     * @param len
     * @param flag
     *
     * @return int
     */
    virtual int recv(void* buf, uint32_t len, uint32_t flag);

    /**
     * �����أ��ж�Recv BufferCache�Ƿ��������İ�
     * @param done
     * @return int, =1,��ʾ�����ݾͰ�
     */
	virtual int doResponse(list<ResponsePacket>& done);
};
//////////////////////////////////////////////////////////
/**
 * UDP ����ʵ��
 */
class UdpTransceiver : public Transceiver
{
public:
	enum
	{
        DEFAULT_RECV_BUFFERSIZE = 64*1024	   /*ȱʡ���ݽ���buffer�Ĵ�С*/
	};

public:
    /**
     * ���캯��
     * @param ep
     * @param fd
     */
    UdpTransceiver(AdapterProxy * pAdapterProxy, const EndpointInfo &ep);

	~UdpTransceiver();
    /**
     * UDP ����ʵ��
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag);

    /**
     * UDP ����ʵ��
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int recv(void* buf, uint32_t len, uint32_t flag);

    /**
     * �����أ��ж�Recv BufferCache�Ƿ��������İ�
     * @param done
     * @return int
     */
	virtual int doResponse(list<ResponsePacket>& done);

private:
	char				*_pRecvBuffer;

};
//////////////////////////////////////////////////////////

}
#endif
