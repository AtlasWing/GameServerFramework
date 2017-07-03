#ifndef BASE_IMP_NLOGGERTHREAD_H_
#define BASE_IMP_NLOGGERTHREAD_H_

#include "base/nThread.h"
#include "base/nTimer.h"

namespace Tidy
{
	class nLoggerThread : public nThread
	{
		public:
			nLoggerThread();
			~nLoggerThread();

		public:
			bool addLogger(Tidy::nLogger *logger);
			bool removeLogger(Tidy::nLogger *logger);
			void flushAll();
			void dealHupSig();

		private:
			virtual void run();

		public:
			void loadConfig();

		private:
			Tidy::nMutex _logger_list_lock;
			Tidy::vector<nLogger*> _logger_list;
			DWORD _sync_time_in_msec;
	};
};
#endif
