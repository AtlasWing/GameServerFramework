///////////////////////////////////////////////////////////////////////////////
// @brief : �����������
///////////////////////////////////////////////////////////////////////////////
#pragma once

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning(disable : 4200)
#include <WinDef.h>
#endif

#include <stdarg.h>
#ifdef _GNU_SOURCE
#include <string.h>
#endif

#include <cassert>

#ifndef BASE_TYPES_DEFINE
#define BASE_TYPES_DEFINE
//tolua_begin
typedef signed char		    SBYTE;
typedef unsigned char		BYTE;
typedef unsigned short		WORD;
typedef signed short		SWORD;
#ifndef _MSC_VER
typedef unsigned int		DWORD;
#endif
#ifdef _MSC_VER
typedef long int			SDWORD;
#else
typedef signed int			SDWORD;
#endif
typedef unsigned long long	QWORD;
typedef signed long long	SQWORD;
//tolua_end


const BYTE  ZERO_BYTE = 0;
const BYTE  MIN_BYTE = 0;
const BYTE  MAX_BYTE = (BYTE)-1;
const WORD ZERO_WORD = 0;
const WORD  MIN_WORD = 0;
const WORD  MAX_WORD = (WORD)-1;
const DWORD ZERO_DWORD = 0;
const DWORD MIN_DWORD = 0;
const DWORD MAX_DWORD = (DWORD)-1;
const float ZERO_FLOAT = 0.0;
const float MAX_FLOAT = 99999999.0f;
const float MIN_FLOAT = -99999999.0f;
#endif

#ifndef MAX_NAMESIZE
#define MAX_NAMESIZE 32
#endif
typedef char NameStr[MAX_NAMESIZE];
/// @brief �˺���󳤶�
const DWORD MAX_ACCOUNT_NAME_SIZE = 48;
/// @brief ���볤��
const DWORD MAX_PASSWORD_LEN = 32;
/// @brief IP��ַ��󳤶�
#ifndef MAX_IP_LENGTH
#define MAX_IP_LENGTH 16
#endif
#ifndef MAX_URL_LENGTH
#define MAX_URL_LENGTH 64
#endif

/// @brief ������������û���Ŀ
const int MAX_GATEWAYUSER = 4000;
/// @brief ������󳤶�
const int MAX_PASSWORD = 16;
/// @brief ��������
const int MAX_NUMPASSWORD = 32;
/// @brief �ʼ�������󳤶�
const int MAX_MAIL_TEXT_SIZE = 1680;
/// @brief �ʼ����͵����������
const DWORD MAX_MAIL_ATTACHMENT_NUM	= 4;
/// @brief �ʼ����͵��ߵ����ֶ��ַ���󳤶� 
const DWORD MAX_MAIL_ITEM_LIST_SIZE = 64;
/// @brief �̳����͵����������
const DWORD MAX_SEND_SHOP_ITEM_NUM = 9;
/// @brief �̳����͵�����������ֽ���
const DWORD MAX_SEND_SHOP_ITEM_TEXT_SIZE = 300;
/// @brief ǿ��װ�����ʹ����ʯ�� 
const DWORD INTENSIFY_EQUIP_MAX_USE_DIAMOND_NUM = 10;

const double CP5_PI = 3.1415926535897932384626433832795f;

const int MAX_CLUB_BULLETIN_LEN = 120;
const int MAX_QQ_LEN = 16;
const int ITEMS_PER_PAGE = 10; // ÿҳ��ʾ������

const int ClubMenuID = 16;

const unsigned short MAX_DISCONVERY_GRID = 8;
/// �ƶ�����Ķ���
enum 
{
    _DIR_MIN        = 0,    /// ����
    _DIR_UP         = 0,    /// ����
    _DIR_UPRIGHT    = 1,    /// ����
    _DIR_RIGHTUP    = 1,    /// ����
    _DIR_RIGHT      = 2,    /// ����
    _DIR_RIGHTDOWN  = 3,    /// ����
    _DIR_DOWNRIGHT  = 3,    /// ����
    _DIR_DOWN       = 4,    /// ����
    _DIR_DOWNLEFT   = 5,    /// ����
    _DIR_LEFTDOWN   = 5,    /// ����
    _DIR_LEFT       = 6,    /// ����
    _DIR_LEFTUP     = 7,    /// ����
    _DIR_UPLEFT     = 7,    /// ����
    _DIR_RANDOM     = 8,    /// �������
    _DIR_WRONG      = 8,    /// ������
    _DIR_MAX        = 8 
};

const int MoveDirectAdjust[][2]= { {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, 0} };
#ifndef MAX_FILE_NAMESIZE
#define MAX_FILE_NAMESIZE 128
#endif
/// @brief �Ա���
enum SexType
{
    SexType_None	= 0,	///< δ����
    SexType_Male	= 1,	///< ��
    SexType_Female	= 2,	///< Ů
	SexType_Max
};

inline SexType GetOppositeSex(SexType sex)
{
	switch(sex)
	{
	case SexType_Male:
		return SexType_Female;
	case SexType_Female:
		return SexType_Male;
	case SexType_None:
		return SexType_None;
	default:
		assert(0);
		return SexType_None;
	}
}

enum eMapNeedUnlockType{
   eMapNeedUnlockType_None          = 0,        ///< ��ͼ����Ҫ����
   eMapNeedUnlockType_Level         = 1,        ///< ͼ������Ҫ��Ҵﵽ��Ӧ�ȼ�
   eMapNeedUnlockType_Star          = 2,        ///< ͼ������Ҫ��һ�������
   eMapNeedUnlockType_StarAndLevel  = 3,        ///< ��ͼ������Ҫ��Ҵﵽ��Ӧ�ȼ��һ�������
};

enum eRoomEntityType
{
    eRoomEntityType_None    = 0,
    eRoomEntityType_Player  = 1,
    eRoomEntityType_Pet     = 2,
    eRoomEntityType_NPC     = 3,
    eRoomEntityType_Max
};

enum eSceneType
{
    eSceneType_None         = 0,
    eSceneType_NormalDupl   = 1,
    eSceneType_MainScene    = 2,
    eSceneType_LoginScene   = 3,
    eSceneType_GoldScene    = 4,
    eSceneType_Discovery    = 5,
    eSceneType_DouMoDuChang = 6,
    eSceneType_PVP          = 7,
};

//tolua_begin
// ��Ӫ����
enum eCampType{
    eCampType_None,
    eCampType_Blue      = 1,
    eCampType_Red       = 2,
    eCampType_Max,
};

// ս�����
enum eWarResultType{
    eWarResultType_None     =   0,
    eWarResultType_Win      =   1,      // ʤ��
    eWarResultType_Lose     =   2,      // ʧ��
    eWarResultType_Draw     =   3,      // ƽ��
    eWarResultType_RunOff   =   4,      // ����
    eWarResultType_Winner   =   5,      // ��һ��
    eWarResultType_Second   =   6,      // �ڶ���
    eWarResultType_Third    =   7,      // ������
};

enum eWarEndType{
	eWarEndType_None			=	0,
	eWarEndType_SavePrincess	=	1,
	eWarEndType_Score			=	2,
	eWarEndType_CampLifeZero	=	3,
	eWarEndType_CampLifeCompare =	4,
};

enum eWarMaxUserNum{
	eWarMaxUserNum_None		=	0,
	eWarMaxUserNum_Two		=	2,
	eWarMaxUserNum_Six		=	6,
	eWarMaxUserNum_Ten		=	10,
	eWarMaxUserNum_TwentyFour	=	24,
};

enum eWarMaxTime{
	eWarMaxTime_None	=	0,
	eWarMaxTime_ThreeHundredSec  =	300,
	eWarMaxTime_SixHundredSec	 =	600,
	eWarMaxTime_OneThousandTwoHundredSec	 =	1200,
};
// ְҵö��,ע���ְҵ�������ж�Ӧ��
enum eProfessionType{
    eProfessionType_None    = 0,
    eProfessionType_Init    = 1,
    eProfessionType_NongMin = 1,
    eProfessionType_GongRen = 2,
    eProfessionType_ZhanShi = 3,
    eProfessionType_GongJianShou= 4,
    eProfessionType_FaShi   = 5,
    eProfessionType_JiShi   = 6,	
    eProfessionType_AduitNum,
    eProfessionType_Zombie  = 51,
    eProfessionType_XiaoJiXiaoYa  = 101,
    eProfessionType_Max
};
enum eScenePointType
{
    eScenePointTypeBorn = 0
};

enum AddictionType
{
    AddictionType_None      = 0,    //ȫ������
    AddictionType_HalfAward = 1,    //�������
    AddictionType_NoAward   = 2,    //û���κ�����
};

enum eRaceModeRank
{
	eRaceModeRank_None		= 0,
	eRaceModeRank_First		= 1,
	eRaceModeRank_Second	= 2,
	eRaceModeRank_Third		= 3,
};
//tolua_end

inline eCampType getEnemyCampType(eCampType campType)
{
	if(campType == eCampType_Blue){
		return eCampType_Red;
	}
	if(campType == eCampType_Red){
		return eCampType_Blue;
	}
	return eCampType_None;
}

/// @brief ��ͨ����
enum TrafficType
{
    TrafficType_Walk    = 0,
    TrafficType_Run     = 1,
};

/// @brief ��������ַ�����
const int MAX_CHATMSGLEN = 256;

/// @brief ��ϷƵ��
enum eGameChannelType{
	eGameChannelType_None		= 0,
	eGameChannelType_Sys		= 1,//ϵͳƵ�������֣���սģʽ�����
	eGameChannelType_Newer		= 2,//����
	eGameChannelType_Primer		= 3,//����
	eGameChannelType_Monster	= 4,//Զ��
	eGameChannelType_Max,		
};
struct ZoneInfo
{
	union
	{
		DWORD dwID;                 ///< ��id
		struct
		{
			///< @brief ��Ϸ�������
			WORD zone;
			///< @brief ��Ϸ������
			WORD game;
		};
	};
	BYTE zonename[MAX_NAMESIZE];    ///< ����
	DWORD createdate;               ///< ����ʱ��
	DWORD destzone;                 ///< �������id
	ZoneInfo()
	{
	}
};

#ifdef  _MSC_VER

inline void* bcopy(const void* src, void* dest, size_t count)
{
    return memcpy(dest, src, count);
}

inline void* bzero(void* dest, size_t count)
{
    return memset(dest, 0, count);
}
#endif

/// @brief ��ȫɾ��һ��ָ��
#ifndef SAFE_DELETE
#define SAFE_DELETE(x)		{ if (x) { delete (x); (x) = NULL; } }
#endif

/// @brief ��ȫɾ��һ��ָ������
#define SAFE_DELETE_VEC(x)	{ if (x) { delete [] (x); (x) = NULL; } }

template <bool>
struct CommonCompileAssert {};
#define common_assert_compile(expr) \
	{\
	typedef CommonCompileAssert<(bool(expr))> compile_assert_occur_at_line_[bool(expr) ? 1 : -1];\
	};

#ifdef _MSC_VER
#pragma warning( pop )
#endif

