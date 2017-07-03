#ifndef BASE_IMP_NTHREADMONITOR_H_
#define BASE_IMP_NTHREADMONITOR_H_

#include "base/nThread.h"
#include "base/nTimer.h"

namespace Tidy
{
	class nThreadManager;

	class nThreadMonitor : public nThread
	{
		public:
			nThreadMonitor(nThreadManager *threadManager, const DWORD timeoutTimeInMsec, const DWORD maxTimeoutTimes, const DWORD resetSeedTimeInSec);
			~nThreadMonitor();

		public:
			DWORD getNextRandValue();

		private:
			virtual void run();

		private:
			nThreadManager *_thread_manager;
			nTimer _check_timer;
			DWORD _max_timeout_times;
			FILE *_random_dev_file;
	};
};
#endif
