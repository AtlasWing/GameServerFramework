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
 * 网络传输基类，主要提供send/recv接口
 */
class Transceiver
{
public:
    /**
     * 连接状态
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
     * 构造函数
     * @param ep
     * @param fd
     */
    Transceiver(AdapterProxy * pAdapterProxy,const EndpointInfo &ep);

    /**
     *
     *析构函数
     */
    virtual ~Transceiver();

    /**
     * 初始化FIFO，MMAP
     */
    virtual void init(){};

    /**
     * 检查连接是否超时
     */
    void checkTimeout();

    /**
     * 重新创建连接
     */
    void reconnect();

    /**
     * 创建连接，初始化
     */
    void connect();

     /**
     * 获取文件描述符
     * @return int
     */
    virtual int fd() const
    {
        return _iFd;
    }

	/**
	 * 获取读文件描述符
	 */
	virtual int getrfd() const
    {
        return _iFd;
    }

	/**
	 * 获取写文件描述符
	 */
	virtual int getwfd() const
    {
        return _iFd;
    }

    /**
     * 关闭连接
     */
    virtual void close();

    /**
     * 是否有效
     * @return bool
     */
	bool isValid() const
    {
        return (_iFd != -1);
    }

    /**
     * 端口信息
     * @return const EndpointInfo&
     */
	EndpointInfo& getEndpointInfo() { return _ep;}

    /**
     * 获取connect所属的adapter
     * @return AdapterProxy
     */
    AdapterProxy * getAdapterProxy()
    {
        return _pAdapterProxy;
    }

public:
    /**
     * 处理请求，判断Send BufferCache是否有完整的包
     * @return int
     */
	virtual int doRequest();

    /**
     * 处理返回，判断Recv BufferCache是否有完整的包
     * @param done
     * @return int
     */
	virtual int doResponse(list<ResponsePacket>& done) = 0;

    /**
     * 往fd里面发送数据
     * 如果fd缓冲区已满,返回错误
     * 如果数据发送一半，缓冲区满了,返回成功
     */
    int sendRequest(const char * pData,size_t iSize);

public:
	/**
	 * 判断是否已经连接到服务端
	 */
	bool hasConnected() { return isValid() && (_connStatus == eConnected); }

	/**
	 * 设置当前连接态
	 */
	void setConnected();

    /**
     * 正在Bus连接
     *
     * @return bool
     */
    bool isConnecting() { return isValid() && (_connStatus == eConnecting); }

    /**
     * 设置当前连接态
     */
    void setBusConnecting() { _connStatus = eBusConnecting; }

    /**
     * 正在连接
     *
     * @return bool
     */
    bool isBusConnecting() { return isValid() && (_connStatus == eBusConnecting); }

    /**
     * 设置连接失败
     *
     */
    void setConnectFailed() { _connStatus = eUnconnected; }

public:

    /**
     * 网络发送接口
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag) = 0;

    /**
     * 网络接收接口
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

	ConnectStatus _connStatus;	    //表示是否已经连接到服务端

    //连接的超时时间
    int64_t     _conTimeoutTime;

	FDInfo _fdInfo;

};

//////////////////////////////////////////////////////////
/**
 * TCP 传输实现
 */
class TcpTransceiver : public Transceiver
{
public:
    /**
     * 构造函数
     * @param ep
     * @param fd
     */
    TcpTransceiver(AdapterProxy * pAdapterProxy, const EndpointInfo &ep);

    /**
     * TCP 发送实现
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag);

    /**
     * TCP 接收实现
     * @param buf
     * @param len
     * @param flag
     *
     * @return int
     */
    virtual int recv(void* buf, uint32_t len, uint32_t flag);

    /**
     * 处理返回，判断Recv BufferCache是否有完整的包
     * @param done
     * @return int, =1,表示有数据就包
     */
	virtual int doResponse(list<ResponsePacket>& done);
};
//////////////////////////////////////////////////////////
/**
 * UDP 传输实现
 */
class UdpTransceiver : public Transceiver
{
public:
	enum
	{
        DEFAULT_RECV_BUFFERSIZE = 64*1024	   /*缺省数据接收buffer的大小*/
	};

public:
    /**
     * 构造函数
     * @param ep
     * @param fd
     */
    UdpTransceiver(AdapterProxy * pAdapterProxy, const EndpointInfo &ep);

	~UdpTransceiver();
    /**
     * UDP 发送实现
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag);

    /**
     * UDP 接收实现
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int recv(void* buf, uint32_t len, uint32_t flag);

    /**
     * 处理返回，判断Recv BufferCache是否有完整的包
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
