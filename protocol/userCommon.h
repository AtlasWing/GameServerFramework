#ifndef _PROTOCOL_USER_COMMON_H_
#define _PROTOCOL_USER_COMMON_H_

#include "baseCommand.h"

#pragma pack(1)

namespace protocol {
    namespace user {
        // 在线玩家的数据,其他玩家需要访问
        struct BaseUserData {
            DWORD worldLevelRank;
        };

        // 主角数据,只有自己需要访问
        struct MainUserData {
			BYTE	userType;//玩家类型eUserType
            BYTE  	sex;    // 性别
            WORD  	level;  // 等级
            DWORD 	bornDate;//生日
            // 最近一次下线时间
            DWORD 	lastOfflineTime;
            // 第一次消费时间
            DWORD 	firstConsumeTime;
            // 创建ip
            DWORD 	createIP;
            // 最后一次登陆ip
            char    lastLoginIP[MAX_NAMESIZE];
            // 总在线时长
            DWORD 	onlineTime;
            // 创建时间
            DWORD  createTime;
            DWORD  bitMask;                     // 标志掩码
            DWORD  curExp;                      // 当前经验值
            bool   isAdult;                     // 是否成年
            DWORD  silver;                      // 银两
            DWORD  diamond;                     // 钻石(非绑定) 
            DWORD  bindDiamond;                 // 绑定钻石 
            DWORD  gmLevel;                     // GM等级 
        };

        enum eSexType
        {
            eSexType_Female     = 0,
            eSexType_Male       = 1,
        };

        enum eCharBaseFlag
        {
            eCharBaseFlag_NewChar    =   1<<0,
            eCharBaseFlag_NotSetItem =   1<<1,
        };

        /// @brief 玩家状态数据
        struct CharState
        {
            CharState(){ bzero(this,sizeof(CharState));}
            QWORD   needExp;                    ///< 升到下级所需经验值
        };

        struct EntityData
        {
            DWORD entityID;
            BYTE entityType;
        };

		//tolua_begin
        enum eAddExpType{
            eAddExpType_None            =   0,
            eAddExpType_WarResult       =   1,  // 胜利结算
            eAddExpType_Reward          =   2,  // 登陆奖励,在线奖励等
            eAddExpType_QuestReward     =   3,  // 任务奖励
            eAddExpType_Test            =   4,  // 测试
			eAddExpType_GiftBag			=	5,	// 礼包奖励
			eAddExpType_GiftBox			=	6,	// 礼盒奖励
            eAddExpType_GM              =   7, // GM指令提升经验
            eAddExpType_ExpDupl         =   8, // 经验本结算经验
            eAddExpType_LuckyBag        =   9, // 开启福袋获得经验
            eAddExpType_Task            =   10, // 任务获得经验
            eAddExpType_Mail            =   11, // 邮件领取 
        };
        enum eAddMoneyType{
            eAddMoneyType_None				=   0,
            eAddMoneyType_SellItem			=   1,  //出售道具
        };
        enum eSubMoneyType{
            eSubMoneyType_None                  =   0,
            eSubMoneyType_BuyItem               =   1,  //购买道具
        };
        //tolua_end

		enum eUserConfigFlagType{
			eUserConfigFlagType_None			=	0,
			eUserConfigFlagType_ForbidUnion		=	1,	//屏蔽公会邀请
			eUserConfigFlagType_ForbidFight		=	2,	//屏蔽战斗邀请
			eUserConfigFlagType_NoStrangerChat	=	3,	//屏蔽陌生人私聊
			eUserConfigFlagType_NoStrangerMail	=	4,	//屏蔽陌生人邮件
			eUserConfigFlagType_Max
		};

		struct TimeEndEffect{
			DWORD endTimeInSec;		// 过期时间戳
			BYTE param;				// 额外参数
		};
		struct UseTimesEffect{
			DWORD useTimes;			// 剩余使用次数
			BYTE param;				// 额外参数
		};

		enum eFunctionUnlockType
        {
            eFunctionUnlockType_None    = 1,
            eFunctionUnlockType_Base    = 2,
            eFunctionUnlockType_Quest   = 3,
            eFunctionType_Max
		};

    };
};

#pragma pack()

#endif
