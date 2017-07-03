#ifndef _N_UNIQUEID_H_
#define _N_UNIQUEID_H_

#include "base/nMutex.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	/// @brief nUniqueID模板
	// 本模板实现了唯一ID生成器，并保证线程安全。回收的ID会重用
	// 可以用各种长度的无符号整数作为ID。
	template <class T, bool lock = false>
		class nUniqueID
		{
			public:
				/// @brief 默认构造函数 
				// 开始ID为1，最大有效ID为(T)-2,无效ID为(T)-1
				nUniqueID()
				{
					initIDRange(1,(T)-1);
				}

				/// @brief 构造函数 
				// 用户自定义起始ID，最大有效ID为(T)-2,无效ID为(T)-1
				/// @param startID 用户自定义的起始ID
				nUniqueID(T startID)
				{
					initIDRange(startID,(T)-1);
				}

				/// @brief 构造函数 
				// 用户自定义起始ID，及最大无效ID,最大有效ID为最大无效ID-1
				/// @param startID 用户自定义的起始ID
				/// @param endID 用户自定义的最大无效ID
				nUniqueID(T startID,T endID)
				{
					initIDRange(startID,endID);
				}

				/// @brief 析构函数 
				// 回收已分配的ID内存。
				~nUniqueID()
				{
					_mutex.lock();
					_reuse_id_list.clear();
					_mutex.unlock();
				}

			protected:
				/// @brief 初始化分配器
				/// @param min	最小值
				/// @param max	最大值
				/// @return 
				void initIDRange(T min,T max)
				{
					_min_range_id=min;
					_max_range_id=max;
					_cur_id=_min_range_id;
				}

			public:
				/// @brief 得到一个唯一ID 
				/// @return 返回一个唯一ID，如果返回最大无效ID，比表示所有ID都已被用，无可用ID。
				T getUniqueID(T *id)
				{
					if((*id) != 0){
						return 0;
					}
					T ret;
					_mutex.lock();

					// 优先从_reuse_id_list获取.否则的话就成了内存泄露
					if(_reuse_id_list.empty() == false){
						ret=_reuse_id_list.back();
						_reuse_id_list.pop_back();
					}
					else if(_cur_id < _max_range_id){
						ret=_cur_id;
						_cur_id++;
					}
					else{
						ret = _max_range_id;
						assert_fail("没有ID可以用");
					}

					_mutex.unlock();
					*id = ret;
					return ret;
				}

				T getUniqueID()
				{
					T ret = 0;
					return getUniqueID(&ret);
				}

				/// @brief 将ID放回ID池，以便下次使用。 
				// 放回的ID必须是由get函数得到的。并且不能保证放回的ID,没有被其他线程使用。
				// 所以用户要自己保证还在使用的ID不会被放回去。以免出现ID重复现象。
				/// @param id 由get得到的ID.
				void putUniqueID(T id)
				{
					if(id < _max_range_id && id >= _min_range_id){
						_mutex.lock();
#ifdef _DEBUG
						assert_debug(std::find(_reuse_id_list.begin(), _reuse_id_list.end(), id) == _reuse_id_list.end(), "");
#endif
						_reuse_id_list.push_front(id);
						_mutex.unlock();
					}
					else{
						assert_fail("id 重复");
					}
				}

				/// @brief 得到最大无效ID 
				/// @return 返回最大无效ID
				T invalid()
				{
					return _max_range_id;
				}

			private:
				/// @brief 锁
				nMutexT<lock> _mutex;
				/// @brief ID生成器可用列表类型
				typedef std::list<T, __gnu_cxx::__pool_alloc<T> > nUniqueList;
				/// @brief ID生成器可用列表类型迭代
				typedef typename nUniqueList::iterator nUniqueList_iter;
				/// @brief 可用的id列表
				nUniqueList _reuse_id_list;
				/// @brief 分配器中最大的id
				T _max_range_id;
				/// @brief 分配器中最小的id
				T _min_range_id;
				/// @brief 分配器中下一个可用id
				T _cur_id;
		};

	//顺序增加,ID不复用
	template <class T, bool lock = false>
		class nUniqueAutoIncID
		{
			public:
				/// @brief 默认构造函数 
				// 开始ID为1，最大有效ID为(T)-2,无效ID为(T)-1
				nUniqueAutoIncID() {
					initIDRange(1,(T)-1);
				}

				/// @brief 构造函数 
				// 用户自定义起始ID，最大有效ID为(T)-2,无效ID为(T)-1
				/// @param startID 用户自定义的起始ID
				explicit nUniqueAutoIncID(T startID) {
					initIDRange(startID,(T)-1);
				}

				/// @brief 构造函数 
				// 用户自定义起始ID，及最大无效ID,最大有效ID为最大无效ID-1
				/// @param startID 用户自定义的起始ID
				/// @param endID 用户自定义的最大无效ID
				nUniqueAutoIncID(T startID,T endID) {
					initIDRange(startID,endID);
				}

				/// @brief 析构函数 
				// 回收已分配的ID内存。
				~nUniqueAutoIncID() {
				}

			public:
				/// @brief 初始化分配器
				/// @param min	最小值
				/// @param max	最大值
				/// @return 
				void initIDRange(T min,T max) {
					_min_range_id = min;
					_max_range_id = max;
					_cur_id = min;
					assert_debug(min < max, "");
				}

				/// @brief 得到一个唯一ID 
				/// @return 返回一个唯一ID，如果返回最大无效ID，比表示所有ID都已被用，无可用ID。
				T getUniqueID(T *id) {
					if((*id) != 0) {
						assert_fail("id不为0");
						return 0;
					}
					_mutex.lock();
					if(_cur_id >= _max_range_id) {
						_cur_id = _min_range_id;
						*id = _min_range_id;
						baseServer->fatal("ID池被用完[%u,%u]", _min_range_id, _max_range_id);
					}
					else {
						*id = _cur_id;
						++ _cur_id;
					}
					_mutex.unlock();
					return *id;
				}

				T getUniqueID() {
					T id = 0;
					return getUniqueID(&id);
				}

				/// @brief 将ID放回ID池，以便下次使用。 
				// 放回的ID必须是由get函数得到的。并且不能保证放回的ID,没有被其他线程使用。
				// 所以用户要自己保证还在使用的ID不会被放回去。以免出现ID重复现象。
				/// @param id 由get得到的ID.
				void putUniqueID(T id) {
				}

				/// @brief 得到最大无效ID 
				/// @return 返回最大无效ID
				T invalid() {
					return _max_range_id;
				}
				/// @brief 测试这个ID是否被分配出去
				/// @return 被分配出去返回true,无效ID和未分配ID返回false
				bool hasAssigned(T testid) {
					return testid < _cur_id;
				}

			private:
				/// @brief 锁
				nMutexT<lock> _mutex;
				T _max_range_id;
				T _cur_id;
				T _min_range_id;
		};

	/// @brief 不带锁的DWORD的id分配器
	typedef nUniqueID<DWORD> nUniqueDWORDID;
	/// @brief 不带锁的WORD的id分配器
	typedef nUniqueID<WORD> nUniqueWORDID;

	/// @brief 带锁的DWORD的id分配器
	typedef nUniqueID<DWORD, true> nUniqueMutexDWORDID;
	/// @brief 带锁的WORD的id分配器
	typedef nUniqueID<WORD, true> nUniqueMutexWORDID;

	/// @brief 不带锁的DWORD的id分配器
	typedef nUniqueAutoIncID<DWORD> nUniqueMaxDWORDID;
	/// @brief 不带锁的WORD的id分配器
	typedef nUniqueAutoIncID<WORD> nUniqueMaxWORDID;

	/// @brief 带锁的DWORD的id分配器
	typedef nUniqueAutoIncID<DWORD, true> nUniqueMaxMutexDWORDID;
	/// @brief 带锁的WORD的id分配器
	typedef nUniqueAutoIncID<WORD, true> nUniqueMaxMutexWORDID;

	/// @brief	不分配临时ID的分配器,只为了实现nIndexTempID类似于nIndexID的索引
	template <class T>
		class nUniqueIDNull
		{
			public:
				T getUniqueID(T &id)
				{
					return id;
				}
				void putUniqueID(T &id)
				{
				}
		}; 

	struct TimeIDData{
		TimeIDData(const DWORD reuseID, const DWORD currTimeInSec):
			id(reuseID),
			putTimeInSec(currTimeInSec)
		{
		}
		DWORD id;
		DWORD putTimeInSec;
	};
	class nTimeReuseIDManager
	{
		public:
			nTimeReuseIDManager(const DWORD reuseIDIntervalTimeInSec):
				_reuse_id_interval_time_in_sec(reuseIDIntervalTimeInSec),
				_max_range_id((DWORD)-1),
				_cur_id(0)
		{
		}
			~nTimeReuseIDManager()
			{
			}

		public:
			DWORD getUniqueID(DWORD *id)
			{
				*id = invalid();
				if(_reuse_id_list.empty() == false){
					const TimeIDData &data = _reuse_id_list.front();
					if(baseServer->time().sec() > (data.putTimeInSec + _reuse_id_interval_time_in_sec)){
						*id = data.id;
						_reuse_id_list.pop_front();
						return *id;
					}
				}

				if(_cur_id < _max_range_id){
					++ _cur_id;
				}
				else{
					assert_fail("超出最大ID范围[%u,%u]");
					_cur_id = 1;
				}
				*id = _cur_id;
				return *id;
			}
			void putUniqueID(const DWORD id)
			{
				_reuse_id_list.push_back(TimeIDData(id, baseServer->time().sec()));
			}
			DWORD invalid() const
			{
				return _max_range_id;
			}

		private:
			const DWORD _reuse_id_interval_time_in_sec;
			std::list<TimeIDData> _reuse_id_list;
			DWORD _max_range_id;
			DWORD _cur_id;
			DWORD _min_range_id;
	};
};
#endif
