#ifndef BASE_NCONNECTSERVERDATA_H
#define BASE_NCONNECTSERVERDATA_H

#include "base/nSTL.h"

namespace Tidy
{
	struct CommandCtl;
	class nConnectServerData
	{
		public:
			nConnectServerData(const DWORD serverID, const DWORD serverType, const char *name, const char* ip, const WORD port, const char *destUnixSocketPath, const BYTE zipMethod, const bool reConn, const DWORD reConnTimer, const bool waitConnected, const bool needHeartTick, const DWORD tickTimerInMsec, const CommandCtl *ctl);

		public:
			const DWORD serverID;
			const DWORD serverType;
			const std::string serverName;
			const std::string serverIP;
			const WORD serverPort;
			const std::string serverUnixSocketPath;
			const BYTE zipMethod;
			const bool needReconnect;;
			const DWORD reconnectTimer;
			const bool needWaitConnected;
			const bool needCheckHeartTick;
			const DWORD tickTimerInMsec;
			const CommandCtl *commandCtl;
	};
};
#endif
