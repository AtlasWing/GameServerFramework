#ifndef __TC_SOCKET_H
#define __TC_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <string>
#include <sys/un.h>
#include "util/tc_ex.h"
using namespace std;

namespace taf
{
/////////////////////////////////////////////////
/** 
* @file tc_socket.h 
* @brief  socket��װ��. 
*  
* @author  jarodruan@tencent.com
*/
/////////////////////////////////////////////////

/**
* @brief socket�쳣��
*/
struct TC_Socket_Exception : public TC_Exception
{
    TC_Socket_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Socket_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_Socket_Exception() throw() {};
};

/**
 * @brief ���������쳣
 */
struct TC_SocketConnect_Exception : public TC_Socket_Exception
{
    TC_SocketConnect_Exception(const string &buffer) : TC_Socket_Exception(buffer){};
    TC_SocketConnect_Exception(const string &buffer, int err) : TC_Socket_Exception(buffer, err){};
    ~TC_SocketConnect_Exception() throw() {};
};


/**
* @brief  Socket��, ��װ��socket�Ļ�������
*/
class TC_Socket
{
public:
	/**
     * @brief  ���캯��
	 */
	TC_Socket();

    /**
     * @brief  ����
     */
    virtual ~TC_Socket();

    /**
	 * @brief  ��ʼ��. 
	 *  
     * @param fd      socket���
	 * @param bOwner  �Ƿ�ӵ��socket
	 * @param iDomain sokectЭ���壬ȱʡΪAF_INET������TCP/IPЭ���� 
     */
    void init(int fd, bool bOwner, int iDomain = AF_INET);

    /**
	 * @brief �����Ƿ�ӵ�и�fd. 
	 *  
     * @param bOwner ture��ʾӵ�У���������Ϊfalse
     */
    void setOwner(bool bOwner)  { _bOwner = bOwner; }

    /**
	 * @brief �����׽������� 
	 *  
	 * @param iDomain Ҫ���õ��׽��ֵ����� 
	 *  			 AF_INET��TCP/IPЭ����
	 *  			 AF_LOCAL��
     */
    void setDomain(int iDomain) { _iDomain = iDomain; }

    /**
	* @brief  ����socket, ����Ѿ�������ǰ��socket, ���ͷŵ�, 
	*   	  �����µ�.
	*  
    * @param iDomain      socket��ʽSOCK_STREAM|SOCK_DGRAM
    * @param iSocketType  socket���ͣ�ȱʡAF_INET��TCP/IP��
    * @throws             TC_Socket_Exception
    * @return
    */
	void createSocket(int iSocketType = SOCK_STREAM, int iDomain = AF_INET);

    /**
	* @brief ��ȡsocket���. 
	*  
    * @return  socket���
    */
	int getfd() const { return _sock; }

    /**
	* @brief  socket�Ƿ���Ч. 
	*  
    * @return true��ʶ��Ч�����򷵻�false
    */
	bool isValid() const { return _sock != INVALID_SOCKET; }

    /**
	* @brief  �ر�socket. 
	*  
    * @return void
    */
    void close();

    /**
	* @brief  ��ȡ�Ե��ip�Ͷ˿�,��AF_INET��socket��Ч. 
	*  
    * @param sPeerAddress  �Ե��ip��ַ
    * @param iPeerPort     �Ե�Ķ˿ڵ�ַ
    * @throws              TC_Socket_Exception
    * @return
    */
    void getPeerName(string &sPeerAddress, uint16_t &iPeerPort);

    /**
	* @brief  ��ȡ�Ե��ip�Ͷ˿�,��AF_LOCAL��socket��Ч. 
	*  
    * @param sPathName  �ļ�·��
    * @throws           TC_Socket_Exception
    * @return
    */
    void getPeerName(string &sPathName);

    /**
	* @brief  ��ȡ�Լ���ip�Ͷ˿�,��AF_INET��socket��Ч. 
	*  
    * @param sSockAddress  ip��ַ
    * @param iSockPort     �˿ڵ�ַ
    * @throws              TC_Socket_Exception
    * @return
    */
    void getSockName(string &sSockAddress, uint16_t &iSockPort);

    /**
	 * @brief  ��ȡsocket�ļ�·��,��AF_LOCAL��socket��Ч. 
	 *  
     * @param sPathName
     * @param TC_Socket_Exception
     */
    void getSockName(string &sPathName);

    /**
	* @brief  �޸�socketѡ��. 
	*  
    * @param opt       ѡ������
    * @param pvOptVal  ѡ��ֵָ��
    * @param optLen    pvOptVal��Ӧ�ĳ���
    * @param level     socket�������, Ĭ����socket��
    * @return int
    */
	int setSockOpt(int opt, const void *pvOptVal, socklen_t optLen, int level = SOL_SOCKET);

    /**
	* @brief  ��ȡsocketѡ��ֵ. 
	*  
    * @param opt       ѡ������
    * @param pvOptVal  ���, ѡ��ֵָ��
    * @param optLen    ����pvOptValָ��Ļ���ĳ���
    * @param level     socket�������, Ĭ����socket��
    * @return          socketѡ��ֵ
    */
	int getSockOpt(int opt, void *pvOptVal, socklen_t &optLen, int level = SOL_SOCKET);

    /**
	* @brief  accept. 
	*  
    * @param tcSock       �ͻ���socket�ṹ
    * @param pstSockAddr  �ͻ��˵�ַ
    * @param iSockLen     pstSockAddr����
    * @return             int : > 0 ,�ͻ���socket; <0, ����
    */
	int accept(TC_Socket &tcSock, struct sockaddr *pstSockAddr, socklen_t &iSockLen);

    /**
	* @brief ��,��AF_INET��socket��Ч. 
	*  
    * @param port          �˿�
    * @param sServerAddr   ��������ַ
    * @throws              TC_Socket_Exception
    * @return 
    */
	void bind(const string &sServerAddr, int port);

    /**
	 * @brief  �����׽���,��AF_LOCAL��socket��Ч. 
	 *  
     * @param sPathName
     */
    void bind(const char *sPathName);

    /**
	* @brief  ������������,��AF_INET��socket��Ч(ͬ������). 
	*  
    * @param sServerAddr  ip��ַ
    * @param port         �˿�
    * @throws             TC_Socket_Exception
    * @return
    */
	void connect(const string &sServerAddr, uint16_t port);

    /**
	 * @brief ���ӱ����׽���,��AF_LOCAL��socket��Ч(ͬ������). 
	 *  
     * @param sPathName
     * @throws TC_Socket_Exception
     */
    void connect(const char *sPathName);

    /**
	 * @brief �������ӣ�����ʧ�ܵ�״̬��ͨ���쳣����, 
	 *  	  ͨ��connect�ķ���ֵ,���첽���ӵ�ʱ����Ҫ
     * @param sServerAddr  ip��ַ
     * @param port         �˿�
	 * @throws             TC_Socket_Exception: 
	 *  				   ����������ͨ���쳣����(����),�����ַ����
     * @return int
     */
    int connectNoThrow(const string &sServerAddr, uint16_t port);


    /**
	 * @brief �������ӣ�����ʧ�ܵ�״̬��ͨ���쳣����, 
	 *  	  ͨ��connect�ķ���ֵ,���첽���ӵ�ʱ����Ҫ
     * @param addr socketֱ�ӿ��õĵ�ַ
	 * @throws TC_Socket_Exception  
	 *  	  ����������ͨ���쳣����(����),�����ַ����
     * @return int
     */
    int connectNoThrow(struct sockaddr* addr);

    /**
	 * @brief �������ӣ�����ʧ�ܵ�״̬��ͨ���쳣����, 
	 *  	  ͨ��connect�ķ���ֵ,���첽���ӵ�ʱ����Ҫ
     * @param sPathName
     * @throws TC_Socket_Exception:����������ͨ���쳣����(����),�����ַ����
     * @return int
     */
    int connectNoThrow(const char *sPathName);

    /**
	* @brief ��socket�ϼ���. 
	*  
    * @param connBackLog  ���Ӷ��и���
    * @throws             TC_Socket_Exception
    */
	void listen(int connBackLog);

    /**
	* @brief  ��������(һ������tcp). 
	*  
    * @param pvBuf  ����buffer
    * @param iLen   buffer����
    * @param iFlag  ��ʾ
    * @return int   ���յ����ݳ���
    */
    int  recv(void *pvBuf, size_t iLen, int iFlag = 0);

    /**
	* @brief  ��������(һ������tcp). 
	*  
    * @param pvBuf ����buffer
    * @param iLen  buffer����
    * @param iFlag ��ʾ
    * @return int  �����˵����ݳ���
    */
	int  send(const void *pvBuf, size_t iLen, int iFlag = 0);

    /**
	* @brief  ��������(һ������udp). 
	*  
    * @param pvBuf      ����buffer
    * @param iLen        buffer����
    * @param sFromAddr  ���, �����ip��ַ
    * @param iFromPort  ���, ����˶˿�
    * @param iFlag      ��ʾ
    * @return int       �����˵����ݳ���
    */
    int recvfrom(void *pvBuf, size_t iLen, string &sFromAddr, uint16_t &iFromPort, int iFlags = 0);

    /**
	* @brief  ��������(һ������udp). 
	*  
    * @param pvBuf      ����buffer
    * @param iLen        buffer����
    * @param pstFromAddr ��ַ
    * @param iFromLen    ���, pstFromAddr����
    * @param iFlag       ��ʾ
    * @return int        �����˵����ݳ���
    */
    int recvfrom(void *pvBuf, size_t iLen, struct sockaddr *pstFromAddr, socklen_t &iFromLen, int iFlags = 0);

    /**
	* @brief ��������(һ������udp). 
	*  
    * @param pvBuf    ����buffer
    * @param iLen     buffer����
    * @param sToAddr  �����ip��ַ, ���sToAddrΪ��, ��udp�㲥
    * @param iToPort  ����˶˿�
    * @param iFlag    ��ʾ
    * @return          int : >0 ���͵����ݳ��� ;<=0, ����
    */
	int sendto(const void *pvBuf, size_t iLen, const string &sToAddr, uint16_t iToPort, int iFlags = 0);

    /**
	* @brief  ��������(һ������udp). 
	*  
    * @param pvBuf       ����buffer
    * @param iLen        buffer����
    * @param pstToAddr   ����˵�ַ
    * @param iToLen      pstToAddr����
    * @param iFlag      ��ʾ
    * @return           int : >0 ���͵����ݳ��� ;<=0, ����
    */
    int sendto(const void *pvBuf, size_t iLen, struct sockaddr *pstToAddr, socklen_t iToLen, int iFlags = 0);

    /**
	* @brief �ر�. 
	*  
    * @param iHow �رշ�ʽ:SHUT_RD|SHUT_WR|SHUT_RDWR
    * @throws     TC_Socket_Exception
    * @return
    */
	void shutdown(int iHow);

    /**
	* @brief ����socket��ʽ .
	*  
    * @param   bBlock true, ����; false, ������
    * @throws  TC_Socket_Exception
    * @return
    */
    void setblock(bool bBlock = false);

    /**
	* @brief ���÷�closewait״̬, ��������socket. 
	*  
    * @throws TC_Socket_Exception
    * @return void
    */
    void setNoCloseWait();

    /**
	 * @brief ����Ϊclosewait״̬, ��ȴ����. 
	 *  
     * @param delay  �ȴ�ʱ����
     * @throws       TC_Socket_Exception
     */
    void setCloseWait(int delay = 30);

    /**
	 * @brief ����closewaitȱʡ״̬�� 
	 *  	  close�Ժ����Ϸ��ز����������ݷ��ͳ�ȥ
     * @throws TC_Socket_Exception
     */
    void setCloseWaitDefault();

    /**
	 * @brief ����nodelay(ֻ���ڴ�keepalive����Ч).
	 *  
     * @throws TC_Socket_Exception
     */
    void setTcpNoDelay();

    /**
	 * @brief ����keepalive. 
	 *  
     * @throws TC_Socket_Exception
     * @return 
     */
    void setKeepAlive();

    /**
	* @brief ��ȡrecv buffer ��С. 
	*  
    * @throws TC_Socket_Exception
    * @return recv buffer �Ĵ�С
    */
    int getRecvBufferSize();

    /**
	* @brief ����recv buffer ��С. 
	* @param  recv buffer ��С 
	* @throws TC_Socket_Exception 
    */
    void setRecvBufferSize(int sz);

    /**
	* @brief ��ȡ����buffer��С.
	* @param ����buffer��С  
    * @throws TC_Socket_Exception 
     */
    int getSendBufferSize();

    /**
	 * @brief ���÷���buffer��С. 
	 *  
     * @throws TC_Socket_Exception
     * @param  ����buffer��С
     */
    void setSendBufferSize(int sz);

    /**
     * @brief  ��ȡ��������ip.
     * 
     * @throws TC_Socket_Exception
     * @return ��������ip
     */
    static vector<string> getLocalHosts();

    /**
	* @brief ����socket��ʽ. 
	*  
    * @param fd      ���
    * @param bBlock  true, ����; false, ������
    * @throws        TC_Socket_Exception
    * @return
    */
    static void setblock(int fd, bool bBlock);

    /**
	 * @brief ���ɹܵ�,�׳��쳣ʱ��ر�fd. 
	 *  
     * @param fds    ���
     * @param bBlock true, ����; false, ������
     * @throws       TC_Socket_Exception
     */
    static void createPipe(int fds[2], bool bBlock);

    /**
	* @brief ������ַ, ���ַ���(ip������), ������in_addr�ṹ. 
	*  
    * @param sAddr   �ַ���
    * @param stAddr  ��ַ
    * @throws        TC_Socket_Exception
    * @return
    */
	static void parseAddr(const string &sAddr, struct in_addr &stAddr);

	/**
	* @brief ��. 
	*  
	* @param pstBindAddr  ��Ҫ�󶨵ĵ�ַ
	* @param iAddrLen      pstBindAddrָ��Ľṹ�ĳ���
	* @throws              TC_Socket_Exception
	* @return
	*/
	void bind(struct sockaddr *pstBindAddr, socklen_t iAddrLen);

    #if 0
    /**
	* @brief no implementation. 
	*  
    */
	TC_Socket(const TC_Socket &tcSock);

    /**
    * @brief  no implementation.
    */
	TC_Socket& operator=(const TC_Socket &tcSock);
    #endif


protected:

    /**
	* @brief ������������. 
	*  
    * @param pstServerAddr  �����ַ
    * @param serverLen      pstServerAddrָ��Ľṹ�ĳ���
    * @return int
    */
    int connect(struct sockaddr *pstServerAddr, socklen_t serverLen);	

    /**
	* @brief ��ȡ�Ե�ĵ�ַ. 
	*  
    * @param pstPeerAddr ��ַָ��
    * @param iPeerLen    pstPeerAddrָ��Ľṹ����
    * @throws            TC_Socket_Exception
    * @return 
    */
    void getPeerName(struct sockaddr *pstPeerAddr, socklen_t &iPeerLen);

    /**
	* @brief ��ȡ�Լ��ĵ�ip�Ͷ˿�. 
	*  
    * @param pstSockAddr ��ַָ��
    * @param iSockLen    pstSockAddr
    * @throws            TC_Socket_Exception
    * @return
    */
    void getSockName(struct sockaddr *pstSockAddr, socklen_t &iSockLen);

private:

protected:
	static const int INVALID_SOCKET = -1;

    /**
     * socket���
     */
	int  _sock;

    /**
     * �Ƿ�ӵ��socket
     */
	bool _bOwner;

    /**
     * socket����
     */
    int  _iDomain;
};

}
#endif
