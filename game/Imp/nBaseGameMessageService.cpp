#include "base/nMisc.h"

#include "game/nBaseGameMessageService.h"
#include "game/nGameServer.h"

namespace Game
{
	nBaseGameMessageService::nBaseGameMessageService()
	{
	}

	nBaseGameMessageService::~nBaseGameMessageService()
	{
	}

	bool nBaseGameMessageService::sendMoniterAlertMessage(const BYTE alertLevel, const char* pattern, ...)
	{
        return true;
	}

};
