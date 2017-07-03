#ifndef BASE_NTHREAD_H_
#define BASE_NTHREAD_H_

#include "base/nMutex.h"
#include "base/nCondition.h"
#include "base/Tidy.h"
#include "base/nObjLogger.h"

namespace Tidy
{
	class nThread : public nObjLogger<nThread>
	{
		protected:
			DISALLOW_COPY_AND_ASSIGN(nThread);
			/// @param	name 线程名称
			/// @param	joinable 是否需要等待一个线程的结束
			nThread(const char *name,bool joinable=true);

		public:
			virtual ~nThread();

		public:
			const DWORD getID() const;
			const char* getName() const;
			nLogger* getLogger() const;
			const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;

		public:
			void dealStartRun();
			bool start();
			void dealEndRun();
			bool checkRunning();
			/// @brief	设置正常标志
			inline void setRunning()
			{
				_running = true;
			}
			pthread_t getThreadID();
			void join(const bool needCheckCoreThread = true);
			const bool isAlive() const;
			const bool isJoinable() const;
			void stop();
			const bool isFinal() const;
			DWORD getRunTimeOutTimes() const;
			void incRunTimeOutTimes();
			void clearRunTimeOutTimes();
			void updateSeed(const DWORD seed);
			void setNeedMonitor();
			void clearNeedMonitor();
			bool checkNeedMonitor() const;

		public:
			static void sleep(const long sec);
			static void msleep(long millis);
			static void usleep(const long usec);
			/// @brief	加法函数，原子操作，在多线程情况下保证操作安全
			/// @param	val 被加长整数地址
			/// @param	x 加数
			/// @return	val被加前值
			static long atomicAdd(volatile long * val,long x);
			/// @brief	加1函数，原子操作
			/// @param	val 被增加值地址
			/// @return	val被加前值
			static long atomicInc(volatile long * val);
			/// @brief	减1函数，原子操作
			/// @param	val 被减值地址
			/// @return	val被减前值
			static long atomicDec(volatile long * val);
			/// @brief  绑定当前线程在第cpuIndex个CPU上运行
			static bool bindRunCpu(const DWORD cpuIndex);
			static bool setSigaction();
			static nThread* getRunningThread();
			static void resetSigaction(int signo);

		private:
			/// @brief	线程回调函数
			static void *threadProc(void *arg);
			/// @brief	线程实际运行函数
			virtual void run() = 0;

		private:
			DWORD _id;
			std::string _name;
			/// @brief	互斥锁
			nMutex _mlock;
			/// @brief	条件变量
			nCondition _cond;
			/// @brief	线程属性，是否设置joinable标记
			bool _joinable;
			/// @brief	线程ID
			pthread_t _thread;
			DWORD _run_timeout_times;
			/// @brief	线程是否已经启动
			volatile bool _alive;
			/// @brief	线程是否将结束
			volatile bool _complete;
			/// @brief	线程运行标志
			volatile bool _running;
			/// @brief	线程监控
			volatile bool _need_monitor;
			DWORD *_thread_rand_seed;

		private:
			/// @brief	线程id分配
			static long _tempid;
			static __thread nThread *_running_thread;
	};
};

#endif
