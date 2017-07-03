#ifndef _N_LOGGER_H_
#define _N_LOGGER_H_

#include "base/nTime.h"
#include "base/nMutex.h"
#include "base/nSys.h"

namespace Tidy
{
	/// @brief	单条日志的最大长度
#define MSGBUF_MAX 10240

	//vsnprintf 返回值: "a return value of size or more means that the output was truncated"
#define getMessage(msg,msglen,pat)  \
	DWORD msgLen; \
	do  \
	{   \
		(msg)[msglen-1] = '\0';\
		va_list ap; \
		va_start(ap, pat);      \
		msgLen = vsnprintf(msg, msglen - 1, pat, ap);    \
		if(msgLen >= (msglen - 1)){\
			msgLen = msglen - 1;\
		}\
		va_end(ap); \
	}while(false)

	/// @brief	日志级别
	enum eLoggerLevel {
		eLoggerLevel_Debug   =   0,		///< 调试级别 
		eLoggerLevel_Trace   =   1,		///< 跟踪级别 
		eLoggerLevel_Info    =   2,		///< 信息输出级别 
		eLoggerLevel_Warn    =   3,		///< 警告级别 
		eLoggerLevel_Error   =   4,		///< 错误级别 
		eLoggerLevel_Fatal   =   5,		///< 致命错误级别 
		eLoggerLevel_Max
	};

	extern const char* LoggerLevelStr[];

	class nLogger 
	{
		public:
			/// @brief	构造函数
			/// @param	name 名字,出现在日志前面
			/// @return	
			nLogger(const char *name, const bool needLock);
			/// @brief	析构函数
			virtual ~nLogger();

		public:
			/// @brief	debug日志输出
			/// @param	pattern  输入串
			/// @return	
			void debug(const char * pattern, ...) __attribute__((format(printf,2,3)));
			/// @brief	trace日志输出
			/// @param	pattern  输入串
			/// @return	
			void trace(const char * pattern, ...) __attribute__((format(printf,2,3)));
			/// @brief	info日志输出
			/// @param	pattern  输入串
			/// @return	
			void info(const char * pattern, ...) __attribute__((format(printf,2,3)));
			/// @brief	warn日志输出
			/// @param	pattern  输入串
			/// @return	
			void warn(const char * pattern, ...) __attribute__((format(printf,2,3)));
			/// @brief	error日志输出
			/// @param	pattern  输入串
			/// @return	
			void error(const char * pattern, ...) __attribute__((format(printf,2,3)));
			/// @brief	fatal日志输出
			/// @param	pattern  输入串
			/// @return	
			void fatal(const char * pattern, ...) __attribute__((format(printf,2,3)));
			bool write(const char * pattern, ...) __attribute__((format(printf,2,3)));

		public:
			/// @brief	获得日志名
			/// @param	
			/// @return	
			std::string getName(void){return _log_name;};
			/// @brief	设置日志中关键字名字
			/// @param	n 设置日志中关键字名字(如服务器名字)
			/// @return	
			void setName(const std::string& n){_log_name = n;};
			/// @brief	不向终端打印日志
			void clearNeedConsole(){_need_console = false;};
			/// @brief	不向文件打印日志
			void clearNeedLogFile(){_need_logfile = false;};
			void setNeedSysLog();
			void clearNeedSysLog();
			/// @brief	设置输出日志文件名
			/// @param	name 文件名
			/// @return	
			bool initLogFile(const std::string& fileNamePrefix, const DWORD splitTimeInMin);
			/// @brief	设置日志输出级别
			/// @param	level 要设置的级别
			/// @return	
			void setLogLevel(const char *level);
			void setAsyncWrite();
			void clearAsyncWrite();
			bool checkAsyncWrite() const;
			void flushToFile(bool needFlushLock);

		public:
			bool checkLoggerLevel(const eLoggerLevel level) const;
			bool writeLog(const char* msg, const DWORD len);
			const DWORD getContentHeader(char* headerBuffer, const DWORD bufferSize, const DWORD loggerLevel);

		private:
			/// @brief	把日志写到本地
			/// @param	msg 输入内容
			/// @return	
			bool writeLogToFile(const char* msg, const DWORD len);
			/// @brief	把日志写到终端
			/// @param	msg 输入内容
			/// @return	
			void writeLogToConsole(const char* msg);
			void writeLogToSysLog(const char* msg);
			bool checkNeedLock() const;

		private:
			virtual void dealChangeFile();
			virtual bool getFileName(nDayTime &time, const std::string &prefixName, std::string *out);

		private:
			/// @brief	日志文件名字
			std::string _file_name_prefix;
			/// @brief	日志句柄的名字
			std::string _log_name;
			/// @brief	写日志文件流
			std::ofstream* _ofile;
			/// @brief	互斥锁
			nMutex _mutex;
			/// @brief	写日志的时间,因为这个需要精确时间
			// 			每行日志都是精确时间
			nDayTime _time;
			/// @brief	上一行日志的输出时间,用来判断是否需要切换文件
			DWORD _last_log_time_in_sec;
			/// @brief	分割日志文件的时间段
			DWORD _split_time_in_min;
			/// @brief  缓存日志时间,一秒内只格式化一次
			char  _last_log_time_str[128];
			/// @brief	是否打印到终端
			static bool _need_console;
			/// @brief	是否打印到文件
			static bool _need_logfile;
			/// @brief	是否输出到syslog
			static bool _need_sys_log;
			// 从_level映射到syslog的level
			static const int syslogLevelMap[];
			/// @brief	设置日志输出级别eLoggerLevel
			eLoggerLevel _level;
			bool _is_need_lock;
			bool _is_async_write;
			Tidy::nMutex _flush_lock;
			BYTE _write_buffer_index;
			std::string _write_buffer_list[2];
	};
};

#endif
