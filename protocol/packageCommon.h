#ifndef PROTOCOL_PACKAGECOMMON_H
#define PROTOCOL_PACKAGECOMMON_H
#include "commonType.h"

namespace protocol { namespace package {

	#define  ITEM_TIME_FOREVER 999 
	#define  PET_EQUIP_POS_NUM 6 
	const DWORD MAIN_PACKAGE_CAPACITY = 300; 

	/// @brief ��������
	enum ePackageType
	{
		ePackageType_None            = 0,
		ePackageType_Main            = 1,	///< ������
		ePackageType_Equip           = 2,	///< װ������
		ePackageType_PetEquip        = 3,	///< ����װ������
		ePackageType_Max
	};

	/// @brief ����ID
	enum ePackageID
	{
		ePackageID_None            = 0,
		ePackageID_Main			   = 1,	///< ������
		ePackageID_Equip           = 2,	///< װ������
		ePackageID_PetEquip        = 3,	///< ����װ������
		ePackageID_Max
	};

    // װ������
    enum eEquipType
    {
		eEquipType_Tou          =   0,      // 0 ͷ 
		eEquipType_Yifu		    =   1,      // 1 �·�
		eEquipType_KuZi         =   2,      // 2 ���� 
		eEquipType_XiangLian    =   3,      // 3 ���� 
		eEquipType_ShouTao      =   4,      // 4 ����
		eEquipType_ShouZhuo     =   5,      // 5 ����
		eEquipType_XieZi	    =   6,      // 6 Ь��
		eEquipType_JieZhi	    =   7,      // 7 ��ָ 
        eEquipType_WuQi         =   8,      // ������
        eEquipType_Pet          =   100,    // ����װ�� 
        eEquipType_Max          =   255
    };

    // װ��λ��
    enum eEquipPos
    {
		eEquipPos_Tou       =   0,      // 0 ͷ 
		eEquipPos_Yifu		=   1,      // 1 �·�
		eEquipPos_KuZi      =   2,      // 2 ���� 
		eEquipPos_XiangLian =   3,      // 3 ���� 
		eEquipPos_ShouTao   =   4,      // 4 ����
		eEquipPos_ShouZhuo  =   5,      // 5 ����
		eEquipPos_XieZi	    =   6,      // 6 Ь��
		eEquipPos_JieZhi	=   7,      // 7 ��ָ 
        eEquipPos_ZhuWuQi   =   8,      // ������
        eEquipPos_HuFu      =   9,      // 9 ���� 
        eEquipPos_Max    
    };

    // ����װ��λ��
    enum ePetEquipPos
    {
		ePetEquipPos_First     =   0,  // ����װ��1-6�Ÿ��� 
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
        // ����
        return *(reinterpret_cast<DWORD*>(NULL)) > 0;
    }
}}
#endif
