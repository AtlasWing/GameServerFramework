#ifndef BASE_N_SEQUENCE_H_
#define BASE_N_SEQUENCE_H_

#include "base/nSTL.h"

namespace Tidy
{
	template<typename SeqPolicyT, bool downSeq = true>
		struct SeqValueCompare
		{
			bool operator()(const std::pair<typename SeqPolicyT::KeyT, typename SeqPolicyT::ValueT> &data_0, const std::pair<typename SeqPolicyT::KeyT, typename SeqPolicyT::ValueT> &data_1) const
			{
				return (data_0.second > data_1.second || (data_0.second == data_1.second && SeqPolicyT::greater(data_0.first, data_1.first)));
			}
			bool operator()(const typename SeqPolicyT::KeyT &data_0, const typename SeqPolicyT::KeyT &data_1) const
			{
				return (SeqPolicyT::getValue(data_0) > SeqPolicyT::getValue(data_1) || (SeqPolicyT::getValue(data_0) == SeqPolicyT::getValue(data_1) && SeqPolicyT::greater(data_0, data_1)));
			}
		};

	//升序偏特化
	template<typename SeqPolicyT>
		struct SeqValueCompare<SeqPolicyT, false>
		{
			bool operator()(const std::pair<typename SeqPolicyT::KeyT, typename SeqPolicyT::ValueT> &data_0, const std::pair<typename SeqPolicyT::KeyT, typename SeqPolicyT::ValueT> &data_1) const
			{
				return (data_0.second < data_1.second || (data_0.second == data_1.second && SeqPolicyT::greater(data_1.first, data_0.first)));
			}
			bool operator()(const typename SeqPolicyT::KeyT &data_0, const typename SeqPolicyT::KeyT &data_1) const
			{
				return (SeqPolicyT::getValue(data_0) < SeqPolicyT::getValue(data_1) || (SeqPolicyT::getValue(data_0) == SeqPolicyT::getValue(data_1) && SeqPolicyT::greater(data_1, data_0)));
			}
		};

	template<typename SeqPolicyT, bool needStoreValue=true>
		struct VisitPolicy
		{
			typedef typename SeqPolicyT::KeyT KeyT;
			typedef typename SeqPolicyT::ValueT ValueT;
			typedef std::pair<KeyT, ValueT> DataSetValueT;

			//不提供getValueByKey接口,使用的话编译出错
			static const ValueT& getValueByKey(const KeyT& key);
			inline static const ValueT& getDataSetValue(const DataSetValueT& setValue)
			{
				return setValue.second;
			}
			inline static DataSetValueT constructDataSetValue(const KeyT& key, const ValueT& value)
			{
				return DataSetValueT(key, value);
			}
			inline static const KeyT& getDataSetKey(const DataSetValueT& setValue)
			{
				return setValue.first;
			}
		};

	template<typename SeqPolicyT>
		struct VisitPolicy<SeqPolicyT, false>
		{
			typedef typename SeqPolicyT::KeyT KeyT;
			typedef typename SeqPolicyT::ValueT ValueT;
			typedef KeyT DataSetValueT;

			inline static const ValueT getValueByKey(const KeyT& key)
			{
				return SeqPolicyT::getValue(key);
			}
			inline static const ValueT getDataSetValue(const DataSetValueT& setValue)
			{
				return SeqPolicyT::getValue(setValue);
			}
			inline static const KeyT& getDataSetKey(const DataSetValueT& setValue)
			{
				return setValue;
			}
			inline static const DataSetValueT constructDataSetValue(const KeyT& key, const ValueT& value)
			{
				return key;
			}
		};

	/** example
	  struct Policy
	  {
	  typedef RelationUser* KeyT;
	  typedef DWORD ValueT;
	  };
	 **/
	template<typename SeqPolicyT, bool downSeq, bool needStoreValue>
		class nSequence
		{
			public:
				typedef typename SeqPolicyT::KeyT KeyT;
				typedef typename SeqPolicyT::ValueT ValueT;
				typedef VisitPolicy<SeqPolicyT, needStoreValue> VistPolicyT;

			public:
				nSequence(const BYTE seqType, const DWORD maxSize) : _seq_type(seqType), _max_size(maxSize), _updated(false)
			{
				assert_debug(_max_size > 0, "数量为0");
			}
				~nSequence()
				{
				}

			public:
                bool removeData(const KeyT& key) {
                    KeyMapIterator keyIter = _key_map.find(key);
                    if(keyIter == _key_map.end()) {
                        return true;
                    }
                    DataSetIterator dataIter(static_cast<typename DataSetType::iterator::_Link_type>(keyIter->second));

                    DWORD orderNum = 1;
                    DataSetIterator iter = _data_set.begin();
                    for(; iter != dataIter; ++ iter, ++ orderNum) {
                    }

                    _data_set.erase(dataIter);
                    _key_map.erase(keyIter);

                    //名次更新处理
                    SeqPolicyT::dealChangeOrderNum(key, orderNum, 0);
                    ++ iter;
                    ++ orderNum;
                    for(; iter != _data_set.end(); ++ iter, ++ orderNum) {
                        SeqPolicyT::dealChangeOrderNum(VistPolicyT::getDataSetKey(*iter), orderNum, orderNum-1);
                    }
                    assert_debug(iter == _data_set.end(), "迭代器错误");
                    _updated = true;
#ifdef _DEBUG
                    checkDataValid();
#endif
                    return true;
                }

				bool updateData(const KeyT& key) {
					return updateData(key, VistPolicyT::getValueByKey(key));
				}

				bool updateData(const KeyT& key, const ValueT value) {
					KeyMapIterator keyIter = _key_map.find(key);
					if(keyIter == _key_map.end()) {
						bool isFull = (_key_map.size() >= _max_size);
						if(isFull == true) {
							if(downSeq == true && VistPolicyT::getDataSetValue((*_data_set.rbegin())) >= value) {
								return true;
							}
							if(downSeq == false && VistPolicyT::getDataSetValue((*_data_set.rbegin())) <= value) {
								return true;
							}
						}
						DataSetIterator dataIter = (_data_set.insert(VistPolicyT::constructDataSetValue(key, value))).first;
						_key_map.insert(typename KeyMapType::value_type(key, dataIter._M_node));

						DWORD orderNum = 1;
						DataSetIterator iter = _data_set.begin();
						for(; iter != dataIter; ++ iter, ++ orderNum) {
						}
						//名次更新处理
						//在effect size 内
						SeqPolicyT::dealChangeOrderNum(key, 0, orderNum);
						++ iter;
						++ orderNum;
						//在effect size 内顺序递加的元素
						for(; iter != _data_set.end(); ++ iter, ++ orderNum) {
							SeqPolicyT::dealChangeOrderNum(VistPolicyT::getDataSetKey(*iter), orderNum - 1, orderNum);
						}
						assert_debug(iter == _data_set.end(), "迭代器错误");

						if(isFull == true) {
							const DataSetValueType& removeData = *_data_set.rbegin();
							const KeyT& removeKey = VistPolicyT::getDataSetKey(removeData);
							_key_map.erase(removeKey);
							_data_set.erase(--(_data_set.rbegin().base()));
						}
						_updated = true;
					}
					else {
						DataSetIterator dataIter(static_cast<typename DataSetIterator::_Link_type>(keyIter->second));
						if(needStoreValue == false || value != VistPolicyT::getDataSetValue(*dataIter)) {
							DWORD oldOrderNum = 1;
							for(DataSetIterator iter = _data_set.begin(); iter != dataIter; ++ iter, ++ oldOrderNum) {
							}

							DataSetIterator insertIter = dataIter;
							if(dataIter == _data_set.begin()){
								++ insertIter;
							}
							else{
								-- insertIter;
							}
							_data_set.erase(dataIter);
							if(_data_set.empty() == true){
								insertIter = _data_set.end();
							}
							dataIter = _data_set.insert(insertIter, VistPolicyT::constructDataSetValue(key, value));
							keyIter->second = dataIter._M_node;

							DWORD orderNum = 1;
							for(DataSetIterator iter = _data_set.begin(); iter != dataIter && orderNum <= _max_size; ++ iter, ++ orderNum) {
							}
							//名次更新处理
							if(oldOrderNum != orderNum && (oldOrderNum <= _max_size || orderNum <= _max_size)) {
								if(oldOrderNum > _max_size && orderNum <= _max_size) {
									SeqPolicyT::dealChangeOrderNum(key, 0, orderNum);
									//在effect size 内
									DataSetIterator iter = dataIter;
									++ iter;
									++ orderNum;
									//在effect size 内顺序递加的元素
									for(; iter != _data_set.end(); ++ iter, ++ orderNum) {
										SeqPolicyT::dealChangeOrderNum(VistPolicyT::getDataSetKey(*iter), orderNum-1, orderNum);
									}
									assert_debug(iter == _data_set.end(), "迭代器错误");
								}
								else if(oldOrderNum <= _max_size && orderNum <= _max_size) {
									SeqPolicyT::dealChangeOrderNum(key, oldOrderNum, orderNum);
									if(oldOrderNum < orderNum) {
										if(insertIter == _data_set.end()) {
											insertIter = _data_set.begin();
										}
										else {
											++ insertIter;
										}
										DataSetIterator iter = insertIter;
										for(; iter != dataIter && iter != _data_set.end(); ++ iter, ++ oldOrderNum) {
											SeqPolicyT::dealChangeOrderNum(VistPolicyT::getDataSetKey(*iter), oldOrderNum+1, oldOrderNum);
										}
										assert_debug(iter != _data_set.end(), "死循环");
									}
									else if(oldOrderNum > orderNum) {
										++ insertIter;
										DataSetIterator iter = dataIter;
										++ iter;
										for(; iter != insertIter && iter != _data_set.end(); ++ iter, ++ orderNum) {
											SeqPolicyT::dealChangeOrderNum(VistPolicyT::getDataSetKey(*iter), orderNum, orderNum+1);
										}
										assert_debug(insertIter == _data_set.end() || iter != _data_set.end(), "");
									}
								}
								else if(oldOrderNum <= _max_size && orderNum > _max_size) {
									SeqPolicyT::dealChangeOrderNum(key, oldOrderNum, 0);
									DataSetIterator iter = insertIter;
									if(insertIter == _data_set.end()) {
										iter = _data_set.begin();
									}
									else {
										++ iter;
									}
									for(; iter != _data_set.end() && oldOrderNum < _max_size; ++ iter, ++ oldOrderNum) {
										SeqPolicyT::dealChangeOrderNum(VistPolicyT::getDataSetKey(*iter), oldOrderNum+1, oldOrderNum);
									}
									if(iter != _data_set.end()) {
										assert_debug(oldOrderNum == _max_size, "死循环");
										SeqPolicyT::dealChangeOrderNum(VistPolicyT::getDataSetKey(*iter), 0, oldOrderNum);
									}
								}
							}
							_updated = true;
						}
					}
#ifdef _DEBUG
                    checkDataValid();
#endif
					return true;
				}

				const bool getValue(const KeyT& key, ValueT& value) const
				{
					typename KeyMapType::const_iterator iter = _key_map.find(key);
					if(iter == _key_map.end()) {
						return false;
					}
					value = VistPolicyT::getDataSetValue((static_cast<typename DataSetIterator::_Link_type>(iter->second))->_M_value_field);
					return true;
				}

				//获取排名
				const bool getOrderNum(const KeyT& key, DWORD *orderNum) const
				{
					const_KeyMapIterator keyIter = _key_map.find(key); 
					if(keyIter == _key_map.end()) {
						*orderNum = 0;
						return false;
					}
					*orderNum = 1;
					DataSetIterator dataIter(static_cast<typename DataSetIterator::_Link_type>(keyIter->second));
					const_DataSetIterator endIter = _data_set.end();
					DataSetIterator iter = _data_set.begin();
					//TODO 有死循环,不应该,先加一个iter != endIter 避免
					for(; iter != dataIter && iter != endIter; ++ iter, ++ (*orderNum)) {
					}
					assert_debug(iter != endIter, "死循环");
					return true;
				}

				void clear()
				{
					_key_map.clear();
					_data_set.clear();
					_updated = true;
				}

				const bool isUpdated() const
				{
					return _updated;
				}

				void clearUpdated()
				{
					_updated = false;
				}

			public:
				template<typename ExecT>
					bool execAllInSeq(ExecT& exec, DWORD num)
					{
						DWORD seq = 0;
						for(typename DataSetType::iterator iter = _data_set.begin(); iter != _data_set.end(); ++ iter) {
							++ seq;
							if(exec.exec(VistPolicyT::getDataSetKey(*iter), VistPolicyT::getDataSetValue(*iter), seq) == false) {
								return false;
							}
							if(num > 0 && seq >= num){
								return true;
							}
						}

						return true;
					}

			public:
				const bool checkIn(const KeyT& key) const
				{
					return _key_map.find(key) != _key_map.end();
				}

            private:
                void checkDataValid()
                {
                    for(typename KeyMapType::iterator keyIter = _key_map.begin(); keyIter != _key_map.end(); ++ keyIter){
                        DataSetIterator dataIter(static_cast<typename DataSetIterator::_Link_type>(keyIter->second));
                        const DataSetValueType &dataSetValue = *dataIter;
                        if(VistPolicyT::getDataSetKey(dataSetValue) != keyIter->first){
                            assert_fail("排行榜[%u]数据错误", _seq_type);
                        }
                    }
                }

			public:
				//typedef std::pair<KeyT, ValueT> DataSetValueType;
				typedef typename VistPolicyT::DataSetValueT DataSetValueType;
				typedef std::set<DataSetValueType, SeqValueCompare<SeqPolicyT, downSeq> > DataSetType;
				typedef typename DataSetType::iterator DataSetIterator;
				typedef typename DataSetType::const_iterator const_DataSetIterator;
				typedef typename DataSetType::reverse_iterator DataSetReverseIterator;
				//数量大的话可以用Tidy::hash_map
				typedef std::map<KeyT, typename DataSetType::iterator::_Base_ptr> KeyMapType;
				typedef typename KeyMapType::iterator KeyMapIterator;
				typedef typename KeyMapType::const_iterator const_KeyMapIterator;
				DataSetType _data_set;
				KeyMapType _key_map;
                const BYTE _seq_type;
				DWORD _max_size;
				bool _updated;
		};
};

#endif
