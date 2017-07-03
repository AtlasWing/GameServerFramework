#ifndef _N_USER_MANAGER_H_
#define _N_USER_MANAGER_H_

#include "base/nRWLock.h"
#include "base/nManager.h"
#include "base/nDebugger.h"
#include "base/nBaseServer.h"

namespace Game
{
	template<typename EntityT>
		struct UserGetCharIDPolicy
		{
			typedef EntityT EntityType;
			static const DWORD getID(const EntityT* entity)
			{
				return entity->getCharID();
			}
		};

	/// @brief 角色管理器
	//			 实现了ID、名字的索引,所以这些值不能重复
	template <typename T, typename ManagerBaseT, bool lock=true>
		class nUserManager : public ManagerBaseT, public Tidy::nRWLocker<lock>
	{
		protected:
			//typedef Tidy::nManager<true,Tidy::nIndexByID< UserGetCharIDPolicy<T> >, Tidy::nIndexByName< Tidy::CommonGetNamePolicy<T> > > UserManagerBaseType;
			typedef ManagerBaseT UserManagerBaseType;
			typedef Tidy::nIndexByID< UserGetCharIDPolicy<T> > IDIndexType;

		protected:
			nUserManager()
			{
			}

			~nUserManager()
			{
			}

		public:
			/// @brief 根据角色名字得到角色
			/// @param name 角色名字
			/// @return 角色指针,如果返回NULL表示没找到角色
			T * getUserByName( const char * name)
			{
				Tidy::nRWLocker<lock>::rdlock();
				T *ret =UserManagerBaseType::getByName(name);
				Tidy::nRWLocker<lock>::unlock();
				return ret;
			}

			/// @brief 根据角色ID得到角色
			/// @param id 角色ID
			/// @return 角色指针,如果返回NULL表示没找到角色
			T * getUserByID( DWORD id)
			{
				Tidy::nRWLocker<lock>::rdlock();
				T *ret = ManagerBaseT::getByID(id);
				Tidy::nRWLocker<lock>::unlock();
				return ret;
			}

			/// @brief 添加角色
			/// @param user 角色
			/// @return 添加是否成功
			bool addUser(T *user)
			{
				Tidy::nRWLocker<lock>::wrlock();
				bool ret =add(user);
				assert_debug(ret == true, "");
				Tidy::nRWLocker<lock>::unlock();
				return ret;
			}

			/// @brief 移出角色
			/// @param user 角色
			void removeUser(T *user)
			{
				Tidy::nRWLocker<lock>::wrlock();
				UserManagerBaseType::remove(user);
				Tidy::nRWLocker<lock>::unlock();
			}

			/// @brief 移出角色
			/// @param id 角色id
			void removeUserByID(DWORD id)
			{
				T *user = getUserByID(id);
				if(user)
				{
					Tidy::nRWLocker<lock>::wrlock();
					remove(user);
					Tidy::nRWLocker<lock>::unlock();
				}
			}

			/// @brief 移出角色
			/// @param name 角色名称
			void removeUserByName(const char *name)
			{
				T *user = getUserByName(name);
				if(user)
				{
					Tidy::nRWLocker<lock>::wrlock();
					remove(user);
					Tidy::nRWLocker<lock>::unlock();
				}
			}

			/// @brief 对每个用户执行
			/// @param exec 执行接口
			template <class ExecT>
				bool execEveryUser(ExecT &exec)
				{
					Tidy::nRWLocker<lock>::rdlock();
					bool ret=execAll(exec);
					Tidy::nRWLocker<lock>::unlock();
					return ret;
				}

			/// @brief 得到当前管理器中的成员个数
			/// @return 个数
			size_t getSize()
			{
				return UserManagerBaseType::size();
			}

			/// @brief 判断容器是否为空
			/// @return 个数
			bool isEmpty()
			{
				return UserManagerBaseType::empty();
			}

			void writeLock()
			{
				Tidy::nRWLocker<lock>::wrlock();
			}

			void readLock()
			{
				Tidy::nRWLocker<lock>::rdlock();
			}

			void unlock()
			{
				Tidy::nRWLocker<lock>::unlock();
			}
	};
};
#endif
