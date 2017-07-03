#ifndef _TABLE_H_
#define _TABLE_H_

#include <string>
#include "game/nTableBase.h"
#include "game/nTableManager.h"
#include "game/nTableFactory.h"
#include "game/nPublicDefine.h"
#include "game/nAlgebra.h"
#include "game/nPos.h"
#include "base/nStringTool.h"
#include "base/nScheduleTimer.h"

#include "protocol/itemCommon.h"

namespace Game 
{
    using namespace std;
    void RegisterTableCreateInstance();
    void RegisterSpecialType();
    ///ItemBaseTable lines:82

    class ItemBaseTable : public TableBase
	{
	public :
		ItemBaseTable() { Name = "ItemBaseTable"; }
		ItemBaseTable(ItemBaseTable &other)
		{
			*this = other;
		}
		~ItemBaseTable() { }
		static void * createInstance(string content = "")
		{
			return new ItemBaseTable();
		}

		SYNTHESIZE(ItemBaseTable, int , id);
		SYNTHESIZE(ItemBaseTable, string , name);
		SYNTHESIZE(ItemBaseTable, int , type);
		SYNTHESIZE(ItemBaseTable, int , childType);
		SYNTHESIZE(ItemBaseTable, int , maxRepeat);
		SYNTHESIZE(ItemBaseTable, int , CDGroup);
		SYNTHESIZE(ItemBaseTable, int , CDTime);
		SYNTHESIZE(ItemBaseTable, int , sellPrice);
		SYNTHESIZE(ItemBaseTable, int , bindType);
		SYNTHESIZE(ItemBaseTable, int , levelNeeded);
		SYNTHESIZE(ItemBaseTable, int , quality);
        SYNTHESIZE(ItemBaseTable, string , effectType);
		virtual void registProperty()
		{
			SAVE_MEMBER(id,int);
			SAVE_MEMBER(name,string);
			SAVE_MEMBER(type,int);
			SAVE_MEMBER(childType,int);
			SAVE_MEMBER(maxRepeat,int);
			SAVE_MEMBER(CDGroup,int);
			SAVE_MEMBER(CDTime,int);
			SAVE_MEMBER(sellPrice,int);
			SAVE_MEMBER(bindType,int);
			SAVE_MEMBER(levelNeeded,int);
			SAVE_MEMBER(quality,int);
            SAVE_MEMBER(effectType,string);
		}
        virtual bool initTableParam()
        {
            return true;
        }
	public :
		/// <summary> 道具ID </summary>
		int id;
		/// <summary> 名字 </summary>
		string name;
		/// <summary> 道具类型1.装备、2功能、3材料、4仙灵 </summary>
		int type;
		/// <summary> 道具子类 </summary>
		int childType;
		/// <summary> 叠加上限 </summary>
		int maxRepeat;
		/// <summary> 道具使用cd组 </summary>
		int CDGroup;
		/// <summary> 道具使用cd </summary>
		int CDTime;
		/// <summary> 游戏币出售价格 </summary>
		int sellPrice;
		/// <summary> 绑定类型 </summary>
		int bindType;
		/// <summary> 等级要求 </summary>
		int levelNeeded;
		/// <summary> 道具品质（1-白；2-4绿；5-19蓝；20-39紫；40+橙色） </summary>
		int quality;
        /// <summary> 效果类型(1是增加体力值，格式：1-10,第二个字段为体力值数) </summary>
        string effectType;

        // 自定义
        DWORD tempCoreData;
		string GetKey1()
		{
			return "id";
		}
		string GetKey2()
		{
			return "id";
		}

		ItemBaseTable* New()
		{
			return new ItemBaseTable(*this);
		}
        DWORD getNeedCapacity(const DWORD num) const{
            assert_debug(maxRepeat > 0, "无法放进包裹");
            if((num%maxRepeat) == 0){
                return num / maxRepeat;
            }
            return (num + maxRepeat)/maxRepeat;
        }
	};

    ///SceneTable lines:46

    class SceneTable : public TableBase
    {
        public :
            SceneTable() { Name = "SceneTable"; }
            SceneTable(SceneTable &other)
            {
                *this = other;
            }
            ~SceneTable() { }
            static void * createInstance(string content = "")
            {
                return new SceneTable();
            }

            SYNTHESIZE(SceneTable, int , ID);
            SYNTHESIZE(SceneTable, string , Name);
            SYNTHESIZE(SceneTable, int , SceneType);
            virtual void registProperty()
            {
                SAVE_MEMBER(ID,int);
                SAVE_MEMBER(Name,string);
                SAVE_MEMBER(SceneType,int);
            }
            virtual bool initTableParam()
            {
                return true;
            }
        public :
            /// <summary> 主键 </summary>
            int ID;
            /// <summary> 场景名 </summary>
            string Name;
            /// <summary> 场景类型(1 通天之路战斗场景，2 主场景 3登录场景 4金币本，5 星海探秘,6斗魔赌场) </summary>
            int SceneType;

            // 自定义数据
            DWORD tempCoreData;
            string GetKey1()
            {
                return "ID";
            }
            string GetKey2()
            {
                return "ID";
            }

            SceneTable* New()
            {
                return new SceneTable(*this);
            }
    };

    ///FilterBaseTable lines:42

    class FilterBaseTable : public TableBase
    {
        public :
            FilterBaseTable() { Name = "FilterBaseTable"; }
            FilterBaseTable(FilterBaseTable &other)
            {
                *this = other;
            }
            ~FilterBaseTable() { }
            static void * createInstance(string content = "")
            {
                return new FilterBaseTable();
            }

            SYNTHESIZE(FilterBaseTable, int , ID);
            SYNTHESIZE(FilterBaseTable, string , Name);
            virtual void registProperty()
            {
                SAVE_MEMBER(ID,int);
                SAVE_MEMBER(Name,string);
            }
        public :
            /// <summary> ID </summary>
            int ID;
            /// <summary> 屏蔽字 </summary>
            string Name;

            // 自定义数据
            DWORD tempCoreData;
            string GetKey1()
            {
                return "ID";
            }
            string GetKey2()
            {
                return "ID";
            }

            FilterBaseTable* New()
            {
                return new FilterBaseTable(*this);
            }
    };
    //ENDOFCLASS
};


#endif

