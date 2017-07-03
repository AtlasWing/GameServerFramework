#ifndef GAME_NITEM_H_
#define GAME_NITEM_H_

#include "base/nObjLogger.h"

#include "game/nItemData.h"

enum eItemAddResult{
	// 添加失败
	eItemAddResult_Fail =   0,
	// 添加到新位置上
	eItemAddResult_Succ,
	// 全部合并了
	eItemAddResult_AllUnioned,
	// 部分合并了
	eItemAddResult_PartUnioned,
	// 没有变化
	eItemAddResult_NoChange,
	// 邮件发送
	eItemAddResult_Mail,
	eItemAddResult_Max,
};

namespace Game{
    class ItemBaseTable;
};
namespace protocol{
	namespace item{
		struct ItemSerialize;
	};
};
namespace protocol{
	namespace item{
		struct ItemBinary;
	};
};

namespace Game
{
	class nItem : public Tidy::nObjLogger<nItem>
	{
		public:
			nItem(const QWORD globalID, const DWORD serialID, const ItemBaseTable *itemBaseData, const bool needBind, const DWORD num, const DWORD quality, const WORD identify);
			~nItem();

		public:
			const DWORD getID() const;
			const char* getName() const;
			DWORD getBaseID() const;
			Tidy::nLogger* getLogger() const;
			const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;

		public:
			bool init();

		public:
			bool checkUnion(const nItem *srcItem, const WORD num) const;
			bool checkUnion(const ItemBaseTable *baseData, const bool bind) const;

		public:
			WORD getLeftOverlayNum() const;
			void setItem(const nItem *other);

		public:
			bool serialize(protocol::item::ItemSerialize *out, const DWORD maxSize) const;
			bool serialize(std::string *out) const;
			DWORD unSerialize(const protocol::item::ItemSerialize *in );

		public:
			bool restoreData(const protocol::item::ItemBinary &saveData);
			bool restoreData(const protocol::item::ItemAttribute &itemAttr);
			bool saveData(protocol::item::ItemBinary &itemBinary) const;

		public:
			BYTE getPackageID() const;
			WORD getPackagePos() const;
			const protocol::item::ItemLocation& getLocation() const;
			void setLocation(const BYTE packageType, const WORD packagePos);
			bool checkLocationValid() const;
			void clearLocation();
			void setLocation(DWORD value);

		public:
			static bool checkUnion(const ItemBaseTable *srcItemBaseData, const bool srcItemBind, const ItemBaseTable *destItemBaseData, const bool destItemBind);

		public:
			nItemData data;       ///< 道具自身的所有数据
			const ItemBaseTable *const baseItemData;

		public:
			/// @brief 	整个服务器道具的唯一标识,包括创建时间和创建时的序列号
			const QWORD globalID;
			const DWORD serialID;

		private:
			protocol::item::ItemLocation _location;
	};
    struct RewardItemData{
        RewardItemData(const ItemBaseTable *itemBaseData, const DWORD itemNum, const bool bind, const DWORD quality, const WORD identify) :
            baseData(itemBaseData),
            num(itemNum),
            needBind(bind),
            quality(quality),
            identify(identify)
        {
        }
        const ItemBaseTable *baseData;
        DWORD num;
        bool needBind;
        DWORD quality;
        WORD identify;
    };
};
#endif
