/**
 * @author	牛延升,niuyansheng@gmail.com
 * @brief	定义事件观察者的接口,维护所有注册过的事件
 *
 * 
 */

#ifndef _NEVENTOBSERVER_H_
#define _NEVENTOBSERVER_H_

#include "base/Tidy.h"

#include "base/nEventLaunch.h"
#include "base/nSTL.h"

namespace Tidy
{
	/// @brief	注册到某个launch对象上的<事件,对象>集合
	typedef std::pair<DWORD, nEventLaunch*> EventCheckRegValueType;
	typedef Tidy::set<EventCheckRegValueType> EventCheckRegItem;
	/// @brief	nEventCheck为key的事件注册索引
	typedef Tidy::map<nEventCheck*, EventCheckRegItem> RegEventCheckContainer;
	typedef RegEventCheckContainer::iterator RegEventCheckContainer_iter;

	class nEventObserver
	{
		protected:
			friend class nEventLaunch;
			/// @brief	构造函数
			nEventObserver()
			{
			}
			/// @brief	析构函数
			~nEventObserver()
			{
				unregAllEvent();
			}

		public:
			/// @brief	撤销注册的事件检查对象,这个在事件检查对象的unregMe中用到,会撤销所有的这个对象的注册
			/// @param	eventCheck 事件检查对象
			/// @return	撤销注册是否成功
			bool unregEventByEventCheck(nEventCheck *eventCheck);
			/// @brief	删除所有注册过的事件
			void unregAllEvent();

		private:
			/// @brief	添加注册事件。只有nEventLaunch::regEvent使用。
			/// @param	launch 要监听的对象
			bool addEvent(DWORD &eventID, nEventLaunch *launch, nEventCheck *eventCheck);
			/// @brief	删除注册的事件。只有nEventLaunch::unregEvent使用。
			/// @param	eventID 事件ID
			/// @param	launch 监听的对象
			/// @param	eventCheck 事件检查对象
			bool removeEvent(const DWORD &eventID, nEventLaunch *launch, nEventCheck *eventCheck);

		public:
			/// @brief	测试使用.打印出当前注册过的所有事件检查对象的名字
			/// @param	ownerLog 日志头标识
			void printAllEventCheck(const char *ownerLog) const;

		private:
			/// @brief	维护注册过的所有nEventCheck索引,这个索引主要用来根据nEventCheck来撤销注册
			RegEventCheckContainer _reg_event_check_con;
	};
};

#endif
