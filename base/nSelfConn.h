#ifndef BASE_NSELFCONN_H_
#define BASE_NSELFCONN_H_

#include "base/nServerConn.h"

namespace Tidy
{
	class nSelfConn : public nServerConnQueue
	{
		public:
			nSelfConn(const DWORD serverID, const DWORD serverType);
			~nSelfConn();

		public:
			virtual bool sendCmd(const void *command, const DWORD len) const;
			virtual bool sendRawData(const void *command, const DWORD len) const;
	};
};
#endif
