#ifndef _PROTOCOL_SERIALIZE_COMMON_H_
#define _PROTOCOL_SERIALIZE_COMMON_H_

#pragma pack(1)

namespace protocol 
{
	namespace serialize
	{
		enum eUserSaveType
		{
			eUserSaveType_None           =   0,
			eUserSaveType_Timetick,		///< 定时存档
			eUserSaveType_Logout,		///< 下线存档
			eUserSaveType_ChangeLogic,	///< 跨服存档
			eUserSaveType_Active,		///< 主动存档
			UserSaveType_Max,
		};

        enum eMiscDataSerializeType
        {
            eMiscDataSerializeType_None                        =   0,
            eMiscDataSerializeType_UserSequenceData            =   1,      //排行榜数据存档
            eMiscDataSerializeType_ZoneCloseTime               =   2,      //服务器上次关闭时间
            eMiscDataSerializeType_Activity                    =   3,      //正在进行的活动存档
            eMiscDataSerializeType_WeekQuestData               =   4,      //周挑战任务数据
            eMiscDataSerializeType_MonsterChallengeSequenceData=   5,      //远征的关卡的平排行榜数据
            eMiscDataSerializeType_EloSequenceData		       =   6,      //匹配的积分排行榜
            eMiscDataSerializeType_CurrRankMatchData		   =   7,      //当前排位赛的数据

        };

		struct FollowerData{
			DWORD charID;
			DWORD joinTime;
		};
        struct FriendData{
            DWORD charID;
            DWORD lastGiveFriendVitalityInSec;
            DWORD lastGiveMeVitalityInSec;
            bool  isReceived;
        };
    };
};

#pragma pack()
#endif
