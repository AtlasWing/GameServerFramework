#ifndef BASE_NGROUP_MANAGER_H
#define BASE_NGROUP_MANAGER_H

#include "base/nAssert.h"
#include "base/nType.h"

namespace Tidy{
    template<typename ManagerT>
        class nGroupManager
        {
            public:
                typedef typename ManagerT::EntityType EntityType;
                nGroupManager(DWORD managerVecNum);
                nGroupManager();
                ~nGroupManager();

            public:
                bool add(const EntityType *obj);
                void remove(const EntityType *obj);
                EntityType* getByID(const DWORD id) const;
                DWORD size() const;
                template<typename GetExecT, typename EntityList>
                    void get(const GetExecT& exec, EntityList& entityList) const;
                template<typename EntityList>
					void getAll(EntityList& entityList) const;
                template<typename ExecT>
                    bool execAll(ExecT &exec);
                template<typename ExecT>
                    bool execAll(ExecT &exec) const;
                template<typename ExecT>
                    bool execNextGroup(ExecT &exec);
                template<typename ExecT>
                    bool execNextGroup(ExecT &exec) const;

            protected:
                bool init(const DWORD groupNum);

            private:
                DWORD hashGroupID(DWORD id) const;

            private:
                DWORD _group_num;
                DWORD _next_group_index;
                ManagerT *_manager_vec;
        };

    template<typename ManagerT>
        nGroupManager<ManagerT>::nGroupManager(DWORD managerGroupNum):
            _group_num(managerGroupNum),
            _next_group_index(0),
            _manager_vec(NULL)
    {
        assert_debug(managerGroupNum > 0, "");
        _manager_vec = new ManagerT[managerGroupNum];
    }

    template<typename ManagerT>
        nGroupManager<ManagerT>::nGroupManager():
            _group_num(0),
            _next_group_index(0),
            _manager_vec(NULL)
    {
    }

    template<typename ManagerT>
        nGroupManager<ManagerT>::~nGroupManager()
        {
            SAFE_DELETE_VEC(_manager_vec);
        }

    template<typename ManagerT>
        bool nGroupManager<ManagerT>::init(const DWORD groupNum)
        {
            if(groupNum == 0){
                assert_fail("组数为0");
                return false;
            }
            _group_num = groupNum;
            _manager_vec = new ManagerT[_group_num];
            return true;
        }

    template<typename ManagerT>
        bool nGroupManager<ManagerT>::add(const typename ManagerT::EntityType *obj)
        {
            DWORD groupID = hashGroupID(obj->getID());
            return _manager_vec[groupID].add(const_cast<typename ManagerT::EntityType*>(obj));
        }

    template<typename ManagerT>
        void nGroupManager<ManagerT>::remove(const typename ManagerT::EntityType *obj)
        {
            DWORD groupID = hashGroupID(obj->getID());
            _manager_vec[groupID].remove(obj);
        }

    template<typename ManagerT>
        DWORD nGroupManager<ManagerT>::size() const
        {
            DWORD num = 0; 
            for(DWORD i = 0; i < _group_num; ++ i){
                num += _manager_vec[i].size();
            }
            return num;
        }

    template<typename ManagerT>
        typename ManagerT::EntityType* nGroupManager<ManagerT>::getByID(const DWORD id) const
        {
            DWORD groupID = hashGroupID(id);
            return _manager_vec[groupID].getByID(id);
        }

    template<typename ManagerT>
        template<typename GetExecT, typename EntityList>
        void nGroupManager<ManagerT>::get(const GetExecT& exec, EntityList& entityList) const
        {
            for(DWORD i = 0; i < _group_num; ++ i){
                _manager_vec[i].get(exec, entityList);
            }
        }

    template<typename ManagerT>
        template<typename EntityList>
        void nGroupManager<ManagerT>::getAll(EntityList& entityList) const
        {
            for(DWORD i = 0; i < _group_num; ++ i){
                _manager_vec[i].getAll(entityList);
            }
        }

        template<typename ManagerT>
        template<typename ExecT>
        bool nGroupManager<ManagerT>::execAll(ExecT &exec)
        {
            for(DWORD i = 0; i < _group_num; ++ i){
                if(_manager_vec[i].execAll(exec) == false){
                    return false;
                }
            }
            return true;
        }

    template<typename ManagerT>
        template<typename ExecT>
        bool nGroupManager<ManagerT>::execAll(ExecT &exec) const
        {
            for(DWORD i = 0; i < _group_num; ++ i){
                if(_manager_vec[i].execAll(exec) == false){
                    return false;
                }
            }
            return true;
        }

    template<typename ManagerT>
        template<typename ExecT>
        bool nGroupManager<ManagerT>::execNextGroup(ExecT &exec)
        {
            _next_group_index = (_next_group_index + 1) % _group_num;
            return _manager_vec[_next_group_index].execAll(exec);
        }

    template<typename ManagerT>
        template<typename ExecT>
        bool nGroupManager<ManagerT>::execNextGroup(ExecT &exec) const
        {
            _next_group_index = (_next_group_index + 1) % _group_num;
            return _manager_vec[_next_group_index].execAll(exec);
        }

    template<typename ManagerT>
        DWORD nGroupManager<ManagerT>::hashGroupID(DWORD id) const
        {
            return id % _group_num;
        }
};
#endif
