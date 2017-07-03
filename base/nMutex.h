/**
 * @author  牛延升,niuyansheng@gmail.com 
 * @brief 互斥锁
 *
 * 
 */


#ifndef _NMUTEX_H_
#define _NMUTEX_H_

#include "base/nSys.h"
#include "base/nMisc.h"

namespace Tidy
{
	/// @brief	互斥锁
	class nMutex
	{
		public:
			DISALLOW_COPY_AND_ASSIGN(nMutex);
			friend class nCondition;

			/// @brief	锁类型枚举
			enum MutexType
			{
				fast = PTHREAD_MUTEX_ADAPTIVE_NP,			//for "fast" mutexes
				recursive = PTHREAD_MUTEX_RECURSIVE_NP,		//for "recursive" mutexes
				timed = PTHREAD_MUTEX_TIMED_NP,				//for "timed" mutexes
				errorcheck = PTHREAD_MUTEX_ERRORCHECK_NP	//for "error checking" mutexes
			};

			/// @brief	构造函数，构造一个互斥体对象
			/// @param	_t 锁的类型MutexType
			/// @return	
			nMutex(MutexType _t = fast) 
			{
				pthread_mutexattr_t attr;
				::pthread_mutexattr_init(&attr);
				::pthread_mutexattr_settype(&attr, _t);
				::pthread_mutex_init(&mutex, &attr);
			}

			/// @brief	析构函数，销毁一个互斥体对象
			~nMutex() { ::pthread_mutex_destroy(&mutex); }

			/// @brief	加锁一个互斥体
			void lock() { ::pthread_mutex_lock(&mutex); }

			/// @brief	解锁一个互斥体
			void unlock() { ::pthread_mutex_unlock(&mutex); }

			/// @brief   试图加锁一个互斥体
			/// @return  true	加锁成功
			//			false	加锁失败(已经被其他线程锁住)
			bool trylock() { return (::pthread_mutex_trylock(&mutex) == 0); }

		private:

			/// @brief	系统互斥体
			pthread_mutex_t mutex;

	};

	/// @brief	Wrapper
	// 			方便在复杂函数中锁的使用
	class nMutex_scope_lock
	{
		public:
			DISALLOW_COPY_AND_ASSIGN(nMutex_scope_lock);
			/// @brief	构造函数
			// 			对锁进行lock操作
			/// @param 	m 锁的引用
			nMutex_scope_lock(nMutex &m) : mlock(m) { mlock.lock(); }

			/// @brief	析购函数
			// 			对锁进行unlock操作
			~nMutex_scope_lock() { mlock.unlock(); }

		private:

			/// @brief	锁的引用
			nMutex &mlock;

	};

	class nMutex_scope_trylock
	{
		public:
			DISALLOW_COPY_AND_ASSIGN(nMutex_scope_trylock);
			/// @brief   构造函数，试图加锁
			nMutex_scope_trylock(nMutex &m) : mlock(m), bLocked(false) { bLocked = mlock.trylock(); }

			/// @brief	析构函数，如果构造函数加了所就进行unlock操作
			~nMutex_scope_trylock() { if( bLocked ) mlock.unlock(); }

			/// @brief	构造函数是否加锁成功
			bool isLocked() const { return bLocked; }

		private:
			/// @brief	锁的引用
			nMutex &mlock;

			/// @brief	是否成功加锁
			bool bLocked;
	};

	/// @brief	定义一个锁的模版
	template <bool locker = true>
		class nMutexT
		{
			public:
				/// @brief	加锁操作
				void lock() { _lock.lock(); }
				/// @brief	解锁操作
				void unlock() { _lock.unlock(); }
			private:
				nMutex _lock;
		};

	/// @brief	特化锁的模版
	template <>
		class nMutexT<false>
		{
			public:
				/// @brief	加锁操作
				// 		这里是空函数
				void lock() { }
				/// @brief	解锁操作
				//			这里是空函数
				void unlock() { }
		};
};
#endif

