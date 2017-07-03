#ifndef GAME_NTASKMANAGER_H_
#define GAME_NTASKMANAGER_H_

#include "base/nManager.h"

namespace Game
{
    template<typename TaskT>
        class nTaskManager : public Tidy::nManager<true, Tidy::nIndexByID< Tidy::CommonGetIDPolicy<TaskT> > >
    {
        public:
            typedef TaskT TaskType;
            
        public:
            nTaskManager();
            ~nTaskManager();

        public:
            bool addTask(TaskType* task);
            void removeTask(TaskType* task);
            TaskType* getTaskByID(DWORD id) const;
            void destoryAll();
            
        public:
            template<typename ExecT>
                bool execEveryTask(ExecT& exec)
                {
                    return execAll(exec);
                }
            template<typename ExecT>
                bool execEveryTask(ExecT& exec) const
                {
                    return execAll(exec);
                }
    };

    template<typename TaskT>
        nTaskManager<TaskT>::nTaskManager()
        {
        }

    template<typename TaskT>
        nTaskManager<TaskT>::~nTaskManager()
        {
        }
    template<typename TaskT>
        bool nTaskManager<TaskT>::addTask(TaskType* task)
        {
            if(add(task) == false){
                return false;
            }
            return true;
        }
    template<typename TaskT>
        void nTaskManager<TaskT>::removeTask(TaskType* task)
        {
            this->remove(task);
        }
    template<typename TaskT>
        TaskT* nTaskManager<TaskT>::getTaskByID(DWORD id) const
        {
            return this->getByID(id);
        }
};

#endif
