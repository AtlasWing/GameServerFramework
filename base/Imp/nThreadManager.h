#ifndef BASE_IMP_NTHREADMANAGER_H_
#define BASE_IMP_NTHREADMANAGER_H_

#include "base/nManager.h"
#include "base/nThread.h"
#include "base/nRWLock.h"

namespace Tidy
{
	class nThreadManager : public nManager<false, nIndexByID< CommonGetIDPolicy<nThread> > >
	{
		public:
			nThreadManager();
			~nThreadManager();

		public:
			bool addThread(nThread *thread);
			void removeThread(nThread *thread);

			template <class ExecT>
				bool execEveryThread(ExecT &exec)
				{
					nScopeRDLock lock(_thread_manager_lock);
					return execAll(exec);
				}

		private:
			nRWLock _thread_manager_lock;
	};
};
#endif
