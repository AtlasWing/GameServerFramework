#include <netdb.h>

#include "base/nMisc.h"
#include "base/nSys.h"
#include "base/nBaseServer.h"
#include "base/nDebugger.h"
#include "base/nZip.h"

#include "base/Imp/nSocket.h"

namespace Tidy
{
	nSocket::nSocket(const int sock, const struct sockaddr *addr, const CommandCtl* ctl, const BYTE zipMethod, const bool needQuickAck):
		_bit_mask(0),
		_sock(sock),
		_peer_ip_str("0.0.0.0"),
		_peer_port(0),
		_local_ip_str("0.0.0.0"),
		_local_port(0),
		rd_msec(T_RD_MSEC),
		wr_msec(T_WR_MSEC),
		_rcv_raw_size(0),
		_current_packet(0),
		_command_ctl(ctl),
		_zip_method(zipMethod),
		_need_enc(false),
		_enc_data(NULL),
		_need_close(false),
		_need_quick_ack(needQuickAck)
	{
		assert_debug(-1 != _sock, "");

		if(addr->sa_family == AF_INET){
			_peer_ip_str = inet_ntoa(reinterpret_cast<const struct sockaddr_in*>(addr)->sin_addr);
			_peer_port = ntohs(reinterpret_cast<const struct sockaddr_in*>(addr)->sin_port);

			struct sockaddr_in localAddr;
			socklen_t len = sizeof(struct sockaddr_in);
			getsockname(_sock, reinterpret_cast<struct sockaddr*>(&localAddr), &len);
			_local_ip_str = inet_ntoa(localAddr.sin_addr);
			_local_port = ntohs(localAddr.sin_port);
		}
		else if(addr->sa_family == AF_LOCAL){
			_need_quick_ack = false;
		}

		fcntl(this->_sock, F_SETFD,
				fcntl(this->_sock, F_GETFD, 0)
				| FD_CLOEXEC);
		setNonblock();

		set_flag(INCOMPLETE_READ | INCOMPLETE_WRITE);
	}

	nSocket::~nSocket()
	{
		baseServer->debug("关闭套接口连接[%p]", this);
		sync();
		::shutdown(_sock, SHUT_RDWR);
		TEMP_FAILURE_RETRY(::close(_sock));

		_sock = -1;
		SAFE_DELETE(_enc_data);
	}

	/**
	 * \brief 从接收到数据的buffer中提取一个消息
	 *
	 */
#define success_unpack() \
	do { \
		if(_unpack_cmd_queue.rd_ready()) {\
			int commandLen = _command_ctl->unpackCmd(this, _unpack_cmd_queue, NULL, commandBuffer, commandBufferLen); \
			if(commandLen < 0){\
				assert_fail("unpackCmd fail");\
				setNeedClose();\
				return 0;\
			}\
			if(commandLen > 0){\
				return commandLen;\
			}\
		}\
		assert_debug(_rcv_queue.rd_size() >= _rcv_raw_size, "");\
		if (_rcv_raw_size >= _command_ctl->getMinPacketSize()){ \
			int nRecordLen = _command_ctl->getPacketSize(_rcv_queue, 0); \
			if(nRecordLen < 0){ \
				assert_fail("获取包数据大小错误");\
				setNeedClose();\
				return 0;\
			} \
			else if (_rcv_raw_size >= static_cast<DWORD>(nRecordLen)){ \
				DWORD rcvSize = _rcv_queue.rd_size();\
				int commandLen = _command_ctl->unpackCmd(this, _rcv_queue, &_unpack_cmd_queue, commandBuffer, commandBufferLen); \
				if(commandLen < 0){\
					assert_fail("unpackCmd fail");\
					setNeedClose();\
					return 0;\
				}\
				if(rcvSize != _rcv_queue.rd_size()){\
					assert_debug((rcvSize - _rcv_queue.rd_size()) == static_cast<DWORD>(nRecordLen), "");\
					_rcv_raw_size -= (rcvSize - _rcv_queue.rd_size()); \
				}\
				assert_debug(_rcv_queue.rd_size() >= _rcv_raw_size, "");\
				return commandLen; \
			} \
		} \
	} while(0)

	DWORD nSocket::recvToCmd(void *commandBuffer, const DWORD commandBufferLen, const bool wait)
	{
		//够一个完整的纪录，不需要向套接口接收数据
		success_unpack();

		do {
			int retval = recvToBuf();
			if (-1 == retval || (0 == retval && !wait))
				return retval;

			//够一个完整的纪录，不需要向套接口接收数据
			success_unpack();
		} while(true);

		return 0;
	}

	int nSocket::sendRawData(const void *pBuffer, const DWORD nSize)
	{
		int retcode = 0;
		if (isset_flag(INCOMPLETE_WRITE))
		{
			clear_flag(INCOMPLETE_WRITE);
			goto do_select;
		}

		retcode = TEMP_FAILURE_RETRY(::send(_sock, pBuffer, nSize, MSG_NOSIGNAL));
		if (retcode == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
		{
do_select:
			retcode = waitForWrite();
			if (1 == retcode)
				retcode = TEMP_FAILURE_RETRY(::send(_sock, pBuffer, nSize, MSG_NOSIGNAL));
			else
				return retcode;
		}

		if (retcode > 0 && static_cast<DWORD>(retcode) < nSize)
			set_flag(INCOMPLETE_WRITE);

		return retcode;
	}

	bool nSocket::sendRawDataIM(const void *pBuffer, const DWORD nSize)
	{
		assert_debug(pBuffer != NULL && nSize > 0, "数据无效");
		DWORD offset = 0;
		do
		{
			int retcode = sendRawData(&((char *)pBuffer)[offset], nSize - offset);
			if (-1 == retcode)
			{
				return false;
			}
			offset += retcode;
		}
		while(offset < nSize);

		return (offset == nSize);
	}

	bool nSocket::sendCmd(const void *nullCommand, const DWORD commandLen, const bool buffer)
	{
		if (NULL == nullCommand) {
			assert_fail("消息数据乱");
			return false;
		}

		bool retval = true;
		if (buffer) {
			if(checkNeedZip() == true) {
				_mutex.lock();
				_wait_zip_packet_queue.wr_reserve(_command_ctl->getCmdBufferSize(commandLen));
				DWORD len = _command_ctl->putPacket(eZipMethod_None, nullCommand, commandLen, _wait_zip_packet_queue.wr_buf(), _wait_zip_packet_queue.wr_size(), true, &_current_packet);
				_wait_zip_packet_queue.wr_flip(len);
				_mutex.unlock();
			}
			else {
				_mutex.lock();
				_snd_queue.wr_reserve(_command_ctl->getCmdBufferSize(commandLen));
				DWORD len = _command_ctl->putPacket(eZipMethod_None, nullCommand, commandLen, _snd_queue.wr_buf(), _snd_queue.wr_size(), true, &_current_packet);
				_snd_queue.wr_flip(len);
				_mutex.unlock();
			}
		}
		else {
			if(_command_ctl->getCmdBufferSize(commandLen) < PACKET_ZIP_BUFFER) {
				StackCmdQueue commandQueue;
				commandQueue.wr_flip(_command_ctl->putPacket(_zip_method, nullCommand, commandLen, commandQueue.wr_buf(), commandQueue.wr_size(), true, &_current_packet));
				if (_need_enc) {
					packetPackEnc(commandQueue, commandQueue.rd_size(), 0);
				}
				_mutex.lock();
				retval = sendRawDataIM(commandQueue.rd_buf(), commandQueue.rd_size());
				_mutex.unlock();
			}
			else {
				BufferCmdQueue commandQueue;
				commandQueue.wr_reserve(_command_ctl->getCmdBufferSize(commandLen));
				commandQueue.wr_flip(_command_ctl->putPacket(_zip_method, nullCommand, commandLen, commandQueue.wr_buf(), commandQueue.wr_size(), true, &_current_packet));
				if (_need_enc) {
					packetPackEnc(commandQueue, commandQueue.rd_size(), 0);
				}
				_mutex.lock();
				retval = sendRawDataIM(commandQueue.rd_buf(), commandQueue.rd_size());
				_mutex.unlock();
			}
		}

		return retval;
	}

	bool nSocket::sync()
	{
		DWORD nSize = 0, currentCmd = 0, offset = 0, len = 0;

		if(checkNeedZip() == true && _wait_zip_packet_queue.rd_ready()) {
			_mutex.lock();
			if(_wait_zip_packet_queue.rd_ready()) {
				if(_wait_zip_packet_queue.rd_size() >= PACKET_ZIP_BUFFER) {
					BufferCmdQueue cmdQueue;
					cmdQueue.put(_wait_zip_packet_queue.rd_buf(), _wait_zip_packet_queue.rd_size());
					_wait_zip_packet_queue.reset();
					_mutex.unlock();

					_snd_queue.wr_reserve(_command_ctl->getBufferSize(cmdQueue.rd_size()));
					len = _command_ctl->putPacket(_zip_method, cmdQueue.rd_buf(), cmdQueue.rd_size(), _snd_queue.wr_buf(), _snd_queue.wr_size(), false, &_current_packet);
					_snd_queue.wr_flip(len);
				}
				else {
					StackCmdQueue cmdQueue;
					cmdQueue.put(_wait_zip_packet_queue.rd_buf(), _wait_zip_packet_queue.rd_size());
					_wait_zip_packet_queue.reset();
					_mutex.unlock();

					_snd_queue.wr_reserve(_command_ctl->getBufferSize(cmdQueue.rd_size()));
					len = _command_ctl->putPacket(_zip_method, cmdQueue.rd_buf(), cmdQueue.rd_size(), _snd_queue.wr_buf(), _snd_queue.wr_size(), false, &_current_packet);
					_snd_queue.wr_flip(len);
				}
			}
			else {
				_mutex.unlock();
			}
		}

		if (_snd_queue.rd_ready()) {
			_mutex.lock();
			if (_snd_queue.rd_ready()) {
				nSize = _snd_queue.rd_size();
				currentCmd = _current_packet;
				offset = _enc_queue.rd_size();
				_enc_queue.put(_snd_queue.rd_buf(), nSize);
				_snd_queue.rd_flip(nSize);
				_current_packet = 0;
			}
			_mutex.unlock();
		}

		if (_need_enc && nSize > 0) {
			packetPackEnc(_enc_queue, currentCmd, offset);
		}

		if (_enc_queue.rd_ready()) {
			int retcode = sendRawData_NoPoll(_enc_queue.rd_buf(), _enc_queue.rd_size());
			if (retcode > 0) {
				_enc_queue.rd_flip(retcode);
			}
			else if (-1 == retcode) {
				return false;
			}
		}

		return true;
	}

	int nSocket::checkIOForRead()
	{
		struct pollfd pfd;

		pfd.fd = _sock;
		pfd.events = POLLIN | POLLERR | POLLPRI;
		pfd.revents = 0;

		int retcode = TEMP_FAILURE_RETRY(::poll(&pfd, 1, 0));
		if (retcode > 0 && 0 == (pfd.revents & POLLIN))
		{
			retcode = -1;
		}

		return retcode;
	}

	int nSocket::checkIOForWrite()
	{
		struct pollfd pfd;

		pfd.fd = _sock;
		pfd.events = POLLOUT | POLLERR | POLLPRI;
		pfd.revents = 0;

		int retcode = TEMP_FAILURE_RETRY(::poll(&pfd, 1, 0));
		if (retcode > 0 && 0 == (pfd.revents & POLLOUT))
			retcode = -1;

		return retcode;
	}

	/**
	 * \brief 将接收到的数据解密
	 *
	 */
#define success_recv_and_dec() \
	do { \
		if ((unsigned int)retcode < _rcv_queue.wr_size()) \
		{ \
			set_flag(INCOMPLETE_READ); \
		} \
		_rcv_queue.wr_flip(retcode); \
		unsigned int size = _rcv_queue.rd_size() - _rcv_raw_size - ((_rcv_queue.rd_size() - _rcv_raw_size) % 8); \
		if(size) \
		{ \
			_command_ctl->encdecData(_enc_data, (void*)(&_rcv_queue.rd_buf()[_rcv_raw_size]), size, false); \
			_rcv_raw_size += size; \
		} \
	} while(0)

	/**
	 * \brief 接收数据
	 *
	 */
#define success_recv() \
	do { \
		if ((unsigned int)retcode < _rcv_queue.wr_size()) \
		{ \
			set_flag(INCOMPLETE_READ); \
		} \
		_rcv_queue.wr_flip(retcode); \
		_rcv_raw_size += retcode; \
	} while(0)

	int nSocket::recvToBuf_NoPoll()
	{
		int retcode =0; 
		if(_need_enc)
		{
			_rcv_queue.wr_reserve(MAX_PROTO_CMD_LEN);
			retcode = TEMP_FAILURE_RETRY(::recv(_sock, _rcv_queue.wr_buf(), _rcv_queue.wr_size(), MSG_NOSIGNAL));
			if (retcode == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
			{
				baseServer->debug("尝试重新接收数据:[ip=%s,retcode=%u,err=%u]",getIP(),retcode,errno);
				return 0;//should retry
			}

			if (retcode > 0) 
				success_recv_and_dec();
		}
		else
		{
			_rcv_queue.wr_reserve(MAX_PROTO_CMD_LEN);
			retcode = TEMP_FAILURE_RETRY(::recv(_sock, _rcv_queue.wr_buf(), _rcv_queue.wr_size(), MSG_NOSIGNAL));
			if (retcode == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
			{
				baseServer->debug("尝试重新接收数据:[ip=%s,retcode=%u,err=%u]",getIP(),retcode,errno);
				return 0;//should retry
			}

			if (retcode > 0)
				success_recv();
		}

		if (0 == retcode)
			return -1;//EOF 

		if(_need_quick_ack == true){
			keepQuickAck();
		}
		return retcode;
	}

	DWORD nSocket::recvToCmd_NoPoll(void *commandBuffer, const DWORD commandBufferLen)
	{
		//够一个完整的纪录，不需要向套接口接收数据
		success_unpack();

		return 0;
	}

	void nSocket::addEpoll(int kdpfd, __uint32_t events, void *ptr)
	{
		struct epoll_event ev;
		ev.events = events;
		ev.data.ptr = ptr;
		_mutex.lock();
		if (-1 == epoll_ctl(kdpfd, EPOLL_CTL_ADD, _sock, &ev)){
			char _buf[100];
			strerror_r(errno, _buf, sizeof(_buf));
			assert_fail("注册epoll事件失败");
		}
		_mutex.unlock();
	}

	void nSocket::delEpoll(int kdpfd, __uint32_t events)
	{
		struct epoll_event ev;
		ev.events = events;
		_mutex.lock();
		if (-1 == epoll_ctl(kdpfd, EPOLL_CTL_DEL, _sock, &ev)){
			char _buf[100];
			strerror_r(errno, _buf, sizeof(_buf));
			assert_fail("取消epoll注册失败");
		}
		_mutex.unlock();
	}

	int nSocket::sendRawData_NoPoll(const void *data, const DWORD len)
	{
		int retcode = TEMP_FAILURE_RETRY(::send(_sock, data, len, MSG_NOSIGNAL));

		if (retcode == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
		{
			baseServer->debug("尝试重新发送数据:[ip=%s,retcode=%d,err=%d,size=%u]", getIP(), retcode, errno, len);
			return 0;//should retry
		}

		if (retcode > 0 && static_cast<DWORD>(retcode) < len){
			set_flag(INCOMPLETE_WRITE);
#ifdef _DEBUG_NYS
			baseServer->debug("未完成发送[%d,%u]", retcode, len);
#endif
		}

		return retcode;
	}

	bool nSocket::setNonblock()
	{
		int fd_flags;
		if(_local_port != 0){
			//unix domain socket 不设置此选项,否则会阻塞
			int nodelay = 1;
			if (::setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY, (void *)&nodelay, sizeof(nodelay))){
				return false;
			}
		}

		fd_flags = ::fcntl(_sock, F_GETFL, 0);

#if defined(O_NONBLOCK)
		fd_flags |= O_NONBLOCK;
#elif defined(O_NDELAY)
		fd_flags |= O_NDELAY;
#elif defined(FNDELAY)
		fd_flags |= O_FNDELAY;
#else
		/* XXXX: this breaks things, but an alternative isn't obvious...*/
		return false;
#endif

		if (::fcntl(_sock, F_SETFL, fd_flags) == -1) {
			return false;
		}

		return true;
	}

	int nSocket::waitForRead()
	{
		struct pollfd pfd;

		pfd.fd = _sock;
		pfd.events = POLLIN | POLLERR | POLLPRI;
		pfd.revents = 0;

		int retcode = TEMP_FAILURE_RETRY(::poll(&pfd, 1, rd_msec));
		if (retcode > 0 && 0 == (pfd.revents & POLLIN))
			retcode = -1;

		return retcode;
	}

	int nSocket::waitForWrite()
	{
		struct pollfd pfd;

		pfd.fd = _sock;
		pfd.events = POLLOUT | POLLERR | POLLPRI;
		pfd.revents = 0;

		int retcode = TEMP_FAILURE_RETRY(::poll(&pfd, 1, wr_msec));
		if (retcode > 0 && 0 == (pfd.revents & POLLOUT))
			retcode = -1;

		return retcode;
	}

	int nSocket::recvToBuf()
	{
		int retcode = 0;

		if(_need_enc)
		{
			if (isset_flag(INCOMPLETE_READ))
			{
				clear_flag(INCOMPLETE_READ);
				goto do_select_enc;
			}

			_rcv_queue.wr_reserve(MAX_PROTO_CMD_LEN);
			retcode = TEMP_FAILURE_RETRY(::recv(_sock, _rcv_queue.wr_buf(), _rcv_queue.wr_size(), MSG_NOSIGNAL));
			if (retcode == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
			{
do_select_enc:
				retcode = waitForRead();
				if (1 == retcode)
					retcode = TEMP_FAILURE_RETRY(::recv(_sock, _rcv_queue.wr_buf(), _rcv_queue.wr_size(), MSG_NOSIGNAL));
				else
					return retcode;
			}

			if (retcode > 0) 
				success_recv_and_dec();
		}
		else
		{
			if (isset_flag(INCOMPLETE_READ))
			{
				clear_flag(INCOMPLETE_READ);
				goto do_select;
			}
			_rcv_queue.wr_reserve(MAX_PROTO_CMD_LEN);
			retcode = TEMP_FAILURE_RETRY(::recv(_sock, _rcv_queue.wr_buf(), _rcv_queue.wr_size(), MSG_NOSIGNAL));
			if (retcode == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
			{
do_select:
				retcode = waitForRead();
				if (1 == retcode)
					retcode = TEMP_FAILURE_RETRY(::recv(_sock, _rcv_queue.wr_buf(), _rcv_queue.wr_size(), MSG_NOSIGNAL));
				else
					return retcode;
			}

			if (retcode > 0)
				success_recv();
		}

		if (0 == retcode)
			return -1;//EOF 

		if(_need_quick_ack == true){
			keepQuickAck();
		}
		return retcode;
	}

	bool nSocket::heartTick() const
	{
		char buff[1];
		::send(_sock, buff, 0, MSG_NOSIGNAL);
		return true;
	}

	DWORD nSocket::getSendBufferSize() const 
	{ 
		return _wait_zip_packet_queue.rd_size() + _snd_queue.rd_size() + _enc_queue.rd_size(); 
	}

	const CommandCtl* nSocket::getCommandCtl() const
	{
		return _command_ctl;
	}

	void nSocket::setNeedClose()
	{
		_need_close = true;
	}

	bool nSocket::checkNeedClose() const
	{
		return _need_close;
	}

	void nSocket::keepQuickAck()
	{
		// 避免延迟确认带来的延迟.每次调用在0.4us左右,可以接受.
		int quickAck = 1;
		setsockopt(_sock, IPPROTO_TCP, TCP_QUICKACK, (void*)&quickAck, sizeof(int));
	}

	int nSocket::createTcpSocket()
	{
		int sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (-1 == sock){
			baseServer->error("创建套接口失败");
			return sock;
		}

		//设置套接口为可重用状态
		int reuse = 1;
		if (-1 == ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))){
			baseServer->error("不能设置套接口为可重用状态");
			TEMP_FAILURE_RETRY(::close(sock));
			return -1;
		}

		//设置套接口发送接收缓冲，并且服务器的必须在accept之前设置
		socklen_t window_size = 128 * 1024;
		if (-1 == ::setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &window_size, sizeof(window_size))){
			TEMP_FAILURE_RETRY(::close(sock));
			return -1;
		}
		if (-1 == ::setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &window_size, sizeof(window_size))){
			TEMP_FAILURE_RETRY(::close(sock));
			return -1;
		}

		return sock;
	}

	int nSocket::createUnixSocket()
	{
		int sock = ::socket(AF_LOCAL, SOCK_STREAM, 0);
		if (-1 == sock){
			baseServer->error("创建套接口失败");
			return sock;
		}

		//设置套接口为可重用状态
		int reuse = 1;
		if (-1 == ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))){
			baseServer->error("不能设置套接口为可重用状态");
			TEMP_FAILURE_RETRY(::close(sock));
			return -1;
		}

		return sock;
	}

	int nSocket::bindTcpServer(const std::string &ip, WORD port)
	{
        assert_debug(port > 0, "");
		int socket = createTcpSocket();
		if(socket == -1){
			return -1;
		}

		struct sockaddr_in addr;
		bzero(&addr, sizeof(addr));
		addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        // 域名绑定IP使用INADDR_ANY
        if(ip.empty() == true || ip.data()[0] == '*' || addr.sin_addr.s_addr == INADDR_NONE){
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
        }
		addr.sin_port = htons(port);

		int retcode = ::bind(socket, (struct sockaddr *) &addr, sizeof(addr));
		if (-1 == retcode){
			TEMP_FAILURE_RETRY(::close(socket));
			return -1;
		}

		baseServer->info("服务器 %s:[%s,%u] 端口初始化绑定成功", baseServer->getServerName(), ip.c_str(), port);

		return socket;
	}

	int nSocket::bindUnixServer(const char *path)
	{
		int socket = createUnixSocket();
		if(socket == -1){
			return -1;
		}

		struct sockaddr_un sa;
		memset(&sa,0,sizeof(sa));
		sa.sun_family = AF_LOCAL;
		copyStr(sa.sun_path, path, sizeof(sa.sun_path));

		int retcode = ::bind(socket, (struct sockaddr *) &sa, sizeof(sa));
		if (-1 == retcode){
			TEMP_FAILURE_RETRY(::close(socket));
			return -1;
		}

		baseServer->info("服务器 %s:%s 端口初始化绑定成功", baseServer->getServerName(), path);
		return socket;
	}

	nSocket* nSocket::connectTcpServer(const char *ip, int port, const CommandCtl *commandCtl, const BYTE zipMethod)
	{
		char dnsBuffer[8192];
		struct hostent hostInfo;
		struct hostent *host = NULL;
		int rc = 0;
		if(gethostbyname_r(ip, &hostInfo, dnsBuffer, sizeof(dnsBuffer), &host, &rc) != 0 || host == NULL){
            assert_fail("连接失败,无法解析host[%s,%d]", ip, rc);
			return NULL;
		}
		int socket = nSocket::createTcpSocket();
		if(socket == -1){
			return NULL;
		}

		struct sockaddr_in addr;
		bzero(&addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = ((struct in_addr*)(host->h_addr))->s_addr;
		addr.sin_port = htons(port);

		int retcode = TEMP_FAILURE_RETRY(::connect(socket, (struct sockaddr *) &addr, sizeof(addr)));
		if (-1 == retcode && errno != EINPROGRESS){
			baseServer->error("创建到服务器 %s(%u) 的连接失败", ip, port);
			TEMP_FAILURE_RETRY(::close(socket));
			return NULL;
		}

		baseServer->info("创建到服务器 %s:%u 的连接成功", ip, port);

		return new nSocket(socket, reinterpret_cast<const struct sockaddr*>(&addr), commandCtl, zipMethod, false);
	}

	nSocket* nSocket::connectUnixServer(const char *destUnixSocketPath, const CommandCtl *commandCtl, const BYTE zipMethod)
	{
		int socket = nSocket::createUnixSocket();
		if(socket == -1){
			return NULL;
		}

		struct sockaddr_un sa;
		sa.sun_family = AF_LOCAL;
		copyStr(sa.sun_path, destUnixSocketPath, sizeof(sa.sun_path));

		int retcode = TEMP_FAILURE_RETRY(::connect(socket, (struct sockaddr *) &sa, sizeof(sa)));
		if (-1 == retcode){
			baseServer->error("创建到服务器 [%s] 的连接失败", destUnixSocketPath);
			TEMP_FAILURE_RETRY(::close(socket));
			return NULL;
		}

		baseServer->info("创建到服务器 [%s] 的连接成功", destUnixSocketPath);

		struct sockaddr_in addr;
		bzero(&addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		addr.sin_port = htons(0);

		return new nSocket(socket, reinterpret_cast<const struct sockaddr*>(&addr), commandCtl, zipMethod, false);
	}

	nEncrypt* nSocket::getEncData()
	{
		_enc_data = new nEncrypt();
		return _enc_data;
	}

	void nSocket::changeCommandCtl(const CommandCtl* commandCtl)
	{
        assert_debug(_command_ctl != commandCtl, "");
        _command_ctl = commandCtl;
	}

};
