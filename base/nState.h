#ifndef _NSTATE_H_
#define _NSTATE_H_

#include "base/nObj.h"

namespace Tidy
{
	class nState : public nObj
	{
		public:
			nState()
			{
			}
			virtual ~nState()
			{
			}

		public:
			/// @brief	进入状态
			bool enterState(nObj *entity, const nState *srcState)
			{
				return doEnter(entity, srcState);
			}
			/// @brief	状态更新
			bool updateState(nObj *entity)
			{
				return doUpdate(entity);
			}
			/// @brief	状态结束.
			bool exitState(nObj *entity, const nState *destState)
			{
				return doExit(entity, destState);
			}
			const bool isEqualTo(const nState *src) const
			{
				return this == src;
			}

		private:
			/// @brief	进入状态.子类重写.
			virtual bool doEnter(nObj *entity, const nState *srcState)
			{
				return true;
			}
			/// @brief	状态更新.子类重写
			virtual bool doUpdate(nObj *entity)
			{
				return true;
			}
			/// @brief	状态结束.子类重写
			virtual bool doExit(nObj *entity, const nState *destState)
			{
				return true;
			}
	};
};

#endif
