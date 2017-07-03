#ifndef _PROTOCOL_MONEY_COMMON_H
#define _PROTOCOL_MONEY_COMMON_H

#include "baseCommand.h"

#pragma pack(1)

namespace protocol{
    namespace money{
        const DWORD MAX_MONEY_VALUE = 999999999;

        enum eMoneyType
        {
            eMoneyType_None         = 0,
            eMoneyType_Silver       = 1,    // 银两
            eMoneyType_Diamond      = 2,    // 钻石
            eMoneyType_BindDiamond  = 3,    // 绑钻
            eMoneyType_HonorPoints  = 4,    // 荣誉点
            eMoneyType_ClubPoints   = 5,    // 仙盟货币
            eMoneyType_Max,
        };

        enum eMoneyOperType
        {
            eMoneyOperType_None = 0,
            eMoneyOperType_Add  = 1,
            eMoneyOperType_Sub  = 2
        };
    };
};

#pragma pack()
#endif
