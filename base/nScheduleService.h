#ifndef BASE_NSCHEDULESERVICE_H_
#define BASE_NSCHEDULESERVICE_H_

#include "base/nManager.h"
#include "base/nScheduleTimer.h"

namespace Tidy
{
	class nSchedule
	{
		public:
			nSchedule(const SWORD month, const SWORD monthDay, const SWORD weekDay, const BYTE hour, const BYTE minute, const BYTE sec);
			virtual ~nSchedule();

		public:
			void setSequenceID(const DWORD id);
			const DWORD getSequenceID() const;

		public:
			bool checkSchedule(nDayTime &lastCheckTime, nDayTime &nowTime);

		public:
			virtual bool fireEvent() = 0;

		private:
			DWORD _id;
			nScheduleTime _time;
	};

	class nScheduleService : public nManager<true, nIndexSequenceID<CommonVisitSequenceIDPolicy<nSchedule> > >
	{
		public:
			nScheduleService();
			~nScheduleService();

		public:
			bool initService();
			void finalService();
			bool doTick(nDayTime &time);

        public:
            void deleteSchedule(const nSchedule *schedule);

		protected:
			bool addSchedule(nSchedule *schedule);
	};
};

#endif
