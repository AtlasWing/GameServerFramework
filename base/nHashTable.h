#ifndef _N_HASH_TABLE_H_
#define _N_HASH_TABLE_H_

#include "base/nSTL.h"
#include "base/nRandom.h"

namespace Tidy
{
	template <class keyT, class valueT, class policyT>
		class nHashTable
		{
			protected:
				DISALLOW_COPY_AND_ASSIGN(nHashTable);
				nHashTable()
				{
				}

				~nHashTable()
				{
				}

			protected:
				bool insert(const keyT &key, const valueT &value)
				{
					return _imp_map.insert(hashmap_value_type(key, value)).second;
				}

				bool find(const keyT &key,valueT &value) const
				{
					imp_map_const_iter it = _imp_map.find(key);
					if(it != _imp_map.end()) {
						value = it->second;
						return true;
					}
					return false;
				}

				size_t remove(const keyT &key)
				{
					return _imp_map.erase(key);
				}

				void clear()
				{
					_imp_map.clear();
				}

				size_t size() const
				{
					return _imp_map.size();
				}

				bool empty() const
				{
					return _imp_map.empty();
				}

			public:
				typedef __gnu_cxx::hash_map<keyT, valueT, __gnu_cxx::hash<keyT>, Tidy::key_equal> ImpMapType;
				typedef typename ImpMapType::iterator imp_map_iter;
				typedef typename ImpMapType::const_iterator imp_map_const_iter;
				typedef typename ImpMapType::value_type hashmap_value_type;

			protected:
				ImpMapType _imp_map;
		};

	/// @brief 有限桶nMultiHashTable管理模板,非线程安全
	// 目前支持两种key类型(DWORD , char *),value类型不作限制,但此类型要可copy的。
	template <class keyT, class valueT>
		class nMultiHashTable
		{
			protected:
				DISALLOW_COPY_AND_ASSIGN(nMultiHashTable);
				nMultiHashTable()
				{
				}
				~nMultiHashTable()
				{
				}

			protected:
				/// @brief 插入数据，如果原来存在相同key值的数据，原来数据将会被替换
				/// @param key key值
				/// @param value 要插入的数据
				/// @return 成功返回true，否则返回false
				bool insert(const keyT &key, const valueT &value)
				{
					_imp_map.insert(value_type(key, value));
					return true;
				}

				void erase(const keyT &key, const valueT &value)
				{
					__gnu_cxx::pair<imp_map_iter,imp_map_iter> its = _imp_map.equal_range(key);
					for(imp_map_iter it = its.first; it != its.second; ++it)
					{
						if (it->second == value)
						{
							_imp_map.erase(it);
							return;
						}
					}
				}

				bool find(keyT key,  valueT &value, const bool random) const
				{
					__gnu_cxx::pair<imp_map_const_iter,imp_map_const_iter> its = _imp_map.equal_range(key);
					if(its.first == _imp_map.end()){
						return false;
					}
					if(random == false) {
						value = its.first->second;
					}
					else{
						std::vector<const valueT*> resultList;
						for(imp_map_const_iter iter = its.first; iter != its.second; ++iter) {
							resultList.push_back(&iter->second);
						}
						value = *resultList[Tidy::randBetween(0, resultList.size() - 1)];
					}
					return true;
				}

				void clear()
				{
					_imp_map.clear();
				}

				size_t size() const
				{
					return _imp_map.size();
				}

				bool empty() const
				{
					return _imp_map.empty();
				}

			protected:
				/// @brief hash_multimap容器
				typedef __gnu_cxx::hash_multimap<keyT, valueT,__gnu_cxx::hash<keyT>,Tidy::key_equal> ImpMapType;
				/// @brief 迭代
				typedef typename ImpMapType::iterator imp_map_iter;
				/// @brief 迭代
				typedef typename ImpMapType::const_iterator imp_map_const_iter;
				/// @brief 值类型
				typedef typename ImpMapType::value_type value_type;
				/// @brief hash_multimap容器
				ImpMapType _imp_map;
		};
};
#endif
