#ifndef _N_SERVER_TICK_TIMER_H_
#define _N_SERVER_TICK_TIMER_H_

#include "base/nTimer.h"

namespace Tidy
{

class nServerTickTimer : public nTimer
{
    public:
        nServerTickTimer();
        /// @brief	构造函数
        /// @param	how_long	定时器的时间，单位：毫秒
		/// @param	first		有些定时器可能希望在启动时就可以执行一次,所以不能直接addDelay哦
		nServerTickTimer(const long how_long, bool first = false, const long delay = 0); 

    public:
        const bool isReady();
};

};
#endif
