#ifndef BASE_SCHEDULETIMER_H
#define BASE_SCHEDULETIMER_H

#include "base/nTime.h"

namespace Tidy
{
	class nScheduleTime
	{
		public:
			nScheduleTime();
			nScheduleTime(const SWORD monthTime, SWORD monthDayTime, SWORD weekDayTime, BYTE hourTime, BYTE minuteTime, BYTE sec, DWORD continueTimeInSec = 0);

		public:
			bool checkPast(nDayTime &lastCheckTime, nDayTime &time) const;
			bool getNextTime(nTime *time) const;
			bool getScheduleStartTime(nTime *startTime, const DWORD periodTimeInSec) const;
            DWORD getContinueTimeInSec() const;

		private:
			SWORD _year;
			SWORD _month;
			SWORD _month_day;
			SWORD _week_day;
			BYTE  _hour;
			BYTE  _minute;
			BYTE  _sec;
			DWORD _sec_day;
            DWORD _continue_time_in_sec;
	};

	class nScheduleTimer
	{
		public:
			nScheduleTimer(const SWORD month, SWORD monthDay, SWORD weekDay, BYTE hour, BYTE minute, BYTE sec);
			~nScheduleTimer();

		public:
			bool isReady(nDayTime &cur);

		private:
			nScheduleTime _fire_time;
			nDayTime _last_check_time;
	};
};
#endif
