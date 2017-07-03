#include "base/nBaseServer.h"
#include "base/Tidy.h"
#include "base/nSocketPortData.h"
#include "base/Imp/nListenService.h"
#include "base/Imp/nSocket.h"

namespace Tidy
{
	nListenService::nListenService()
	{
		_epoll_fd = epoll_create(1);
		assert(-1 != _epoll_fd);
		_epoll_event_vec.resize(8);
	}

	nListenService::~nListenService() 
	{
		TEMP_FAILURE_RETRY(::close(_epoll_fd));

		SocketPortData* data = NULL;
		for(Tidy::vector<SocketPortData*>::iterator iter=_socket_port_list.begin(); iter != _socket_port_list.end(); ++ iter)
		{
			data = (*iter);
			if (-1 != data->socket)
			{
				::shutdown(data->socket, SHUT_RD);
				TEMP_FAILURE_RETRY(::close(data->socket));
			}
			SAFE_DELETE(data);
		}

		_socket_port_list.clear();
	}

	bool nListenService::startListen()
	{
		for(DWORD i=0; i < _socket_port_list.size(); ++ i)
		{
			if(bindNewPort(_socket_port_list[i]) == false)
			{
				return false;
			}
		}
		return true;
	}

	bool nListenService::bindNewPort(SocketPortData* portData) 
	{
		assert_debug(portData->port == 0 ^ portData->path.empty(), "至少指定一个协议");
		int socket = -1;
		if(portData->port > 0){
			socket = nSocket::bindTcpServer(portData->ip, portData->port);
		}
		else{
			socket = nSocket::bindUnixServer(portData->path.c_str());
		}
		if(socket == -1){
			baseServer->error("不能绑定服务器端口[%u, %s]", portData->port, portData->path.c_str());
			TEMP_FAILURE_RETRY(::close(socket));
			return false;
		}

		int  nb = 1;
		if(ioctl(socket, FIONBIO, &nb) < 0){
			assert_fail("设置非阻塞失败");
			return false;
		}

		int retcode = ::listen(socket, MAX_WAITQUEUE);
		if (-1 == retcode){
			baseServer->error("监听套接口失败[%u, %s]", portData->port, portData->path.c_str());
			TEMP_FAILURE_RETRY(::close(socket));
			return false;
		}

		struct epoll_event ev;
		ev.events = ASSERT_CAST(__typeof__(ev.events), EPOLLIN);
		ev.data.ptr = portData;

		if(epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, socket, &ev) != 0){
			assert_fail("epoll_ctl 失败");
			TEMP_FAILURE_RETRY(::close(socket));
			return false;
		}

		portData->socket = socket;

		return true;
	}

	int nListenService::acceptConn()
	{
		int eventCount = epoll_wait(_epoll_fd, &_epoll_event_vec[0], _socket_port_list.size(), T_MSEC);
		if (eventCount > 0){
			struct sockaddr_in inAddr;
			bzero(&inAddr, sizeof(struct sockaddr_in));
			socklen_t inLen = sizeof(struct sockaddr_in);

			struct sockaddr_un unAddr;
			bzero(&unAddr, sizeof(struct sockaddr_un));
			socklen_t unLen = sizeof(struct sockaddr_un);

			const SocketPortData* data = NULL;
			int socket = -1;
			struct sockaddr *addr = NULL;
			socklen_t *len = NULL;

			for(int i = 0; i < eventCount; i++){
				if (_epoll_event_vec[i].events & EPOLLIN){
					data = reinterpret_cast<const SocketPortData*>(_epoll_event_vec[i].data.ptr);
					if(data->path.empty() == true){
						addr = reinterpret_cast<struct sockaddr *>(&inAddr);
						len = &inLen;
					}
					else{
						addr = reinterpret_cast<struct sockaddr *>(&unAddr);
						len = &unLen;
					}
					do{
						socket = TEMP_FAILURE_RETRY(accept(data->socket, addr, len));
						if(socket == -1){
							if(errno == EAGAIN || errno == EWOULDBLOCK){
								break;
							}
							else{
								assert_fail("未知错误");
							}
							break;
						}

						if(baseServer->dealNewTaskConn(socket, addr, data) == false){
							assert_fail("创建连接失败");
						}
					}while(true);
				}
			}
		}

		return true;
	}

	bool nListenService::addTcpServer(const nListenData &listenData, const BYTE zipMethod, const bool checkHeartTick, const DWORD heartTickTimerInMsec, const bool needQuickAck, CommandCtl* ctl)
	{
		for(DWORD i = 0; i < _socket_port_list.size(); ++ i){
			if(_socket_port_list[i]->port == listenData.bindPort && (_socket_port_list[i]->ip.empty() == true || listenData.bindIP.empty() == true || (_socket_port_list[i]->ip.compare(listenData.bindIP) == 0))){
				assert_fail("重复添加监听端口号.[%s,%u]", listenData.bindIP.c_str(), listenData.bindPort);
				return false;
			}
		}

		SocketPortData *data = new SocketPortData(listenData.connType, listenData.netType, -1, listenData.bindIP, listenData.bindPort, "", zipMethod, checkHeartTick, heartTickTimerInMsec, needQuickAck, ctl);
		_socket_port_list.push_back(data);
		if(_socket_port_list.size() >= _epoll_event_vec.size()){
			_epoll_event_vec.resize(_epoll_event_vec.size() + 8);
		}

		return true;
	}

	bool nListenService::addUnixServer(const DWORD connType, const char *unixSocketPath, const BYTE zipMethod, const bool checkHeartTick, const DWORD heartTickTimerInMsec, CommandCtl* ctl)
	{
		for(DWORD i = 0; i < _socket_port_list.size(); ++ i){
			if(strcmp(unixSocketPath, _socket_port_list[i]->path.c_str()) == 0){
				assert_fail("重复添加监听端口号.");
				return false;
			}
		}

		SocketPortData *data = new SocketPortData(connType, 0, -1, "", 0, unixSocketPath, zipMethod, checkHeartTick, heartTickTimerInMsec, false, ctl);
		_socket_port_list.push_back(data);
		if(_socket_port_list.size() >= _epoll_event_vec.size()){
			_epoll_event_vec.resize(_epoll_event_vec.size() + 8);
		}

		return true;
	}
};
