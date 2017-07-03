#include "base/Imp/nThreadMonitor.h"
#include "base/Imp/nThreadManager.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	nThreadMonitor::nThreadMonitor(nThreadManager *threadManager, const DWORD timeoutTimeInMsec, const DWORD maxTimeoutTimes, const DWORD resetSeedTimeInSec):
		nThread("nThreadMonitor", true),
		_thread_manager(threadManager),
		_check_timer(timeoutTimeInMsec),
		_max_timeout_times(maxTimeoutTimes),
		_random_dev_file(NULL)
	{
		assert_debug(timeoutTimeInMsec > 0 && maxTimeoutTimes > 0, "%u, %u", timeoutTimeInMsec, maxTimeoutTimes);
		_random_dev_file = fopen("/dev/urandom", "r");
		assert_debug(_random_dev_file != NULL, "打开失败");
	}

	nThreadMonitor::~nThreadMonitor()
	{
		fclose(_random_dev_file);
	}

	namespace{
		struct CheckThreadRunExec{
			nThreadMonitor *_thread_monitor;
			DWORD _max_timeout_times;
			CheckThreadRunExec(nThreadMonitor *threadMonitor, const DWORD maxTimeoutTimes) : _thread_monitor(threadMonitor), _max_timeout_times(maxTimeoutTimes){
			}
			bool exec(nThread *thread){
				if(thread->checkRunning() == false){
					if(thread->getRunTimeOutTimes() > _max_timeout_times){
						baseServer->dealThreadRunTimeOut(thread);
						thread->clearRunTimeOutTimes();
					}
					else{
						thread->incRunTimeOutTimes();
					}
				}
				else if(thread->isAlive() == true){
					thread->clearRunTimeOutTimes();
					thread->updateSeed(_thread_monitor->getNextRandValue());
				}
				return true;
			}
		};
	};

	void nThreadMonitor::run()
	{
		nTime currentTime;
		CheckThreadRunExec checkThreadRunExec(this, _max_timeout_times);

		while(!isFinal()){
			this->setRunning();
			currentTime.now();

			if(_check_timer(currentTime) == true){
				_thread_manager->execEveryThread(checkThreadRunExec);
			}

			nThread::msleep(ONE_SEC);
		}
	}

	DWORD nThreadMonitor::getNextRandValue()
	{
		DWORD value = 0;
		fread(&value, sizeof(value), 1, _random_dev_file);
		return value;
	}
};
