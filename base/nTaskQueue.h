#ifndef _N_TASK_QUEUE_H_
#define _N_TASK_QUEUE_H_

namespace Tidy
{

template < typename T>
class nTaskQueue
{
    private:
        typedef std::deque<T *, __gnu_cxx::__pool_alloc<T *> > TaskContainer;
    public:
        nTaskQueue() :_size(0) {}
        virtual ~nTaskQueue() {}
        inline void add(T *task)
        {
            mlock.lock();
            _queue.push_back(task);
            ++ _size;
            mlock.unlock();
        }
        /// @brief	检查是否有线程等待添加
        inline void check_queue()
        {
            if(_size == 0)
            {
                return;
            }
            mlock.lock();
            if(_size > 0)
            {
                _temp_queue.swap(_queue);
                _size = 0;
            }
            mlock.unlock();
            for(typename TaskContainer::iterator iter=_temp_queue.begin(); iter != _temp_queue.end(); ++ iter)
            {
                _add(*iter);
            }
            _temp_queue.clear();
        }
    protected:
        /// @brief	虚函数
        /// @param	task 任务
        virtual void _add(T *task) = 0;
        /// @brief	队列中等待处理的task数量
        unsigned int _size;
        /// @brief	锁
        nMutex mlock;

        /// @brief	等待中的task队列
        TaskContainer _queue;

        /// @brief	缓冲一下需要转交线程的连接
        //			这样可以保证再锁的外层调用虚函数_add()
        //			将不再惧怕_add()导致死锁等问题
        TaskContainer _temp_queue;
};

};
#endif
