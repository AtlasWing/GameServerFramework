#ifndef _N_CONDITION_H_
#define _N_CONDITION_H_

#include "base/nMutex.h"
#include "base/nSys.h"

namespace Tidy
{
	/// @brief	封装了系统条件变量，使用上要简单
	// 			省去了手工初始化和销毁系统条件变量的工作
	// 			这些工作都可以由构造函数和析构函数来自动完成
	class nCondition
	{
		public:
			DISALLOW_COPY_AND_ASSIGN(nCondition);
			/// @brief	构造函数，用于创建一个条件变量
			nCondition()
			{
				::pthread_cond_init(&cond, NULL);
			}

			/// @brief	析构函数，用于销毁一个条件变量
			~nCondition()
			{
				::pthread_cond_destroy(&cond);
			}

			/// @brief	对所有等待这个条件变量的线程广播发送信号，使这些线程能够继续往下执行
			void broadcast()
			{
				::pthread_cond_broadcast(&cond);
			}

			/// @brief	对所有等待这个条件变量的线程发送信号，但是只唤醒其中的一个线程
			void signal()
			{
				::pthread_cond_signal(&cond);
			}

			/// @brief	等待特定的条件变量满足
			/// @param 	mutex 需要等待的互斥体
			void wait(nMutex &mutex)
			{
				::pthread_cond_wait(&cond, &mutex.mutex);
			}

			/// @brief   可设置超时时间的等待
			/// @param 	mutex 需要等待的互斥体
			//			timeout 超时时间(单位：毫秒)
			/// @return	true 等待事件发生
			//			false 等待超时
			bool timedwait(nMutex &mutex, unsigned int timeout)
			{
				struct timeval now; // 精确到微秒
				::gettimeofday( &now, NULL );

				struct timespec tsp; // 精确到纳秒
				tsp.tv_sec = now.tv_sec + timeout / 1000;
				tsp.tv_nsec = now.tv_usec * 1000 + ( timeout % 1000 ) * 1000 * 1000;

				if ( 0 == ::pthread_cond_timedwait(&cond, &mutex.mutex, &tsp) )
					return true;
				else
					return false;
			}

		private:

			/// @brief	系统条件变量
			pthread_cond_t cond;

	};
};

#endif


