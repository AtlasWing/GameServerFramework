///////////////////////////////////////////////////////////////////////////////
// @brief : 邮件相关命令
// @author: 尤龙 
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "protocol/baseCommand.h"

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning(disable : 4200)
#endif

#pragma pack(1)

namespace protocol{ namespace mail{

	const DWORD MAX_SUGGEST_TITLE_SIZE		=	128;
	const DWORD MAX_SUGGEST_CONTENT_SIZE	=	3072;

    enum MailState
    {
        MailState_None  =   0,
        MailState_New   =   1,
        MailState_Readed=   2,
		MailState_Revert=	3,
    };

    enum MailType
    {
        MailType_None   =   0,
        MailType_Sys    =   1,
        MailType_User   =   2,
    };
    
    enum MailDeleteState
    {
        MailDeleteState_None = 0,
        MailDeleteState_Delete = 1,
    };

	enum eSuggestType
	{
		eSuggestType_None	=	0,
		eSuggestType_Idea	=	1,		//普通建议
		eSuggestType_PKBug	=	2,		//战斗BUG
		eSuggestType_UIBug	=	3,		//UIbug
		eSuggestType_Optimization=4,	//优化建议
		eSuggestType_Misc	=	5,		//灌水
		eSuggestType_Max
	};

    struct MailMoneyData
    {
        MailMoneyData()
        {
            money = 0;
            moneyType = 0;
        }
        DWORD money;
        WORD moneyType;
    };

    enum eMailMixType
    {
        eMailMixType_None         = 0,
        eMailMixType_Silver       = 1,    // 银两
        eMailMixType_Diamond      = 2,    // 钻石
        eMailMixType_BindDiamond  = 3,    // 绑钻
        eMailMixType_HonorPoints  = 4,    // 荣誉点
        eMailMixType_ClubPoints   = 5,    // 荣誉点
        eMailMixType_Exp          = 100,    // 经验 
        eMailMixType_Vitility     = 101,    // 体力值 
        eMailMixType_Max,
    };
}}
#pragma pack()

#ifdef _MSC_VER
#pragma warning( pop )
#endif

