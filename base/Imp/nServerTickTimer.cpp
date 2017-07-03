#include "base/nServerTickTimer.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	nServerTickTimer::nServerTickTimer()
	{
	}

	nServerTickTimer::nServerTickTimer(const long how_long, bool first, const long delay) : nTimer(how_long, first, baseServer->time())
	{
	}

	const bool nServerTickTimer::isReady()
	{
		return nTimer::isReady(baseServer->time());
	}
};
