#ifndef _RELATION_COMMON_H_
#define _RELATION_COMMON_H_

#include "commonType.h"

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning(disable : 4200)
#endif
#pragma pack(1)

namespace protocol { namespace relation{

	/// @brief 社会关系类型
	enum RelationType
	{
		RelationType_None			= 0,
		RelationType_Friend,					///< 好友
		RelationType_Max
	};

	/// @brief 好友类型
	enum FriendType
	{
		FriendType_None				= 0,
		FriendType_Normal,						///< 普通好友
	};

    // 列表玩家数据状态
    enum eCommonListUserDataState{
        eCommonListUserDataState_IsOnline   =   1 << 0, // 在线
        eCommonListUserDataState_InPKRoom   =   1 << 1, // 在战斗房间
    };

    struct CommonListData {
		DWORD userID;
        BYTE  commandListUserDataState;     //eCommonListUserDataState
        BYTE  sex;
        BYTE  level;
        BYTE  vipLevel;
        BYTE  vipType;
        char charName [MAX_NAMESIZE];
        DWORD  winTimes;                    // 胜利次数
        DWORD  loseTimes;                   // 失败次数
        DWORD  pvpAllTimes;                 // 玩家游戏总次数
        DWORD  runOffTimes;                 // 逃跑次数
        BYTE   placeType;                   // 所在位置 eUserPlaceType
        DWORD  roomID;                      // 房间id
        WORD   mapID;                       // 地图id
        bool   isGM;                        //是否是GM
		char   clubName[MAX_NAMESIZE];		//俱乐部名字
        BYTE   clubTitle;                   //俱乐部职位
    };

	///@ brief 好友信息数据
	struct FriendUserData : public CommonListData
	{
	};

	/// @brief 名字请求类型
	enum RelatioNameType
	{
		RelationNameType_None			= 0,
		RelationNameType_User,					///< 个人
		RelationNameType_Max
    };

    // 排行榜玩家数据
    struct SequenceUserData : public CommonListData
    {
        DWORD  level;                       // 角色等级
        DWORD  diamond;                     // 总钻石数
        DWORD  diamondConsume;              // 消费的钻石数 
		WORD   commonEloValue;				// 天梯积分 
    };

    //排行榜类型
    enum SequenceType
    {
        SequenceType_UserLevel          = 0,    // 玩家等级    
        SequenceType_FightPower         = 1,    // 战斗力    
        SequenceType_Pet                = 2,    // 仙灵战斗力    
        SequenceType_UserElo            = 3,    // 天梯积分
        SequenceType_UserDiamond        = 4,    // 钻石
        SequenceType_UserDiamondConsume = 5,    // 消费
        SequenceType_Max
    };
    // 战力分组
    enum eEloGroupType
    {
        eEloGroupType_None          = 0,
        eEloGroupType_CopperLow     = 1, // 青铜2
        eEloGroupType_CopperHigh    = 2, // 青铜1
        eEloGroupType_SilverLow     = 3, // 白银2
        eEloGroupType_SilverHigh    = 4, // 白银1
        eEloGroupType_GoldLow       = 5, // 黄金2
        eEloGroupType_GoldHigh      = 6, // 黄金1
        eEloGroupType_PlatinumLow   = 7, // 白金2
        eEloGroupType_PlatinumHigh  = 8, // 白金1
        eEloGroupType_DiamondLow    = 9, // 钻石2
        eEloGroupType_DiamondHigh   = 10, // 钻石1
        eEloGroupType_King          = 11, // 王者 
    };

	struct NamePair
	{
		DWORD id;
		char name[MAX_NAMESIZE];
	};

    struct VitalityData{
        bool isGiveFriendVitalityToday;
        bool isGiveMeVitalityToday;
        bool isReceived;
    };
    struct SequenceGroupData
    {
        DWORD eloValue;
        DWORD rank;
    };

}}
#pragma pack()
#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
