#include "base/Imp/nThreadManager.h"

namespace Tidy
{
	nThreadManager::nThreadManager()
	{
	}

	nThreadManager::~nThreadManager()
	{
	}

	bool nThreadManager::addThread(nThread *thread)
	{
		nScopeWRLock lock(_thread_manager_lock);
		return add(thread);
	}

	void nThreadManager::removeThread(nThread *thread)
	{
		nScopeWRLock lock(_thread_manager_lock);
		remove(thread);
	}
};
