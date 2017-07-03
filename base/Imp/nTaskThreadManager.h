#ifndef _N_THREADMANAGER_H_
#define _N_THREADMANAGER_H_

#include "base/nThread.h"
#include "base/nRWLock.h"
#include "base/nManager.h"

namespace Tidy
{
	template <typename T>
		class nTaskThreadManager : public nManager<true, nIndexByID< CommonGetIDPolicy<T> > >
	{
		public:
			nTaskThreadManager() : _max_task_thread_num(0)
		{
		}
			~nTaskThreadManager()
			{
			}

		public:
			bool init(const DWORD maxTaskThreadNum, const DWORD maxTaskNumInOneThread, const char* name, void *threadParam)
			{
				_max_task_thread_num = maxTaskThreadNum;
				_max_task_num_in_one_thread = maxTaskNumInOneThread;

				nScopeWRLock lock(_thread_manager_lock);
				for(DWORD i = 0; i < maxTaskThreadNum; ++i) {
					T *thread = new T();
					if(ThreadManagerBaseType::add(thread) == false) {
						SAFE_DELETE(thread);
						return false;
					}
				}

				return true;
			}

			struct GetBestExec
			{
				T *_best_thread;
				const DWORD _max_task_num;

				GetBestExec(const DWORD maxTaskNum):
					_best_thread(NULL),
					_max_task_num(maxTaskNum)
				{
				}
				bool exec(T *thread) {
					if(thread->isFinal() == true){
						return true;
					}
					if(thread->isAlive() == false) {
						if(thread->start() == false) {
							assert_fail("启动线程失败");
							return false;
						}
					}
					if(_best_thread == NULL){
						_best_thread = thread;
					}
					if(thread->taskSize() < _max_task_num) {
						_best_thread = thread;
						return false;
					}
					else if(thread->taskSize() < _best_thread->taskSize()){
						_best_thread = thread;
						return false;
					}
					return true;
				}
			};

			T *getOne()
			{
				GetBestExec exec(_max_task_num_in_one_thread);
				nScopeRDLock lock(_thread_manager_lock);
				execAll(exec);
				return exec._best_thread;
			}

			/// @brief 得到本线程组总共的任务个数
			/// @return 任务个数
			struct GetTaskNumExec
			{
				int _size;
				GetTaskNumExec():_size(0){}
				bool exec(T *t){
					_size += t->taskSize();
					return true;
				}
			};

			int getTaskNum()
			{
				GetTaskNumExec exec;
				nScopeRDLock lock(_thread_manager_lock);
				execAll(exec);
				return exec._size;
			}

			/// @brief 回收所有线程
			struct RemoveAllExec {
				bool exec(T *entity) {
					entity->join();
					SAFE_DELETE(entity);
					return true;
				}
			};

			void final()
			{
				RemoveAllExec exec;
				nScopeWRLock lock(_thread_manager_lock);
				execAll(exec); 
				ThreadManagerBaseType::clear();
			}

		private:
			typedef nManager<true, nIndexByID< CommonGetIDPolicy<T> > > ThreadManagerBaseType;
			nRWLock _thread_manager_lock;
			DWORD _max_task_thread_num;
			DWORD _max_task_num_in_one_thread;
	};
};

#endif
