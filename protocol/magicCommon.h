#ifndef PROTOCOL_MAGIC_COMMON_H_
#define PROTOCOL_MAGIC_COMMON_H_

#include "baseCommand.h"
#pragma pack(1)

namespace protocol {
namespace magic {
    enum ePKState{
        ePKState_None        =   0,
        ePKState_Normal      =   1,
        ePKState_Death       =   2,
    };
    enum eStatusEffectType{
        eStatusEffectType_None   = 0,
        eStatusEffectType_Plus   = 1,
        eStatusEffectType_Debuff = 2,
    };

    enum eSkillPartHurtType{
        eSkillPartHurtType_Hurt = 0,
        eSkillPartHurtType_None = 1,
        eSkillPartHurtType_Cure = 2,
    };

    enum eSkillBaseType
    {
        eSkillBaseType_None     = 0,
        eSkillBaseType_Active   = 1, //主动技能
        eSkillBaseType_Passive  = 2, //被动技能
    };
};
};
#pragma pack()
#endif
