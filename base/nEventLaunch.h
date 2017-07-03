#ifndef _NEVENT_LAUNCH_H_
#define _NEVENT_LAUNCH_H_

#include "base/nSTL.h"
#include "base/nType.h"

namespace Tidy
{
	/// @brief	被删除的事件注册项的最大数量
#define ERASED_EVENT_ITEM_MAX (25)

	class nEventObserver;
	class nEventCheck;

	struct EventRegValue{
		EventRegValue(nEventObserver *observer, nEventCheck *eventCheck);
		nEventObserver *observer;
		nEventCheck *eventCheck;
		bool isValid;
	};
	struct EventRegItem{
		typedef Tidy::list<EventRegValue>::iterator iterator;
		typedef Tidy::list<EventRegValue>::const_iterator const_iterator;
		Tidy::list<EventRegValue> eventRegValueList;
		bool reg(nEventObserver *observer, nEventCheck *eventCheck);
		bool unreg(const nEventObserver *observer, const nEventCheck *eventCheck);
	};
	typedef Tidy::map<DWORD, EventRegItem> EventRegContainer;

	/// @brief	<EventID, EventCheck>事件ID和事件检查的list,用于存储注册的<EventID, EventCheck>
	typedef std::pair<DWORD, nEventCheck *> EventCheckListValue;
	typedef Tidy::list<EventCheckListValue> EventCheckList;

	class nEventLaunch
	{
		public:
			friend class nEventObserver;
			/// @brief	构造函数
			nEventLaunch();
			/// @brief	析构函数.自动删除所有注册
			~nEventLaunch();

		public:
			/// @brief	注册接口
			/// @param	eventID 要监听的事件ID
			/// @param	observer 观察者
			/// @param	eventCheck 事件的处理对象.const是为了方便传递this指针
			/// @return	是否成功
			bool regEvent(DWORD eventID, nEventObserver *observer, const nEventCheck *eventCheck);
			/// @brief	取消注册接口
			/// @param	eventID 事件ID
			/// @param	observer 观察者
			/// @param	eventCheck 事件检测对象
			/// @return	是否成功
			bool unregEvent(DWORD eventID, nEventObserver* observer, const nEventCheck *eventCheck);
			/// @brief	触发事件,通知所有监听此事件的对象.触发此事件的对象为this
			/// @param	eventID 事件ID
			/// @param	eventParam 事件参数
			bool fireEvent(DWORD eventID, const struct EventParam *eventParam=NULL, const bool initResult = true) const;
			/// @brief	触发事件,通知所有监听此事件的对象,指定了触发事件的对象
			/// @param	launch 触发事件的对象
			/// @param	eventParam 事件参数
			bool fireEvent(nEventLaunch *luanch, DWORD eventID, const EventParam *eventParam=NULL, const bool initResult = true) const;
			/// @brief	获得某个注册者注册的所有事件检查对象列表.这个执行的代价可能比较大,遍历所有注册
			/// @param	eventCheckList 传入的列表,传递引用
			/// @return	列表的数量
			DWORD getEventCheckListByObserver(const nEventObserver* observer, Tidy::list<std::pair<DWORD, nEventCheck*> > &eventCheckList) const;
			/// @brief	清除所有需要删除的注册事件
			DWORD clearErasedEventRegItem();
			const bool hasEventReged(const DWORD eventID) const;

		private:
			/// @brief	删除注册,oberver的删除,nEventObserver的析够函数使用
			/// @param	eventID 事件ID
			/// @param	observer 注册者
			/// @param	eventCheck 事件检查对象
			bool deleteEvent(const DWORD &eventID, nEventObserver* observer, nEventCheck *eventCheck);

		private:
			/// @brief	注册的所有事件的集合
			EventRegContainer _event_container;
			/// @brief	被删除的注册事件项的数量
			DWORD _erased_num;
	};
};

#endif
