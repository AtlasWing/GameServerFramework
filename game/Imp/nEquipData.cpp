#include "game/nEquipData.h"

#include "protocol/equipAttribute.h"
#include "protocol/Proto/EquipAttribute.pb.h"

namespace Game
{
    nEquipData::nEquipData()
    {
    }

    nEquipData::~nEquipData()
    {
    }

    bool nEquipData::saveAttr(protocol::item::AttributeList *equipAttr) const
    {
#define EQUIP_BASE_ATTRIBUTE(type, name, protoname, defaultValue, desc) \
        equipAttr->set_##protoname(this->get##name());
#include "protocol/equipAttribute.h"
#undef EQUIP_BASE_ATTRIBUTE
        return true;
    }

    bool nEquipData::restoreAttr(const protocol::item::AttributeList &attr)
    {
#define EQUIP_BASE_ATTRIBUTE(type, name, protoname, defaultValue, desc) \
        set##name(attr.protoname());
#include "protocol/equipAttribute.h"
#undef EQUIP_BASE_ATTRIBUTE
        return true;
    }

    bool nEquipData::setData(const nEquipData &srcData)
    {
#define EQUIP_BASE_ATTRIBUTE(type, name, protoname, defaultValue, desc) \
        set##name(srcData.get##name());
#include "protocol/equipAttribute.h"
#undef EQUIP_BASE_ATTRIBUTE
    }
}
