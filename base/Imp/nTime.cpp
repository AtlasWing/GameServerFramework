#include "base/nTime.h"
#include "base/Tidy.h"

namespace Tidy
{
	long nTime::timeZone = 0L;
	long nTime::adjustTime = 0L;

	bool nTime::setTimeZone()
	{
		if(0L == timeZone)
		{
			tzset();
			timeZone = timezone;
			return true;
		}
		return false;
	}

	void nTime::addAdjustTime(const SDWORD offset)
	{
		adjustTime -= offset;
	}

	void nTime::setAdjustTime(const SDWORD offset)
	{
		adjustTime = offset;
	}
};
