#include "base/nTimerManager.h"

namespace Tidy
{
	DWORD timerInterval[eTimerIntervalType_Max] = {0, 200, 500, ONE_SEC, TEN_SEC, ONE_MIN, ONE_HOUR};

	namespace
	{
		bool test1()
		{
			baseServer->debug("测试1");
			return false;
		}
		bool test2()
		{
			baseServer->debug("测试2");
			return false;
		}
		bool test3()
		{
			baseServer->debug("测试3");
			return false;
		}
	};

	nTimerManager::nTimerManager()
	{
		for(DWORD i = 0; i < eTimerIntervalType_Max; ++ i)
		{
			_timer_signal_list[i] = NULL;
		}
#ifdef _DEBUG_NYS
		DWORD time = 1002;
		this->registerTimeEvent(time, Tidy::ptr_fun0(&test1), true);
		time = 10000;
		this->registerTimeEvent(time, Tidy::ptr_fun0(&test2), true);
		time = 100000;
		this->registerTimeEvent(time, Tidy::ptr_fun0(&test3), true);
		doTick();
#endif
	}

	nTimerManager::~nTimerManager()
	{
		for(DWORD i = 0; i < eTimerIntervalType_Max; ++ i)
		{
			if(_timer_signal_list[i] != NULL)
			{
				_timer_signal_list[i]->signal.emit(true);
				SAFE_DELETE(_timer_signal_list[i]);
			}
		}
	}

	bool nTimerManager::doTick()
	{
		for(DWORD i = 0; i < eTimerIntervalType_Max; ++ i)
		{
			if(_timer_signal_list[i] != NULL && _timer_signal_list[i]->timer(baseServer->time()) == true)
			{
				_timer_signal_list[i]->signal.emit(false);
			}
		}
		return true;
	}

	const eTimerIntervalType nTimerManager::getTimerIntervalType(const DWORD intervalInMsec) const
	{
		switch(intervalInMsec)
		{
			case 0 ... (200-1):
				{
					return eTimerIntervalType_Tick;
				}
				break;
			case 200 ... (1000-1):
				{
					return eTimerIntervalType_TwoHundredMsec;
				}
				break;
			case 1000 ... (10000-1):
				{
					return eTimerIntervalType_OneSec;
				}
				break;
			case 10000 ... (60000-1):
				{
					return eTimerIntervalType_TenSec;
				}
				break;
			case 60000 ... (3600000-1):
				{
					return eTimerIntervalType_OneMin;
				}
				break;
			default:
				{
					return eTimerIntervalType_OneHour;
				}
		}

		return eTimerIntervalType_OneHour;
	}

	namespace
	{
		void timeEventCall(bool isFinal, Tidy::connection *connect, QWORD& nextFireTimeInMsec, DWORD timerInMsec, TimerSlotType& slot)
		{
			if(isFinal == true)
			{
				if(connect != NULL)
				{
					SAFE_DELETE(connect);
				}
				return;
			}

			if(nextFireTimeInMsec > 0 && nextFireTimeInMsec > baseServer->time().msec())
			{
				return;
			}
			if(slot() == false && connect != NULL)
			{
				connect->disconnect();
				SAFE_DELETE(connect);
			}
			else if(nextFireTimeInMsec > 0)
			{
				nextFireTimeInMsec += timerInMsec;
				if(nextFireTimeInMsec <= baseServer->time().msec())
				{
					nextFireTimeInMsec = baseServer->time().msec() + timerInMsec;
				}
			}
		}
	};

	bool nTimerManager::registerTimeEvent(const DWORD timerInMsec, const TimerSlotType& slot, const bool needDisconnect)
	{
		const eTimerIntervalType intervalType = getTimerIntervalType(timerInMsec);
		if(_timer_signal_list[intervalType] == NULL)
		{
			_timer_signal_list[intervalType] = new TimerSignalData(timerInterval[intervalType]);
		}

		QWORD nextFireTime = 0;
		//如果符合timerInterval[intervalType]就不需要使用nextFireTime了
		if(timerInMsec > timerInterval[intervalType])
		{
			nextFireTime = baseServer->time().msec() + timerInMsec;
		}
		Tidy::connection *connect = NULL;
		if(needDisconnect == true)
		{
			connect = new Tidy::connection();
			(*connect) = _timer_signal_list[intervalType]->signal.connect(Tidy::bind(&timeEventCall, connect, nextFireTime, timerInMsec, slot));
		}
		else
		{
			_timer_signal_list[intervalType]->signal.connect(Tidy::bind(&timeEventCall, connect, nextFireTime, timerInMsec, slot));
		}

		return true;
	}
};
