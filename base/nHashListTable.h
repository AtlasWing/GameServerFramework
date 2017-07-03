#ifndef BASE_NHASHLISTTABLE_H
#define BASE_NHASHLISTTABLE_H

#include "base/nSTL.h"

namespace Tidy
{
	template <class keyT, class valueT, class policyT>
		class nHashListTable
		{
			protected:
				DISALLOW_COPY_AND_ASSIGN(nHashListTable);
				nHashListTable()
				{
				}

				~nHashListTable()
				{
				}

			protected:
				bool insert(const keyT &key, const valueT &value)
				{
					HashMapIter iter = _hash_map.find(key);
					if(iter != _hash_map.end()){
						return false;
					}
					_imp_map.push_front(imp_map_value_type(key, value));
					ListNode *node = static_cast<ListNode*>(_imp_map.begin()._M_node);
					if(_hash_map.insert(HashMapValueType(key, node)).second == false){
						assert_fail("插入失败");
						_imp_map.pop_front();
						return false;
					}
					return true;
				}

				bool find(const keyT &key,valueT &value) const
				{
					HashMapConstIter it = _hash_map.find(key);
					if(it != _hash_map.end()) {
						value = it->second->_M_data.second;
						return true;
					}
					return false;
				}

				size_t remove(const keyT &key)
				{
					HashMapIter iter = _hash_map.find(key);
					if(iter == _hash_map.end()){
						return 0;
					}
					_imp_map.erase(imp_map_iter(iter->second));
					_hash_map.erase(iter);
					return 1;
				}

				void clear()
				{
					_hash_map.clear();
					_imp_map.clear();
				}

				size_t size() const
				{
					return _hash_map.size();
				}

				bool empty() const
				{
					return _imp_map.empty();
				}

			public:
				typedef Tidy::list< std::pair<keyT, valueT> > ImpMapType;
				typedef typename ImpMapType::iterator imp_map_iter;
				typedef typename ImpMapType::const_iterator imp_map_const_iter;
				typedef typename ImpMapType::value_type imp_map_value_type;
				typedef typename std::_List_node<imp_map_value_type> ListNode;

			protected:
				ImpMapType _imp_map;

			private:
				typedef Tidy::hash_map<keyT, ListNode*> HashMapType;
				typedef typename HashMapType::iterator HashMapIter;
				typedef typename HashMapType::const_iterator HashMapConstIter;
				typedef typename HashMapType::value_type HashMapValueType;
				HashMapType _hash_map;
		};
};
#endif
