#include "protocol/Proto/ItemData.pb.h"

#include "game/nItemData.h"

namespace Game
{
	nItemData::nItemData()
	{
	}

	nItemData::~nItemData()
	{
	}

	bool nItemData::saveAttr(protocol::item::ItemAttribute *itemAttr) const
	{
#define ITEM_BASE_ATTRIBUTE(type, name, protoname, defaultValue, desc) \
		itemAttr->set_##protoname(this->get##name());

#define ITEM_RARE_ATTRIBUTE(type,name,protoname,defaultValue,desc) \
		if(_rare_attribute.hasData(protocol::item::eItemRareAttribute_##name) == true){\
			itemAttr->set_##protoname(get##name());\
		}

#include "protocol/itemAttribute.h"

#undef ITEM_RARE_ATTRIBUTE
#undef ITEM_BASE_ATTRIBUTE
		return true;
	}

	bool nItemData::restoreAttr(const protocol::item::ItemAttribute &attr)
	{
#define ITEM_BASE_ATTRIBUTE(type,name,protoname, defaultValue, desc) \
		set##name(attr.protoname());

#define ITEM_RARE_ATTRIBUTE(type,name,protoname,defaultValue,desc) \
		if(attr.has_##protoname() == true){\
			set##name(attr.protoname());\
		}

#include "protocol/itemAttribute.h"

#undef ITEM_RARE_ATTRIBUTE
#undef ITEM_BASE_ATTRIBUTE
		return true;
	}

	bool nItemData::setData(const nItemData &srcData)
	{
#define ITEM_BASE_ATTRIBUTE(type,name,protoname, defaultValue, desc) \
		set##name(srcData.get##name());

#include "protocol/itemAttribute.h"

#undef ITEM_BASE_ATTRIBUTE
		_rare_attribute.setData(srcData._rare_attribute);
		return true;
	}

	bool nItemData::initData(const ItemBaseTable *baseItemData)
	{
		return true;
	}
};
