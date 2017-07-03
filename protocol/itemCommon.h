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
	const DWORD FOREVER_OVERDUE_TIME_IN_SEC	=	999999999;	///< 道具永久有效期时间定义

    //特殊道具ID begin
	const WORD ITEM_BASE_ID_START		=	1003;	///< 特殊道具ID 开始
    //特殊道具ID end
    
    //tolua_begin
    /// @brief 道具添加方式,用于日志统计
    enum ItemAddType
    {
        ItemAddType_None            = 0,
        ItemAddType_Normal          = 1,    ///< 普通添加
        ItemAddType_Union           = 2,	///< 合并添加
        ItemAddType_Scene           = 4,	///< 场景拾取
        ItemAddType_Shop			= 5,	///< 商店购买
        ItemAddType_Trade			= 6,	///< 交易获得
        ItemAddType_Mail			= 7,	///< 邮件获得
        ItemAddType_GmCommand		= 8,	///< GM指令fetch

        ItemAddType_Max
    };

    // 商店类型
    enum eShopType
    {
        eShopType_None      =   0,
        eShopType_Main      =   1,
        eShopType_Max
    };
    //tolua_end

    /// @brief 道具使用的方式类型,发给服务器时只能指定一种方式
    enum ItemUseType
    {
        ItemUseType_None			= 0,	

        ItemUseType_Self			= 1<<0,	///< 右键直接使用
        ItemUseType_TargetNpc		= 1<<1,	///< 指向NPC, 数据 ItemUseTargetData
        ItemUseType_TargetUser		= 1<<2,	///< 指向用户, 数据 ItemUseTargetData
        ItemUseType_TargetItem		= 1<<3,	///< 指向另一个道具, 数据 ItemUseTargetData
        ItemUseType_String			= 1<<4,	///< 携带字符串

        ItemUseType_Max
    };

    // 道具绑定类型
    enum eItemBindType
    {
        eItemBindType_None              =   0,  ///<不绑定
        eItemBindType_OnCreate          =   1,  ///<创建就绑定
        eItemBindType_OnUse             =   2,  ///<使用了绑定,比如装备
        eItemBindType_OnTrade           =   3,  ///<交易后绑定
    };
    /// @brief 道具创建类型(道具数值生成时用)
    enum ItemCreateType
    {
        ItemCreateType_None				= 0,	
        ItemCreateType_Normal			= 1,	///< 普通(系统)
        ItemCreateType_NpcFall			= 2,	///< 掉落
    };

    /// @brief 道具使用，指向型
    struct ItemUseTargetData
    {
        DWORD targetID;	///< 目标ID
    };

    /// @brief 道具使用返回
    enum ItemUseReturn
    {
        ItemUseReturn_None				= 0,

        ItemUseReturn_SubNum			= 1,				///< 扣数量
        ItemUseReturn_SubTimes			= 2,				///< 扣使用次数
        ItemUseReturn_SubDuable			= 3,				///< 扣耐久

        ItemUseReturn_Max
    };

    ///@brief 道具的冷却类型
    enum ItemCoolDownType
    {
        ItemCoolDownType_None			= 0,
        ItemCoolDownType_HP				= 1,		///< 红药
        ItemCoolDownType_Max,
    };
    /// @brief 耐久度单位
    const WORD Item_Durable_Unit			= 1000;

    const WORD nInvalidPackagePos = 0xffff;
    const WORD nInvalidPackageType = 0xffff;
    const DWORD nInvalidPackageLocation = 0xffffff;

    /// @brief 	物品位置
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
        //TODO 危险的做法
        operator DWORD() const{
            return _value;
        }
        private:
        union{
            struct{
                BYTE _package_id;/// 所在的包裹类型
                WORD _pos;		/// 包裹中的x坐标
            };
            DWORD _value;		/// 位置
        };
    };

    /// @brief 道具基本数据
    struct ItemBase
    {
        ItemBase()
        {
        }
        DWORD	serialID;   		///< 物品ID,随机数生成,逻辑服务器唯一，可能改变 
        DWORD	baseID;  			///< 物品基本ID，即在道具表里的ID 
        ItemLocation location;
    };

    /// @brief 发给客户端的一个道具数据,仅用于网络序列化
    struct ItemSerialize
    {
        ItemSerialize():size(0){}
        DWORD dataSize() const { return size* sizeof(data[0]);}
        DWORD allSize() const { return sizeof(*this) + dataSize(); }

        ItemBase itemBase;
        WORD size;							///< 道具属性字节大小
        char data[0];	                    ///< 道具属性数据 @ref protocol/Proto/ItemData.proto
    };

    /// @brief 道具信息的对象级管理
    struct ItemData  : public ItemBase
    {
        ItemAttribute attr;

        bool init(const ItemSerialize& serialize)
        {
            memcpy(this, &serialize, sizeof(ItemBase));
                return this->attr.ParseFromArray(serialize.data, serialize.size);
        }
    };

	// 道具冷却数据
	struct ItemColdData
	{
		WORD	coldType;			///< 冷却类型
		DWORD	endTimeInSec;		///< 冷却截止时间戳
	};

    struct ItemOperTypeData{
        BYTE itemOperType;
        char desc[MAX_NAMESIZE];
    };
    //tolua_begin
    // 道具操作类型
    enum eItemOperType{
        eItemOperType_None          =   0,
        eItemOperType_UsedSubNum    =   1,//物品使用
        eItemOperType_SellSubNum    =   2,//出售减少
        eItemOperType_QuestSubNum   =   3,//任务扣除
        eItemOperType_SplitSubNum   =   4,//拆分减少
        eItemOperType_BuyCreate     =   5,//物品购买创建
        eItemOperType_QuestCreate   =   6,//物品任务创建
        eItemOperType_SendCreate    =   7,//购买赠送
        eItemOperType_MailCreate    =   8,//邮件创建
        eItemOperType_GMCreate      =   9,//GM指令创建
        eItemOperType_GMMailCreate  =   10,//GM邮件创建
        eItemOperType_OtherCreate   =   11,//其他方式创建(默认)
        eItemOperType_OtherDelete   =   12,//其他方式删除(默认)
        eItemOperType_UnionedSuccDelete =   13,//合并物品成功后删除
        eItemOperType_AddFailDelete =   14,//添加失败删除
        eItemOperType_UseDelete     =   15,//使用删除
        eItemOperType_TempCreate    =   16,//临时创建
        eItemOperType_InitNewChar   =   17,//新角色创建
        eItemOperType_FetchMail     =   18,//邮件附件获得
        eItemOperType_FetchFail     =   19,//邮件附件获取失败删除
        eItemOperType_SendSuccDelete    =   20,//邮件附件发送成功后删除 
        eItemOperType_SendFailDelete    =   21,//邮件附件发送失败后删除
        eItemOperType_SummonPet         =   22,//召唤宠物
        eItemOperType_PetUpStarLevel    =   23,//宠物升级消耗道具
        eItemOperType_DuplReward        =   24,//副本结算奖励获得
        eItemOperType_ExchangeItemCreate=   25,//兑换道具创建
        eItemOperType_ExchangeItemSub   =   26,//兑换道具减少
        eItemOperType_PetEquipDelete    =   27,//穿戴宠物装备删除 
        eItemOperType_FirstPassDupl     =   28,//副本首次通关获得 
        eItemOperType_DuplRealmBox      =   29,//境界宝箱获得 
        eItemOperType_EquipIntensifySub =   30,//强化装备部位扣除 
        eItemOperType_UnlockTrump       =   31,//解锁法宝扣除道具
        eItemOperType_DuplMopUpItemSub  =   32,//扫荡扣除扫荡券券
        eItemOperType_UpLevelTrump      =   33,//升级法宝扣除道具
        eItemOperType_threeStarExtraAward       =   34,//仙骸秘境三星通关奖励
        eItemOperType_Raffle                    =   35, //召唤抽奖
        eItemOperType_LuckyBagSub               =   36, //开启福袋扣除
        eItemOperType_LuckyBagAdd               =   37, //开启福袋获得
        eItemOperType_MoneyDrawingBoxAdd        =   37, //招财进宝宝箱获得
        eItemOperType_ActiveStone               =   38, //激活宝石消耗 
        eItemOperType_UpStoneLevel              =   39, //升级宝石消耗 
        eItemOperType_GamlingDupl               =   40, //斗魔赌场获得 
        eItemOperType_TaskAdd                   =   41, //斗魔赌场获得 
        eItemOperType_QuickBuy                  =   42, //快捷购买道具
        eItemOperType_DiscoveryBox              =   43, //星海探秘宝箱获得
        eItemOperType_ClubBuy                   =   44, //工会商店购买获得
        eItemOperType_DiscoveryShopBuy          =   45, //星海探秘商店购买
        eItemOperType_SignAdd                   =   46, //签到获得
        eItemOperType_DiscoveryOfflineReward    =   47, //星海托管获得
        eItemOperType_DiscoveryKillMonster      =   48, //星海杀怪
        eItemOperType_DiscoveryAddSpeed         =   49, //星海加速采集 
        eItemOperType_UpWing                    =   50, //升级翅膀 
        eItemOperType_VipBox                    =   51, //vip宝箱 
        eItemOperType_Task                      =   52, //斗魔赌场获得 
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
    // 道具类型
    enum eItemType
    {
        eItemType_None      =   0,
        eItemType_Equip     =   1,  // 装备
		eItemType_Function	=   2,	// 功能类，不可直接使用
        eItemType_Stuff     =   3,  // 材料
        eItemType_PetEquip  =   4,  // 宠物装备
        eItemType_LuckyBag  =   5,  // 福袋
    };

    // 装备子类型
    enum eItemEquipType
    {
        eItemEquipType_None      =   0,
        eItemEquipType_Jian      =   1,	// 剑 
        eItemEquipType_Gun       =   2,	// 棍 
        eItemEquipType_Dao       =   3,	// 刀
        eItemEquipType_Quan      =   4,	// 拳套
		eItemEquipType_Max
    };

    // 功能类道具子类型
    enum eItemFuncSubType
    {
		eItemFuncSubType_None		=	0,
        eItemFuncSubType_Consume	=   1,	// 消耗类
        eItemFuncSubType_Function	=   2,	// 功能类
        eItemFuncSubType_Resource	=   3,	// 材料类
		eItemFuncSubType_Max
    };
    // 道具品质分类
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
    // 道具品级分类
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
    // 道具鉴定状态
    enum eItemIdentifyType
    {
        eItemIdentifyType_No        = 0,
        eItemIdentifyType_Yes       = 1,
    };

    // 鉴定结果
    enum eIdentifyResultType
    {
        eIdentifyResultType_Fail    = 0,
        eIdentifyResultType_Succ    = 1,
    };

    // 道具使用效果类型
    enum eItemEffectType
    {
        eItemEffectType_None        = 0,
        eItemEffectType_AddVitality = 1,
    };
    // 购买道具类型 
    enum eBuyItemType
    {
        eBuyItemType_Mystery    = 0, // 神秘商店
        eBuyItemType_QuickBuy   = 1, // 快捷购买
        eBuyItemType_Club       = 2, // 仙盟商店 
        eBuyItemType_Discovery  = 3, // 星海探秘(念力值) 
        eBuyItemType_PVP        = 4, // PVP商店 
    };
}}
#endif
