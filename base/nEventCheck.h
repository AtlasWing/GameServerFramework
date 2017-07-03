/**
 * @brief	定义事件检查的基类。
 *
 * 
 */

#ifndef _NEVENT_CHECK_H_
#define _NEVENT_CHECK_H_

#include "base/nType.h"
#include "base/nEventObserver.h"


namespace Tidy
{
	class nEventLaunch;
	class nEventCheck
	{
		protected:
			friend class nEventLaunch;
			/// @brief	构造函数
			nEventCheck()
			{
			}
			/// @brief	析构函数
			virtual ~nEventCheck()
			{
			}

		public:
			/// @brief	事件处理，子类重写
			virtual bool notifyEvent(nEventLaunch *launch, nEventObserver* observer, DWORD eventID, const EventParam *eventParam = NULL) = 0;
			/// @brief	注册事件.子类重写
			virtual void regMe(nEventObserver* observer)
			{
			}
			/// @brief	收集事件.如果丢失的事件对于逻辑有问题的话,可以重新收集这些事件
			/// @param	observer 事件的观察者.
			virtual void collectOutEvent(nEventObserver* observer)
			{
				return;
			}
			/// @brief	撤销注册
			/// @param	某个观察者撤销注册.一般子类不需要重写,先写成非虚函数,有需要再改成虚函数
			virtual void unregMe(nEventObserver* observer)
			{
				observer->unregEventByEventCheck((nEventCheck*)this);
			}
			virtual void dealRemoveEvent(const DWORD &eventID, nEventLaunch *launch)
			{
			}

		public:
			/// @brief	事件的名字
			const char *name;

	};
};
#endif
