#ifndef _NTIME_H_
#define _NTIME_H_

#include "base/nSys.h"

/// @brief	秒,分,时,日,周的毫秒数的宏.FOREVER代表不再执行,是DWORD的最大值
#define ONE_SEC (1000)
#define HALF_SEC (500)
#define HUNDRED_MSEC (100)
#define TWO_HUNDRED_MSEC (200)
#define FIVE_SEC (5000)
#define TEN_SEC (10000)
#define ONE_MIN (60000)
#define HALF_MIN (30000)
#define FIVE_MIN (300000)
#define FIVE_MIN_IN_SEC (300)
#define TEN_MIN (600000)
#define ONE_HOUR (3600000)
#define HALF_HOUR (1800000)
#define ONE_DAY (86400000)
#define ONE_DAY_IN_SEC (86400)
#define ONE_WEEK_IN_SEC (604800)
#define TWO_DAY (172800000)
#define HALF_DAY (43200000)
#define ONE_WEEK (604800000)
#define FOREVER (4294967294)

#define ONE_MONTH_IN_SEC (2592000)
#define ONE_BIG_MONTH_IN_SEC (2678400)     //31天
#define ONE_MIN_IN_SEC (60)
#define ONE_HOUR_IN_SEC (3600)

namespace Tidy
{
	class nTime
	{
		public:
			friend class nClocker;
			/// @brief	设置时区
			static bool setTimeZone();
			/// @brief	设置系统相对时间
			/// @param	offset 偏移量(秒)
			static void setAdjustTime(const SDWORD offset = 0);
			static void addAdjustTime(const SDWORD offset = 0);

			/// @brief	保留的时区信息
			//			程序初始化的时候设置，运行过程中一般不变
			static long timeZone;
			/// @brief	调整系统相对时间
			// 			主要用于在线调整服务器运行时间而不影响系统时间
			static long adjustTime;

		public:
			nTime()
			{
				now();
			}
			nTime(const nTime &rt)
			{
				_tv = rt._tv;
				if(rt._tm_ok == true){
					_tv.tv_sec += adjustTime;
				}
				_tm_ok = false;
			}
			nTime(const DWORD timeInSec)
			{
				_tv.tv_sec = timeInSec + adjustTime;
				_tv.tv_usec = 0;
				_tm_ok = false;
			}

			void setTimeInSec(const DWORD timeInSec)
			{
				_tv.tv_sec = timeInSec + adjustTime;
				_tv.tv_usec = 0;
				_tm_ok = false;
			}

			/// @brief 调用得到当前的系统时间
			void now()
			{
				/// @brief 获得timespec真实时间,用来填充结构,gettimeofday 取得的时UTC时间
				gettimeofday(&_tv, NULL);
				_tm_ok = false;
			}

			inline unsigned long sec() const
			{
				if(_tm_ok == false){
					return _tv.tv_sec - adjustTime;
				}
				return _tv.tv_sec;
			}

			inline unsigned long zoneSec() const
			{
				if(_tm_ok == false){
					return _tv.tv_sec - adjustTime - timeZone;
				}
				return _tv.tv_sec - timeZone;
			}
			inline QWORD msec() const
			{
				return sec() * 1000L + _tv.tv_usec / 1000L;
			}

			inline QWORD usec() const
			{
				return sec() * 1000000L + _tv.tv_usec;
			}

			/// @brief 计时器延迟
			/// @param delay 延迟的时间(毫秒)
			inline void addDelay(const QWORD delay)
			{
				_tv.tv_usec += (delay * 1000L);
				if(_tm_ok == true){
					_tv.tv_sec += adjustTime;
				}
				_tv.tv_sec += (_tv.tv_usec / 1000000L);
				_tv.tv_usec %= 1000000L;
				_tm_ok = false;
			}

			inline bool operator > (const nTime &rt) const
			{
				if (msec() <= rt.msec())
					return false;
				else
					return true;
			}

			inline bool operator >= (const nTime &rt) const
			{
				if (msec() >= rt.msec())
					return true;
				else
					return false;
			}

			inline bool operator < (const nTime &rt) const
			{
				if ( this->msec() >= rt.msec())
					return false;
				else
					return true;
			}

			inline bool operator <= (const nTime &rt) const
			{
				if ( this->msec() > rt.msec())
					return false;
				else
					return true;
			}

			inline bool operator == (const nTime &rt) const
			{
				return (rt.msec() == msec());
			}

			/// @brief	计时器消逝的时间，单位毫秒
			/// @param	rt 当前时间，需要比较的时间
			/// @return	计时器消逝的时间，单位毫秒
			unsigned long elapse(const nTime &rt) const
			{
				if(rt.msec() < msec()){
					return 0;
				}
				return (rt.msec() - msec());
			}

			/// @brief	得到本地时间
			/// @param	tv1 需要输出的时间结构
			/// @param	timeValue 输入时间
			/// @return	
			static void getLocalTime(struct tm & tv1, time_t timValue)
			{
				timValue = timValue - adjustTime - timeZone;
				gmtime_r(&timValue, &tv1);
			}

		protected:
			/// @brief	真实时间(精确到微秒级)
			struct timeval _tv;
			/// @brief	标志_tm结构体数据是否有效
			bool _tm_ok;
	};

	class nDayTime : public nTime
	{
		public:
			nDayTime()
			{
			}
			nDayTime(const DWORD timeInSec) :
				nTime(timeInSec)
		{
		}

		public:
			/// @brief	获得dayDiff前/后的日期.
			/// @param	dayDiff 正数表示以后,负数表示之前
			int getMDays(SWORD dayDiff)
			{
				if(dayDiff == 0) {
					return getMDays();
				}
				checkTM();
				time_t time = _tv.tv_sec + dayDiff * (ONE_DAY/1000);
				struct tm tv;
				getLocalTime(tv, time);
				return tv.tm_mday;
			}

			/// @brief	检查_tm结构体是否有效
			void checkTM()
			{
				if(!_tm_ok) 
				{ 
					getLocalTime(_tm, _tv.tv_sec); 
					_tv.tv_sec -= adjustTime;
					_tm_ok = true; 
				}
			}

			/// @brief 	对时间进行格式化
			// 			按照指定的格式把时间格式化成字符串形式
			/// @param 	buffer 输出字符串缓冲
			/// @param 	bufferlen 输出字符串缓冲大小
			/// @param 	format 时间格式，请参见man strftime
			/// @return	
			DWORD format(char *buffer, size_t bufferlen, const char *format = "%C/%m/%d %T")
			{
				checkTM();
				return strftime(buffer, bufferlen, format, &_tm);
			}

			/// @brief	获取分钟内的秒数
			/// @param	
			/// @return	分钟内的秒数
			int getSeconds()
			{
				checkTM();
				return _tm.tm_sec;
			}

			int getMinutes()
			{
				checkTM();
				return _tm.tm_min;
			}

			int getHours()
			{
				checkTM();
				return _tm.tm_hour;
			}

			/// @brief	获取一月内的天数
			/// @param	
			/// @return	一月内的天数
			int getMDays()
			{
				checkTM();
				return _tm.tm_mday;
			}

			/// @brief	获取一年内的月份数
			/// @param	
			/// @return	一年内的月份数
			int getMonths()
			{
				checkTM();
				return _tm.tm_mon;
			}

			/// @brief	获取自1900后的年数
			/// @param	
			/// @return	自1900后的年数
			DWORD getYears()
			{
				checkTM();
				return _tm.tm_year + 1900;
			}

			/// @brief	获取一周内的天数
			/// @param	
			/// @return	一周内的天数
			int getWDays()
			{
				checkTM();
				return _tm.tm_wday;
			}

			/// @brief	获取一年内的天数
			/// @param	
			/// @return	一年内的天数
			int getYDays()
			{
				checkTM();
				return _tm.tm_yday;
			}
			WORD getDayMinutes()
			{
				return getHours() * 60 + getMinutes();
			}
			DWORD getDaySec()
			{
				return getHours() * 3600 + getMinutes() * 60 + getSeconds();
			}
			// 获取当天的开始时间,用于标识日期,可以用于时间差计算.不能使用取模,时区问题
			DWORD getDayStartInSec()
			{
				checkTM();
				return sec() - getDaySec();
			}
			// 获取当周的开始时间,用于标识周,可以用于时间差计算.不能使用取模,时区问题
			DWORD getWeekStartInSec()
			{
				checkTM();
				//return getDayStartInSec() - (_tm.tm_wday + 7)%8 * (24 * 3600);
				return getDayStartInSec() - _tm.tm_wday * (24 * 3600);
			}
			DWORD getWeekNum()
			{
				checkTM();
				return _tm.tm_wday;
			}
			DWORD getMonthStartInSec()
			{
				checkTM();
				return getDayStartInSec() - (_tm.tm_mday - 1) * (24 * 3600);
			}
			bool checkInOneDay(const DWORD timeInSec)
			{
				DWORD dayStartInSec = getDayStartInSec();
				return timeInSec >= dayStartInSec && timeInSec < (dayStartInSec + ONE_DAY_IN_SEC);
			}
			bool checkInOneDay(nDayTime &otherTime)
			{
				return checkInOneDay(otherTime.sec());
			}
			bool checkInOneWeek(nDayTime &otherTime)
			{
				return getWeekStartInSec() == otherTime.getWeekStartInSec();
			}
			bool checkInOneMonth(nDayTime &otherTime)
			{
				return getMonthStartInSec() == otherTime.getMonthStartInSec();
			}
			static DWORD getTimeInSec(const char *timeStr, const char *format = "%Y-%m-%d %H:%M:%S")
			{
				struct tm destTm;
				strptime(timeStr, format, &destTm);
				return mktime(&destTm);
			}
			static std::string getTimeInStr(DWORD timeInSec, const char *format = "%Y-%m-%d %H:%M:%S")
			{
				char timeBuffer[48];
				nDayTime(timeInSec).format(timeBuffer, sizeof(timeBuffer), format); 
				return timeBuffer;
			}
		private:
			/// @brief	存放日期数据
			struct tm _tm;
	};
};
#endif
