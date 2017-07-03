#ifndef BASE_NCLIENTCONNSERVICE_H_
#define BASE_NCLIENTCONNSERVICE_H_

#include "base/Tidy.h"
#include "base/nClientConn.h"
#include "base/nTaskQueue.h"
#include "base/Imp/nTaskThreadManager.h"

namespace Tidy
{
	class nClientConnService;
	/// @brief	连接任务链表
	typedef std::list<nClientConn *, __gnu_cxx::__pool_alloc<nClientConn *> > ClientConnList;
	/// @brief	连接任务队列
	//typedef std::queue<nClientConn *, std::deque<nClientConn *, __gnu_cxx::__pool_alloc<nClientConn *> > >  nClientConnQueue;
	/// @brief	连接任务链表叠代器
	typedef ClientConnList::iterator nClientConn_IT;

	/// @brief	epoll事件向量
	typedef std::vector<struct epoll_event> epollfdContainer;

	/// @brief	检测TCP连接状况,如果未连接,尝试连接
	class nCheckClientConnectThread : public nThread, public nTaskQueue<nClientConn>
	{
		public:
			nCheckClientConnectThread();
			~nCheckClientConnectThread();

		public:
			DWORD taskSize()const;

		private:
			virtual void run();
			virtual void _add(nClientConn *task);

		private:
			nClientConn_IT remove(nClientConn_IT &iter);

		private:
			ClientConnList _client_task_list;
			ClientConnList::size_type _client_task_num;
	};

	/// @brief	处理TCP连接的验证，如果验证不通过，需要回收这个连接
	class nCheckClientWaitThread : public nThread, public nTaskQueue<nClientConn>
	{
		public:
			nCheckClientWaitThread();
			~nCheckClientWaitThread();

		public:
			DWORD taskSize()const;

		private:
			virtual void run();
			virtual void _add(nClientConn *task);

		private:
			void remove(nClientConn *task);
			void remove(nClientConn_IT &it);

		private:
			ClientConnList _client_task_list;
			/// @brief	_client_task_list计数(保证线程安全)
			ClientConnList::size_type _client_task_num;

			/// @brief	epoll描述符
			int kdpfd;
			/// @brief	epoll事件向量
			epollfdContainer epfds;
	};

	/// @brief	TCP连接的主处理线程，一般一个线程带几个TCP连接，这样可以显著提高效率
	class nClientRunThread : public nThread, public nTaskQueue<nClientConn>
	{
		public:
			nClientRunThread();
			~nClientRunThread();

		public:
			/// @brief	本线程加入的任务个数
			DWORD taskSize()const;

		private:
			virtual void run();
			virtual void _add(nClientConn *task);

		private:
			void remove(nClientConn_IT &it);
			void remove(nClientConn *task);

		private:
			/// @brief	任务列表
			ClientConnList _client_task_list;
			/// @brief	_client_task_list计数(保证线程安全)
			ClientConnList::size_type _client_task_num;
			/// @brief	epoll描述符
			int kdpfd;
			/// @brief	epoll事件向量
			epollfdContainer epfds;
	};

	/// @brief	连接线程池类，封装了一个线程处理多个连接的线程池框架
	class nClientConnService
	{
		public:
			DISALLOW_COPY_AND_ASSIGN(nClientConnService);
			/// us 循环等待时间
			explicit nClientConnService(const DWORD us);
			~nClientConnService();

		public:
			bool initService();
			void finalService();

			/// @brief	连接池中的总任务个数
			DWORD getTotalSize();

			/// @brief	把任务添加到连接测试线程中
			/// @param	task 待添加的任务
			bool addToConnect(nClientConn *task);
			/// @brief	把任务添加到等待连接认证返回的队列中
			/// @param	task 待添加的任务
			void addToCheckwait(nClientConn *task);

			/// @brief	把任务添加到主处理循环中
			/// @param	task 待添加的任务
			/// @return	添加是否成功
			bool addToOkay(nClientConn *task);

			/// @brief	
			/// @param	time 
			void setUsleepTime(const long time)
			{
				usleep_time = time;
			}

		private:
			/// @brief	连接测试线程
			nTaskThreadManager<nCheckClientConnectThread> checkconnectThread;
			/// @brief	验证线程
			nTaskThreadManager<nCheckClientWaitThread> checkwaitThread;
			/// @brief	主线程
			nTaskThreadManager<nClientRunThread> _all_task_thread_manager;

		public:
			/// @brief	循环等待时间
			long usleep_time;
	};
};

#endif
