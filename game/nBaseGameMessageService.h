#ifndef GAME_NBASEGAMEMESSAGESERVICE_H
#define GAME_NBASEGAMEMESSAGESERVICE_H

#include "base/nType.h"

namespace protocol{
	namespace base{
		struct MonitorCommandBase;
	};
};

namespace Game
{
	class nBaseGameMessageService
	{
		protected:
			nBaseGameMessageService();
			virtual ~nBaseGameMessageService();

		public:
			bool sendMoniterAlertMessage(const BYTE alertLevel, const char* pattern, ...);

		private:
			virtual bool sendCmdToMonitorServerFromZone(const protocol::base::MonitorCommandBase *cmd, const DWORD len) const = 0;
	};
};
#endif
