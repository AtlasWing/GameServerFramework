#include "base/nAssert.h"
#include "base/nBaseServer.h"
#include "base/nDebugger.h"

static const DWORD MAX_TRACE_TIMES_IN_ONE_SEC = 8;
void assertLog(const bool needCore, const char *funcName, const char *fileName, const int line, const char *expr, const char * desc, ...)
{
    // 限制一秒钟内执行backTrace的次数,避免连续的断言触发导致卡
    static DWORD lastBacktraceTimeInSec = Tidy::baseServer->time().sec();
    static DWORD backtraceTimesInOneSec = 0;
    if(lastBacktraceTimeInSec == Tidy::baseServer->time().sec()){
        ++ backtraceTimesInOneSec; 
    }
    else{
        lastBacktraceTimeInSec = Tidy::baseServer->time().sec();
        backtraceTimesInOneSec = 1;
    }
	std::string backTrace;
    if(backtraceTimesInOneSec < MAX_TRACE_TIMES_IN_ONE_SEC){
        backTrace = Tidy::nDebugger::backTrace();
    }
	Tidy::baseServer->fatal("ASSERT FAIL:expr=[%s] desc=[%s], FILE=[%s : %d] funcName=[%s] backTrace=%s", expr, desc, fileName, line, funcName, backTrace.c_str());

	char descBuffer[MSGBUF_MAX];
	getMessage(descBuffer, sizeof(descBuffer), desc);
	Tidy::baseServer->dealAssertFail(needCore, funcName, fileName, line, expr, descBuffer);
}
