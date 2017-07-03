#include "base/nConnectServerData.h"

namespace Tidy
{
	nConnectServerData::nConnectServerData(const DWORD id, const DWORD type, const char *name, const char* ip, const WORD port, const char *destUnixSocketPath, const BYTE zipMethodData, const bool reConn, const DWORD reConnTimer, const bool waitConnected, const bool needHeartTick, const DWORD timeTickTimerInMsec, const CommandCtl *ctl):
		serverID(id),
		serverType(type),
		serverName(name),
		serverIP(ip),
		serverPort(port),
		serverUnixSocketPath(destUnixSocketPath),
		zipMethod(zipMethodData),
		needReconnect(reConn),
		reconnectTimer(reConnTimer),
		needWaitConnected(waitConnected),
		needCheckHeartTick(needHeartTick),
		tickTimerInMsec(timeTickTimerInMsec),
		commandCtl(ctl)
	{
	}
};
