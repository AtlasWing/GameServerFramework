#ifndef BASE_NSOCKETPORTDATA_H_
#define BASE_NSOCKETPORTDATA_H_

#include "base/nSTL.h"

namespace Tidy
{
	struct CommandCtl;
    struct nListenData{
        nListenData(const DWORD taskConnType, const BYTE listenNetType, const std::string &ip, const WORD port);
        DWORD connType;
        BYTE  netType;
        std::string bindIP;
        WORD  bindPort;
    };
    class nListenConfig{
        public:
            nListenConfig();

        public:
            bool parseConfig(const std::string &data);
            const std::vector<nListenData>& getListenDataList() const;

        private:
            std::vector<nListenData> _listen_data_list;
    };
	class SocketPortData
	{
		public:
			SocketPortData(const DWORD connType, const BYTE listenNetType, int serverSocket, const std::string &bindIP, int bindPort, const char *serverPath, const BYTE zipMethodData, const bool checkHeartTick, const DWORD heartTickTimer, const bool quickAck, CommandCtl* ctl) :
                taskConnType(connType),
                netType(listenNetType),
				socket(serverSocket),
                ip(bindIP),
				port(bindPort),
				path(serverPath),
				zipMethod(zipMethodData),
				needCheckHeartTick(checkHeartTick),
				heartTickTimerInMsec(heartTickTimer),
				needQuickAck(quickAck),
				commandCtl(ctl)
		{
		}

		public:
            DWORD taskConnType;
            BYTE netType;
			int socket;
            std::string ip;
			const int port;
			std::string path;
			const BYTE zipMethod;
			bool needCheckHeartTick;
			DWORD heartTickTimerInMsec;
			bool needQuickAck;
			CommandCtl* commandCtl;
	};
};

#endif
