#ifndef _TC_CLIENTSOCKET_H__
#define _TC_CLIENTSOCKET_H__

#include "util/tc_socket.h"
//#include "util/tc_http.h"

namespace taf
{
/////////////////////////////////////////////////
/**
 * @file  tc_clientsocket.h
 * @brief �ͻ��˷����հ���.
 *
 * @author  jarodruan@tencent.com
 */
/////////////////////////////////////////////////
/**
*  @brief ����endpoint�쳣��
*/
struct TC_EndpointParse_Exception : public TC_Exception
{
    TC_EndpointParse_Exception(const string &buffer) : TC_Exception(buffer){};
    ~TC_EndpointParse_Exception() throw() {};
};

/**
 *  @brief ��ʾһ������˿�,֧�����¸�ʽ:
 *
 * 1:tcp -h 127.0.0.1 -p 2345 -t 10000
 *
 * 2:tcp -h /tmp/sock.sock -p 0 -t 10000
 *
 * 3:udp -h 127.0.0.1 -p 2345 -t 10000
 *
 * -p 0:��ʾ�����׽���
 *
 * -q 0:��ʾqos��dscpֵ
 *
 * ��ʱ-h��ʾ���ļ�·��
 */
class TC_Endpoint
{
public:
    /**
     *
     */
    TC_Endpoint();

    /**
     * @brief ���캯��
     * @param host
     * @param port
     * @param timeout, ��ʱʱ��, ����
     * @param type, SOCK_STREAM��SOCK_DGRAM
     */
    TC_Endpoint(const string& host, int port, int timeout, int istcp = true, int grid = 0,int qos = 0)
    {
        _host    = host;
        _port    = port;
        _timeout = timeout;
        _istcp   = istcp;
        _grid    = grid;
        _qos     = qos;
    }

    /**
     * @brief ���ַ�������������
     * @param desc
     */
    TC_Endpoint(const string& desc)
    {
        parse(desc);
    }

    /**
     * @brief ��������
     * @param l
     */
    TC_Endpoint(const TC_Endpoint& l)
    {
        _host   = l._host;
        _port   = l._port;
        _timeout= l._timeout;
        _istcp  = l._istcp;
        _grid   = l._grid;
        _qos    = l._qos;
    }

    /**
     * @brief ��ֵ����
     * @param l
     *
     * @return TC_Endpoint&
     */
    TC_Endpoint& operator = (const TC_Endpoint& l)
    {
        if(this != &l)
        {
            _host   = l._host;
            _port   = l._port;
            _timeout= l._timeout;
            _istcp  = l._istcp;
            _grid   = l._grid;
            _qos    = l._qos;
        }

        return *this;
    }

    /**
     * ==
     * @param l
     *
     * @return bool
     */
    bool operator == (const TC_Endpoint& l)
    {
        return (_host == l._host && _port == l._port && _timeout == l._timeout && _istcp == l._istcp && _grid == l._grid && _qos == l._qos);
    }

    /**
     * @brief ����ip
     * @param str
     */
    void setHost(const string& host)    { _host = host; }

    /**
     * @brief ��ȡip
     *
     * @return const string&
     */
    string getHost() const              { return _host; }

    /**
     * @brief ���ö˿�
     * @param port
     */
    void setPort(int port)              { _port = port; }

    /**
     * @brief ��ȡ�˿�
     *
     * @return int
     */
    int getPort() const                 { return _port; }

    /**
     * @brief ���ó�ʱʱ��
     * @param timeout
     */
    void setTimeout(int timeout)        { _timeout = timeout; }

    /**
     * @brief ��ȡ��ʱʱ��
     *
     * @return int
     */
    int getTimeout() const              { return _timeout; }

    /**
     * @brief  �Ƿ���TCP, ������ΪUDP
     *
     * @return bool
     */
    bool isTcp() const                  { return _istcp; }

    /**
     * @brief ����ΪTCP��UDP
     * @param bTcp
     */
    void setTcp(bool bTcp)              { _istcp = bTcp; }

    /**
     * @brief ��ȡ·��״̬
     * @param grid
     */
    int getGrid() const                 { return _grid; }

    /**
     * @brief ����·��״̬
     * @param grid
     */
    void setGrid(int grid)              { _grid = grid; }

    /**
     * @brief ��ȡ·��״̬
     * @param grid
     */
    int getQos() const                 { return _qos; }

    /**
     * @brief ����·��״̬
     * @param grid
     */
    void setQos(int qos)              { _qos = qos; }

    /**
     * @brief �Ƿ��Ǳ����׽���
     *
     * @return bool
     */
    bool isUnixLocal() const            { return _port == 0; }

    /**
     * @brief �ַ�������
     *
     * @return string
     */
    string toString()
    {
        ostringstream os;
        os << (isTcp()?"tcp" : "udp") << " -h " << _host << " -p " << _port << " -t " << _timeout;
        if (_grid != 0) os << " -g " << _grid;
        if (_qos != 0) os << " -q " << _qos;
        return os.str();
    }

    /**
     * @brief  �ַ�����ʽ�Ķ˿�
	 * tcp:SOCK_STREAM
	 *
	 * udp:SOCK_DGRAM
	 *
	 * -h: ip
	 *
	 * -p: �˿�
	 *
	 * -t: ��ʱʱ��, ����
	 *
	 * -p �� -t����ʡ��, -tĬ��10s
	 *
	 * tcp -h 127.0.0.1 -p 2345 -t 10000
	 *
     * @param desc
     */
    void parse(const string &desc);

protected:
    /**
     * ip
     */
    std::string _host;

    /**
     * �˿�
     */
    int         _port;

    /**
     * ��ʱʱ��
     */
    int         _timeout;

    /**
     * ����
     */
    int         _istcp;

    /**
     * ·��״̬
     */
    int         _grid;

    /**
     *  ����Qos��dscpֵ
     */
    int         _qos;
};

/*************************************TC_ClientSocket**************************************/

/**
* @brief �ͻ���socket��ز�������
*/
class TC_ClientSocket
{
public:

    /**
    *  @brief ���캯��
	 */
	TC_ClientSocket() : _port(0),_timeout(3000) {}

    /**
     * @brief ��������
     */
	virtual ~TC_ClientSocket(){}

    /**
    * @brief ���캯��
    * @param sIP      ������IP
	* @param iPort    �˿�, portΪ0ʱ:��ʾ�����׽��ִ�ʱipΪ�ļ�·��
    * @param iTimeout ��ʱʱ��, ����
	*/
	TC_ClientSocket(const string &sIp, int iPort, int iTimeout) { init(sIp, iPort, iTimeout); }

    /**
    * @brief ��ʼ������
    * @param sIP      ������IP
	* @param iPort    �˿�, portΪ0ʱ:��ʾ�����׽��ִ�ʱipΪ�ļ�·��
    * @param iTimeout ��ʱʱ��, ����
	*/
	void init(const string &sIp, int iPort, int iTimeout)
    {
        _socket.close();
        _ip         = sIp;
        _port       = iPort;
        _timeout    = iTimeout;
    }

    /**
    * @brief ���͵�������
    * @param sSendBuffer ����buffer
    * @param iSendLen    ����buffer�ĳ���
    * @return            int 0 �ɹ�,<0 ʧ��
    */
    virtual int send(const char *sSendBuffer, size_t iSendLen) = 0;

    /**
    * @brief �ӷ��������ز�����iRecvLen���ֽ�
    * @param sRecvBuffer ����buffer
	* @param iRecvLen    ָ�����ն��ٸ��ַ��ŷ���,����������ݵĳ���
    * @return            int 0 �ɹ�,<0 ʧ��
    */
    virtual int recv(char *sRecvBuffer, size_t &iRecvLen) = 0;

    /**
    * @brief  ���巢�͵Ĵ���
    */
    enum
    {
        EM_SUCCESS  = 0,      	/** EM_SUCCESS:���ͳɹ�*/
		EM_SEND     = -1,		/** EM_SEND:���ʹ���*/
		EM_SELECT   = -2,	    /** EM_SELECT:select ����*/
		EM_TIMEOUT  = -3,		/** EM_TIMEOUT:select��ʱ*/
		EM_RECV     = -4,		/** EM_RECV: ���ܴ���*/
		EM_CLOSE    = -5,		/**EM_CLOSE: �����������ر�*/
		EM_CONNECT  = -6,		/** EM_CONNECT : ����������ʧ��*/
		EM_SOCKET   = -7		/**EM_SOCKET : SOCKET��ʼ��ʧ��*/
    };

protected:
    /**
     * �׽��־��
     */
	TC_Socket 	_socket;

    /**
     * ip���ļ�·��
     */
	string		_ip;

    /**
     * �˿ڻ�-1:��ʾ�Ǳ����׽���
     */
	int     	_port;

    /**
     * ��ʱʱ��, ����
     */
	int			_timeout;
};

/**
 * @brief TCP�ͻ���Socket
 * ���߳�ʹ�õ�ʱ�򣬲��ö��߳�ͬʱsend/recv��С�Ĵ�����
 */
class TC_TCPClient : public TC_ClientSocket
{
public:
    /**
    * @brief  ���캯��
	 */
	TC_TCPClient(){}

    /**
    * @brief  ���캯��
    * @param sIp       ������Ip
    * @param iPort     �˿�
    * @param iTimeout  ��ʱʱ��, ����
	*/
	TC_TCPClient(const string &sIp, int iPort, int iTimeout) : TC_ClientSocket(sIp, iPort, iTimeout)
    {
    }

    /**
    * @brief  ���͵�������
    * @param sSendBuffer  ����buffer
    * @param iSendLen     ����buffer�ĳ���
    * @return             int 0 �ɹ�,<0 ʧ��
    */
    int send(const char *sSendBuffer, size_t iSendLen);

    /**
    * @brief  �ӷ��������ز�����iRecvLen���ֽ�
    * @param sRecvBuffer ����buffer
	* @param iRecvLen    ָ�����ն��ٸ��ַ��ŷ���,����������ݵĳ���
    * @return            int 0 �ɹ�,<0 ʧ��
    */
    int recv(char *sRecvBuffer, size_t &iRecvLen);

    /**
	*  @brief �ӷ�����ֱ��������(ע������Ƿ��������صĽ�����,
	*         �������м�ķ��� ) ֻ����ͬ������
    * @param sRecvBuffer ����buffer, �����ָ���
    * @param sSep        �ָ���
    * @return            int 0 �ɹ�,<0 ʧ��
    */
    int recvBySep(string &sRecvBuffer, const string &sSep);

    /**
     * @brief ���յ��������ر�����Ϊֹ
     * @param recvBuffer
     *
     * @return int 0 �ɹ�,<0 ʧ��
     */
    int recvAll(string &sRecvBuffer);

    /**
     * @brief  �ӷ���������iRecvLen���ֽ�
     * @param sRecvBuffer, sRecvBuffer��buffer���ȱ�����ڵ���iRecvLen
	 * @param iRecvLen
     * @return int 0 �ɹ�,<0 ʧ��
     */
    int recvLength(char *sRecvBuffer, size_t iRecvLen);

    /**
    * @brief  ���͵�������, �ӷ��������ز�����iRecvLen���ֽ�
    * @param sSendBuffer  ����buffer
    * @param iSendLen     ����buffer�ĳ���
    * @param sRecvBuffer  ����buffer
	* @param iRecvLen     ����buffer�ĳ���ָ��[in/out],
	*   			      ����ʱ��ʾ����buffer�Ĵ�С,����ʱ��ʾ�����˶��ٸ��ַ�
    * @return             int 0 �ɹ�,<0 ʧ��
    */
	int sendRecv(const char* sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen);

    /**
    * @brief  ���͵�������, ���ȴ�������ֱ����β�ַ�, ������β�ַ�
	* sSep�����Ƿ��������صĽ�����,�������м�ķ��ţ�ֻ����ͬ������
	* (һ�ν���һ�����ȵ�buffer,���ĩβ��sSep�򷵻�,
	* ��������ȴ�����)
    *
    * @param sSendBuffer  ����buffer
    * @param iSendLen     ����buffer�ĳ���
    * @param sRecvBuffer  ����buffer
    * @param sSep         ��β�ַ�
    * @return             int 0 �ɹ�,<0 ʧ��
    */
	int sendRecvBySep(const char* sSendBuffer, size_t iSendLen, string &sRecvBuffer, const string &sSep);

    /**
    * @brief  ���͵�������, ���ȴ�������ֱ����β�ַ�(\r\n), ����\r\n
    * ע������Ƿ��������صĽ�����,�������м�ķ���
	* ֻ����ͬ������
    * (һ�ν���һ�����ȵ�buffer,���ĩβ��\r\n�򷵻�,��������ȴ�����)
    *
    * @param sSendBuffer  ����buffer
    * @param iSendLen     ����buffer�ĳ���
    * @param sRecvBuffer  ����buffer
    * @param sSep         ��β�ַ�
    * @return             int 0 �ɹ�,<0 ʧ��
    */
	int sendRecvLine(const char* sSendBuffer, size_t iSendLen, string &sRecvBuffer);

    /**
     * @brief  ���͵�������, ����ֱ���������ر�����Ϊֹ
     * ��ʱ�������ر����Ӳ���Ϊ����
     * @param sSendBuffer
     * @param iSendLen
     * @param sRecvBuffer
     *
     * @return int
     */
    int sendRecvAll(const char* sSendBuffer, size_t iSendLen, string &sRecvBuffer);

protected:
    /**
     * @brief  ��ȡsocket
     *
     * @return int
     */
    int checkSocket();
};

/*************************************TC_TCPClient**************************************/
 /**
  * @brief  ���߳�ʹ�õ�ʱ�򣬲��ö��߳�ͬʱsend/recv��С�Ĵ���
  */
class TC_UDPClient : public TC_ClientSocket
{
public:
    /**
    * @brief  ���캯��
	 */
	TC_UDPClient(){};

    /**
    * @brief  ���캯��
    * @param sIp       ������IP
    * @param iPort     �˿�
    * @param iTimeout  ��ʱʱ��, ����
	*/
	TC_UDPClient(const string &sIp, int iPort, int iTimeout) : TC_ClientSocket(sIp, iPort, iTimeout)
    {
    }

    /**
     * @brief  ��������
     * @param sSendBuffer ����buffer
     * @param iSendLen    ����buffer�ĳ���
     *
     * @return            int 0 �ɹ�,<0 ʧ��
     */
    int send(const char *sSendBuffer, size_t iSendLen);

    /**
     * @brief  ��������
     * @param sRecvBuffer  ����buffer
	 * @param iRecvLen     ����/����ֶ�
     * @return             int 0 �ɹ�,<0 ʧ��
     */
    int recv(char *sRecvBuffer, size_t &iRecvLen);

    /**
     * @brief  ��������, ������Զ�̵Ķ˿ں�ip
     * @param sRecvBuffer ����buffer
     * @param iRecvLen    ����/����ֶ�
     * @param sRemoteIp   ����ֶ�, Զ�̵�ip
     * @param iRemotePort ����ֶ�, Զ�̶˿�
     *
     * @return int 0 �ɹ�,<0 ʧ��
     */
    int recv(char *sRecvBuffer, size_t &iRecvLen, string &sRemoteIp, uint16_t &iRemotePort);

    /**
     * @brief  ���Ͳ���������
     * @param sSendBuffer ����buffer
     * @param iSendLen    ����buffer�ĳ���
     * @param sRecvBuffer ����/����ֶ�
     * @param iRecvLen    ����/����ֶ�
     *
     * @return int 0 �ɹ�,<0 ʧ��
     */
    int sendRecv(const char *sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen);

    /**
     * @brief  ���Ͳ���������, ͬʱ��ȡԶ�̵�ip�Ͷ˿�
     * @param sSendBuffer  ����buffer
     * @param iSendLen     ����buffer�ĳ���
     * @param sRecvBuffer  ����/����ֶ�
     * @param iRecvLen     ����/����ֶ�
     * @param sRemoteIp    ����ֶ�, Զ�̵�ip
     * @param iRemotePort  ����ֶ�, Զ�̶˿�
     *
     * @return int 0 �ɹ�,<0 ʧ��
     */
    int sendRecv(const char *sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen, string &sRemoteIp, uint16_t &iRemotePort);

protected:
    /**
     * @brief  ��ȡsocket
     *
     * @return TC_Socket&
     */
    int checkSocket();
};

}

#endif
