#include "protocol/Proto/ItemData.pb.h"
#include "protocol/Proto/EquipAttribute.pb.h"

#include "game/nItem.h"
#include "game/nGameServer.h"
#include "game/nTable.h"
#include "game/nTableManager.h"
#include "base/nRandom.h"
#include "protocol/equipData.h"

namespace Game
{
	nItem::nItem(const QWORD itemGlobalID, const DWORD itemSerialID, const ItemBaseTable *itemBaseData, const bool needBind, const DWORD num, const DWORD quality, const WORD identify):
        baseItemData(itemBaseData),
		globalID(itemGlobalID),
        serialID(itemSerialID)
	{
		data.setBind(needBind);
		data.setNum(num);
        data.setQuality(quality);
        data.setIdentify(identify);
		_location.clear();
        _location.setPos(protocol::item::nInvalidPackagePos);
	}

	nItem::~nItem()
	{
#ifdef _DEBUG
		*(const_cast<ItemBaseTable**>(&baseItemData)) = NULL;
		*(const_cast<DWORD*>(&serialID)) = 0;
		*(const_cast<QWORD*>(&globalID)) = 0;
#endif
	}

	const DWORD nItem::getID() const
	{
		return serialID;
	}

	const char* nItem::getName() const
	{
		return baseItemData->name.c_str();
	}

	DWORD nItem::getBaseID() const
	{
		return ASSERT_CAST(DWORD, baseItemData->id);
	}

	Tidy::nLogger* nItem::getLogger() const
	{
		return gameServer->getLogger();
	}

	const DWORD nItem::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const
	{
		return snprintf(headerBuffer, bufferSize, "[%u,%s]", getID(), getName());
	}

	WORD nItem::getLeftOverlayNum() const
	{
		if(baseItemData->maxRepeat <= data.getNum()){
            return 0;
		}
		return baseItemData->maxRepeat - data.getNum();
	}

	bool nItem::serialize( std::string *out) const
	{
        /*
		BUFFER_USER_CMD(protocol::item::ItemSerialize, data);
		if(serialize(data, USER_CMD_CAPACITY(protocol::item::ItemSerialize)) == false){
			assert_fail("消息包长度不够");
            return false;
		}
		out->assign(reinterpret_cast<const char*>(data), data->allSize());
        */
        debug("item error!!!!!!!!!!");
		return true;
	}

	bool nItem::serialize( protocol::item::ItemSerialize *out, const DWORD maxSize) const
	{
		out->itemBase.serialID = serialID;
		out->itemBase.baseID = getBaseID();
		out->itemBase.location = getLocation();
		protocol::item::ItemAttribute attr;
		if(data.saveAttr(&attr) == false){
			assert_fail("保存属性失败");
			return false;
		}
		if(attr.SerializePartialToArray(out->data, maxSize) == false){
			assert_fail("序列化道具失败");
			return false;
		}
		out->size = attr.GetCachedSize();
		return true;
	}

	DWORD nItem::unSerialize(const protocol::item::ItemSerialize *in )
	{
		assert_fail("TODO");
		return 0;
	}

	bool nItem::saveData(protocol::item::ItemBinary &itemBinary) const
	{
		itemBinary.set_globalid(globalID);
		itemBinary.set_baseid(baseItemData->id);
		itemBinary.set_location(getLocation().getValue());
		return data.saveAttr(itemBinary.mutable_attr());
	}

	bool nItem::restoreData(const protocol::item::ItemBinary &itemBinary)
	{
		assert_debug(itemBinary.globalid() == globalID && baseItemData->id == itemBinary.baseid(), "");
		setLocation(itemBinary.location());
		return data.restoreAttr(itemBinary.attr());
	}

	bool nItem::restoreData(const protocol::item::ItemAttribute &itemAttr)
	{
		return data.restoreAttr(itemAttr);
	}

	bool nItem::checkUnion(const nItem *srcItem, const WORD num) const
	{
		assert_debug(num > 0, "");
		if(srcItem->data.getNum() < num){
			return false;
		}
		return checkUnion(srcItem->baseItemData, srcItem->data.getBind());
	}

	bool nItem::checkUnion(const ItemBaseTable *baseData, const bool bind) const
	{
		return this->baseItemData->id == baseData->id && this->data.getBind() == bind && getLeftOverlayNum() > 0;
	}
	
	bool nItem::checkUnion(const ItemBaseTable *srcItemBaseData, const bool srcItemBind, const ItemBaseTable *destItemBaseData, const bool destItemBind)
	{
		return srcItemBaseData->id == destItemBaseData->id && srcItemBind == destItemBind;
	}

	bool nItem::init()
	{
        return true;
	}

	BYTE nItem::getPackageID() const
	{
		return _location.getPackageID();
	}

	WORD nItem::getPackagePos() const
	{
		return _location.getPackagePos();
	}

	void nItem::setLocation(const BYTE packageType, const WORD packagePos)
	{
		setLocation(protocol::item::ItemLocation(packageType, packagePos));
	}

	void nItem::setLocation(DWORD value)
	{
		_location.setValue(value);
	}

	const protocol::item::ItemLocation& nItem::getLocation() const
	{
		return _location;
	}

	bool nItem::checkLocationValid() const
	{
		return _location.isValid();
	}

	void nItem::clearLocation()
	{
		_location.clear();
	}

	void nItem::setItem(const nItem *other)
	{
		assert_debug(baseItemData->id == other->baseItemData->id, "");
		_location = other->getLocation();
		data.setData(other->data);
	}
};
