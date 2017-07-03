#ifndef BASE_NTASKCONNSERVICE_H_
#define BASE_NTASKCONNSERVICE_H_

#include "base/nThread.h"
#include "base/Tidy.h"
#include "base/nTaskConn.h"
#include "base/nTaskQueue.h"
#include "base/Imp/nTaskThreadManager.h"

namespace Tidy
{
	class nTaskConnService;
	/// @brief	连接任务链表.注意不要调用size()
	typedef std::list<nTaskConn *, __gnu_cxx::__pool_alloc<nTaskConn *> > nTaskConnContainer;

	/// @brief	连接任务链表叠代器
	typedef nTaskConnContainer::iterator nTaskConn_IT;

	/// @brief	epoll事件向量
	typedef std::vector<struct epoll_event> epollfdContainer;

	/// @brief	处理TCP连接的验证，如果验证不通过，需要回收这个连接
	class nTaskConnPoolVerifyThread : public nThread, public nTaskQueue<nTaskConn>
	{
		public:
			static DWORD maxTaskNumInOneThread;

		public:
			nTaskConnPoolVerifyThread();
			~nTaskConnPoolVerifyThread();

		public:
			DWORD taskSize() const;

		private:
			virtual void run();
			virtual void _add(nTaskConn *conn);

		private:
			void remove(nTaskConn *conn);
			void remove(nTaskConn_IT &it);

		private:
			nTaskConnContainer _task_conn_list;
			/// @brief	_task_conn_list计数(保证线程安全)
			nTaskConnContainer::size_type _task_conn_num;
			/// @brief	epoll描述符
			int kdpfd;
			/// @brief	epoll事件向量
			epollfdContainer epfds;
	};

	/// @brief	同步线程
	class nTaskConnPoolSyncThread : public nThread, public nTaskQueue<nTaskConn>
	{
		public:
			nTaskConnPoolSyncThread();
			~nTaskConnPoolSyncThread();

		public:
			DWORD taskSize() const;

		private:
			virtual void run();
			virtual void _add(nTaskConn *conn);

		private:
			nTaskConn_IT remove(nTaskConn_IT &iter);

		private:
			nTaskConnContainer _task_conn_list;
			DWORD _task_conn_num;
	};

	/// @brief	连接回收线程，回收所有无用的TCP连接，释放相应的资源
	class nTaskConnPoolRecycleThread : public nThread, public nTaskQueue<nTaskConn>
	{
		public:
			nTaskConnPoolRecycleThread();
			~nTaskConnPoolRecycleThread();

		public:
			DWORD taskSize() const;

		private:
			virtual void run();
			virtual void _add(nTaskConn *conn);

		private:
			nTaskConnContainer _task_conn_list;
			DWORD _task_conn_num;
	};

	/// @brief	TCP连接的主处理线程，一般一个线程带几个TCP连接，这样可以显著提高效率
	class nTaskConnPoolOkayThread : public nThread, public nTaskQueue<nTaskConn>
	{
		public:
			static DWORD maxTaskNumInOneThread;

		public:
			nTaskConnPoolOkayThread();
			~nTaskConnPoolOkayThread();

		public:
			DWORD taskSize() const;

		private:
			virtual void run();
			virtual void _add(nTaskConn *conn);

		private:
			void remove(nTaskConn_IT &it);

		private:
			/// @brief	任务列表
			nTaskConnContainer _task_conn_list;
			/// @brief	_task_conn_list计数(保证线程安全)
			nTaskConnContainer::size_type _task_conn_num;
			/// @brief	epoll描述符
			int kdpfd;
			/// @brief	epoll事件向量
			epollfdContainer epfds;
	};

	class nTaskConnService
	{
		public:
			DISALLOW_COPY_AND_ASSIGN(nTaskConnService);
			explicit nTaskConnService(const int us=50000);
			~nTaskConnService();

		public:
			bool initService();
			void finalService();
			/// 把一个TCP连接添加到验证队列中，因为存在多个验证队列，需要按照一定的算法添加到不同的验证处理队列中
			bool addToVerify(nTaskConn *conn);
			/// 把一个通过验证的TCP连接添加到等待同步验证队列中
			void addToSync(nTaskConn *conn);
			/// 把一个通过验证的TCP处理队列中
			bool addToOkay(nTaskConn *conn);
			/// 把一个TCP连接添加到回收处理队列中
			void addToRecycle(nTaskConn *conn);

		public:
			const int getAllSize();
			/// @brief	设置主线程的sleep时间(微秒)
			/// @param	time 需要sleep的时间
			static void setUsleepTime(const long time)
			{
				usleep_time=time;
			}

		private:
			nTaskThreadManager<nTaskConnPoolVerifyThread> _verify_thread_manager;
			nTaskThreadManager<nTaskConnPoolSyncThread> _sync_thread_manager;
			nTaskThreadManager<nTaskConnPoolOkayThread> _main_thread_manager;
			nTaskThreadManager<nTaskConnPoolRecycleThread> _recycle_thread_manager;

		public:
			/// @brief	循环等待时间
			static long usleep_time;
	};
};

#endif
