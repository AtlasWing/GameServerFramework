#ifndef _N_LIST_TABLE_H_
#define _N_LIST_TABLE_H_

#include "base/nSTL.h"

namespace Tidy
{
	template <class keyT, class valueT>
		class nListTable
		{
			protected:
				DISALLOW_COPY_AND_ASSIGN(nListTable);
				nListTable()
				{
				}

				~nListTable()
				{
				}

			protected:
				bool insert(const keyT &key, const valueT &value)
				{
					imp_map_iter endIter = _imp_map.end();
					for(imp_map_iter iter = _imp_map.begin(); iter != endIter; ++ iter) {
						if(key == iter->first) {
							return false;
						}
					}
					_imp_map.push_back(value_type(key, value));
					return true;
				}

				bool find(const keyT &key,valueT &value) const
				{
					imp_map_const_iter endIter = _imp_map.end();
					for(imp_map_const_iter iter = _imp_map.begin(); iter != endIter; ++ iter) {
						if(key == iter->first) {
							value = iter->second;
							return true;
						}
					}
					return false;
				}

				size_t remove(const keyT &key)
				{
					imp_map_iter endIter = _imp_map.end();
					for(imp_map_iter iter = _imp_map.begin(); iter != endIter; ++ iter) {
						if(key == iter->first) {
							_imp_map.erase(iter);
							return 1;
						}
					}
					return 0;
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
				typedef std::pair<keyT, valueT> value_type;
				typedef Tidy::list<value_type> ImpMapType;
				typedef typename ImpMapType::iterator imp_map_iter;
				typedef typename ImpMapType::const_iterator imp_map_const_iter;

			protected:
				ImpMapType _imp_map;
		};
};

#endif
