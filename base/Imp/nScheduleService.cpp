#include "base/nScheduleService.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	nSchedule::nSchedule(const SWORD month, const SWORD monthDay, const SWORD weekDay, const BYTE hour, const BYTE minute, const BYTE sec):
		_id(0),
		_time(month, monthDay, weekDay, hour, minute, sec)
	{
	}

	nSchedule::~nSchedule()
	{
	}

	void nSchedule::setSequenceID(const DWORD id)
	{
		this->_id = id;
	}

	const DWORD nSchedule::getSequenceID() const
	{
		return _id;
	}

	bool nSchedule::checkSchedule(nDayTime &lastCheckTime, nDayTime &nowTime)
	{
		if(_time.checkPast(lastCheckTime, nowTime) == true){
			return fireEvent();
		}
		return true;
	}

	bool nSchedule::fireEvent()
	{
		baseServer->debug("fireEvent");
		return true;
	}

	namespace{
		class CheckScheduleExec{
			public:
				CheckScheduleExec(): _is_first_check(true){
				}
				~CheckScheduleExec(){
				}
				void startNewCheck(){
					_now_time.now();
					if(_is_first_check == true){
						_last_check_time = _now_time;
						_is_first_check = false;
					}
				}
				void finishCheck(){
					_last_check_time = _now_time;
					_recycle_list.clear();
					_last_check_time.checkTM();
				}
				bool exec(nSchedule *schedule){
					if(schedule->checkSchedule(_last_check_time, _now_time) == false){
						_recycle_list.push_back(schedule);
					}
					return true;
				}

			public:
				Tidy::list<nSchedule*> _recycle_list;

			private:
				nDayTime _last_check_time;
				nDayTime _now_time;
				bool _is_first_check;
		}checkScheduleExec;
	};

	nScheduleService::nScheduleService()
	{
	}

	nScheduleService::~nScheduleService()
	{
	}

	bool nScheduleService::initService()
	{
		return true;
	}

	void nScheduleService::finalService()
	{
	}

	bool nScheduleService::doTick(nDayTime &time)
	{
		checkScheduleExec.startNewCheck();
		execAll(checkScheduleExec);
		for(Tidy::list<nSchedule*>::iterator iter = checkScheduleExec._recycle_list.begin(); iter != checkScheduleExec._recycle_list.end(); ++ iter){
			deleteSchedule(*iter);
		}
		checkScheduleExec.finishCheck();

		return true;
	}

	bool nScheduleService::addSchedule(nSchedule *schedule)
	{
		return add(schedule);
	}

	void nScheduleService::deleteSchedule(const nSchedule *schedule)
	{
		remove(schedule);
		SAFE_DELETE(schedule);
	}
};

