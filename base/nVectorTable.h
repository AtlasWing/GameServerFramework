#ifndef _N_VECTOR_TABLE_H_
#define _N_VECTOR_TABLE_H_

#include "base/nSTL.h"

namespace Tidy
{
	template <class keyT, class valueT>
		class nVectorTable
		{
			protected:
				DISALLOW_COPY_AND_ASSIGN(nVectorTable);
				nVectorTable()
				{
				}

				~nVectorTable()
				{
				}

			protected:
				bool insert(const keyT &key, const valueT &value)
				{
					for(imp_map_iter iter = _imp_map.begin(); iter != _imp_map.end(); ++ iter) {
						if(key == iter->first) {
							iter->second = value;
							return false;
						}
					}
					_imp_map.push_back(value_type(key, value));
					return true;
				}

				bool find(const keyT &key,valueT &value) const
				{
					for(imp_map_const_iter iter = _imp_map.begin(); iter != _imp_map.end(); ++ iter) {
						if(key == iter->first) {
							value = iter->second;
							return true;
						}
					}
					return false;
				}

				size_t remove(const keyT &key)
				{
					for(imp_map_iter iter = _imp_map.begin(); iter != _imp_map.end(); ++ iter) {
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
				typedef Tidy::vector<value_type> ImpMapType;
				typedef typename ImpMapType::iterator imp_map_iter;
				typedef typename ImpMapType::const_iterator imp_map_const_iter;

			protected:
				ImpMapType _imp_map;
		};
};

#endif

