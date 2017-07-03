#ifndef _N_TCP_SERVICE_H_
#define _N_TCP_SERVICE_H_

#include "base/nSTL.h"

namespace Tidy
{
	struct CommandCtl;
	class SocketPortData;
    class nListenData;

	class nListenService
	{
		public:
			DISALLOW_COPY_AND_ASSIGN(nListenService);
			nListenService();
			~nListenService();

		public:
			bool startListen();
            int acceptConn();
            bool addTcpServer(const nListenData &listenData, const BYTE zipMethod, const bool checkHeartTick, const DWORD heartTickTimerInMsec, const bool needQuickAck, CommandCtl* ctl);
			bool addUnixServer(const DWORD connType, const char *unixSocketPath, const BYTE zipMethod, const bool checkHeartTick, const DWORD heartTickTimerInMsec, CommandCtl* ctl);

		private:
			bool bindNewPort(SocketPortData* portData);

		private:
			/// @brief	轮询超时，毫秒
			static const int T_MSEC =2100;
			/// @brief	最大等待队列
			static const int MAX_WAITQUEUE = 2000;

		private:
			int _epoll_fd;
			Tidy::vector<struct epoll_event> _epoll_event_vec;
			Tidy::vector<SocketPortData*> _socket_port_list;
	};
};

#endif
