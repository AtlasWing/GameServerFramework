#ifndef _RELATION_COMMON_H_
#define _RELATION_COMMON_H_

#include "commonType.h"

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning(disable : 4200)
#endif
#pragma pack(1)

namespace protocol { namespace relation{

	/// @brief ����ϵ����
	enum RelationType
	{
		RelationType_None			= 0,
		RelationType_Friend,					///< ����
		RelationType_Max
	};

	/// @brief ��������
	enum FriendType
	{
		FriendType_None				= 0,
		FriendType_Normal,						///< ��ͨ����
	};

    // �б��������״̬
    enum eCommonListUserDataState{
        eCommonListUserDataState_IsOnline   =   1 << 0, // ����
        eCommonListUserDataState_InPKRoom   =   1 << 1, // ��ս������
    };

    struct CommonListData {
		DWORD userID;
        BYTE  commandListUserDataState;     //eCommonListUserDataState
        BYTE  sex;
        BYTE  level;
        BYTE  vipLevel;
        BYTE  vipType;
        char charName [MAX_NAMESIZE];
        DWORD  winTimes;                    // ʤ������
        DWORD  loseTimes;                   // ʧ�ܴ���
        DWORD  pvpAllTimes;                 // �����Ϸ�ܴ���
        DWORD  runOffTimes;                 // ���ܴ���
        BYTE   placeType;                   // ����λ�� eUserPlaceType
        DWORD  roomID;                      // ����id
        WORD   mapID;                       // ��ͼid
        bool   isGM;                        //�Ƿ���GM
		char   clubName[MAX_NAMESIZE];		//���ֲ�����
        BYTE   clubTitle;                   //���ֲ�ְλ
    };

	///@ brief ������Ϣ����
	struct FriendUserData : public CommonListData
	{
	};

	/// @brief ������������
	enum RelatioNameType
	{
		RelationNameType_None			= 0,
		RelationNameType_User,					///< ����
		RelationNameType_Max
    };

    // ���а��������
    struct SequenceUserData : public CommonListData
    {
        DWORD  level;                       // ��ɫ�ȼ�
        DWORD  diamond;                     // ����ʯ��
        DWORD  diamondConsume;              // ���ѵ���ʯ�� 
		WORD   commonEloValue;				// ���ݻ��� 
    };

    //���а�����
    enum SequenceType
    {
        SequenceType_UserLevel          = 0,    // ��ҵȼ�    
        SequenceType_FightPower         = 1,    // ս����    
        SequenceType_Pet                = 2,    // ����ս����    
        SequenceType_UserElo            = 3,    // ���ݻ���
        SequenceType_UserDiamond        = 4,    // ��ʯ
        SequenceType_UserDiamondConsume = 5,    // ����
        SequenceType_Max
    };
    // ս������
    enum eEloGroupType
    {
        eEloGroupType_None          = 0,
        eEloGroupType_CopperLow     = 1, // ��ͭ2
        eEloGroupType_CopperHigh    = 2, // ��ͭ1
        eEloGroupType_SilverLow     = 3, // ����2
        eEloGroupType_SilverHigh    = 4, // ����1
        eEloGroupType_GoldLow       = 5, // �ƽ�2
        eEloGroupType_GoldHigh      = 6, // �ƽ�1
        eEloGroupType_PlatinumLow   = 7, // �׽�2
        eEloGroupType_PlatinumHigh  = 8, // �׽�1
        eEloGroupType_DiamondLow    = 9, // ��ʯ2
        eEloGroupType_DiamondHigh   = 10, // ��ʯ1
        eEloGroupType_King          = 11, // ���� 
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
