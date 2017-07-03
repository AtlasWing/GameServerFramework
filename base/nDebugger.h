#ifndef _N_DEBUGGER_H_
#define _N_DEBUGGER_H_

#include "base/nSys.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	/// @brief	调试信息.如果需要打印堆栈,需要加上编译选项 -rdynamic
	class nDebugger
	{
		public:
			/// @brief	获得调用栈信息
			static std::string backTrace();
			/// @brief	获得调用栈信息
			static std::string& backTrace(std::string& backTraceResult, ucontext_t *uc = NULL);
	};

	/// @brief 局部计时器
	class nScopeTiming
	{
		public:
			/// @brief 构造函数开始计时
			nScopeTiming(const QWORD logUsec, std::string logTitle, std::string logContent):
				_log_usec(logUsec),
				_log_title(logTitle),
				_log_content(logContent)
		{
			gettimeofday(&_begin, NULL);
		}
			/// @brief 析构函数结束计时
			~nScopeTiming()
			{
				gettimeofday(&_end, NULL);
				unsigned long int usec = (_end.tv_sec - _begin.tv_sec) * 1000000 + (_end.tv_usec - _begin.tv_usec);
				if(usec >= _log_usec){
					baseServer->debug("[%s][%s] 执行时间 %lu 微秒", _log_title.c_str(), _log_content.c_str(), usec);
				}
			}
		private:
			struct timeval _begin, _end;
			QWORD _log_usec;			/// 超过多少微秒输出警告日志
			std::string _log_title;		/// 日志标题
			std::string _log_content;	/// 日志内容
	};

};
/// @brief	性能测量.
/// @param	times 执行次数
/// @param	name 名字表示。需要加上引号""
#define TEST_SPEED_BEGIN(times,name) \
	do{ \
		struct timespec _tv; \
		clock_gettime(CLOCK_REALTIME, &_tv); \
		long begin= _tv.tv_sec*1000L + _tv.tv_nsec/1000000L; \
		debug("%s { begin time=%ld",name,begin); \
		for(DWORD i = 0; i < times; ++i) \
		{

#define TEST_SPEED_END() \
		} \
		clock_gettime(CLOCK_REALTIME, &_tv); \
		long end= _tv.tv_sec*1000L + _tv.tv_nsec/1000000L; \
		debug("         process time=%ld", end-begin); \
		debug("         end time=%ld }", end); \
	}while(0);

#endif

/// @brief	执行时间测试
/// @param	log 日志关键字
#ifdef _DEBUG
#define TEST_TIME_INIT() \
	struct timeval begin, end;\
unsigned long int usec;
#define TEST_TIME_BEGIN() \
	gettimeofday(&begin, NULL);
#define TEST_TIME_END(logFunc, logStr) \
	gettimeofday(&end, NULL);\
usec = (end.tv_sec - begin.tv_sec) * 1000000 + (end.tv_usec - begin.tv_usec);\
logFunc("[%s] 执行时间 %lu 微秒", logStr, usec);
#else
#define TEST_TIME_INIT()
#define TEST_TIME_BEGIN()
#define TEST_TIME_END(logFunc, logStr)
#endif

/// @brief 统计sql执行时间，单位微秒
#define SQL_EXECUTE_CHECKER(log) \
	Tidy::nScopeTiming timer(50000, "sql执行时间", log);

#define START_ANALYZE() {\
static QWORD ANALYZE_START_TIME_MSEC = 0;\
if(Tidy::baseServer->checkNeedAnalyze() == true){\
        Tidy::nTime time;\
        ANALYZE_START_TIME_MSEC = time.msec();\
    }

#define END_ANALYZE(exp, ...) \
    if(Tidy::baseServer->checkNeedAnalyze() == true) {\
        char logBuffer[MSGBUF_MAX];\
        Tidy::nTime time;\
        const QWORD endTimeInMsec = time.msec();\
        DWORD len = snprintf(logBuffer, sizeof(logBuffer) - 1, "ANALYZE [%llu] [%s,%s,%u]", (endTimeInMsec - ANALYZE_START_TIME_MSEC), __PRETTY_FUNCTION__, __FILE__, __LINE__);\
        len += snprintf(logBuffer + len, sizeof(logBuffer) - 1 - len, exp, ##__VA_ARGS__);\
        Tidy::baseServer->debug(logBuffer);\
        }\
}
