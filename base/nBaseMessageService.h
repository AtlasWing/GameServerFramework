#ifndef BASE_N_BASE_MESSAGE_SERVICE_H_
#define BASE_N_BASE_MESSAGE_SERVICE_H_

#include "base/nType.h"

namespace Tidy
{
	class nServerConn;
	class nTaskConn;
	class nClientConn;
	class nTime;

	class nBaseMessageService
	{
		protected:
			typedef bool (*ParseTaskTickCommandFunc)(nTaskConn *taskConn, const char *cmdBuffer, const DWORD cmdLen);
			typedef bool (*SendTaskTickCommandFunc)(nTaskConn *taskConn, const nTime &ct);
			typedef bool (*SendClientTickCommandFunc)(nClientConn *clientConn);
			typedef bool (*ParseClientTickCommandFunc)(nClientConn *clientConn, const char* cmdBuffer, const DWORD cmdLen);

		protected:
			nBaseMessageService(ParseTaskTickCommandFunc parseTaskTick, SendTaskTickCommandFunc sendTaskTick, ParseClientTickCommandFunc parseClientTick, SendClientTickCommandFunc sendClientTick);

		public:
			virtual ~nBaseMessageService();

		public:
			virtual bool init();
			virtual void final();
			virtual bool parseCommand(nServerConn* serverConn, const void *nullCommand, const DWORD len) = 0;

		public:
			ParseTaskTickCommandFunc parseTaskTickCommand;
			SendTaskTickCommandFunc sendTaskTickCommand;
			ParseClientTickCommandFunc parseClientTickCommand;
			SendClientTickCommandFunc sendClientTickCommand;
	};
};
#endif
