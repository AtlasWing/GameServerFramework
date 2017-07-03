#ifndef _N_BASE_TIMETICKSERVICE_H_
#define _N_BASE_TIMETICKSERVICE_H_

#include "base/nThread.h"
#include "base/nTimer.h"

namespace Tidy
{
	class nBaseTickService : public nThread
	{
		public:
			nBaseTickService();
			virtual ~nBaseTickService();


		protected:
			nTimer _half_sec;
			nTimer _one_sec;
			nTimer _ten_sec;
			nTimer _one_min;
			nTimer _five_min;
			nTimer _ten_min;
			nTimer _one_hour;
			nTimer script_ten_min;
			nTimer script_one_hour;
			nTimer _twentyFourHour_hour;
			nClocker _zero_clocker;
			nClocker _one_hour_clocker;

        private:
            DWORD _tick_time_in_msec;
	};
};
#endif
