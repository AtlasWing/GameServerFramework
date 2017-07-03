#include "base/nAnalysis.h"
#include "base/nMisc.h"
#include "base/Tidy.h"
#include "base/nLogger.h"

#include "nSocket.h"

namespace Tidy
{
	void FunctionInterval::interval(const char *func)
	{
		clock_gettime(CLOCK_REALTIME, &_tv_2);
		long end=_tv_2.tv_sec*1000000L + _tv_2.tv_nsec/1000L;
		long begin= _tv_1.tv_sec*1000000L + _tv_1.tv_nsec/1000L;
		if(end - begin > _need_log)
		{
			baseServer->debug("[%s]执行时间间隔[%ld]us",func,end - begin);
		}
		_tv_1=_tv_2;
	}
	FunctionTime::~FunctionTime()
	{
		clock_gettime(CLOCK_REALTIME, &_tv_2);
		long end=_tv_2.tv_sec*1000000L + _tv_2.tv_nsec/1000L;
		long begin= _tv_1.tv_sec*1000000L + _tv_1.tv_nsec/1000L;
		if(end - begin > _need_log)
		{
			baseServer->debug("[%s]执行时间[%ld]us,描述:[%s]",_fun_name,end - begin , _dis);
		}
	}
	FunctionTimes::Times FunctionTimes::_times[256]; 
	FunctionTimes::FunctionTimes(const int which , const char *dis)
	{
		_which = which;
		_times[_which]._mutex.lock(); 
		if(!_times[_which]._times)
		{
			copyStr(_times[_which]._dis,dis,sizeof(_times[_which]._dis));
		}
		clock_gettime(CLOCK_REALTIME, &_tv_1);
	}
	FunctionTimes::~FunctionTimes()
	{
		clock_gettime(CLOCK_REALTIME, &_tv_2);
		long end=_tv_2.tv_sec*1000000L + _tv_2.tv_nsec/1000L;
		long begin= _tv_1.tv_sec*1000000L + _tv_1.tv_nsec/1000L;
		_times[_which]._times++;
		_times[_which]._total_time += (end - begin);
		nTime ct;
		if(_times[_which]._log_timer(ct))
		{
			baseServer->debug("执行次数[(%d):%d],执行总时间:[%d]us,说明:[%s]",_which,_times[_which]._times,_times[_which]._total_time ,_times[_which]._dis);
			_times[_which]._times=0;
			_times[_which]._total_time=0;
		}
		_times[_which]._mutex.unlock(); 
	}
};

