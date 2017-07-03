#include "base/nScheduleTimer.h"
#include "base/nAssert.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	nScheduleTime::nScheduleTime()
	{
	}

	nScheduleTime::nScheduleTime(const SWORD monthTime, SWORD monthDayTime, SWORD weekDayTime, BYTE hourTime, BYTE minuteTime, BYTE sec, DWORD continueTimeInSec):
		_year(-1),
		_month(monthTime),
		_month_day(monthDayTime),
		_week_day(weekDayTime),
		_hour(hourTime),
		_minute(minuteTime),
		_sec(sec),
        _continue_time_in_sec(continueTimeInSec)
	{
		assert_debug(_hour < 24 && _minute < 60, "");
		assert_debug(_week_day < 7, "");
		//assert_debug(!(monthTime == -1 ^ monthDayTime == -1), "");
		_sec_day = _hour * 3600 + _minute * 60 + _sec;
	}

    DWORD nScheduleTime::getContinueTimeInSec() const
    {
        return _continue_time_in_sec;
    }

	bool nScheduleTime::checkPast(nDayTime &lastCheckTime, nDayTime &time) const
	{
		//日期改变,可以一次调整多天的时间
		if(lastCheckTime.checkInOneDay(time) == false){
			//昨天的剩下的任务
			if(_sec_day > lastCheckTime.getDaySec()){
				if(_month == lastCheckTime.getMonths() || _month == -1){
					if(_week_day == lastCheckTime.getWDays() || _week_day == -1){
						if(_month_day == lastCheckTime.getMDays() || _month_day == -1){
							return true;
						}
					}
				}
			}
			//今天的任务
			if(_sec_day <= time.getDaySec()){
				if(_month == time.getMonths() || _month == -1){
					if(_week_day == time.getWDays() || _week_day == -1){
						if(_month_day == time.getMDays() || _month_day == -1){
							return true;
						}
					}
				}
			}
			if((lastCheckTime.getDayStartInSec() + ONE_DAY_IN_SEC) < time.getDayStartInSec()){
				// 针对一次调整多天时间的处理
				nDayTime checkTime = lastCheckTime;
				QWORD dayTimeInMsec = ONE_DAY_IN_SEC * 1000;
				checkTime.addDelay(dayTimeInMsec);
				while(checkTime.checkInOneDay(time) == false){
					if(_month == checkTime.getMonths() || _month == -1){
						if(_week_day == checkTime.getWDays() || _week_day == -1){
							if(_month_day == checkTime.getMDays() || _month_day == -1){
								return true;
							}
						}
					}
					checkTime.addDelay(dayTimeInMsec);
				}
			}
		}
		//非0点情况
		else{
			if(_month == time.getMonths() || _month == -1){
				if(_week_day == time.getWDays() || _week_day == -1){
					if(_month_day == time.getMDays() || _month_day == -1){
						if((_sec_day > lastCheckTime.getDaySec()) && (_sec_day <= time.getDaySec())) {
							return true;
						}
					}
				}
			}
		}

		return false;
	}

	bool nScheduleTime::getNextTime(nTime *nextTime) const
	{
		char timeBuffer[128];
		DWORD timeBufferLen = 0;
		char timeFormatBuffer[128];
		DWORD timeFormatBufferLen = 0;
		nDayTime &currTime = baseServer->time();
		timeBufferLen += snprintf(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%u:", currTime.getYears());
		timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%Y:");
		if(_month != -1){
			timeBufferLen += snprintf(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%u:", _month);
			timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%m:");

			assert_debug(_month_day != -1, "设置月的日期");
			timeBufferLen += snprintf(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%u:", _month_day);
			timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%d:");
		}
		else if(_week_day != -1){
			timeBufferLen += currTime.format(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%W:");
			timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%W:");

			timeBufferLen += snprintf(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%u:", _week_day);
			timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%w:");
		}
		else{
			// 转换的月份需要加1
			timeBufferLen += snprintf(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%u:", (currTime.getMonths() + 1));
			timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%m:");

			timeBufferLen += snprintf(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%u:", currTime.getMDays());
			timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%d:");
		}
		timeBufferLen += snprintf(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%u:", _hour);
		timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%H:");

		timeBufferLen += snprintf(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%u:0", _minute);
		timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%M:%%S");

		struct tm time;
		strptime(timeBuffer, timeFormatBuffer, &time);
		DWORD timeStamp = mktime(&time);
		if(timeStamp <= baseServer->time().sec()){
			if(_week_day != -1){
				// 加一周
				timeStamp += (ONE_WEEK/1000);
			}
			else if(_month_day != -1){
				return false;
			}
			else{
				timeStamp += ONE_DAY_IN_SEC;
			}
		}
		nextTime->setTimeInSec(timeStamp);
		return true;
	}

	bool nScheduleTime::getScheduleStartTime(nTime *startTime, const DWORD periodTimeInSec) const
	{
		char timeBuffer[128];
		DWORD timeBufferLen = 0;
		char timeFormatBuffer[128];
		DWORD timeFormatBufferLen = 0;
		nDayTime &currTime = baseServer->time();
		DWORD yearStrLen = 0;
		if(_year == -1){
			yearStrLen = snprintf(&timeBuffer[0], sizeof(timeBuffer), "%u:", currTime.getYears());
		}
		else{
			yearStrLen = snprintf(&timeBuffer[0], sizeof(timeBuffer), "%d:", _year);
		}
		timeBufferLen += yearStrLen;
		timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%Y:");
		if(_month != -1){
			timeBufferLen += snprintf(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%u:", _month);
			timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%m:");

			assert_debug(_month_day != -1, "设置月的日期");
			timeBufferLen += snprintf(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%u:", _month_day);
			timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%d:");
		}
		else if(_week_day != -1){
			timeBufferLen += currTime.format(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%W:");
			timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%W:");

			timeBufferLen += snprintf(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%u:", _week_day);
			timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%w:");
		}
		else{
			// 转换的月份需要加1
			timeBufferLen += snprintf(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%u:", (currTime.getMonths() + 1));
			timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%m:");

			timeBufferLen += snprintf(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%u:", currTime.getMDays());
			timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%d:");
		}
		timeBufferLen += snprintf(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%u:", _hour);
		timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%H:");

		timeBufferLen += snprintf(&timeBuffer[timeBufferLen], (sizeof(timeBuffer) - timeBufferLen), "%u:0", _minute);
		timeFormatBufferLen += snprintf(&timeFormatBuffer[timeFormatBufferLen], (sizeof(timeFormatBuffer) - timeFormatBufferLen), "%%M:%%S");

		struct tm time;
		strptime(timeBuffer, timeFormatBuffer, &time);
		DWORD timeStamp = mktime(&time);
		if(timeStamp > baseServer->time().sec()){
			if(_week_day != -1){
				if((timeStamp - ONE_WEEK/1000) <= baseServer->time().sec() && (timeStamp - ONE_WEEK/1000 + periodTimeInSec) > baseServer->time().sec()){
					// 减一周
					timeStamp -= (ONE_WEEK/1000);
				}
			}
			else if(_month_day != -1){
			}
			else{
				if((timeStamp - ONE_DAY_IN_SEC) <= baseServer->time().sec() && (timeStamp - ONE_DAY_IN_SEC + periodTimeInSec) > baseServer->time().sec()){
					timeStamp -= ONE_DAY_IN_SEC;
				}
			}
		}
		else if((timeStamp + periodTimeInSec) <= baseServer->time().sec()){
			if(_week_day != -1){
				// 加一周
				timeStamp += (ONE_WEEK/1000);
			}
			else if(_month_day != -1){
				if(_year != -1){
					return false;
				}
				else{
					// 加一年.覆盖年的设置
					snprintf(&timeFormatBuffer[0], yearStrLen, "%u:", (currTime.getYears() + 1));
					timeStamp = mktime(&time);
				}
			}
			else{
				// 加一天
				timeStamp += ONE_DAY_IN_SEC;
			}
		}
		startTime->setTimeInSec(timeStamp);
		return true;
	}

	nScheduleTimer::nScheduleTimer(const SWORD month, SWORD monthDay, SWORD weekDay, BYTE hour, BYTE minute, BYTE sec):
		_fire_time(month, monthDay, weekDay, hour, minute, sec)
	{
		_last_check_time.now();
	}

	nScheduleTimer::~nScheduleTimer()
	{
	}

	bool nScheduleTimer::isReady(nDayTime &cur)
	{
		if(_fire_time.checkPast(_last_check_time, cur) == false){
			return false;
		}
		_last_check_time = cur;
		_last_check_time.checkTM();
		return true;
	}
};
