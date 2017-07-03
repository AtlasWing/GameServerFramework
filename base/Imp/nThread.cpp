#include "base/nRandom.h"
#include "base/nThread.h"
#include "base/nLogger.h"
#include "base/nBaseServer.h"
#include "base/nBaseMonitorService.h"
#include "base/Imp/nTaskThreadManager.h"

namespace Tidy
{
	__thread unsigned int seedp = 0;
	long nThread::_tempid = 0;
	__thread nThread* nThread::_running_thread = NULL;

	nThread::nThread(const char *name, bool joinable):
		_name(name),
		_joinable(joinable),
		_thread(0),
		_run_timeout_times(0),
		_alive(false),
		_complete(false),
		_running(true),
		_need_monitor(true),
		_thread_rand_seed(NULL)
	{
		_id = ++_tempid;
		baseServer->baseMonitorService->addThread(this);
	}

	nThread::~nThread()
	{
		if(baseServer->baseMonitorService != NULL){
			baseServer->baseMonitorService->removeThread(this);
		}
		_thread = 0;
	}

	const DWORD nThread::getID() const
	{
		return _id;
	}

	const char* nThread::getName() const
	{
		return _name.c_str();
	}

	nLogger* nThread::getLogger() const
	{
		return baseServer->getLogger();
	}

	const DWORD nThread::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const
	{
		return 0;
	}

	bool nThread::checkRunning()
	{
		if(checkNeedMonitor() == false){
			return true;
		}
		if(!isAlive()){
			return true;
		}
		if(_running){
			_running = false;
			return true;
		}
		return false;
	}

	void *nThread::threadProc(void *arg)
	{
		nThread *thread = reinterpret_cast<nThread*>(arg);

		//运行线程的主回调函数
		thread->dealStartRun();
		thread->run();

		thread->dealEndRun();
		//如果不是_joinable，需要回收线程资源
		if (!thread->isJoinable()) {
			SAFE_DELETE(thread);
		}

		::pthread_exit(NULL);
	}

	bool nThread::start()
	{
		_mlock.lock();
		if(!_alive) {
			pthread_attr_t threadAttr;
			pthread_attr_init(&threadAttr);
			if(_joinable == false){
				pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
			}
			int ret = ::pthread_create(&_thread, &threadAttr, threadProc, this);
			if(0 != ret) {
				baseServer->error("启动线程:%s失败,错误号:%d", this->getName(), ret);
				_mlock.unlock();
				return false;
			}
		}
		else {
			_mlock.unlock();
		}
		return true;
	}

	void nThread::join(const bool needCheckCoreThread)
	{
		if(needCheckCoreThread == true && baseServer->isCoreThread(this) == true){
			return;
		}
		this->info("关闭线程");
		_complete = true;
		if (0 != _thread && _joinable) {
			::pthread_join(_thread, NULL);
			_thread = 0;
			_mlock.lock();
			while(_alive) {
				_cond.wait(_mlock);
			}
			_mlock.unlock();
		}
	}

	pthread_t nThread::getThreadID()
	{
		return _thread;
	}

	void nThread::sleep(const long sec)
	{
		::sleep(sec);
	}

	void nThread::msleep(long millis)
	{
		::usleep(1000 * millis);
	}

	void nThread::usleep(const long usec)
	{
		::usleep(usec);
	}

	void nThread::stop()
	{
		this->info("关闭线程");
		_complete = true;
	}

	const bool nThread::isFinal() const 
	{
		return _complete;
	}

	DWORD nThread::getRunTimeOutTimes() const
	{
		return _run_timeout_times;
	}

	void nThread::incRunTimeOutTimes()
	{
		++ _run_timeout_times;
		baseServer->fatal("线程超时[%s,%u]", _name.c_str(), _run_timeout_times);
	}

	void nThread::clearRunTimeOutTimes()
	{
		_run_timeout_times = 0;
	}

	long nThread::atomicAdd(volatile long * val, long x)
	{
		return __gnu_cxx::__exchange_and_add((volatile _Atomic_word *)val, x ) + x;
	}

	long nThread::atomicInc(volatile long * val)
	{
		return atomicAdd(val,1);
	}

	long nThread::atomicDec(volatile long * val)
	{
		return atomicAdd(val,-1);
	}

	bool nThread::bindRunCpu(const DWORD cpuIndex)
	{
		cpu_set_t cpuSet;
		pthread_getaffinity_np(pthread_self(), sizeof(cpuSet), &cpuSet);
		__CPU_SET_S(cpuIndex, sizeof(cpuSet), &cpuSet);
		if(pthread_setaffinity_np(pthread_self(), sizeof(cpuSet), &cpuSet) < 0){
			return false;
		}
		return true;
	}

	const bool nThread::isAlive() const
	{       
		return _alive;
	}  

	const bool nThread::isJoinable() const
	{
		return _joinable;
	}

	static void ctrlcHandler(int signum)
	{
		baseServer->dealCtrlcSig();
	}

	static void hupHandler(int signum)
	{
		baseServer->setNeedCheckHup();
	}

	static void segvHandler(int sig)
	{
		baseServer->dealCore(sig, NULL, NULL);
	}

	struct SigActionData{
		int signo;
		void (*handler)(int);
	};
	static SigActionData signalsActionList[] = {
		{SIGABRT, segvHandler},
		{SIGBUS, segvHandler},
		{SIGFPE, segvHandler},
		{SIGILL, segvHandler},
		{SIGIOT, segvHandler},
		{SIGSEGV, segvHandler},
		{SIGINT, ctrlcHandler},
		{SIGQUIT, ctrlcHandler},
		{SIGTERM, ctrlcHandler},
		{SIGHUP, hupHandler},
	};
	bool nThread::setSigaction()
	{
		struct sigaction sig;
		for(DWORD i = 0; i < (sizeof(signalsActionList)/sizeof(signalsActionList[0])); ++ i){
			memset(&sig, 0, sizeof(struct sigaction));
			sig.sa_handler = signalsActionList[i].handler;
			sigemptyset(&sig.sa_mask);
			sigaction(signalsActionList[i].signo, &sig, NULL);
		}

		return true;
	}

	nThread* nThread::getRunningThread()
	{
		return nThread::_running_thread;
	}

	void nThread::resetSigaction(int signo)
	{
		struct sigaction act;
		memset(&act, 0, sizeof(struct sigaction));
		sigemptyset (&act.sa_mask);
		act.sa_handler = SIG_DFL;
		sigaction(signo, &act, NULL);
	}

	void nThread::dealStartRun()
	{
		_running_thread = this;

		_alive = true;
		_cond.broadcast();
		_mlock.unlock();

		//设置线程信号处理句柄
		sigset_t sig_mask;
		sigemptyset(&sig_mask);
		pthread_sigmask(SIG_SETMASK, &sig_mask, NULL);

		_thread_rand_seed = &Tidy::seedp;
		*_thread_rand_seed = Tidy::urandomBetween(0, (DWORD)-1);
	}

	void nThread::dealEndRun()
	{
		_mlock.lock();
		_alive = false;
		_cond.broadcast();
		_mlock.unlock();

		_thread_rand_seed = NULL;
	}

	void nThread::updateSeed(const DWORD seed)
	{
		if(_thread_rand_seed != NULL){
			*_thread_rand_seed = seed;
		}
	}
	void nThread::setNeedMonitor()
	{
		_need_monitor = true;
	}

	void nThread::clearNeedMonitor()
	{
		_need_monitor = false;
	}

	bool nThread::checkNeedMonitor() const
	{
		return _need_monitor;
	}
};
