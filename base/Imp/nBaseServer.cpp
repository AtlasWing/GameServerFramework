#include "base/nSys.h"
#include "base/nThread.h"
#include "base/nBaseServer.h"
#include "base/nConfigFile.h"
#include "base/nServerConnManager.h"
#include "base/nBaseTickService.h"
#include "base/nBaseMessageService.h"
#include "base/nBaseMonitorService.h"
#include "base/nDebugger.h"

#include "base/Imp/nArgp.h"
#include "base/Imp/nThreadMonitor.h"
#include "base/Imp/nListenService.h"
#include "base/Imp/nClientConnService.h"
#include "base/Imp/nTaskConnService.h"
#include "base/Imp/nTaskThreadManager.h"


namespace Tidy
{
	const char *argp_program_version;
	nBaseServer* baseServer = NULL;
	nParamMap global; 
	const DWORD MaxServerIFNum = 512;

	nBaseServer::nBaseServer(const char* serverName, const char* logPrefixName, const DWORD serverType, const DWORD codeVersion, const bool queueCmd) : 
		baseImpCodeVersion(1),
		serverCodeVersion(codeVersion), 
		baseMessageService(NULL),
		taskConnService(NULL), 
		clientConnService(NULL),
		baseMonitorService(NULL),
		baseTickService(NULL),
		_server_ID(0), 
		_server_type(serverType),
		_server_name(serverName),
		_server_logger(NULL),
		_listen_service(NULL), 
		_thread_monitor(NULL),
		_startup_complete(false), 
		_close_reason(eServerCloseReason_None),
		_is_in_debug_mode(true),
		_need_core(true),
		_core_thread(NULL),
		_start_time_in_sec(0),
		_argv(NULL),
        _env_data_map(NULL),
        _need_analyze(false),
		_check_hup(false)
	{
		baseServer = this;
		argp_program_version = new char[MAXNAMLEN];
		snprintf((char*)argp_program_version, MAXNAMLEN, "%s baseImpVersion=%u serverVersion=%u", serverName, baseImpCodeVersion, serverCodeVersion);

		_server_logger = new nLogger(logPrefixName, true);

		_listen_service = new nListenService();
        nTime::setTimeZone();
        Tidy::seedp = Tidy::urandomBetween(0, (DWORD)-1);

		_now_time.now();
		_start_time_in_sec = _now_time.sec();
	}

	nBaseServer::~nBaseServer()
	{
		delete []argp_program_version;
		SAFE_DELETE(_server_logger);
	}

	void nBaseServer::setNeedCheckHup()
	{
		_check_hup = true;
	}

	void nBaseServer::dealCtrlcSig()
	{
		terminate(eServerCloseReason_Normal);
	}

	void nBaseServer::dealCore(int sig, siginfo_t *info, void *secret)
	{
		dealCore(sig, info, secret, true);
	}

	void nBaseServer::dealCore(int sig, siginfo_t *info, void *secret, const bool needCloseServer)
	{
		// 重置默认信号处理,如果在关闭的过程中宕机,就不做处理了,直接宕掉
		baseMonitorService->dealCore();
		nThread::resetSigaction(sig);
		if(checkInDebugMode() == true) {
			raise(SIGSEGV);
			return;
		}

		if(isRunningTick() == true){
			restartTimeTick();
		}

		fatal("core");
		std::string backTraceResult = nDebugger::backTrace();
		fatal("O~O~O,Core Down [%s]", backTraceResult.c_str());

		if(sig != SIGSEGV){
			nThread::resetSigaction(SIGSEGV);
		}
		if(_core_thread == NULL){
			_core_thread = nThread::getRunningThread();
			if(needCloseServer == true){
				terminate(eServerCloseReason_Error);
			}
			if(_core_thread != NULL){
				DWORD sleepTimeInSec = 0;
				DWORD CORE_THREAD_WAIT_TIME_IN_SEC = Tidy::global["CORE_THREAD_WAIT_TIME_IN_SEC"].getDword();
				if(CORE_THREAD_WAIT_TIME_IN_SEC == 0){
					CORE_THREAD_WAIT_TIME_IN_SEC = 60;
				}
				while(_core_thread->isFinal() == false && sleepTimeInSec < CORE_THREAD_WAIT_TIME_IN_SEC){
					_core_thread->setRunning();
					nThread::sleep(1);
					++ sleepTimeInSec;
				}
				debug("等待服务器关闭时间[%u]s", sleepTimeInSec);
			}
		}
		raise(SIGSEGV);
	}

	void nBaseServer::setInDebugMode(bool isInDebugMode)
	{
		_is_in_debug_mode = isInDebugMode;
	}

	bool nBaseServer::checkInDebugMode() const
	{
		return _is_in_debug_mode;
	}

	void nBaseServer::setNeedCore(const bool needCore)
	{
		_need_core = needCore;
	}

	void nBaseServer::dealThreadRunTimeOut(nThread *thread)
	{
		// 方便查看堆栈
		assert_fail("线程超时[%s, %u]", thread->getName(), thread->getID());
		assert(false);
	}

	void nBaseServer::dealAssertFail(const bool needCore, const char *funcName, const char *fileName, const int line, const char *expr, const char * desc)
	{
		fatal("ASSERT FAIL. expr=[%s], desc=[%s], FILE=[%s : %d] funcName=[%s]", expr, desc, fileName, line, funcName);
		if(needCore == true && _need_core == true){
			_server_logger->flushToFile(true);
			assert(false);
		}
	}

	bool nBaseServer::dealNewTaskConn(const int sock, const struct sockaddr *addr, const SocketPortData* listenPortData)
	{
		nTaskConn* conn = NULL;
		conn = createTaskConn(sock, addr, listenPortData);
		if(conn == NULL){
			assert_fail("无法创建连接");
			return false;
		}
		if(taskConnService->addToVerify(conn) == false) {
			assert_fail("添加到验证线程失败");
			SAFE_DELETE(conn);
			return false;
		}

		return true;
	}

	bool nBaseServer::parseBaseServerOptions(int key, char *arg, struct argp_state *state)
	{
		switch (key)
		{
			case 'd':
				{
					Tidy::global.set("daemon", "true");
				}
				break;
			case 'l':
				{
					Tidy::global.set("log", arg);
				}
				break;
			case 'f':
				{
					Tidy::global.set("logFile", arg);
				}
				break;
			case 'y':
				{
					Tidy::global.set("mysql", arg);
				}
				break;
			case 'i':
				{
					Tidy::global.set("ifname", arg);
				}
				break;
			case 'c':
				{
					Tidy::global.set("config", arg);
				}
				break;
			default:
				return false;
		}
		return true;
	}

	static error_t parseServerOpt(int key, char *arg, struct argp_state *state)
	{
		if(baseServer->parseOptions(key, arg, state) == true)
		{
			return 0;
		}

		return ARGP_ERR_UNKNOWN;
	}

	static struct argp_option baseServerOptions[] =
	{
		{"daemon",		'd',	0,			0,	"Run service as daemon",						0},
		{"log",			'l',	"level",	0,	"Log level",									0},
		{"logfilename",	'f',	"filename",	0,	"Log file name",								0},
		{"mysql",		'y',	"mysql",	0,	"MySQL[mysql://user:passwd@host:port/dbName]",	0},
		{"ifname",		'i',	"ifname",	0,	"Local network device",							0},
		{"config",		'c',	"config",	0,	"config dir",							        0},
		{0,				0,		0,			0,	0,												0}
	};

	bool nBaseServer::parseMainArgs(int argc, char **argv)
	{
		_argc = argc;
		_argv = argv;
		nArgp::getInstance().add(serverArgOptions, parseServerOpt, 0, Tidy::serverDoc);
		nArgp::getInstance().addOptions(baseServerOptions);
		if(nArgp::getInstance().parse(argc, argv) == false)
		{
			return false;
		}

		return true;
	}

	bool nBaseServer::parseOptions(int key, char *arg, struct argp_state *state)
	{
		if(parseServerOptions(key, arg, state) == true)
		{
			return true;;
		}
		return parseBaseServerOptions(key, arg, state);
	}

	bool nBaseServer::parseServerOptions(int key, char *arg, struct argp_state *state)
	{
		return false;
	}

	void nBaseServer::terminate(eServerCloseReason reason)
	{
		assert_debug(reason != eServerCloseReason_None, "[%d, %d]", _close_reason, reason);
		_close_reason = reason;
	}

	bool nBaseServer::isTerminate() const
	{
		return (_close_reason != eServerCloseReason_None);
	}

	bool nBaseServer::isAbnormalTerminate() const
	{
		return _core_thread != NULL;
	}

	bool nBaseServer::isStartupComplete() const
	{
		return _startup_complete;
	}

	bool nBaseServer::setResourceLimit()
	{
		struct rlimit rlim;
		if (getrlimit(RLIMIT_CORE, &rlim) == -1) {
			assert_fail("获取CORE文件限制失败");
			return false;
		}
		if(rlim.rlim_cur == RLIM_INFINITY && rlim.rlim_max == RLIM_INFINITY){
			return true;
		}
		struct rlimit rlim_new;
		rlim_new.rlim_cur = rlim_new.rlim_max = RLIM_INFINITY;
		if (setrlimit(RLIMIT_CORE, &rlim_new) != 0) {
			rlim_new.rlim_cur = rlim_new.rlim_max = rlim.rlim_max;
			setrlimit(RLIMIT_CORE, &rlim_new);
		}

		if ((getrlimit(RLIMIT_CORE, &rlim) != 0) || rlim.rlim_cur == 0) {
			assert_fail("设置core限制失败");
			return false;
		}
		return true;
	}

	bool nBaseServer::initBaseServer()
	{
		srand( ::time(NULL) ^ getpid());

		char compileTime[128];
		bzero(compileTime, sizeof(compileTime));
		struct tm tv1;
		snprintf(compileTime, sizeof(compileTime), "%s %s", __DATE__, __TIME__);
		strptime(compileTime, "%b %d %Y %H:%M:%S", &tv1);
		strftime(compileTime, sizeof(compileTime), "%Y-%m-%d %T", &tv1);

		debug("编译时间:[%s]", compileTime);
		debug("base库版本:[%u]", baseImpCodeVersion);
		debug("编译svn版本:[%u]", serverCodeVersion);

		if(initLogger() == false) {
			return false;
		}
		if(setResourceLimit() == false){
			assert_fail("设置资源限制失败");
			return false;
		}
		if(initDaemon() == false) {
			return false;
		}
		if(nThread::setSigaction() == false) {
			return false;
		}

		return true;
	}

	bool nBaseServer::initLogger()
	{
		_server_logger->setLogLevel(Tidy::global["LOG_LEVEL"].getCString());
		if(Tidy::global["NEED_SYS_LOG"].getDword() > 0){
			_server_logger->setNeedSysLog();
		}
		//设置写本地日志文件
		if(Tidy::global["LOG_FILE"].empty() == false) {
			_server_logger->initLogFile(Tidy::global["LOG_FILE"].getCString(), 60);
			debug("初始化日志文件名:[%s]", Tidy::global["LOG_FILE"].getCString());
			//防止调用两次
			Tidy::global.erase("LOG_FILE");
		}

		return true;
	}

	bool nBaseServer::doBaseTick()
	{
		if(_check_hup == true) {
			dealHupSig();
			baseMonitorService->dealHupSig();
			_check_hup = false;
		}
		baseMonitorService->doTick();
		return true;
	}

	bool nBaseServer::initDaemon()
	{
		_run_info.ppID = getppid();

		//是否以后台进程的方式运行
		if(0 == strcmp("true", Tidy::global["daemon"])) {
			//防止调用两次
			Tidy::global.set("daemon", "");
			info("Program will be run as a daemon");
			_server_logger->clearNeedConsole();
			daemon(1, 0);
		}

		return true;
	}

	bool nBaseServer::serverMain()
    {
        TEST_TIME_INIT();
        TEST_TIME_BEGIN();

        if(initServer() == false) {
			return false;
		}

		if(initBaseServer() == false) {
			return false;
		}

		if(fillRunInfo() == false) {
			return false;
		}

		if(initBaseService() == false) {
			return false;
		}

		if(startServer() == false) {
			return false;
		}

		if(startTimeTick() == false) {
			return false;
		}

		if(startNetListen() == false) {
			return false;
		}

		if(completeBaseStartup() == false) {
			return false;
		}

		if(completeStartup() == false) {
			return false;
		}
        TEST_TIME_END(debug, "服务器启动过程");

        while(isTerminate() == false) {
            if (acceptConn() == false) {
                break;
            }
        }

        finalBaseServer();
		finalServer();
		finalBaseService();

		if(_core_thread != NULL){
			_core_thread->join(false);
			// 处理ThreadMonitor宕机的问题
			assert(false);
		}

		// 放最后,是避免服务器启动和关闭流程混在一块,这样保证检查出端口冲突
		stopNetListen();
		return true;
	}

	bool nBaseServer::restartTimeTick()
	{
		baseTickService = NULL;
		return startTimeTick();
	}

	bool nBaseServer::startTimeTick()
	{
		assert_debug(baseTickService == NULL, "");
		baseTickService = createTickService();
		if(baseTickService == NULL) {
			return false;
		}
		if(baseTickService->start() == false) {
			SAFE_DELETE(baseTickService);
			return false;
		}
		return true;
	}

	void nBaseServer::stopTimeTick()
	{
		if(baseTickService == NULL) {
			return;
		}
		baseTickService->join();
		SAFE_DELETE(baseTickService);
	}

	bool nBaseServer::completeBaseStartup()
	{
		baseMonitorService->releaseMemory(true);
		_startup_complete = true;
		return true;
	}

	bool nBaseServer::initBaseService()
	{
		baseMonitorService = createMonitorService();
		if(baseMonitorService->initService() == false){
			SAFE_DELETE(baseMonitorService);
			return false;
		}

		if(createConnManager() == false) {
			return false;
		}

		taskConnService = new nTaskConnService(Tidy::global["taskConnSyncDataTimerInUSec"].getDword());
		if(taskConnService->initService() == false) {
			return false;
		}

		clientConnService = new nClientConnService(Tidy::global["clientConnSyncDataTimerInUSec"].getDword());
		if(clientConnService->initService() == false) {
			return false;
		}

		baseMessageService = createMessageService();
		if(baseMessageService == NULL || baseMessageService->init() == false) {
			SAFE_DELETE(baseMessageService);
			return false;
		}

		return true;
	}

	bool nBaseServer::startNetListen()
	{
		return _listen_service->startListen();
	}

	bool nBaseServer::acceptConn()
	{
		return _listen_service->acceptConn();
	}

	void nBaseServer::finalBaseServer()
	{
		destroyConnPool();
		stopTimeTick();
		destroyConnManager();
	}

	void nBaseServer::stopNetListen()
	{
		SAFE_DELETE(_listen_service);
	}

	void nBaseServer::destroyConnPool()
	{
		taskConnService->finalService();
		SAFE_DELETE(taskConnService);

		clientConnService->finalService();
		SAFE_DELETE(clientConnService);
	}

	nBaseMonitorService* nBaseServer::createMonitorService()
	{
		return new nBaseMonitorService();
	}

	nDayTime& nBaseServer::time()
	{
		return _now_time;
	}

	void nBaseServer::now()
	{
		_now_time.now();
	}

	bool nBaseServer::parseCommand(nServerConn* serverConn, const void *nullCommand, const DWORD len)
	{
		return baseMessageService->parseCommand(serverConn, nullCommand, len);
	}

	void nBaseServer::setServerInfo(const WORD serverID)
	{
		this->_server_ID = serverID;
	}

	DWORD nBaseServer::getServerID() const
	{
		return _server_ID;
	}

	void nBaseServer::setServerID(const DWORD serverID)
	{
		this->_server_ID = serverID;
	}

	DWORD nBaseServer::getServerType() const
	{
		return _server_type;
	}

	const char* nBaseServer::getServerName() const
	{
		return _server_name.c_str();
	}

	bool nBaseServer::getAllIPAddr(Tidy::vector<char>& ipAddrList)
	{
		struct ifreq ifr[2*MaxServerIFNum];
		struct ifconf ifc;
		ifc.ifc_len = sizeof(ifr);
		ifc.ifc_ifcu.ifcu_buf = (char*)ifr;

		int fd = socket(AF_INET, SOCK_DGRAM, 0);

		ioctl(fd, SIOCGIFCONF, &ifc);

		DWORD num = ifc.ifc_len / sizeof(struct ifreq);
		ipAddrList.resize(num * MAX_IP_LENGTH);
		if(num > MaxServerIFNum)
		{
			assert_fail("超过最多支持网卡数量 [%u,%u]", num, MaxServerIFNum);
			num = MaxServerIFNum;
		}

		struct sockaddr *sa;
		for (DWORD i = 0; i < num; ++i) 
		{
			char* ipAddr = &ipAddrList[i * MAX_IP_LENGTH];
			sa=(struct sockaddr *)&(ifr[i].ifr_addr);
			switch(sa->sa_family)
			{
				case AF_INET6:
					inet_ntop(AF_INET6,(struct in6_addr *)&(((struct sockaddr_in6 *)sa)->sin6_addr), ipAddr, INET6_ADDRSTRLEN);
					break;
				default : copyStr(ipAddr,inet_ntoa(((struct sockaddr_in *)sa)->sin_addr), MAX_IP_LENGTH);
			}
		}
		close(fd);

		return true;
	}

	bool nBaseServer::checkLocalIP(const char *ip) const
	{
		DWORD addr = inet_addr(ip);
		struct ifreq ifr[2*MaxServerIFNum];
		struct ifconf ifc;
		ifc.ifc_len = sizeof(ifr);
		ifc.ifc_ifcu.ifcu_buf = (char*)ifr;

		int fd = socket(AF_INET, SOCK_DGRAM, 0);

		ioctl(fd, SIOCGIFCONF, &ifc);

		DWORD num = ifc.ifc_len / sizeof(struct ifreq);
		if(num > MaxServerIFNum){
			assert_fail("超过最多支持网卡数量");
			num = MaxServerIFNum;
		}

		struct sockaddr *sa;
		bool isLocal = false;
		for (DWORD i = 0; i < num; ++i){
			sa=(struct sockaddr *)&(ifr[i].ifr_addr);
			switch(sa->sa_family){
				case AF_INET:
					{
						isLocal = ((((struct sockaddr_in *)sa)->sin_addr.s_addr) == addr);
					}
				default:
					{
					}
			}
			if(isLocal == true){
				break;
			}
		}
		close(fd);

		return isLocal;
	}

	void nBaseServer::wait(const DWORD millSec)
	{
		usleep(millSec * 1000);
	}

	const DWORD nBaseServer::getID() const
	{
		return _server_ID;
	}

	nLogger* nBaseServer::getLogger() const
	{
		return _server_logger;
	}

	const DWORD nBaseServer::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const
	{
		return 0;
	}

	bool nBaseServer::addTcpServer(const nListenData &listenData, const BYTE zipMethod, const bool checkHeartTick, const DWORD heartTickTimerInMsec, const bool needQuickAck, CommandCtl* ctl)
	{
		assert_debug(checkHeartTick == false || heartTickTimerInMsec > 0, "[%u]", heartTickTimerInMsec);
		return _listen_service->addTcpServer(listenData, zipMethod, checkHeartTick, heartTickTimerInMsec, needQuickAck, ctl);
	}

	bool nBaseServer::addUnixServer(const DWORD connType, const char *unixSocketPath, const BYTE zipMethod, const bool checkHeartTick, const DWORD heartTickTimerInMsec, CommandCtl* ctl)
	{
		return _listen_service->addUnixServer(connType, unixSocketPath, zipMethod, checkHeartTick, heartTickTimerInMsec, ctl);
	}

	bool nBaseServer::addClientConnToPool(nClientConn *task)
	{
		return clientConnService->addToConnect(task);
	}

	void nBaseServer::finalBaseService()
	{
		baseMessageService->final();
		SAFE_DELETE(baseMessageService);

		baseMonitorService->finalService();
		SAFE_DELETE(baseMonitorService);
	}

	bool nBaseServer::fillRunInfo()
	{
		_run_info.pID = getpid();
		DWORD userID = getuid();
		struct passwd *userInfo = getpwuid(userID);
		if(userInfo != NULL)
		{
			_run_info.userName = userInfo->pw_name;
		}
		getAllIPAddr(_run_info.localIPList);
		_run_info.localIPNum = _run_info.localIPList.size() / MAX_IP_LENGTH;
		return true;
	}

	void nBaseServer::checkMemoryLeak()
	{
		//TODO dup2修改STDERR_FILENO到文件
		char errFileName[512];
		DWORD len = snprintf(errFileName, sizeof(errFileName), "/tmp/memLeak-%s--", getServerName());
		len += const_cast<nDayTime&>(time()).format(&errFileName[len], sizeof(errFileName)-len, "%y%m%d-%T");
		len += snprintf(&errFileName[len], sizeof(errFileName)-len, ".log");

		int fd = open(errFileName, O_RDWR|O_CREAT, 0666);
		int old = dup2(fd, STDERR_FILENO);
		if(HeapLeakChecker::NoGlobalLeaks() == false)
		{
			dup2(old, STDERR_FILENO);
			close(fd);
			debug("ThirdParty/google-perftools-1.7/bin/pprof ./%s/%s \"/tmp/%s.%u._main_-end.heap\" --inuse_objects --lines --heapcheck --edgefraction=1e-10 --nodefraction=1e-10", getServerName(), getServerName(), getServerName(), _run_info.pID);
			assert_fail("内存泄露");
		}
		else
		{
			dup2(old, STDERR_FILENO);
			close(fd);
			remove(errFileName);
		}

		HeapLeakChecker::CancelGlobalCheck();
	}

	const ServerRunInfo& nBaseServer::getRunInfo() const
	{
		return _run_info;
	}

	void nBaseServer::addAdjustServerTime(const SDWORD adjustTimeInSec)
	{
		nTime::addAdjustTime(adjustTimeInSec);
		now();
		dealAjustTimeChange();
	}

	void nBaseServer::setAdjustServerTime(const SDWORD adjustTimeInSec)
	{
		nTime::setAdjustTime(adjustTimeInSec);
		now();
		dealAjustTimeChange();
	}

	SDWORD nBaseServer::getAdjustServerTime() const
	{
		return nTime::adjustTime;
	}

	Tidy::string nBaseServer::getServerTimeStr()
	{
		char timeBuffer[48];
		const_cast<nDayTime&>(time()).format(timeBuffer, sizeof(timeBuffer), "%y%m%d-%T");
		return timeBuffer;
	}

	DWORD nBaseServer::getStartTimeInSec() const
	{
		return _start_time_in_sec;
	}

	bool nBaseServer::setProcessName(const char* processName)
	{
		DWORD oldLen = strlen(getProcessName());
		DWORD newLen = strlen(processName);
		// 传入参数
		char argv[512] = {'\0'};
		DWORD argvLen = 0;
		for(int j = 1; j < _argc; ++ j){
			argvLen += snprintf(argv + argvLen, sizeof(argv) - argvLen, "%s", _argv[j]);
		}
		newLen += argvLen;

		for(int j = 1; j < _argc; ++ j){
			snprintf(_argv[j] + newLen - oldLen + 1, strlen(_argv[j]) + 1, "%s", _argv[j]);
			_argv[j] = _argv[j] + newLen - oldLen + 1;
		}

		if(oldLen >= newLen){
			// 使用strncpy是为了自动填充0
			snprintf(_argv[0], oldLen, "%s %s", processName, argv);
			return true;
		}

		static char *newEnv = NULL;
		static DWORD allSize = 0;
		if(newEnv == NULL){
			// 环境变量改地址,使用环境变量原有的内存存储进程名字
			DWORD i = _argc + 1;
			while(_argv[i] != NULL){
				allSize += (strlen(_argv[i]) + 1);
				++ i;
			}

			newEnv = new char[allSize];
			DWORD len = 0;
			i = _argc + 1;
			while(_argv[i] != NULL){
				char *newArgv = newEnv+len;
				len += (snprintf(newArgv, allSize - len, "%s", _argv[i]) + 1);
				_argv[i] = newArgv;
				++ i;
			}

			allSize += oldLen + 1;
		}

		// 使用环境变量和原有的存储传入参数的原有的内存存储进程名字
		snprintf(_argv[0], allSize, "%s %s", processName, argv);
		return true;
	}

	const char* nBaseServer::getProcessName() const
	{
		return _argv[0];
	}

	const char* nBaseServer::getIPByIfName(const char *ifName)
	{
		int s;
		struct ifreq ifr;
		const static char *none_ip = "0.0.0.0";

		if (NULL == ifName)
			return none_ip;
		s = ::socket(AF_INET, SOCK_DGRAM, 0);
		if (-1 == s) 
			return none_ip;

		bzero(ifr.ifr_name, sizeof(ifr.ifr_name));
		copyStr(ifr.ifr_name, ifName, sizeof(ifr.ifr_name));
		if (-1 == ioctl(s, SIOCGIFADDR, &ifr)) 
		{
			TEMP_FAILURE_RETRY(::close(s));
			return none_ip;
		}

		TEMP_FAILURE_RETRY(::close(s));
		return inet_ntoa(((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr);
	}

	bool nBaseServer::isRunningTick() const
	{
		return nThread::getRunningThread() == baseTickService;
	}

	bool nBaseServer::isCoreThread(const nThread *thread) const
	{
		return _core_thread == thread;
	}

	int nBaseServer::main(int argc, char **argv)
	{
		if(baseServer->parseMainArgs(argc, argv) == false){
			return 1;
		}

		if(baseServer->serverMain() == false){
			SAFE_DELETE(_server_logger);
			return 0;
		}

		baseServer->checkMemoryLeak();

		SAFE_DELETE(baseServer);

		return 1;
	}

	void nBaseServer::dealAjustTimeChange()
    {
    }

    void nBaseServer::setNeedAnalyze()
    {
        _need_analyze = true;
    }

    void nBaseServer::clearNeedAnalyze()
    {
        _need_analyze = false;
    }

    bool nBaseServer::checkNeedAnalyze() const
    {
        return _need_analyze;
    }

	void nBaseServer::dealClientConnRecycle(nClientConn *conn)
	{
	}
};
