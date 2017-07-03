#ifndef _N_RWLOCK_H_
#define _N_RWLOCK_H_

#include "base/nSys.h"
#include "base/nAssert.h"
#include "base/nMisc.h"

namespace Tidy
{
	class nRWLock
	{
		public:
			DISALLOW_COPY_AND_ASSIGN(nRWLock);
			nRWLock() { 
				int result = ::pthread_rwlock_init(&_rw_lock, NULL);
				if(result != 0){
					assert_fail("初始锁失败[%d]", result);
				}
			}
			~nRWLock() { 
				int result = ::pthread_rwlock_destroy(&_rw_lock);
				if(result != 0){
					assert_fail("销毁锁失败[%d]", result);
				}
			}
			void rdlock() {
				int result = ::pthread_rwlock_rdlock(&_rw_lock);
				if(result != 0){
					assert_fail("加读锁失败[%d]", result);
				}
				assert_debug(_rw_lock.__data.__nr_readers < 42949600, "");
			}

			void wrlock() {
				int result = ::pthread_rwlock_wrlock(&_rw_lock);
				if(result != 0){
					assert_fail("加写锁失败[%u]", result);
				}
				assert_debug(_rw_lock.__data.__nr_readers < 42949600, "");
			}

			void unlock() {
				int result = ::pthread_rwlock_unlock(&_rw_lock);
				if(result != 0){
					assert_fail("释放锁失败[%d]", result);
				}
				assert_debug(_rw_lock.__data.__nr_readers < 42949600, "");
			}

		private:
			pthread_rwlock_t _rw_lock;
	};

	template <bool lock=true>
		class nRWLocker
		{
			public:
				void rdlock() { _rw_lock.rdlock(); }
				void wrlock() { _rw_lock.wrlock(); }
				void unlock() { _rw_lock.unlock(); }

			private:
				nRWLock _rw_lock;
		};

	template <>
		class nRWLocker<false>
		{
			public:
				void rdlock() const
				{ 
				}
				void wrlock() const
				{ 
				}
				void unlock() 
				{ 
				}
		};

	class nScopeRDLock
	{
		public:
			DISALLOW_COPY_AND_ASSIGN(nScopeRDLock);
			nScopeRDLock(nRWLock &m): _rw_lock(NULL){
				setLock(m);
			}
			nScopeRDLock() : _rw_lock(NULL){
			}
			~nScopeRDLock() {
				release();
			}

		public:
			void setLock(nRWLock &rwLock){
				assert_debug(_rw_lock == NULL, "");
				_rw_lock = &rwLock;
				_rw_lock->rdlock();
			}
			void release(){
				if(_rw_lock != NULL){
					_rw_lock->unlock();
					_rw_lock = NULL;
				}
			}

		private:
			nRWLock *_rw_lock;
	};

	class nScopeWRLock
	{
		public:
			DISALLOW_COPY_AND_ASSIGN(nScopeWRLock);
			nScopeWRLock(nRWLock &m) : _rw_lock(NULL) { 
				setLock(m);
			}
			nScopeWRLock() : _rw_lock(NULL){
			}
			~nScopeWRLock() {
				release();
			}

		public:
			void setLock(nRWLock &rwLock){
				assert_debug(_rw_lock == NULL, "");
				_rw_lock = &rwLock;
				_rw_lock->wrlock();
			}
			void release(){
				if(_rw_lock != NULL){
					_rw_lock->unlock();
					_rw_lock = NULL;
				}
			}

		private:
			nRWLock *_rw_lock;
	};
};

#endif
