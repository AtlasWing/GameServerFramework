// 道具属性声明,需要严格遵守规则,不要加任何其他代码

/// @brief 道具基础属性
#ifdef ITEM_BASE_ATTRIBUTE

	ITEM_BASE_ATTRIBUTE( WORD,	Num,	    num,	    0,      "数量" )

#endif

/// @brief 道具特殊属性
#ifdef ITEM_RARE_ATTRIBUTE

	ITEM_RARE_ATTRIBUTE( bool,	Bind,	    bind,	    false,  "绑定" )
	ITEM_RARE_ATTRIBUTE( DWORD,	Quality,	quality,    0,	"品质" )
	ITEM_RARE_ATTRIBUTE( DWORD,	Grade,	    grade,      0,	"品级" )
	ITEM_RARE_ATTRIBUTE( DWORD,	Identify,	identify,   0,	"鉴定状态" )
	ITEM_RARE_ATTRIBUTE( bool,	Received,	received,   false,"是否刚获得" )
#endif

#ifdef ITEM_LOG_ATTRIBUTE
    ITEM_LOG_ATTRIBUTE(DWORD, useiteminfo, usenum, 0, "物品使用数量")
    ITEM_LOG_ATTRIBUTE(DWORD, buytypeinfo, shopid, 0, "商店id")
    ITEM_LOG_ATTRIBUTE(DWORD, buytypeinfo, shoppage1, 0, "商店页签一")
    ITEM_LOG_ATTRIBUTE(DWORD, buytypeinfo, shoppage2, 0,"商店页签二")
    ITEM_LOG_ATTRIBUTE(DWORD, buytypeinfo, pos, 0,"位置")
    ITEM_LOG_ATTRIBUTE(DWORD, buytypeinfo, moneytype, 0,"金钱类型")
    ITEM_LOG_ATTRIBUTE(DWORD, buytypeinfo, moneyvalue, 0,"金钱数量")
    ITEM_LOG_ATTRIBUTE(DWORD, buytypeinfo, num, 0,"购买数量")
    ITEM_LOG_ATTRIBUTE(DWORD, movetypeinfo, spacketid, 0,"移动源包裹")
    ITEM_LOG_ATTRIBUTE(DWORD, movetypeinfo, spos, 0,"源位置")
    ITEM_LOG_ATTRIBUTE(DWORD, movetypeinfo, dpacketid, 0, "目标包裹") 
    ITEM_LOG_ATTRIBUTE(DWORD, movetypeinfo, dpos, 0, "目标位置")
#endif
