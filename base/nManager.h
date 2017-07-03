#ifndef _N_MANAGER_H_
#define _N_MANAGER_H_

#include "base/nHashTable.h"
#include "base/nHashListTable.h"
#include "base/nListTable.h"
#include "base/nIDManager.h"

namespace Tidy
{
	template<typename EntityT>
		struct CommonGetIDPolicy
		{
			typedef EntityT EntityType;
			static const DWORD getID(const EntityT* entity)
			{
				return entity->getID();
			}
		};

	template<typename GetIDPolicy, typename ImpTableT = nHashListTable<DWORD, typename GetIDPolicy::EntityType*, GetIDPolicy> >
		class nIndexByID : public ImpTableT
	{
		public:
			typedef typename GetIDPolicy::EntityType EntityType;

		public:
			EntityType * getByID(const DWORD id) const
			{
				EntityType *ret=NULL;
				ImpTableT::find(id,ret);
				return ret;
			}
			size_t removeByID(const DWORD id)
			{
				return ImpTableT::remove(id);
			}

		protected:
			bool add(const EntityType * &entity)
			{
				return insert(GetIDPolicy::getID(entity), entity);
			}

			bool add(EntityType * &entity)
			{
				return insert(GetIDPolicy::getID(entity), entity);
			}

			size_t remove(const EntityType * entity)
			{
				if(entity!=NULL) {
					return ImpTableT::remove(GetIDPolicy::getID(entity));
				}
				return 0;
			}

	};

	template<typename EntityT>
		struct CommonGetNamePolicy
		{
			typedef EntityT EntityType;
			static const char* getName(const EntityT* entity)
			{
				return entity->getName();
			}
		};

	template<typename GetNamePolicy, typename ImpTableT = nHashTable<const char*, typename GetNamePolicy::EntityType*, GetNamePolicy> >
		class nIndexByName : public ImpTableT
	{
		public:
			typedef typename GetNamePolicy::EntityType EntityType;

		public:
			EntityType * getByName(const char* name) const
			{
				EntityType *ret=NULL;
				ImpTableT::find(name, ret);
				return ret;
			}
			void removeByName(const char* name)
			{
				ImpTableT::remove(name);
			}

		protected:
			bool add(const EntityType *entity)
			{
				return insert(entity->getName(), const_cast<EntityType*>(entity));
			}

			size_t remove(const EntityType *entity)
			{
				if(entity!=NULL) {
					return ImpTableT::remove(GetNamePolicy::getName(entity));
				}
				return 0;
			}

	};

	template<typename GetNamePolicy, typename ImpTableT = nMultiHashTable<const char*, typename GetNamePolicy::EntityType*> >
		class nIndexByMultiName : public ImpTableT
	{
		public:
			typedef typename GetNamePolicy::EntityType EntityType;

		public:
			EntityType * getByName(const char* name, const bool randomSelect = false) const
			{
				EntityType *ret = NULL;
				ImpTableT::find(name, ret, randomSelect);
				return ret;
			}

		protected:
			bool add(const EntityType *entity)
			{
				return insert(entity->getName(), const_cast<EntityType*>(entity));
			}

			void remove(const EntityType *entity)
			{
				erase(entity->getName(), const_cast<EntityType*>(entity));
			}
	};

	template<typename EntityT>
		struct CommonVisitSequenceIDPolicy
		{
			typedef EntityT EntityType;
			static void setSequenceID(const DWORD sequenceID, EntityT* entity)
			{
				entity->setSequenceID(sequenceID);
			}

			static const DWORD getSequenceID(const EntityT* entity)
			{
				return entity->getSequenceID();
			}
		};

	template<typename VisitSequenceIDPolicy, typename ImpTableT = nHashListTable<DWORD, typename VisitSequenceIDPolicy::EntityType*, VisitSequenceIDPolicy>, typename UniqueIDT = nUniqueID<DWORD> >
		class nIndexSequenceID : public ImpTableT, public UniqueIDT
	{
		public:
			typedef typename VisitSequenceIDPolicy::EntityType EntityType;

		protected:
			nIndexSequenceID() 
			{
			}
			~nIndexSequenceID() 
			{
			}

		protected:
			bool add(EntityType * entity)
			{
				if(entity == NULL)
				{
					assert_fail("空指针");
					return false;
				}
				DWORD id = UniqueIDT::getUniqueID();
				if(id == 0)
				{
					assert_fail("无效ID");
					return false;
				}
				VisitSequenceIDPolicy::setSequenceID(id, entity);
				if(insert(id, entity) == false)
				{
					UniqueIDT::putUniqueID(id);
					return false;
				}

				return true;
			}

			size_t remove(const EntityType * entity)
			{
				if(entity == NULL) {
					assert_fail("空指针");
					return 0;
				}
				size_t num = ImpTableT::remove(VisitSequenceIDPolicy::getSequenceID(entity));
				UniqueIDT::putUniqueID(VisitSequenceIDPolicy::getSequenceID(entity));
				return num;
			}

		public:
			EntityType * getByID(const DWORD id) const
			{
				EntityType *ret = NULL;
				ImpTableT::find(id,ret);
				return ret;
			}
	};

	class nIndexNull
	{
	};

	template<bool destroy, typename ManagerT>
		class DestroyManagerPolicy
		{
			public:
				DestroyManagerPolicy(ManagerT *manager)
				{
					manager->destroyAll();
				}
		};

	template<typename ManagerT>
		class DestroyManagerPolicy<false, ManagerT>
		{
			public:
				DestroyManagerPolicy(ManagerT *manager)
				{
					assert_debug(manager->empty(), "");
				}
		};

	template<bool needDestroyAll, typename IndexT_0, typename IndexT_1 = nIndexNull, typename IndexT_2 = nIndexNull>
		class nManager : public IndexT_0, public IndexT_1, public IndexT_2
	{
        public:
			typedef typename IndexT_0::EntityType EntityType;
		protected:
			nManager()
			{
			}
			~nManager()
			{
				DestroyManagerPolicy<needDestroyAll, nManager<needDestroyAll, IndexT_0, IndexT_1, IndexT_2> > temp(this);
			}

		public:
			bool add(EntityType * entity)
			{
				if(IndexT_0::add(entity))
				{
					if(IndexT_1::add(entity))
					{
						if(IndexT_2::add(entity))
						{
							return true;
						}
						else
						{
							IndexT_0::remove(entity);
							IndexT_1::remove(entity);
						}
					}
					else
					{
						IndexT_0::remove(entity);
					}
				}
				return false;
			}

			void remove(const EntityType * entity)
			{
				IndexT_0::remove(entity);
				IndexT_1::remove(entity);
				IndexT_2::remove(entity);
			}

			bool empty() const
			{
				return IndexT_0::empty();
			}

			size_t size() const
			{
				return IndexT_0::size();
			}

			void clear()
			{
				IndexT_0::clear();
				IndexT_1::clear();
				IndexT_2::clear();
			}

			template <class ExecT>
				bool execAll(ExecT &eee)
				{
					EntityType *obj = NULL;
					for(typename IndexT_0::imp_map_iter iter = IndexT_0::_imp_map.begin(); iter != IndexT_0::_imp_map.end();) {
						obj = iter->second;
						++ iter;
						if(!eee.exec(obj))
							return false;
					}
					return true;
				}

			template <class ExecT>
				bool execAll(ExecT &eee) const
				{
					const EntityType *obj = NULL;
					for(typename IndexT_0::imp_map_const_iter iter = IndexT_0::_imp_map.begin(); iter != IndexT_0::_imp_map.end();) {
						obj = iter->second;
						++ iter;
						if(!eee.exec(obj))
							return false;
					}
					return true;
				}

			void destroyAll()
			{
				typename IndexT_0::imp_map_iter iter = IndexT_0::_imp_map.begin();
				EntityType* entity = NULL;
				while(iter != IndexT_0::_imp_map.end()) {
					entity = iter->second;
					++ iter;
					SAFE_DELETE(entity);
				}
				clear();
			}

			template<typename CheckDeleteExecT>
				void destroy(const CheckDeleteExecT& exec)
				{
					Tidy::vector<const EntityType*> recycleList;
					for(typename IndexT_0::imp_map_iter iter = IndexT_0::_imp_map.begin(); iter != IndexT_0::_imp_map.end(); ++ iter)
					{
						if(exec.exec(iter->second) == true)
						{
							recycleList.push_back(iter->second);
						}
					}

					for(size_t i = 0; i < recycleList.size(); ++ i)
					{
						remove(recycleList[i]);
						SAFE_DELETE(recycleList[i]);
					}
				}

			template<typename CheckRemoveExecT>
				void remove(const CheckRemoveExecT& exec)
				{
					Tidy::vector<const EntityType*> removeList;
					for(typename IndexT_0::imp_map_iter iter = IndexT_0::_imp_map.begin(); iter != IndexT_0::_imp_map.end(); ++ iter)
					{
						if(exec.exec(iter->second) == true)
						{
							removeList.push_back(iter->second);
						}
					}

					for(size_t i = 0; i < removeList.size(); ++ i)
					{
						remove(removeList[i]);
					}
				}

			template<typename CheckRemoveExecT, typename RemoveList>
				void remove(const CheckRemoveExecT& exec, RemoveList& removeList)
				{
					for(typename IndexT_0::imp_map_iter iter = IndexT_0::_imp_map.begin(); iter != IndexT_0::_imp_map.end(); ++ iter){
						if(exec.exec(iter->second) == true){
							removeList.push_back(iter->second);
						}
					}

					for(size_t i = 0; i < removeList.size(); ++ i){
						remove(removeList[i]);
					}
				}

			template<typename GetExecT, typename EntityList>
				void get(const GetExecT& exec, EntityList& entityList) const
				{
					for(typename IndexT_0::imp_map_const_iter iter = IndexT_0::_imp_map.begin(); iter != IndexT_0::_imp_map.end(); ++ iter){
						if(exec.exec(iter->second) == true){
							entityList.push_back(iter->second);
						}
					}
				}
	}; 

	template<bool needDestroyAll, typename IndexT_0, typename IndexT_1>
		class nManager<needDestroyAll,IndexT_0, IndexT_1, nIndexNull> : public IndexT_0, public IndexT_1
		{
            public:
				typedef typename IndexT_0::EntityType EntityType;
			protected:
				nManager()
				{
				}
				~nManager()
				{
					DestroyManagerPolicy<needDestroyAll, nManager<needDestroyAll, IndexT_0, IndexT_1> > temp(this);
				}

			public:
				bool add(EntityType * entity)
				{
					if(IndexT_0::add(entity))
					{
						if(IndexT_1::add(entity))
						{
							return true;
						}
						else
						{
							IndexT_0::remove(entity);
						}
					}
					return false;
				}

				void remove(EntityType * entity)
				{
					IndexT_0::remove(entity);
					IndexT_1::remove(entity);
				}

				void remove(const EntityType * entity)
				{
					IndexT_0::remove(entity);
					IndexT_1::remove(entity);
				}

				bool empty() const
				{
					return IndexT_0::empty();
				}

				size_t size() const
				{
					return IndexT_0::size();
				}

				void clear()
				{
					IndexT_0::clear();
					IndexT_1::clear();
				}

				template <class ExecT>
					bool execAll(ExecT &eee)
					{
						EntityType *obj = NULL;
						for(typename IndexT_0::imp_map_iter iter = IndexT_0::_imp_map.begin(); iter != IndexT_0::_imp_map.end();) {
							obj = iter->second;
							++ iter;
							if(!eee.exec(obj))
								return false;
						}
						return true;
					}

				template <class ExecT>
					bool execAll(ExecT &eee) const
					{
						const EntityType *obj = NULL;
						for(typename IndexT_0::imp_map_const_iter iter = IndexT_0::_imp_map.begin(); iter != IndexT_0::_imp_map.end();) {
							obj = iter->second;
							++ iter;
							if(!eee.exec(obj))
								return false;
						}
						return true;
					}

				void destroyAll()
				{
					typename IndexT_0::imp_map_iter iter = IndexT_0::_imp_map.begin();
					EntityType* entity = NULL;
					while(iter != IndexT_0::_imp_map.end())
					{
						entity = iter->second;
						++ iter;
						SAFE_DELETE(entity);
					}
					clear();
				}

				template<typename CheckDeleteExecT>
					void destroy(const CheckDeleteExecT& exec)
					{
						Tidy::vector<const EntityType*> recycleList;
						for(typename IndexT_0::imp_map_iter iter = IndexT_0::_imp_map.begin(); iter != IndexT_0::_imp_map.end(); ++ iter)
						{
							if(exec.exec(iter->second) == true)
							{
								recycleList.push_back(iter->second);
							}
						}

						for(size_t i = 0; i < recycleList.size(); ++ i)
						{
							remove(recycleList[i]);
							SAFE_DELETE(recycleList[i]);
						}
					}

				template<typename CheckRemoveExecT>
					void remove(const CheckRemoveExecT& exec)
					{
						Tidy::vector<const EntityType*> removeList;
						for(typename IndexT_0::imp_map_iter iter = IndexT_0::_imp_map.begin(); iter != IndexT_0::_imp_map.end(); ++ iter)
						{
							if(exec.exec(iter->second) == true)
							{
								removeList.push_back(iter->second);
							}
						}

						for(size_t i = 0; i < removeList.size(); ++ i)
						{
							remove(removeList[i]);
						}
					}

				template<typename CheckRemoveExecT, typename RemoveList>
					void remove(const CheckRemoveExecT& exec, RemoveList& removeList)
					{
						for(typename IndexT_0::imp_map_iter iter = IndexT_0::_imp_map.begin(); iter != IndexT_0::_imp_map.end(); ++ iter)
						{
							if(exec.exec(iter->second) == true)
							{
								removeList.push_back(iter->second);
							}
						}

						for(size_t i = 0; i < removeList.size(); ++ i)
						{
							remove(removeList[i]);
						}
					}

				template<typename GetExecT, typename EntityList>
					void get(const GetExecT& exec, EntityList& entityList) const
					{
						for(typename IndexT_0::imp_map_const_iter iter = IndexT_0::_imp_map.begin(); iter != IndexT_0::_imp_map.end(); ++ iter)
						{
							if(exec.exec(iter->second) == true)
							{
								entityList.push_back(iter->second);
							}
						}
					}
		}; 

	template<bool needDestroyAll, typename IndexT>
		class nManager<needDestroyAll,IndexT, nIndexNull, nIndexNull> : public IndexT
		{
			public:
				typedef typename IndexT::EntityType EntityType;
				nManager()
				{
				}
				~nManager()
				{
					DestroyManagerPolicy<needDestroyAll, nManager<needDestroyAll, IndexT> > temp(this);
				}

			public:
				bool add(EntityType * entity)
				{
					if(IndexT::add(entity))
					{
						return true;
					}
					return false;
				}

				size_t remove(EntityType * entity)
				{
					return IndexT::remove(entity);
				}

				bool add(const EntityType * entity)
				{
					if(IndexT::add(entity))
					{
						return true;
					}
					return false;
				}

				size_t remove(const EntityType * entity)
				{
					return IndexT::remove(entity);
				}

				size_t size() const
				{
					return IndexT::size();
				}

				bool empty() const
				{
					return IndexT::empty();
				}

				void clear()
				{
					IndexT::clear();
				}

				template <class ExecT>
					bool execAll(ExecT &eee)
					{
						EntityType *obj = NULL;
						for(typename IndexT::imp_map_iter iter = IndexT::_imp_map.begin(); iter != IndexT::_imp_map.end();) {
							obj = iter->second;
							++ iter;
							if(!eee.exec(obj))
								return false;
						}
						return true;
					}

				template <class ExecT>
					bool execAll(ExecT &eee) const
					{
						const EntityType *obj = NULL;
						for(typename IndexT::imp_map_const_iter iter = IndexT::_imp_map.begin(); iter != IndexT::_imp_map.end();) {
							obj = iter->second;
							++ iter;
							if(!eee.exec(obj))
								return false;
						}
						return true;
					}

				void destroyAll()
				{
					typename IndexT::imp_map_iter it=IndexT::_imp_map.begin();
					EntityType* entity = NULL;
					while(it != IndexT::_imp_map.end())
					{
						entity = it->second;
						++it;
						SAFE_DELETE(entity);
					}
					clear();
				}

				template<typename CheckDeleteExecT>
					void destroy(const CheckDeleteExecT& exec)
					{
						Tidy::vector<const EntityType*> recycleList;
						for(typename IndexT::imp_map_iter iter = IndexT::_imp_map.begin(); iter != IndexT::_imp_map.end(); ++ iter)
						{
							if(exec.exec(iter->second) == true)
							{
								recycleList.push_back(iter->second);
							}
						}

						for(size_t i = 0; i < recycleList.size(); ++ i)
						{
							remove(recycleList[i]);
							SAFE_DELETE(recycleList[i]);
						}
					}

				template<typename CheckRemoveExecT>
					void remove(const CheckRemoveExecT& exec)
					{
						Tidy::vector<const EntityType*> removeList;
						for(typename IndexT::imp_map_iter iter = IndexT::_imp_map.begin(); iter != IndexT::_imp_map.end(); ++ iter)
						{
							if(exec.exec(iter->second) == true)
							{
								removeList.push_back(iter->second);
							}
						}

						for(size_t i = 0; i < removeList.size(); ++ i)
						{
							remove(removeList[i]);
						}
					}

				template<typename CheckRemoveExecT, typename RemoveList>
					void remove(const CheckRemoveExecT& exec, RemoveList& removeList)
					{
						for(typename IndexT::imp_map_iter iter = IndexT::_imp_map.begin(); iter != IndexT::_imp_map.end(); ++ iter)
						{
							if(exec.exec(iter->second) == true)
							{
								removeList.push_back(iter->second);
							}
						}

						for(size_t i = 0; i < removeList.size(); ++ i)
						{
							remove(removeList[i]);
						}
					}

				template<typename GetExecT, typename EntityList>
					void get(const GetExecT& exec, EntityList& entityList) const
					{
						for(typename IndexT::imp_map_const_iter iter = IndexT::_imp_map.begin(); iter != IndexT::_imp_map.end(); ++ iter)
						{
							if(exec.exec(iter->second) == true)
							{
								entityList.push_back(iter->second);
							}
						}
					}

				template<typename EntityList>
					void getAll(EntityList& entityList) const
					{
						for(typename IndexT::imp_map_const_iter iter = IndexT::_imp_map.begin(); iter != IndexT::_imp_map.end(); ++ iter)
						{
							entityList.push_back(iter->second);
						}
					}
		}; 
};

#endif
