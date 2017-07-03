#ifndef _N_TIMER_MANAGER_H_
#define _N_TIMER_MANAGER_H_

#include "base/nSig.h"
#include "base/nTimer.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	enum eTimerIntervalType
	{
		eTimerIntervalType_Tick,
		eTimerIntervalType_TwoHundredMsec,
		eTimerIntervalType_FiveHundredMsec,
		eTimerIntervalType_OneSec,
		eTimerIntervalType_TenSec,
		eTimerIntervalType_OneMin,
		eTimerIntervalType_OneHour,
		eTimerIntervalType_Max,
	};

	extern DWORD timerInterval[eTimerIntervalType_Max]; 

	struct TimerSignalData
	{
		typedef Tidy::signal<void, bool> TimerSignalType;
		TimerSignalData(const DWORD timerIntervalInMsec) : timer(timerIntervalInMsec)
		{
		}
		nTimer timer;
		TimerSignalType signal;
	};

	typedef Tidy::slot0<bool>  TimerSlotType;
	class nTimerManager
	{
		public:
			nTimerManager();
			~nTimerManager();

		public:
			bool doTick();
			bool registerTimeEvent(const DWORD timerInMsec, const TimerSlotType& slot, const bool needDisconnect);

		private:
			const eTimerIntervalType getTimerIntervalType(const DWORD intervalInMsec) const;

		private:
			TimerSignalData* _timer_signal_list[eTimerIntervalType_Max];
	};
};
#endif
