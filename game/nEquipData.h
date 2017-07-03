#ifndef GAME_LGEQUIPATTRIBUTE_H_
#define GAME_LGEQUIPATTRIBUTE_H_
#include "protocol/Proto/EquipAttribute.pb.h"
#include "protocol/equipData.h"

namespace Game
{
    class nEquipData
    {
        public:
            nEquipData();
            ~nEquipData();
        public:
            bool saveAttr(protocol::item::AttributeList *equipAttr) const;
            bool restoreAttr(const protocol::item::AttributeList &attr);
            bool setData(const nEquipData &srcData);
        public:
#define EQUIP_BASE_ATTRIBUTE(type, name, protoname, defaultValue, desc) \
        private:\
                type _##name; \
        public:\
               const type& get##name() const { return _##name; } \
            type& get##name() { return _##name; } \
            void set##name(type value) { _##name=value; } \

#include "protocol/equipAttribute.h"

#undef EQUIP_BASE_ATTRIBUTE
    };
}

#endif
