#include "base/nLogger.h"
#include "base/nStringTool.h"
#include "base/nAssert.h"
#include "base/nFile.h"
#include "base/nBaseServer.h"
#include "base/nBaseMonitorService.h"

namespace Tidy
{
	const char* LoggerLevelStr[] = {"DEBUG", "TRACE", "INFO", "WARN", "ERROR", "FATAL"};
	const int nLogger::syslogLevelMap[] = { LOG_INFO, LOG_INFO, LOG_NOTICE, LOG_WARNING, LOG_ERR, LOG_ALERT};

	bool nLogger::_need_logfile = true;
	bool nLogger::_need_console = true;
	bool nLogger::_need_sys_log = false;

	nLogger::nLogger(const char *name, const bool needLock):
		_log_name(name),
		_ofile(NULL),
		_last_log_time_in_sec(0),
		_split_time_in_min(60),
        _level(eLoggerLevel_Debug),
		_is_need_lock(needLock),
		_is_async_write(false),
		_write_buffer_index(0)
	{
		_last_log_time_str[0] = '\0';
		for(DWORD i = 0; i < (sizeof(_write_buffer_list)/sizeof(_write_buffer_list[0])); ++ i){
			_write_buffer_list[i].reserve(MSGBUF_MAX);
		}
		if(baseServer->baseMonitorService != NULL){
			baseServer->baseMonitorService->addAsyncLogger(this);
		}
	}

	nLogger::~nLogger()
	{
		if(_is_async_write == true && baseServer->baseMonitorService != NULL){
			baseServer->baseMonitorService->removeAsyncLogger(this);
		}
		flushToFile(false);
		SAFE_DELETE(_ofile);
	} 

	bool nLogger::writeLog(const char* msg, const DWORD len)
	{
		if(_need_console){
			writeLogToConsole(msg);
		}
		if(_need_logfile){
			writeLogToFile(msg, len);
		}
		if(_need_sys_log){
			writeLogToSysLog(msg);
		}
		return true;
	}

	void nLogger::writeLogToConsole(const char* msg)
	{
		printf(msg);
		printf("\n");
	}

	bool nLogger::writeLogToFile(const char* msg, const DWORD len)
	{
		DWORD sec = _time.zoneSec();
		if(!_ofile || (sec /(_split_time_in_min*60)) != (_last_log_time_in_sec/(_split_time_in_min*60))) {
			nMutex_scope_lock flushLock(_flush_lock);
			//double check
			if(!_ofile || (sec /(_split_time_in_min*60)) != (_last_log_time_in_sec/(_split_time_in_min*60))) {
				_last_log_time_in_sec = sec;
				if(_ofile != NULL) { 
					flushToFile(false);
					SAFE_DELETE(_ofile); 
				}
				else {
					if(_file_name_prefix.empty()) {
						return false;
					}
				}
				_ofile = new std::ofstream;

				std::string fileName;
				if(getFileName(_time, _file_name_prefix, &fileName) == false){
					assert_fail("获取日志名字失败[%s]", fileName.c_str());
					return false;
				}
				bool isExist = nFile::exists(fileName);
				_ofile->open(fileName.c_str(),std::ios::out|std::ios::app);
				if(_ofile->fail() == true) {
					printf("打开日志文件错误%s\n", fileName.c_str());
					SAFE_DELETE(_ofile);
					return false; 
				}
				std::ostringstream oss;
				oss.str("");
				oss<<"ln --force -s "<< fileName <<" "<<_file_name_prefix;
				system(oss.str().c_str());
				if(isExist == false){
					dealChangeFile();
				}
			}
		}
		if(checkNeedLock() == true){
			_mutex.lock();
		}
		if(msg[0] != '\0'){
			if(_is_async_write == true){
				// 异步写
				std::string &writeBuffer = _write_buffer_list[_write_buffer_index];
				writeBuffer.append(msg, len);
				writeBuffer.push_back('\n');
			}
			else{
				_ofile->write(msg, len);
				(*_ofile) << std::endl;
			}
		}
		_last_log_time_in_sec = sec;
		if(checkNeedLock() == true){
			_mutex.unlock();
		}
		return true;
	}

	void nLogger::writeLogToSysLog(const char* msg)
	{
		syslog(syslogLevelMap[_level], "%s", msg);
	}

	void nLogger::setLogLevel(const char *level)
	{
		if(strcmp(level,"debug") == 0) {
			_level = eLoggerLevel_Debug;
		}
		else if(strcmp(level,"trace") == 0) {
			_level = eLoggerLevel_Trace;
		}
		else if(strcmp(level,"info") == 0) {
			_level = eLoggerLevel_Info;
		}
		else if(strcmp(level,"warn") == 0) {
			_level = eLoggerLevel_Warn;
		}
		else if(strcmp(level,"error") == 0) {
			_level = eLoggerLevel_Error;
		}
		else if(strcmp(level,"fatal") == 0) {
			_level = eLoggerLevel_Fatal;
		}
	}

	bool nLogger::checkLoggerLevel(const eLoggerLevel level) const
	{
		return (level >= _level);
	}

	bool nLogger::initLogFile(const std::string& fileNamePrefix, const DWORD splitTimeInMin)
	{ 
		_file_name_prefix = fileNamePrefix; 
		_split_time_in_min = splitTimeInMin;
		// 刷新文件
		return writeLogToFile("", 0);
	}

	void nLogger::debug(const char *pattern, ...)
	{
		if(checkLoggerLevel(eLoggerLevel_Debug) == false) {
			return;
		}
		char buffer[MSGBUF_MAX];
		DWORD len = getContentHeader(&buffer[0], sizeof(buffer), eLoggerLevel_Debug);
		FormatString(&buffer[len], sizeof(buffer) - len, pattern, len);
		this->writeLog(buffer, len);
	}

	void nLogger::warn(const char *pattern, ...)
	{
		if(checkLoggerLevel(eLoggerLevel_Warn) == false) {
			return;
		}
		char buffer[MSGBUF_MAX];
		DWORD len = getContentHeader(&buffer[0], sizeof(buffer), eLoggerLevel_Warn);
		FormatString(&buffer[len], sizeof(buffer) - len, pattern, len);
		this->writeLog(buffer, len);
	}

	void nLogger::info(const char *pattern, ...)
	{
		if(checkLoggerLevel(eLoggerLevel_Info) == false) {
			return;
		}
		char buffer[MSGBUF_MAX];
		DWORD len = getContentHeader(&buffer[0], sizeof(buffer), eLoggerLevel_Info);
		FormatString(&buffer[len], sizeof(buffer) - len, pattern, len);
		this->writeLog(buffer, len);
	}

	void nLogger::fatal(const char *pattern, ...)
	{
		if(checkLoggerLevel(eLoggerLevel_Fatal) == false) {
			return;
		}
		char buffer[MSGBUF_MAX];
		DWORD len = getContentHeader(&buffer[len], sizeof(buffer) - len, eLoggerLevel_Fatal);
		FormatString(&buffer[len], sizeof(buffer) - len, pattern, len);
		this->writeLog(buffer, len);
	}

	void nLogger::error(const char *pattern, ...)
	{
		if(checkLoggerLevel(eLoggerLevel_Error) == false) {
			return;
		}
		char buffer[MSGBUF_MAX];
		DWORD len = getContentHeader(&buffer[0], sizeof(buffer), eLoggerLevel_Error);
		FormatString(&buffer[len], sizeof(buffer) - len, pattern, len);
		this->writeLog(buffer, len);
	}

	void nLogger::trace(const char *pattern, ...)
	{
		if(checkLoggerLevel(eLoggerLevel_Trace) == false) {
			return;
		}
		char buffer[MSGBUF_MAX];
		DWORD len = getContentHeader(&buffer[0], sizeof(buffer), eLoggerLevel_Trace);
		FormatString(&buffer[len], sizeof(buffer) - len, pattern, len);
		this->writeLog(buffer, len);
	}

	bool nLogger::write(const char *pattern, ...)
	{
		_time.now();
		if(_last_log_time_in_sec != _time.zoneSec()){
			_time.format(&_last_log_time_str[0], sizeof(_last_log_time_str), "%T");
		}
		char buffer[MSGBUF_MAX];
		getMessage(&buffer[0], sizeof(buffer), pattern);
		return this->writeLog(buffer, msgLen);
	}

	const DWORD nLogger::getContentHeader(char* headerBuffer, const DWORD bufferSize, const DWORD loggerLevel)
	{
		DWORD len = 0;
		_time.now();
		if(_last_log_time_in_sec != _time.zoneSec()){
			DWORD timeStrLen = _time.format(&headerBuffer[len], bufferSize - len, "%T");
			nStringTool::copyStr(_last_log_time_str, &headerBuffer[len], sizeof(_last_log_time_str));
			len += timeStrLen;
		}
		else{
			len += nStringTool::copyStr(&headerBuffer[len], _last_log_time_str, bufferSize - len);
		}
		len += snprintf(&headerBuffer[len], bufferSize - len, " %s %s: ", _log_name.c_str(), LoggerLevelStr[loggerLevel]);
		return len;
	}

	void nLogger::setNeedSysLog()
	{
		if(_need_sys_log == true){
			return;
		}
		_need_sys_log = true;
		openlog("", LOG_PID | LOG_NDELAY | LOG_NOWAIT, LOG_LOCAL0);
	}

	void nLogger::clearNeedSysLog()
	{
		if(_need_sys_log == false){
			return;
		}
		closelog();
		_need_sys_log = false;
	}

	void nLogger::dealChangeFile()
	{
	}

	bool nLogger::getFileName(nDayTime &time, const std::string &prefixName, std::string *out)
	{
		out->assign(prefixName);

		char formatBuff[64]= {".%y%m%d"};
		DWORD len = strlen(formatBuff);
		if((_split_time_in_min % (24*60)) != 0){
			len += snprintf(&formatBuff[len], sizeof(formatBuff) - len, "-\%\%H");
		}
		if((_split_time_in_min % 60) != 0){
			len += snprintf(&formatBuff[len], sizeof(formatBuff) - len, "-%02u", (time.getMinutes() / _split_time_in_min * _split_time_in_min));
		}
		char buf[50];
		time.format(buf, sizeof(buf), formatBuff);
		out->append(buf);
		return true;
	}

	void nLogger::flushToFile(bool needFlushLock)
	{
		std::string &buffer = _write_buffer_list[_write_buffer_index];
		if(buffer.empty() == true){
			return;
		}
		_mutex.lock();
		_write_buffer_index = (_write_buffer_index + 1)%2;
		_mutex.unlock();

		if(buffer.empty() == true){
			return;
		}
		if(needFlushLock == true){
			_flush_lock.lock();
		}

		_ofile->write(buffer.data(), buffer.size());
		_ofile->flush();
		buffer.clear();
		DWORD currBufferCapacity = buffer.capacity();
		bool needLogWarnCapacity = false;
		if(baseServer->baseMonitorService != NULL && currBufferCapacity > baseServer->baseMonitorService->getMaxLogWriteBufferSize()){
			// 调用reserve导致capacity比传的参数值大,所以才去清空的办法
			std::string temp;
			buffer.swap(temp);
			needLogWarnCapacity = true;
		}
		if(needFlushLock == true){
			_flush_lock.unlock();
		}
		if(needLogWarnCapacity == true){
			baseServer->fatal("日志超出最大内存空间限制[%u,%u]", currBufferCapacity, baseServer->baseMonitorService->getMaxLogWriteBufferSize());
		}
	}

	bool nLogger::checkNeedLock() const
	{
		return _is_need_lock || _is_async_write;
	}

	void nLogger::setAsyncWrite()
	{
		_is_async_write = true;
	}

	void nLogger::clearAsyncWrite()
	{
		_is_async_write = false;
	}

	bool nLogger::checkAsyncWrite() const
	{
		return _is_async_write;
	}
};
