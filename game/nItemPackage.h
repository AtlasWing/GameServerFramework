#ifndef GAME_NITEMPACKAGE_H_
#define GAME_NITEMPACKAGE_H_

#include "protocol/packageCommon.h"
#include "protocol/itemCommon.h"
#include "protocol/packageCommon.h"

#include "base/nSTL.h"

#include "game/nItemManager.h"
#include "game/nItem.h"
#include "game/nTable.h"

namespace Game
{
	template<typename UserT, typename ItemT>
		class nItemPackage
		{
			public:
				typedef UserT OwnerType;
				typedef ItemT ItemType;

			protected:
				nItemPackage(nItemManager<ItemType> &itemManager, const protocol::package::ePackageID packageID, const WORD capacity);
				virtual ~nItemPackage();

			public:
				eItemAddResult addAndUnionItem(UserT *owner, ItemType *item);
				eItemAddResult addItem(ItemType *item);
				eItemAddResult addItem(ItemType *item, WORD packagePos);
				eItemAddResult unionItem(UserT *user, ItemType *item);

			public:
				WORD getCapacity() const { return _capacity; }
				WORD getSpaceCapacity() const { return _capacity;}
				DWORD getItemCapacity(const ItemBaseTable *baseData, const DWORD itemNum, bool bind) const;

				WORD getSize() const { return _location_map.size() ; }
				bool isEmpty() const { return _location_map.empty();}
				BYTE getPackageID() const { return _package_id ;}
				BYTE getPackageType() const { return protocol::package::getPackageTypeByID(static_cast<protocol::package::ePackageID>(_package_id)) ;}

			public:	
				ItemType *getItemBySerialID(DWORD serialID);
				DWORD getItemListAndNumByBaseID(const DWORD baseID, Tidy::vector<ItemType*> *itemList) const;
				ItemType *getItemByPackagePos(const WORD packagePos) const;

			public:
				DWORD getItemNumByType(protocol::item::eItemType type) const;
				WORD  getItemNumByBaseID(const DWORD baseID) const;
				DWORD getUnionNum(const ItemBaseTable *itemBaseData, const bool bind, const DWORD maxUnionNum) const;  

			public:
				bool removeItem(ItemType *item);
				bool sortItem(UserT *user);

			public:
				bool checkAddAndUnionItem(const ItemBaseTable *baseData, const DWORD itemNum, bool bind) const;
				WORD getLeftCapacity() const;

			public:
				bool checkRemove(ItemType *item);
				bool checkAdd(ItemType *item, const WORD packagePos);
				bool checkAdd(ItemType *item);
				bool checkUnion() const;
				bool checkUse(ItemType *item) const;
				bool checkSwap(ItemT *item, const WORD packagePos);
				bool checkSort();
				bool checkSell(ItemT *item, const WORD num) const;
				bool checkPosEmpty(const WORD packagePos) const;
				bool checkPosValid(const WORD packagePos) const;
				bool checkDataError() const;
				WORD getEmptyPos() const;
				bool checkLeftSpace() const;

			public:
				void printPackage();

			public:
				template <class ExecT>
					bool execEveryItem(ExecT &exec);
				template<typename GetExecT, typename EntityList>
					void get(const GetExecT& exec, EntityList& entityList) const;
				template <class ExecT>
					bool execEveryItem(ExecT &exec)const;

			protected:
				void setCapacity(WORD num) { _capacity = num; }

			private:
				bool checkSwap(ItemType *item, protocol::item::ItemLocation &destLoc);
				bool checkItemCapacity(const ItemBaseTable *baseData, const DWORD itemNum, bool bind) const;

			private:
				virtual bool dealCheckAdd(ItemType *item, const WORD packagePos) const;
				virtual bool dealCheckUse(ItemType *item) const;
				virtual bool dealCheckUnion() const;

			private:
				virtual void dealAdd(ItemType *item) = 0;
				virtual void dealRemove(ItemType *item) = 0;
				virtual bool dealCheckSort() const = 0;
				virtual bool dealCheckSell(ItemType *item, const WORD num) const = 0;
				virtual void dealSort(const Tidy::vector<ItemT*> &removeList) = 0;
				virtual void dealUnion() = 0;
#if 0
			public:
				bool add(ItemType* item);
				bool remove(ItemType* item);
				bool clear();
				bool destory();


				/// @brief 道具操作回调
				virtual bool dealSort();

				/// @brief 道具操作检查回调
				virtual bool dealCheckRemove(ItemType *item);
				virtual bool dealCheckUnionTo(ItemType *item, protocol::item::ItemLocation &destLoc);
				virtual bool dealCheckSort() { return false;}
#endif

			private:
				virtual bool dealCheckRemove(ItemType *item);

			public:
				/// @brief 	包裹内的物品索引容器, key值是ItemType的SerialID
				typedef std::map<WORD, ItemType*> LocationMap;
				typedef typename LocationMap::iterator LocationMapIter;
				typedef typename LocationMap::const_iterator const_LocationMapIter;

			protected:
				LocationMap _location_map;
				nItemManager<ItemT> &_item_manager;///< 物品管理器

			private:
				const BYTE _package_id;                             ///< 包裹类型
				WORD _capacity;										///< 包裹合法可用格数
		};

	template<typename UserT, typename ItemT>
		nItemPackage<UserT, ItemT>::nItemPackage(nItemManager<ItemType> &itemManager, const protocol::package::ePackageID packageID, const WORD capacity):
			_item_manager(itemManager),
			_package_id(packageID),
			_capacity(capacity)
	{
	}

	template<typename UserT, typename ItemT>
		nItemPackage<UserT, ItemT>::~nItemPackage()
		{
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::checkAddAndUnionItem(const ItemBaseTable *baseData, const DWORD itemNum, bool bind) const
		{
			if(baseData->maxRepeat == 0){
				assert_fail("不能自动放到包裹里");
				return false;
			}
			// 只是检查空间
			return checkItemCapacity(baseData, itemNum, bind);
		}

	namespace
	{
		template<typename ItemT>
			struct GetUnionItemCapacityExec{
				typedef ItemT ItemType;
				const ItemBaseTable *_base_data;
				const bool _bind;
				DWORD unionCapacity;
				GetUnionItemCapacityExec(const ItemBaseTable *baseData, const bool bind):
					_base_data(baseData),
					_bind(bind),
					unionCapacity(0)
				{
				}
				bool exec(ItemType *item){
					if(item->checkUnion(_base_data, 1, _bind) == true){
						unionCapacity += item->getLeftOverlayNum();
					}
					return true;
				}
			};
	};
	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::checkItemCapacity(const ItemBaseTable *baseData, const DWORD itemNum, bool bind) const
		{
			DWORD itemCapacity = getLeftCapacity() * baseData->maxRepeat;
			if(itemCapacity >= itemNum){
				return true;
			}

			if(checkUnion() == false){
				return false;
			}

			if(baseData->maxRepeat > 1){
				GetUnionItemCapacityExec<ItemType> exec(baseData, bind);
				execEveryItem(exec);
				itemCapacity += exec.unionCapacity;
				if(itemCapacity >= itemNum)
				{
					return true;
				}
			}

			return true;
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::checkAdd(ItemType *item)
		{
			return checkAdd(item, item->getPackagePos());
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::checkAdd(ItemType *item, const WORD packagePos)
		{
			if(packagePos != protocol::item::nInvalidPackagePos && checkPosEmpty(packagePos) == false){
				return false;
			}
			else if(packagePos == protocol::item::nInvalidPackagePos && checkLeftSpace() == false){
				return false;
			}

			return dealCheckAdd(item, packagePos);
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::checkUnion() const
		{
			return dealCheckUnion();
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::checkUse(ItemType *item) const
		{
			return dealCheckUse(item);
		}

	template<typename UserT, typename ItemT>
		eItemAddResult nItemPackage<UserT, ItemT>::addAndUnionItem(UserT *user, ItemType *item)
		{
			eItemAddResult result = eItemAddResult_Max;
			do{
				eItemAddResult unionResult = eItemAddResult_NoChange;
				if(item->checkLocationValid() == false && item->baseItemData->maxOverlayNum > 1 && checkUnion() == true){
					// 没有设置位置,而且可以重叠才合并
					unionResult = unionItem(user, item);
					if(unionResult == eItemAddResult_AllUnioned){
						result = eItemAddResult_AllUnioned;
						break;
					}
					else if(unionResult == eItemAddResult_Fail){
						result = eItemAddResult_Fail;
						break;
					}
					else if(unionResult == eItemAddResult_PartUnioned || unionResult == eItemAddResult_NoChange){
					}
					else{
						assert_fail("无法处理的返回值");
					}
				}
				result = addItem(item);
				if(result == eItemAddResult_Succ){
					break;
				}
				else if(result == eItemAddResult_Fail){
					if(unionResult == eItemAddResult_PartUnioned){
						result = eItemAddResult_PartUnioned;
					}
					break;
				}
				else{
					assert_fail("未处理的结果");
				}
			}while(0);

			assert_debug(result != eItemAddResult_Max, "无效返回值");
			return result;
		}

	namespace{
		template<typename ItemT>
			struct UnionItemExec{
				typedef ItemT ItemType;
				Tidy::vector<ItemType*> canUnionItemVec;
				DWORD unionNum;
				ItemType *unionItem;
				UnionItemExec(ItemType *item):
					unionNum(0),
					unionItem(item)
				{
				}
				bool exec(ItemType *item){
					if(item->checkUnion(unionItem, 1) == true){
						canUnionItemVec.push_back(item);
						unionNum += item->getLeftOverlayNum();
						if(unionNum >= unionItem->data.getNum()){
							return false;
						}
					}
					return true;
				}
			};
	};

	template<typename UserT, typename ItemT>
		eItemAddResult nItemPackage<UserT, ItemT>::unionItem(UserT *user, ItemType *item)
		{
			assert_debug(item->baseItemData->maxRepeat > 1, "");
			assert_debug(item->checkLocationValid() == false, "");

			UnionItemExec<ItemT> exec(item);
			execEveryItem(exec);

			if(exec.canUnionItemVec.empty() == true){
				return eItemAddResult_NoChange;
			}

			dealUnion();

			for(typename Tidy::vector<ItemType*>::iterator iter = exec.canUnionItemVec.begin(); iter != exec.canUnionItemVec.end(); ++ iter){
				ItemType *destItem = (*iter);
				assert_debug(destItem->getLeftOverlayNum() > 0, "");
				if(destItem->unionItem(item, (destItem->getLeftOverlayNum() > item->data.getNum()) ? item->data.getNum() : destItem->getLeftOverlayNum(), user, "Union") == eItemAddResult_AllUnioned){
					return eItemAddResult_AllUnioned;
				}
			}

			return eItemAddResult_PartUnioned;
		}

	template<typename UserT, typename ItemT>
		eItemAddResult nItemPackage<UserT, ItemT>::addItem(ItemType *item, WORD packagePos)
		{
			if(packagePos == protocol::item::nInvalidPackagePos){
				if(checkLeftSpace() == false){
					return eItemAddResult_Fail;
				}
				packagePos = getEmptyPos();
				if(packagePos == protocol::item::nInvalidPackagePos){
					return eItemAddResult_Fail;
				}
			}
			else if(checkAdd(item, packagePos) == false){
				return eItemAddResult_Fail;
			}

			assert_debug(_location_map.find(packagePos) == _location_map.end(), "");
			_location_map[packagePos] = item;

			if(_item_manager.addItem(item) == false) {
				_location_map.erase(packagePos);
				assert_fail("添加道具失败");
				return eItemAddResult_Fail;
			}

			item->setLocation(protocol::item::ItemLocation(_package_id, packagePos));

			dealAdd(item);

#ifdef _DEBUG
			assert_debug(checkDataError(), "数据错误");
#endif
			return eItemAddResult_Succ;
		}

	template<typename UserT, typename ItemT>
		eItemAddResult nItemPackage<UserT, ItemT>::addItem(ItemType *item)
		{
			return addItem(item, item->getPackagePos());
		}

	template<typename UserT, typename ItemT>
		ItemT* nItemPackage<UserT, ItemT>::getItemByPackagePos(const WORD packagePos) const
		{
			const_LocationMapIter iter = _location_map.find(packagePos);
			if(iter != _location_map.end()){
                return iter->second;
            }
            return NULL;
		}

	template<typename UserT, typename ItemT>
		DWORD nItemPackage<UserT, ItemT>::getItemListAndNumByBaseID(const DWORD baseID, Tidy::vector<ItemType*> *itemList) const
		{
			DWORD num = 0;
			for(const_LocationMapIter iter = _location_map.begin(); iter != _location_map.end(); ++ iter){
				if(iter->second->getBaseID() == baseID){
					itemList->push_back(iter->second);
					num += iter->second->data.getNum();
				}
			}
			return num;
		}

	template<typename UserT, typename ItemT>
		WORD nItemPackage<UserT, ItemT>::getItemNumByBaseID(const DWORD baseID) const
		{
			DWORD num = 0;
			for(const_LocationMapIter iter = _location_map.begin(); iter != _location_map.end(); ++ iter){
				if(iter->second->getBaseID() == baseID){
					num += iter->second->data.getNum();
				}
			}
			return num;
		}

	template<typename UserT, typename ItemT>
		DWORD nItemPackage<UserT, ItemT>::getUnionNum(const ItemBaseTable *itemBaseData, const bool bind, const DWORD maxUnionNum) const
		{
			//使用maxUnionNum来控制部分检测还是全背包检测
			DWORD num = 0;
			for(const_LocationMapIter iter = _location_map.begin(); iter != _location_map.end(); ++ iter){
				if(iter->second->checkUnion(itemBaseData, bind) == true){
					num += iter->second->getLeftOverlayNum();
					if(maxUnionNum > 0 && num >= maxUnionNum){
						break;
					}
				}
			}
			return num;
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::removeItem(ItemType *item)
		{
			assert_debug(_location_map.find(item->getPackagePos()) != _location_map.end(), "");
			_item_manager.removeItem(item);
			_location_map.erase(item->getPackagePos());
			dealRemove(item);
			item->setLocation(protocol::item::ItemLocation(protocol::item::nInvalidPackageLocation));

#ifdef _DEBUG
			assert_debug(checkDataError(), "数据错误");
#endif
			return true;
		}

	namespace {
		template<typename ItemT>
			struct SortItemCompare {
				SortItemCompare(){
				}
				bool operator ()(const ItemT *leftItem, const ItemT *rightItem){
					if(leftItem->baseItemData->type != rightItem->baseItemData->type){
						return leftItem->baseItemData->type < rightItem->baseItemData->type;
					}
					if(leftItem->getBaseID() != rightItem->getBaseID()){
						return leftItem->getBaseID() < rightItem->getBaseID();
					}
					if(leftItem->data.getBind() != rightItem->data.getBind()){
						return leftItem->data.getBind() == false;
					}
					if(leftItem->data.getNum() != rightItem->data.getNum()){
						return leftItem->data.getNum() > rightItem->data.getNum();
					}
					return false;
				}
			};
	};
	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::sortItem(UserT *user)
		{
			Tidy::vector<ItemT*> itemVec;
			for(const_LocationMapIter iter = _location_map.begin(); iter != _location_map.end(); ++ iter){
				itemVec.push_back(iter->second);
			}

			SortItemCompare<ItemT> compare;
			std::sort(itemVec.begin(), itemVec.end(), compare);

			DWORD loc = 0;
			bool isChanged = false;
			Tidy::vector<ItemT*> removeList;
			_location_map.clear();
			for(DWORD i = 0; i < itemVec.size(); ++ i){
				ItemT *item = itemVec[i];
				if(item->getPackagePos() != loc){
					isChanged = true;
					item->setLocation(_package_id, loc);
				}
				_location_map[loc] = itemVec[i];
				++ loc;

				for(DWORD j = i + 1; j < itemVec.size() && item->checkUnion(itemVec[j], 1) == true; ++ j){
					eItemAddResult result = item->unionItem(itemVec[j], (item->getLeftOverlayNum() > itemVec[j]->data.getNum()) ? itemVec[j]->data.getNum() : item->getLeftOverlayNum(), user, "Sort Union");
					if(result == eItemAddResult_AllUnioned){
						removeList.push_back(itemVec[j]);
						++ i;
					}
					else{
						break;
					}
				}
			}

			isChanged = isChanged || (removeList.empty() == false);
			for(DWORD i = 0; i < removeList.size(); ++ i){
				removeList[i]->setLocation(_package_id, loc);
				_location_map[loc] = removeList[i];
				++ loc;
			}

#ifdef _DEBUG
			assert_debug(checkDataError(), "数据错误");
#endif
			if(isChanged == false){
				return true;
			}
			dealSort(removeList);
			return true;
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::checkRemove(ItemT *item){
			assert_debug(getItemByPackagePos(item->getPackagePos()) == item, "");
			return dealCheckRemove(item);
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::checkSwap(ItemT *item, const WORD packagePos){
			if(getItemByPackagePos(packagePos) == NULL){
				assert_fail("要交换的位置上没有道具");
				return false;
			}

			return dealCheckAdd(item, packagePos);
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::checkSort()
		{
			return this->dealCheckSort();
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::checkSell(ItemT *item, const WORD num) const
		{
			if(item->data.getNum() < num){
				const WORD itemNum = getItemNumByBaseID(item->getBaseID());
				if(itemNum < num){
					return false;
				}
			}
			return dealCheckSell(item, num);
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::dealCheckRemove(ItemT *item)
		{
			return true;
		}

	template<typename UserT, typename ItemT>
		WORD nItemPackage<UserT, ItemT>::getEmptyPos() const
		{
			assert_debug(checkLeftSpace() == true, "");

			WORD pos = 0;
			// 利用了_location_map的从小到大的排序
			for(const_LocationMapIter iter = _location_map.begin(); iter != _location_map.end(); ++ iter)
			{
				if(pos == iter->first){
					assert_debug(iter->second != NULL, "");
					++ pos;
				}
				else {
					return pos;
				}
			}

			if(pos < getSpaceCapacity()){
				return pos;
			}

			return protocol::item::nInvalidPackagePos;
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::checkPosEmpty(const WORD packagePos) const
		{
			return _location_map.find(packagePos) == _location_map.end();
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::checkPosValid(const WORD packagePos) const
		{
			return packagePos < _capacity;
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::checkDataError() const
		{
			for(const_LocationMapIter iter = _location_map.begin(); iter != _location_map.end(); ++ iter){
				if(iter->first != iter->second->getPackagePos()){
					return false;
				}
			}

			return true;
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::dealCheckAdd(ItemType *item, const WORD packagePos) const
		{
			return true;
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::dealCheckUse(ItemType *item) const
		{
			return true;
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::dealCheckUnion() const
		{
			return true;
		}

	template<typename UserT, typename ItemT>
		bool nItemPackage<UserT, ItemT>::checkLeftSpace() const	
		{ 
			return getSpaceCapacity() > _location_map.size(); 
		}

	template<typename UserT, typename ItemT>
		WORD nItemPackage<UserT, ItemT>::getLeftCapacity() const 
		{ 
			assert_debug(getSpaceCapacity() >= _location_map.size(), "");
			return getSpaceCapacity() - _location_map.size() ; 
		}

	template<typename UserT, typename ItemT>
		template <class ExecT>
		bool nItemPackage<UserT, ItemT>::execEveryItem(ExecT &exec)
		{
			ItemT *item = NULL;
			for(LocationMapIter iter = _location_map.begin(); iter != _location_map.end(); ){
				item = iter->second;
				++ iter;
				if(exec.exec(item) == false) {
					return false;
				}
			}

			return true;
		}

	template<typename UserT, typename ItemT>
		template <class ExecT>
		bool nItemPackage<UserT, ItemT>::execEveryItem(ExecT &exec) const
		{
			const_LocationMapIter tempIter;
			for(const_LocationMapIter iter = _location_map.begin(); iter != _location_map.end(); ){
				tempIter = iter;
				++ iter;
				if(exec.exec(tempIter->second) == false) {
					return false;
				}
			}

			return true;
		}

};
#endif
