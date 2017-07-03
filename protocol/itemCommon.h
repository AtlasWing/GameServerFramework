#ifndef PROTOCOL_ITEMCOMMON_H
#define PROTOCOL_ITEMCOMMON_H 

#include "packageCommon.h"

#include "Proto/ItemData.pb.h"

namespace protocol { namespace item {

    enum eItemAttribute
    {
#define ITEM_BASE_ATTRIBUTE(type, name, protoname, defaultValue, desc) \
        eItemAttribute_##name,

#define ITEM_RARE_ATTRIBUTE(type,name,protoname,defaultValue,desc) \
        eItemAttribute_##name,

#include "itemAttribute.h"

#undef ITEM_RARE_ATTRIBUTE
#undef ITEM_BASE_ATTRIBUTE
    };

    enum eItemRareAttribute
    {
#define ITEM_RARE_ATTRIBUTE(type,name,protoname,defaultValue,desc) \
        eItemRareAttribute_##name,

#include "itemAttribute.h"

#undef ITEM_RARE_ATTRIBUTE
        eItemRareAttribute_Max
    };
	const DWORD FOREVER_OVERDUE_TIME_IN_SEC	=	999999999;	///< ����������Ч��ʱ�䶨��

    //�������ID begin
	const WORD ITEM_BASE_ID_START		=	1003;	///< �������ID ��ʼ
    //�������ID end
    
    //tolua_begin
    /// @brief ������ӷ�ʽ,������־ͳ��
    enum ItemAddType
    {
        ItemAddType_None            = 0,
        ItemAddType_Normal          = 1,    ///< ��ͨ���
        ItemAddType_Union           = 2,	///< �ϲ����
        ItemAddType_Scene           = 4,	///< ����ʰȡ
        ItemAddType_Shop			= 5,	///< �̵깺��
        ItemAddType_Trade			= 6,	///< ���׻��
        ItemAddType_Mail			= 7,	///< �ʼ����
        ItemAddType_GmCommand		= 8,	///< GMָ��fetch

        ItemAddType_Max
    };

    // �̵�����
    enum eShopType
    {
        eShopType_None      =   0,
        eShopType_Main      =   1,
        eShopType_Max
    };
    //tolua_end

    /// @brief ����ʹ�õķ�ʽ����,����������ʱֻ��ָ��һ�ַ�ʽ
    enum ItemUseType
    {
        ItemUseType_None			= 0,	

        ItemUseType_Self			= 1<<0,	///< �Ҽ�ֱ��ʹ��
        ItemUseType_TargetNpc		= 1<<1,	///< ָ��NPC, ���� ItemUseTargetData
        ItemUseType_TargetUser		= 1<<2,	///< ָ���û�, ���� ItemUseTargetData
        ItemUseType_TargetItem		= 1<<3,	///< ָ����һ������, ���� ItemUseTargetData
        ItemUseType_String			= 1<<4,	///< Я���ַ���

        ItemUseType_Max
    };

    // ���߰�����
    enum eItemBindType
    {
        eItemBindType_None              =   0,  ///<����
        eItemBindType_OnCreate          =   1,  ///<�����Ͱ�
        eItemBindType_OnUse             =   2,  ///<ʹ���˰�,����װ��
        eItemBindType_OnTrade           =   3,  ///<���׺��
    };
    /// @brief ���ߴ�������(������ֵ����ʱ��)
    enum ItemCreateType
    {
        ItemCreateType_None				= 0,	
        ItemCreateType_Normal			= 1,	///< ��ͨ(ϵͳ)
        ItemCreateType_NpcFall			= 2,	///< ����
    };

    /// @brief ����ʹ�ã�ָ����
    struct ItemUseTargetData
    {
        DWORD targetID;	///< Ŀ��ID
    };

    /// @brief ����ʹ�÷���
    enum ItemUseReturn
    {
        ItemUseReturn_None				= 0,

        ItemUseReturn_SubNum			= 1,				///< ������
        ItemUseReturn_SubTimes			= 2,				///< ��ʹ�ô���
        ItemUseReturn_SubDuable			= 3,				///< ���;�

        ItemUseReturn_Max
    };

    ///@brief ���ߵ���ȴ����
    enum ItemCoolDownType
    {
        ItemCoolDownType_None			= 0,
        ItemCoolDownType_HP				= 1,		///< ��ҩ
        ItemCoolDownType_Max,
    };
    /// @brief �;öȵ�λ
    const WORD Item_Durable_Unit			= 1000;

    const WORD nInvalidPackagePos = 0xffff;
    const WORD nInvalidPackageType = 0xffff;
    const DWORD nInvalidPackageLocation = 0xffffff;

    /// @brief 	��Ʒλ��
    struct ItemLocation{
        ItemLocation(): _value(nInvalidPackageLocation)
        {
        }
        explicit ItemLocation(const DWORD value) : _value(value){
        }
        ItemLocation(protocol::package::ePackageID packageID, WORD pos) :
            _package_id(static_cast<BYTE>(packageID)),
            _pos(pos)
        {
        }
        ItemLocation(const BYTE packageID, const WORD pos) :
            _package_id(packageID),
            _pos(pos)
        {
        }
        ItemLocation& operator = (const ItemLocation &location){
            this->_value = location._value; 
            return (*this);
        }
        bool operator == (const ItemLocation &location) const{
            return location._value == this->_value;
        }
        bool operator != (const ItemLocation &location){
            return location._value != this->_value;
        }
        WORD getPackagePos() const {
            return _pos; 
        }
        BYTE getPackageID() const {
            return _package_id; 
        }
        DWORD getValue() const { 
            return _value;
        }
        void setPos(WORD pos) { 
            _pos = pos; 
        }
        void setPackageID(BYTE packageID) { 
            _package_id = packageID; 
        }
        void setValue(DWORD value) { 
            _value = value; 
        }
        bool isValid() const{
            return _pos != nInvalidPackagePos;
        }
        void clear(){
            _value = nInvalidPackageLocation;
        }
        //TODO Σ�յ�����
        operator DWORD() const{
            return _value;
        }
        private:
        union{
            struct{
                BYTE _package_id;/// ���ڵİ�������
                WORD _pos;		/// �����е�x����
            };
            DWORD _value;		/// λ��
        };
    };

    /// @brief ���߻�������
    struct ItemBase
    {
        ItemBase()
        {
        }
        DWORD	serialID;   		///< ��ƷID,���������,�߼�������Ψһ�����ܸı� 
        DWORD	baseID;  			///< ��Ʒ����ID�����ڵ��߱����ID 
        ItemLocation location;
    };

    /// @brief �����ͻ��˵�һ����������,�������������л�
    struct ItemSerialize
    {
        ItemSerialize():size(0){}
        DWORD dataSize() const { return size* sizeof(data[0]);}
        DWORD allSize() const { return sizeof(*this) + dataSize(); }

        ItemBase itemBase;
        WORD size;							///< ���������ֽڴ�С
        char data[0];	                    ///< ������������ @ref protocol/Proto/ItemData.proto
    };

    /// @brief ������Ϣ�Ķ��󼶹���
    struct ItemData  : public ItemBase
    {
        ItemAttribute attr;

        bool init(const ItemSerialize& serialize)
        {
            memcpy(this, &serialize, sizeof(ItemBase));
                return this->attr.ParseFromArray(serialize.data, serialize.size);
        }
    };

	// ������ȴ����
	struct ItemColdData
	{
		WORD	coldType;			///< ��ȴ����
		DWORD	endTimeInSec;		///< ��ȴ��ֹʱ���
	};

    struct ItemOperTypeData{
        BYTE itemOperType;
        char desc[MAX_NAMESIZE];
    };
    //tolua_begin
    // ���߲�������
    enum eItemOperType{
        eItemOperType_None          =   0,
        eItemOperType_UsedSubNum    =   1,//��Ʒʹ��
        eItemOperType_SellSubNum    =   2,//���ۼ���
        eItemOperType_QuestSubNum   =   3,//����۳�
        eItemOperType_SplitSubNum   =   4,//��ּ���
        eItemOperType_BuyCreate     =   5,//��Ʒ���򴴽�
        eItemOperType_QuestCreate   =   6,//��Ʒ���񴴽�
        eItemOperType_SendCreate    =   7,//��������
        eItemOperType_MailCreate    =   8,//�ʼ�����
        eItemOperType_GMCreate      =   9,//GMָ���
        eItemOperType_GMMailCreate  =   10,//GM�ʼ�����
        eItemOperType_OtherCreate   =   11,//������ʽ����(Ĭ��)
        eItemOperType_OtherDelete   =   12,//������ʽɾ��(Ĭ��)
        eItemOperType_UnionedSuccDelete =   13,//�ϲ���Ʒ�ɹ���ɾ��
        eItemOperType_AddFailDelete =   14,//���ʧ��ɾ��
        eItemOperType_UseDelete     =   15,//ʹ��ɾ��
        eItemOperType_TempCreate    =   16,//��ʱ����
        eItemOperType_InitNewChar   =   17,//�½�ɫ����
        eItemOperType_FetchMail     =   18,//�ʼ��������
        eItemOperType_FetchFail     =   19,//�ʼ�������ȡʧ��ɾ��
        eItemOperType_SendSuccDelete    =   20,//�ʼ��������ͳɹ���ɾ�� 
        eItemOperType_SendFailDelete    =   21,//�ʼ���������ʧ�ܺ�ɾ��
        eItemOperType_SummonPet         =   22,//�ٻ�����
        eItemOperType_PetUpStarLevel    =   23,//�����������ĵ���
        eItemOperType_DuplReward        =   24,//�������㽱�����
        eItemOperType_ExchangeItemCreate=   25,//�һ����ߴ���
        eItemOperType_ExchangeItemSub   =   26,//�һ����߼���
        eItemOperType_PetEquipDelete    =   27,//��������װ��ɾ�� 
        eItemOperType_FirstPassDupl     =   28,//�����״�ͨ�ػ�� 
        eItemOperType_DuplRealmBox      =   29,//���籦���� 
        eItemOperType_EquipIntensifySub =   30,//ǿ��װ����λ�۳� 
        eItemOperType_UnlockTrump       =   31,//���������۳�����
        eItemOperType_DuplMopUpItemSub  =   32,//ɨ���۳�ɨ��ȯȯ
        eItemOperType_UpLevelTrump      =   33,//���������۳�����
        eItemOperType_threeStarExtraAward       =   34,//�ɺ��ؾ�����ͨ�ؽ���
        eItemOperType_Raffle                    =   35, //�ٻ��齱
        eItemOperType_LuckyBagSub               =   36, //���������۳�
        eItemOperType_LuckyBagAdd               =   37, //�����������
        eItemOperType_MoneyDrawingBoxAdd        =   37, //�вƽ���������
        eItemOperType_ActiveStone               =   38, //���ʯ���� 
        eItemOperType_UpStoneLevel              =   39, //������ʯ���� 
        eItemOperType_GamlingDupl               =   40, //��ħ�ĳ���� 
        eItemOperType_TaskAdd                   =   41, //��ħ�ĳ���� 
        eItemOperType_QuickBuy                  =   42, //��ݹ������
        eItemOperType_DiscoveryBox              =   43, //�Ǻ�̽�ر�����
        eItemOperType_ClubBuy                   =   44, //�����̵깺����
        eItemOperType_DiscoveryShopBuy          =   45, //�Ǻ�̽���̵깺��
        eItemOperType_SignAdd                   =   46, //ǩ�����
        eItemOperType_DiscoveryOfflineReward    =   47, //�Ǻ��йܻ��
        eItemOperType_DiscoveryKillMonster      =   48, //�Ǻ�ɱ��
        eItemOperType_DiscoveryAddSpeed         =   49, //�Ǻ����ٲɼ� 
        eItemOperType_UpWing                    =   50, //������� 
        eItemOperType_VipBox                    =   51, //vip���� 
        eItemOperType_Task                      =   52, //��ħ�ĳ���� 
        eItemOperType_MAX
    };
    //tolua_end
    static ItemOperTypeData itemOperTypeData[] =
    {
        {eItemOperType_UsedSubNum, "user used"},
    };
    inline const char *getItemOperTypeString(eItemOperType type)
    {
        for(DWORD i = 0; i < sizeof(itemOperTypeData)/sizeof(itemOperTypeData[0]); ++ i){
            if(itemOperTypeData[i].itemOperType == type){
                return itemOperTypeData[i].desc;
            }
        }
        return "";
    }
    struct ItemBaseData{
        DWORD itemID;
        char itemName[32];
        BYTE itemType;
        DWORD itemOverdueTime;
    };
    // ��������
    enum eItemType
    {
        eItemType_None      =   0,
        eItemType_Equip     =   1,  // װ��
		eItemType_Function	=   2,	// �����࣬����ֱ��ʹ��
        eItemType_Stuff     =   3,  // ����
        eItemType_PetEquip  =   4,  // ����װ��
        eItemType_LuckyBag  =   5,  // ����
    };

    // װ��������
    enum eItemEquipType
    {
        eItemEquipType_None      =   0,
        eItemEquipType_Jian      =   1,	// �� 
        eItemEquipType_Gun       =   2,	// �� 
        eItemEquipType_Dao       =   3,	// ��
        eItemEquipType_Quan      =   4,	// ȭ��
		eItemEquipType_Max
    };

    // ���������������
    enum eItemFuncSubType
    {
		eItemFuncSubType_None		=	0,
        eItemFuncSubType_Consume	=   1,	// ������
        eItemFuncSubType_Function	=   2,	// ������
        eItemFuncSubType_Resource	=   3,	// ������
		eItemFuncSubType_Max
    };
    // ����Ʒ�ʷ���
    enum eItemQualityType
    {
        eItemQualityType_None       = 0,
        eItemQualityType_White      = 1,
        eItemQualityType_Green      = 2,
        eItemQualityType_Blue       = 5,
        eItemQualityType_Purple     = 20,
        eItemQualityType_Orange     = 40,
        eItemQualityType_Max
    };
    // ����Ʒ������
    enum eItemGradeType
    {
        eItemGradeType_None         = 0,
        eItemGradeType_C            = 1,
        eItemGradeType_B            = 2,
        eItemGradeType_A            = 3,
        eItemGradeType_S            = 4,
        eItemGradeType_SS           = 5,
        eItemGradeType_Max
    };
    // ���߼���״̬
    enum eItemIdentifyType
    {
        eItemIdentifyType_No        = 0,
        eItemIdentifyType_Yes       = 1,
    };

    // �������
    enum eIdentifyResultType
    {
        eIdentifyResultType_Fail    = 0,
        eIdentifyResultType_Succ    = 1,
    };

    // ����ʹ��Ч������
    enum eItemEffectType
    {
        eItemEffectType_None        = 0,
        eItemEffectType_AddVitality = 1,
    };
    // ����������� 
    enum eBuyItemType
    {
        eBuyItemType_Mystery    = 0, // �����̵�
        eBuyItemType_QuickBuy   = 1, // ��ݹ���
        eBuyItemType_Club       = 2, // �����̵� 
        eBuyItemType_Discovery  = 3, // �Ǻ�̽��(����ֵ) 
        eBuyItemType_PVP        = 4, // PVP�̵� 
    };
}}
#endif
