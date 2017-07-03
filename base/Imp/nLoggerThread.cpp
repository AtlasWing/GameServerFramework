#include "base/Imp/nLoggerThread.h"

namespace Tidy
{
	nLoggerThread::nLoggerThread():
		nThread("nLoggerThread", true),
		_sync_time_in_msec(1000)
	{
		loadConfig();
	}

	nLoggerThread::~nLoggerThread()
	{
	}

	bool nLoggerThread::addLogger(Tidy::nLogger *logger)
	{
		nMutex_scope_lock lock(_logger_list_lock);
		_logger_list.push_back(logger);
		logger->setAsyncWrite();
		return true;
	}

	bool nLoggerThread::removeLogger(Tidy::nLogger *logger)
	{
		nMutex_scope_lock lock(_logger_list_lock);
		_logger_list.remove(logger);
		logger->clearAsyncWrite();
		return true;
	}

	void nLoggerThread::flushAll()
	{
		_logger_list_lock.lock();
		for(DWORD i = 0; i < _logger_list.size(); ++ i){
			_logger_list[i]->flushToFile(true);
		}
		_logger_list_lock.unlock();
	}

	void nLoggerThread::dealHupSig()
	{
		loadConfig();
	}

	void nLoggerThread::loadConfig()
	{
		_sync_time_in_msec = Tidy::global["SYNC_LOG_FILE_TIME_IN_MSEC"].getDword();
		if(_sync_time_in_msec == 0){
			_sync_time_in_msec = 1000;
		}
	}

	void nLoggerThread::run()
	{
		while(!isFinal()){
			this->setRunning();
			flushAll();
			nThread::msleep(_sync_time_in_msec);
		}
		flushAll();
	}
};
