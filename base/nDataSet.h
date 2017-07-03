#ifndef BASE_NDATASET_H_
#define BASE_NDATASET_H_

#include "base/nSTL.h"

namespace Tidy
{
	template<typename DataT>
		class nDataSet
		{
			protected:
				nDataSet();
				~nDataSet();

			protected:
				bool add(const DataT &data);
				bool remove(const DataT &data);
				template<typename ExecT>
					bool execAll(ExecT &exec);
				template<typename ExecT>
					bool execAll(ExecT &exec) const;
				template<typename ListT>
					bool getAll(ListT &list) const;

			private:
				typedef Tidy::hash_set<DataT> DataSet;
				typedef typename DataSet::const_iterator const_DataSetIter;
				typedef typename DataSet::iterator DataSetIter;
				DataSet _data_set;
		};

	template<typename DataT>
		nDataSet<DataT>::nDataSet()
		{
		}

	template<typename DataT>
		nDataSet<DataT>::~nDataSet()
		{
		}

	template<typename DataT>
		bool nDataSet<DataT>::add(const DataT &data)
		{
			_data_set.insert(data);
			return true;
		}

	template<typename DataT>
		bool nDataSet<DataT>::remove(const DataT &data)
		{
			_data_set.erase(data);
			return true;
		}

	template<typename DataT>
		template<typename ExecT>
		bool nDataSet<DataT>::execAll(ExecT &exec)
		{
			for(DataSetIter iter = _data_set.begin(); iter != _data_set.end();  ++ iter){
				if(exec.exe(*iter) == false){
					return false;
				}
			}
			return true;
		}

	template<typename DataT>
		template<typename ExecT>
		bool nDataSet<DataT>::execAll(ExecT &exec) const
		{
			for(const_DataSetIter iter = _data_set.begin(); iter != _data_set.end();  ++ iter){
				if(exec.exe(*iter) == false){
					return false;
				}
			}
			return true;
		}

	template<typename DataT>
		template<typename ListT>
		bool nDataSet<DataT>::getAll(ListT &list) const
		{
			for(const_DataSetIter iter = _data_set.begin(); iter != _data_set.end();  ++ iter){
				list.push_back(*iter);
			}
			return true;
		}
};

#endif
