#include "base/nSelfConn.h"

namespace Tidy
{
	nSelfConn::nSelfConn(const DWORD serverID, const DWORD serverType) : nServerConnQueue(serverID, serverType)
	{
	}

	nSelfConn::~nSelfConn()
	{
	}

	bool nSelfConn::sendCmd(const void *cmd, const DWORD len) const
	{
		return const_cast<nSelfConn*>(this)->pushCmd(cmd, len);
	}

	bool nSelfConn::sendRawData(const void *data, const DWORD len) const
	{
		return true;
	}
};
