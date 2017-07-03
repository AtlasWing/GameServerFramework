#ifndef _N_TIMER_H_
#define _N_TIMER_H_

#include "base/nTime.h"
#include "base/nRandom.h"
#include "base/nBaseServer.h"

namespace Tidy{

/// @brief	定时器
// 			固定时间间隔的定时器，方便对于时间间隔的判断，精确到毫秒级
class nTimer
{
	public:
		nTimer() :
            _long(0), 
            _msec(0)
		{
		}
		/// @brief	构造函数
		/// @param	how_long	定时器的时间，单位：毫秒
		/// @param	first		有些定时器可能希望在启动时就可以执行一次,所以不能直接addDelay
		explicit nTimer(const long how_long, bool first = false, const long delay = 0) 
			: _long(how_long)
		{
			nTime timeNow;
			_msec = timeNow.msec();
			if(!first)
			{
				addDelay(_long+delay);
			}
		}

		/// @brief	构造函数
		/// @param	how_long	定时器的时间，单位：毫秒
		/// @param	first		有些定时器可能希望在启动时就可以执行一次,所以不能直接addDelay哦
		/// @param	ctv		当前时间
		explicit nTimer(const long how_long, bool first , nTime &ctv) 
			: _long(how_long), _msec(ctv.msec())
		{
			if(!first)
			{
				addDelay(_long);
			}
		}

		void reset(const unsigned long how_long, const nTime &cur)
		{
			_long = how_long;
			_msec = cur.msec();
			addDelay(_long);
		}

		/// @brief	重新设置定时器的时间
		/// @param	cur		指定定时器启动的时间
		void current(const nTime &cur)
		{
			_msec = cur.msec();
		}

		/// @brief	延时定时器时间
		/// @param	cur	指定定时器启动的时间
		/// @param	delay	延时时间
		void next(const nTime &cur, const DWORD delay)
		{
			_msec = cur.msec();
			addDelay(delay);
		}

		/// @brief	重新设置定时器的时间
		/// @param	cur		指定定时器启动的时间
		void next(const nTime &cur)
		{
			_msec = cur.msec();
			addDelay(_long);
		} 

		/// @brief	倒计时剩余秒数.不受时间调整影响.
		/// @param	cur	当前时间	
		/// @return	剩余描述
		inline DWORD leftSec(const nTime &cur)
		{
			return leftMSec(cur) / 1000;
		} 

		/// @brief	倒计时剩余毫秒数.受时间调整影响
		/// @param	cur	当前时间	
		/// @return	剩余值
		inline unsigned long leftMSec(const nTime &cur)
		{
            assert_debug(_msec > 0, "");
			return (_msec > cur.msec()) ? (_msec - cur.msec()) : 0; 
		}

		inline bool operator() (const nTime &cur)
		{
			return isReady(cur);
		}

		/// @brief	定时器检查
		/// @param	cur	检查定时器的时间
		/// @return	是否到达指定时间
		inline bool isReady(const nTime &cur)
		{
			assert_debug(_msec != 0, "");
			if (_msec > cur.msec())
			{
				return false;
			}

			if(cur.msec() >= (_msec + _long))
			{
				_msec = cur.msec();
			}
			_msec += _long;
			return true;
		}

		/// @brief	空定时器
		inline bool empty() const
		{
			return _long == 0 ? true : false;
		}
        void clear()
        {
            _long = 0;
            _msec = 0;
        }
        /// @brief  取得间隔时间
        inline unsigned long intervalMsec() const
        {
            return _long;
        }

	private:
		/// @brief	定时器时间间隔
		unsigned long _long;
		/// @brief	上次检查定时器的时间
		unsigned long _msec;

	private:
		inline void addDelay(const unsigned long delay)
		{
			assert_debug(_msec != 0, "");
			_msec += delay;
		}
};

/// @brief  随机定时器	
class nRandTimer
{
	public:
		/// @brief 随机下次定时器的时间
		/// @param _long 时间间隔
#define next_time(_long) (_long / 2 + Tidy::randBetween(0, _long))

		/// @brief 构造函数
		/// @param how_long	定时器的时间，单位：毫秒
		/// @param first		有些定时器可能希望在启动时就可以执行一次,所以不能直接addDelay哦
		explicit nRandTimer(const long how_long,bool first=false) 
			: _long(how_long)
		{
			nTime timeNow;
			_msec = timeNow.msec();
			if(!first)
			{
				addDelay(next_time(_long));
			}
		}

		/// @brief 构造函数
		/// @param how_long	定时器的时间，单位：毫秒
		/// @param first		有些定时器可能希望在启动时就可以执行一次,所以不能直接addDelay哦
		/// @param ctv		当前时间
		explicit nRandTimer(const long how_long,bool first, nTime &ctv) 
			: _long(how_long), _msec(ctv.msec())
		{
			if(!first)
			{
				addDelay(next_time(_long));
			}
		}

		/// @brief 重新设置定时器的时间
		/// @param cur		指定定时器启动的时间
		void next(const nTime &cur)
		{
			_msec = cur.msec();
			addDelay(_long);
		}

		/// @brief 定时器检查
		/// @param cur	检查定时器的时间
		/// @return 是否到达指定时间
		inline bool operator() (const nTime &cur)
		{
			if (_msec > cur.msec()) 
			{
				return false;
			}
			_msec = cur.msec();
			addDelay(next_time(_long));
			return true;
		}

	private:
		inline void addDelay(const unsigned long delay)
		{
			assert_debug(_msec != 0, "");
			_msec += delay;
		}

	private:
		/// @brief 定时器时间间隔
		const unsigned long _long;
		/// @brief 上次检查定时器的时间
		unsigned long _msec;
};

/// @brief 	闹钟 在指定的时间点做指定的事情,以当前时区的0点作为起始点
class nClocker
{
	public:
		/// @brief	构造函数
		// 			要求interval的时间要大于how_long的时间
		/// @param	how_long	间隔,单位:秒
		/// @param	interval	周期,单位:秒,比如以天为间隔就是24*3600,以周为单位就是7*24*3600
		explicit nClocker(const long how_long,const long interval)
			:_long(how_long),_interval(interval),adjustTime(nTime::adjustTime),_clock()
		{
			assert_debug(_long > 0 && _interval >= _long, "");
			delay();
		}

		/// @brief	构造函数
		// 			要求interval的时间要大于how_long的时间
		/// @param	how_long	间隔,单位:秒
		/// @param	interval	周期,单位:秒,比如以天为间隔就是24*3600,以周为单位就是7*24*3600
		/// @param	ctv	当前时间
		explicit nClocker(const long how_long,const long interval,nTime &ctv)
			:_long(how_long),_interval(interval),adjustTime(nTime::adjustTime),_clock(ctv)
		{
			assert(_long);
			assert(_interval >= _long);
			delay();
		}

		explicit nClocker(const long how_long, const long interval, const DWORD timeInSec)
			:_long(how_long), _interval(interval), adjustTime(nTime::adjustTime), _clock(timeInSec)
		{
			assert(_long);
			assert(_interval >= _long);
			delay();
		}

		/// @brief	重新设置闹钟的时间
		/// @param	cur		指定闹钟启动的时间
		void next(const nTime &cur)
		{
			_clock = cur;
			delay();
		} 

		/// @brief	检查当前时间离下一个时间点的剩余时间,单位秒
		/// @param	cur 当前时间
		/// @return	时间差
		inline unsigned long leftSec(const nTime &cur)
		{
			long val = _clock.sec() - cur.sec();
			if (val < 0L)
				return 0L;
			else
				return val;
		}

		/// @brief	闹钟器检查
		/// @param	cur	检查闹钟的时间
		/// @return	是否到达下一个闹钟时间
		inline bool operator() (const nTime &cur)
		{
			if(checkReady(cur) == false) 
			{
                return false;
            }
            adjustTime = nTime::adjustTime;
            _clock = cur;
            delay();
            return true;
        }

		/// @brief	检查闹钟器连续在cur当前时间前触发次数
		/// @param	cur	检查闹钟的时间
		/// @return	触发次数
        inline DWORD getContinueRingTimes(const nTime &cur)
        {
            DWORD times = 0;
            while(checkReady(cur) == true)
            {
                ++ times;
                delay();
            }
            return times;
        }

	private:
		/// @brief	设置下次执行的时间
		void delay()
		{
            // 使用的是时区相关的
			unsigned long escape = _clock.zoneSec()%_interval;
			if(escape < _long)
			{
				_clock.addDelay((_long - escape) * 1000);
			}
			else
			{
				_clock.addDelay((_interval + _long - escape) * 1000);
			}
		}

        const bool checkReady(const nTime& cur) const
        {
			return ((_clock._tv.tv_sec + nTime::adjustTime - adjustTime) <= cur._tv.tv_sec); 
        }

    private:
		/// @brief	闹钟的时间间隔,单位是秒
		const unsigned long _long;
		/// @brief	闹钟的时间周期,单位是秒
		const unsigned long _interval;
		/// @brief	调整系统相对时间
		long adjustTime;
		/// @brief	上次检查闹钟的时间
		nTime _clock;

};

};
#endif
