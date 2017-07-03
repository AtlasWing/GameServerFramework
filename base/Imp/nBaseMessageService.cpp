#include "base/nBaseMessageService.h"
#include "base/nSys.h"

namespace Tidy
{
	nBaseMessageService::nBaseMessageService(ParseTaskTickCommandFunc parseTaskTick, SendTaskTickCommandFunc sendTaskTick, ParseClientTickCommandFunc parseClientTick, SendClientTickCommandFunc sendClientTick):
		parseTaskTickCommand(parseTaskTick),
		sendTaskTickCommand(sendTaskTick),
		parseClientTickCommand(parseClientTick),
		sendClientTickCommand(sendClientTick)
	{
	}

	nBaseMessageService::~nBaseMessageService()
	{
	}

	bool nBaseMessageService::init()
	{
		return true;
	}

	void nBaseMessageService::final()
	{
	}
};
