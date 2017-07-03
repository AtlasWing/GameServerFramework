#include "base/nBaseTickService.h"

namespace Tidy{
    nBaseTickService::nBaseTickService() : 
        nThread("TimeTick"),
        _half_sec(500L),
        _one_sec(1*1000L),
        _ten_sec(10*1000L),
        _one_min(60*1000L),
        _five_min(5*60*1000L),
        _ten_min(10*60*1000L),
        _one_hour(1000*60*60),
        script_ten_min(10*60*1000L),
        script_one_hour(1000*60*60),
        _twentyFourHour_hour(1000*60*60*24),
        _zero_clocker(24 * 3600, 24 * 3600),
        _one_hour_clocker(1 * 3600, 1 * 3600)
    {
    }

    nBaseTickService::~nBaseTickService() 
    {
    }

};
