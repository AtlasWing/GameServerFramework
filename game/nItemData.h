#ifndef GAME_NITEMDATA_H_
#define GAME_NITEMDATA_H_

#include "protocol/itemCommon.h"

#include "base/nAny.h"
#include "base/nAssert.h"
#include "base/nSTL.h"

namespace protocol{
	namespace item{
		struct ItemAttribute;
	};
};
class ItemBaseTable;
namespace Game
{
	class nItemMapAttr
	{
		public:
			nItemMapAttr():_attr_map(NULL){
			}

			~nItemMapAttr(){
				SAFE_DELETE(_attr_map);
			}

		public:
			bool hasData(protocol::item::eItemRareAttribute attr) const {
				return _attr_bit_set.test(attr);
			}

			template<typename ValueT>
				ValueT getData(protocol::item::eItemRareAttribute attr, ValueT defaultValue, Tidy::eAnyDataType dataType) const{
                    if(dataType == Tidy::eAnyDataType_bool){
                        return hasData(attr);
                    }
					if(hasData(attr) == false){
						return defaultValue;
					}
					const_AttrMapIter iter = _attr_map->find(attr);
					if(iter == _attr_map->end()){
						return defaultValue;
					}
					return static_cast<ValueT>(iter->second);
				}

			template<typename ValueT>
				void setData(protocol::item::eItemRareAttribute attr, ValueT value, const ValueT defaultValue, Tidy::eAnyDataType dataType){
                    if(dataType == Tidy::eAnyDataType_bool){
                        assert_debug(defaultValue == false, "默认值必须是默认false,要不然无法区分有属性和值了");
                        if(value == true){
                            _attr_bit_set.set(attr);
                        }
                        else{
                            _attr_bit_set.reset(attr);
                        }
                    }
                    else if(_attr_map == NULL) {
						if(value != defaultValue){
							_attr_map = new std::map<WORD, Tidy::nAny>();
							_attr_map->insert(std::pair<WORD, ValueT>(attr, value));
                            _attr_bit_set.set(attr);
						}
					}
					else{
                        AttrMapIter iter = _attr_map->lower_bound(attr);
                        if(iter == _attr_map->end() || iter->first != attr){
                            if(value != defaultValue){
                                _attr_map->insert(iter, std::pair<WORD, ValueT>(attr, value));
                                _attr_bit_set.set(attr);
                            }
                        }
                        else{
                            if(value != static_cast<ValueT>(iter->second)){
                                if(value == defaultValue){
                                    _attr_map->erase(iter);
                                    _attr_bit_set.reset(attr);
                                }
                                else{
                                    iter->second = value;
                                }
                            }
                        }
					}
				}

            void clearData(protocol::item::eItemRareAttribute attr, Tidy::eAnyDataType dataType){
                if(dataType != Tidy::eAnyDataType_bool && _attr_map != NULL){
                    _attr_map->erase(attr);
                }
                _attr_bit_set.reset(attr);
            }

            void setData(const nItemMapAttr &data){
				if(data._attr_map == NULL){
					return;
				}
				if(_attr_map == NULL){
					_attr_map = new std::map<WORD, Tidy::nAny>();
				}
				_attr_map->insert(data._attr_map->begin(), data._attr_map->end());
                _attr_bit_set = data._attr_bit_set;
			}

		private:
			typedef std::map<WORD, Tidy::nAny>::iterator AttrMapIter;
			typedef std::map<WORD, Tidy::nAny>::const_iterator const_AttrMapIter;
			std::map<WORD, Tidy::nAny> *_attr_map;
            Tidy::BitSet<protocol::item::eItemRareAttribute_Max> _attr_bit_set;
	};

	class nItemData
	{
		public:
			nItemData();
			~nItemData();

		public:
			bool saveAttr(protocol::item::ItemAttribute *itemAttr) const;
			bool restoreAttr(const protocol::item::ItemAttribute &attr);
			bool setData(const nItemData &srcData);
			bool initData(const ItemBaseTable *baseItemData);

		private:
			nItemMapAttr _rare_attribute;

		public:
#define ITEM_BASE_ATTRIBUTE(type, name, protoname, defaultValue, desc) \
		private:\
				type _##name; \
		public:\
			   const type& get##name() const { return _##name; } \
			type& get##name() { return _##name; } \
			void set##name(type value) { _##name=value; } \

#define ITEM_RARE_ATTRIBUTE(type,name,protoname,defaultValue,desc) \
		public:\
			   bool has##name() const{return _rare_attribute.hasData(protocol::item::eItemRareAttribute_##name);}\
			const type get##name() const { return _rare_attribute.getData<type>(protocol::item::eItemRareAttribute_##name, defaultValue, Tidy::eAnyDataType_##type); } \
			void set##name(type value) { _rare_attribute.setData<type>(protocol::item::eItemRareAttribute_##name, value, defaultValue, Tidy::eAnyDataType_##type); } \
			void clear##name() {_rare_attribute.clearData(protocol::item::eItemRareAttribute_##name, Tidy::eAnyDataType_##type);} \
			const type getDefault##name() {return defaultValue;} \

#include "protocol/itemAttribute.h"

#undef ITEM_RARE_ATTRIBUTE
#undef ITEM_BASE_ATTRIBUTE

	};
};

#endif
