#ifndef PROTOCOL_PACKAGECOMMON_H
#define PROTOCOL_PACKAGECOMMON_H
#include "commonType.h"

namespace protocol { namespace package {

	#define  ITEM_TIME_FOREVER 999 
	#define  PET_EQUIP_POS_NUM 6 
	const DWORD MAIN_PACKAGE_CAPACITY = 300; 

	/// @brief 包裹类型
	enum ePackageType
	{
		ePackageType_None            = 0,
		ePackageType_Main            = 1,	///< 主包裹
		ePackageType_Equip           = 2,	///< 装备包裹
		ePackageType_PetEquip        = 3,	///< 宠物装备包裹
		ePackageType_Max
	};

	/// @brief 包裹ID
	enum ePackageID
	{
		ePackageID_None            = 0,
		ePackageID_Main			   = 1,	///< 主包裹
		ePackageID_Equip           = 2,	///< 装备包裹
		ePackageID_PetEquip        = 3,	///< 宠物装备包裹
		ePackageID_Max
	};

    // 装备类型
    enum eEquipType
    {
		eEquipType_Tou          =   0,      // 0 头 
		eEquipType_Yifu		    =   1,      // 1 衣服
		eEquipType_KuZi         =   2,      // 2 裤子 
		eEquipType_XiangLian    =   3,      // 3 项链 
		eEquipType_ShouTao      =   4,      // 4 手套
		eEquipType_ShouZhuo     =   5,      // 5 手镯
		eEquipType_XieZi	    =   6,      // 6 鞋子
		eEquipType_JieZhi	    =   7,      // 7 戒指 
        eEquipType_WuQi         =   8,      // 主武器
        eEquipType_Pet          =   100,    // 宠物装备 
        eEquipType_Max          =   255
    };

    // 装备位置
    enum eEquipPos
    {
		eEquipPos_Tou       =   0,      // 0 头 
		eEquipPos_Yifu		=   1,      // 1 衣服
		eEquipPos_KuZi      =   2,      // 2 裤子 
		eEquipPos_XiangLian =   3,      // 3 项链 
		eEquipPos_ShouTao   =   4,      // 4 手套
		eEquipPos_ShouZhuo  =   5,      // 5 手镯
		eEquipPos_XieZi	    =   6,      // 6 鞋子
		eEquipPos_JieZhi	=   7,      // 7 戒指 
        eEquipPos_ZhuWuQi   =   8,      // 主武器
        eEquipPos_HuFu      =   9,      // 9 护符 
        eEquipPos_Max    
    };

    // 宠物装备位置
    enum ePetEquipPos
    {
		ePetEquipPos_First     =   0,  // 宠物装备1-6号格子 
		ePetEquipPos_Second	   =   1,
		ePetEquipPos_Third     =   2,
		ePetEquipPos_Fourth    =   3,
		ePetEquipPos_Fiveth    =   4,
		ePetEquipPos_Sixth     =   5,
        ePetEquipPos_Max       =   255
    };

	inline ePackageType getPackageTypeByID(ePackageID id)
	{
		switch (id)
        {
            case ePackageID_Main:
                {
                    return ePackageType_Main;
                }
                break;
            case ePackageID_Equip:
                {
                    return ePackageType_Equip;
                }
                break;
            case ePackageID_None:
            case ePackageID_Max:
            default:
                {
                    //assert
                    return ePackageType(*((DWORD*)0));
                }
                break;
        }
		return ePackageType_None;
	}

	inline int GetPackageSize(ePackageID id)
	{
		switch (id)
		{
		case ePackageID_Main:
			{
				return MAIN_PACKAGE_CAPACITY;
			}
			break;
        case ePackageID_Equip:
            {
                return 16;
            }
            break;
		default:
            {
            }
			break;
		}
        // assert
		return *(reinterpret_cast<DWORD*>(NULL));
	}

	inline DWORD GetPackageMaxSize(ePackageID id)
	{
		switch (id)
		{
		case ePackageID_Main:
			{
				return MAIN_PACKAGE_CAPACITY;
			}
			break;
        case ePackageID_Equip:
            {
                return 16;
            }
            break;
		default:
            {
            }
			break;
		}
        // assert
		return *(reinterpret_cast<DWORD*>(NULL));
	}

    inline bool isAutoAddAndUnionPackage(const ePackageType packageType){
        switch(packageType){
            case ePackageType_Main:
                {
                    return true;
                }
                break;
            case ePackageType_Equip:
                {
                    return false;
                }
                break;
            default:
                {
                }
        }
        // 断言
        return *(reinterpret_cast<DWORD*>(NULL)) > 0;
    }
}}
#endif
