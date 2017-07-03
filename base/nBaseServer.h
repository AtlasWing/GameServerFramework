#ifndef _N_BASE_SERVER_H_
#define _N_BASE_SERVER_H_

#include "base/nMisc.h"
#include "base/Tidy.h"
#include "base/nSTL.h"
#include "base/nObjLogger.h"
#include "base/nType.h"

namespace Tidy
{
	class nSuperClient;
	class nListenService;
	class nThreadMonitor;
	class nTaskConnService;
	class nClientConnService;
	class nServerConnManager;
	class nServerConn;
	class nTaskConn;
	class nClientConn;
	class nBaseTickService;
	class nBaseMessageService;
	class nBaseMonitorService;
	class nThread;
	class SocketPortData;
	struct CommandCtl;
	class nDBConnection;
    class nListenData;

	struct ServerRunInfo{
		DWORD pID;
		DWORD ppID;
		const char *userName;
		Tidy::vector<char> localIPList;
		BYTE localIPNum;
	};

	enum eServerCloseReason{
		eServerCloseReason_None     =   0,
		eServerCloseReason_Normal   =   1,
		eServerCloseReason_Error    =   2,
	};
	class nBaseServer : public nObjLogger<nBaseServer>
	{
		protected:
			nBaseServer(const char* serverName, const char* logPrefixName, const DWORD serverType, const DWORD serverCodeVersion, const bool queueCmd);

		public:
			virtual ~nBaseServer();

		public:
			int main(int argc, char **argv);
			bool parseMainArgs(int argc, char **argv);
			void terminate(eServerCloseReason reason);
			bool isTerminate() const;
			bool isStartupComplete() const;
			bool parseOptions(int key, char *arg, struct argp_state *state);
			bool completeBaseStartup();
			bool addClientConnToPool(nClientConn *task);
			bool dealNewTaskConn(const int sock, const struct sockaddr *addr, const SocketPortData* listenPortData);
			void setInDebugMode(bool isInDebugMode);
			bool checkInDebugMode() const;
			void setNeedCore(const bool needCore);
			bool isRunningTick() const;
			bool isCoreThread(const nThread *thread) const;
			bool isAbnormalTerminate() const;

		public:
			virtual bool addTaskConnID(const nTaskConn* conn) = 0;
			virtual bool removeTaskConnID(const nTaskConn* conn) = 0;
			virtual bool addTaskConn(nTaskConn* conn) = 0;
			virtual bool removeTaskConn(nTaskConn* conn) = 0;
			virtual bool addSyncTaskConn(nTaskConn* conn) = 0;
			virtual bool removeSyncTaskConn(nTaskConn* conn) = 0;
			virtual nTaskConn* getSyncTaskConn(const DWORD taskConnID) = 0;
			virtual bool addClientConn(nClientConn* conn) = 0;
			virtual bool removeClientConn(nClientConn* conn) = 0;
			virtual void serverConnectSuccess(nServerConn* conn) = 0;
			virtual void serverConnectClose(nServerConn* conn) = 0;

		public:
			virtual bool parseServerOptions(int key, char *arg, struct argp_state *state);
			virtual void dealCtrlcSig();
			virtual void dealThreadRunTimeOut(nThread *thread);
			virtual void dealAssertFail(const bool needCore, const char *funcName, const char *fileName, const int line, const char *expr, const char * desc);
			virtual void dealCore(int sig, siginfo_t *info, void *secret);
			virtual void dealAjustTimeChange();
			virtual void dealClientConnRecycle(nClientConn *conn);

		public:
			virtual bool verifyTaskConn(nTaskConn* conn, const void* packet, const DWORD nCmdLen) = 0;
			virtual bool checkClientSyncResult(nClientConn* conn, const void* packet, const DWORD len) = 0;
			virtual BYTE syncTaskConn(nTaskConn* conn, const void* packet = NULL, const DWORD len=0) = 0;
			virtual bool loginToServer(nServerConn* conn) = 0;
			virtual void dealDBClosed(nDBConnection *connection) = 0;

		public:
			bool parseCommand(nServerConn* serverConn, const void *nullCommand, const DWORD len);

		public:
			nDayTime& time();
			void now();
			Tidy::string getServerTimeStr();
			DWORD getStartTimeInSec() const;

		public:
            void setServerInfo(const WORD serverID);
            DWORD getServerType() const;
			DWORD getServerID() const;
			void setServerID(const DWORD serverID);
			const char* getServerName() const;
			const ServerRunInfo& getRunInfo() const;
			const char* getProcessName() const;
			bool setProcessName(const char* processName);
			const char *getIPByIfName(const char *ifName);
			SDWORD getAdjustServerTime() const;

		public:
			void wait(const DWORD millSec);
			void checkMemoryLeak();
            void setNeedAnalyze();
            void clearNeedAnalyze();
            bool checkNeedAnalyze() const;
			void setNeedCheckHup();

		public:
			const DWORD getID() const;
			nLogger* getLogger() const;
			const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;

		protected:
            bool addTcpServer(const nListenData &listenData, const BYTE zipMethod, const bool checkHeartTick, const DWORD heartTickTimerInMsec, const bool needQuickAck, CommandCtl* ctl);
            bool addUnixServer(const DWORD connType, const char *unixSocketPath, const BYTE zipMethod, const bool checkHeartTick, const DWORD heartTickTimerInMsec, CommandCtl* ctl);
			bool initLogger();
			bool initDaemon();
			bool doBaseTick();
			bool checkLocalIP(const char *ip) const;
			bool restartTimeTick();
			void dealCore(int sig, siginfo_t *info, void *secret, const bool needCloseServer);

		protected:
			void addAdjustServerTime(const SDWORD adjustTimeInSec);
			void setAdjustServerTime(const SDWORD adjustTimeInSec);

		private:
			bool serverMain();
			bool initBaseServer();

		private:
			bool startTimeTick();
			void stopTimeTick();
			bool initBaseService();
			void finalBaseService();
			bool startNetListen();
			void stopNetListen();
			void destroyConnPool();
			bool acceptConn();
			void finalBaseServer();

		private:
			bool parseBaseServerOptions(int key, char *arg, struct argp_state *state);
			bool getAllIPAddr(Tidy::vector<char>& ipAddrList);
			bool fillRunInfo();
			bool setResourceLimit();

		private:
			virtual nBaseMonitorService* createMonitorService();

		private:
			virtual bool initServer() = 0;
			virtual bool startServer() = 0;
			virtual bool finalServer() = 0;
			virtual bool completeStartup() = 0;
			virtual bool createConnManager() = 0;
			virtual void destroyConnManager() = 0;
			virtual nTaskConn* createTaskConn(const int sock, const struct sockaddr *addr, const SocketPortData* listenPortData) const = 0;
			virtual nBaseTickService* createTickService() = 0;
			virtual nBaseMessageService* createMessageService() = 0;
			virtual void dealHupSig() = 0;

		public:
			const DWORD baseImpCodeVersion;
			const DWORD serverCodeVersion;

		public:
			nBaseMessageService *baseMessageService;
			nTaskConnService *taskConnService;
			nClientConnService *clientConnService;
			nBaseMonitorService *baseMonitorService;
			nBaseTickService *baseTickService;

		private:
			WORD _server_ID;
			const DWORD _server_type;
			std::string _server_name;
			char _server_IP[MAX_IP_LENGTH];

		private:
			nLogger *_server_logger;
			nListenService *_listen_service;
			nThreadMonitor *_thread_monitor;

		private:
			bool _startup_complete;
			eServerCloseReason _close_reason;
			bool _is_in_debug_mode;
			bool _need_core;
			nThread *_core_thread;

		private:
			nDayTime _now_time;
			DWORD _start_time_in_sec;

		private:
			ServerRunInfo _run_info;
			int _argc;
			char **_argv;
			nParamMap *_env_data_map;
            bool _need_analyze;
			bool _check_hup;
	};

	extern nBaseServer* baseServer;
	extern struct argp_option serverArgOptions[];
	extern char   serverDoc[];
};

#endif

