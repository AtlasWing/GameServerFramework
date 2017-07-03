#include <google/malloc_extension.h>
#include <sys/resource.h>

#include "base/nBaseMonitorService.h"
#include "base/Imp/nThreadManager.h"
#include "base/Imp/nThreadMonitor.h"
#include "base/Imp/nLoggerThread.h"
#include "base/nCommandCtl.h"
#include "base/nServerConn.h"
#include "base/nZlib.h"

namespace Tidy
{
	static const DWORD MIN_LOG_WRITE_BUFFER_SIZE = 64 * 1024;
	static DWORD maxCompressPacketSizeMapSize = 10000;
	nBaseMonitorService::nBaseMonitorService():
		_thread_manager(NULL),
		_thread_monitor(NULL),
		_logger_thread(NULL),
		_log_status_timer(static_cast<DWORD>(Tidy::global["MONITOR_LOG_SERVER_STATUS_TIMER_IN_MSEC"])),
		_release_memory_bytes_one_time(50 * 1024  * 1024),
		_max_release_memory_bytes_one_time(0),
		_max_release_memory_cost_time_in_usec(5000),
		_max_log_write_buffer_size(MIN_LOG_WRITE_BUFFER_SIZE),
		_is_in_profile(false),
		_need_packet_compress_statistics(false)
	{
		assert_debug(static_cast<DWORD>(Tidy::global["MONITOR_LOG_SERVER_STATUS_TIMER_IN_MSEC"]) > 0, "时间没有配置");
	}

	nBaseMonitorService::~nBaseMonitorService()
	{
	}

	bool nBaseMonitorService::initService()
	{
		MallocExtension::instance()->Initialize();
		if(loadConfig() == false){
			assert_fail("加载配置失败");
			return false;
		}
		if(MallocExtension::instance()->VerifyAllMemory() == false){
			assert_fail("内存错误");
			return false;
		}
		logMemStatus("init");

		_thread_manager = new nThreadManager();

		_thread_monitor = new nThreadMonitor(_thread_manager, Tidy::global["THREAD_TIMEOUT_TIME_IN_MSEC"].getDword(), Tidy::global["THREAD_TIMEOUT_MAX_TIMES"].getDword(), Tidy::global["THREAD_SEED_RESET_TIME_IN_SEC"].getDword());
		_thread_monitor->start();

		_logger_thread = new nLoggerThread();
		_logger_thread->start();

		addAsyncLogger(baseServer->getLogger());

		return true;
	}

	void nBaseMonitorService::finalService()
	{
		if(_is_in_profile == true){
			stopProfile();
		}
		_logger_thread->join();
		SAFE_DELETE(_logger_thread);

		_thread_monitor->join();
		SAFE_DELETE(_thread_monitor);

		_thread_manager->clear();
		SAFE_DELETE(_thread_manager);
		if(MallocExtension::instance()->VerifyAllMemory() == false){
			assert_fail("内存错误");
		}
		logMemStatus("final");
	}

	void nBaseMonitorService::dealHupSig()
	{
		loadConfig();
		_logger_thread->dealHupSig();
	}

	bool nBaseMonitorService::loadConfig()
	{
		if(Tidy::global["MAX_RELEASE_MEMORY_COST_TIME_IN_MSEC"].empty() == false){
			_max_release_memory_cost_time_in_usec = Tidy::global["MAX_RELEASE_MEMORY_COST_TIME_IN_MSEC"].getDword() * 1000;
		}
		size_t maxThreadCacheSize = 0;
		if(Tidy::global["MAX_TOTAL_THREAD_CACHE_BYTES_IN_MILLION"].empty() == false){
			maxThreadCacheSize = static_cast<size_t>(Tidy::global["MAX_TOTAL_THREAD_CACHE_BYTES_IN_MILLION"].getFloat() * 1024 * 1024);
			MallocExtension::instance()->SetNumericProperty("tcmalloc.max_total_thread_cache_bytes", maxThreadCacheSize);
		}
		MallocExtension::instance()->GetNumericProperty("tcmalloc.max_total_thread_cache_bytes", &maxThreadCacheSize);
		baseServer->info("tcmalloc.max_total_thread_cache_bytes= %f M", 1.0 * maxThreadCacheSize / 1024 / 1024);
		MallocExtension::instance()->SetMemoryReleaseRate(static_cast<double>(Tidy::global["MEMORY_RELEASE_RATE"]));
		_max_log_write_buffer_size = Tidy::global["MAX_LOG_WRITE_BUFFER_SIZE"].getDword();
		if(_max_log_write_buffer_size < MIN_LOG_WRITE_BUFFER_SIZE){
			_max_log_write_buffer_size = MIN_LOG_WRITE_BUFFER_SIZE;
		}
		//capacity并不一定等于reserve的参数
		std::string temp;
		temp.reserve(_max_log_write_buffer_size);
		_max_log_write_buffer_size = temp.capacity();

		if(Tidy::global["ZLIB_PACKET_ZIP_MIN"].empty() == false){
			resetPacketZipMin(eZipMethod_ZLIB, Tidy::global["ZLIB_PACKET_ZIP_MIN"].getDword());
		}
		if(Tidy::global["SNAPPY_PACKET_ZIP_MIN"].empty() == false){
			resetPacketZipMin(eZipMethod_Snappy, Tidy::global["SNAPPY_PACKET_ZIP_MIN"].getDword());
		}
		if(Tidy::global["NEED_PACKET_COMPRESS_STATISTICS"].empty() == false){
			setNeedPacketStatistics(Tidy::global["NEED_PACKET_COMPRESS_STATISTICS"].getDword() > 0);
		}
		if(Tidy::global["ZLIB_COMPRESS_LEVEL"].empty() == false){
			setZlibCompressLevel(Tidy::global["ZLIB_COMPRESS_LEVEL"].getInt());
		}
		if(Tidy::global["MAX_COMPRESS_SIZE_STATISTICS_SIZE"].empty() == false){
			maxCompressPacketSizeMapSize = Tidy::global["MAX_COMPRESS_SIZE_STATISTICS_SIZE"].getDword();
		}
		return true;
	}

	void nBaseMonitorService::addThread(nThread *thread)
	{
		_thread_manager->addThread(thread);
	}

	void nBaseMonitorService::removeThread(nThread *thread)
	{
		if(baseServer->isCoreThread(_thread_monitor) == true){
			// 如果是_thread_monitor宕机,就不走removeThread了,会死锁
			return;
		}
		_thread_manager->removeThread(thread);
	}

	bool nBaseMonitorService::doTick()
	{
		if(_log_status_timer.isReady() == true){
#ifdef _DEBUG
			verifyMemory();
#endif
			releaseMemory(false);
			logServerStatus();
		}
		return true;
	}

	void nBaseMonitorService::logMemStatus(const char *info)
	{
#if 0
		// 明细日志,通过GetStats获取明细内存分配,对分析无用,不打印明细日志
		char statusInfo[64*1024];
		getMemStatus(statusInfo, sizeof(statusInfo));
		baseServer->debug("statusInfo=[%s]", statusInfo);
#endif
		size_t currentAllocatedBytes = 0;
		MallocExtension::instance()->GetNumericProperty("generic.current_allocated_bytes", &currentAllocatedBytes);
		size_t currHeapBytes = 0;
		MallocExtension::instance()->GetNumericProperty("generic.heap_size", &currHeapBytes);
		size_t currUnmappedBytes = 0;
		MallocExtension::instance()->GetNumericProperty("tcmalloc.pageheap_unmapped_bytes", &currUnmappedBytes);
		float MILLION = 1024.0*1024.0;
		baseServer->info("内存占用[%6.3f, %6.3f, %6.3f]", currentAllocatedBytes/MILLION, (currHeapBytes - currUnmappedBytes - currentAllocatedBytes)/MILLION, (currHeapBytes - currUnmappedBytes)/MILLION);
	}

	void nBaseMonitorService::getMemStatus(char *statusInfo, const DWORD maxSize) const
	{
		MallocExtension::instance()->GetStats(statusInfo, maxSize);
	}

	void nBaseMonitorService::logServerStatus()
	{
		for(BYTE i = 0; i < eZipMethod_Max; ++ i){
			_packet_zip_statistics_data_array[i].logZipStatus(i);
		}
		logMemStatus("logServerStatus");
		//getUptimeInfo 没有必要打印日志
	}

	void nBaseMonitorService::releaseMemory(bool needFreeAll)
	{
		size_t oldUnmappedBytes = 0;
		MallocExtension::instance()->GetNumericProperty("tcmalloc.pageheap_unmapped_bytes", &oldUnmappedBytes);
		size_t currentAllocatedBytes = 0;
		MallocExtension::instance()->GetNumericProperty("generic.current_allocated_bytes", &currentAllocatedBytes);
		size_t currentHeapSize = 0;
		MallocExtension::instance()->GetNumericProperty("generic.heap_size", &currentHeapSize);
		assert_debug(currentHeapSize >= oldUnmappedBytes, "[%u,%u]", currentHeapSize, oldUnmappedBytes);
		size_t currentUsedHeapSize = currentHeapSize - oldUnmappedBytes;

		baseServer->time().now();
		QWORD startTimeInUsec = baseServer->time().usec();
		if(needFreeAll == true){
			MallocExtension::instance()->ReleaseFreeMemory();
		}
		else{
			DWORD releaseMemoryBytes = 0;
			// 当内存池空闲内存大于程序使用内存的1/2时,释放空闲内存
			if(currentUsedHeapSize > (currentAllocatedBytes/2*3)){
				releaseMemoryBytes = currentUsedHeapSize - (currentAllocatedBytes/2*3);
				if(releaseMemoryBytes > _release_memory_bytes_one_time){
					releaseMemoryBytes = _release_memory_bytes_one_time;
				}
			}
			if(releaseMemoryBytes == 0){
				return;
			}
			MallocExtension::instance()->ReleaseToSystem(releaseMemoryBytes);
		}
		baseServer->time().now();
		QWORD endTimeInUsec = baseServer->time().usec();
		size_t currUnmappedBytes = 0;
		MallocExtension::instance()->GetNumericProperty("tcmalloc.pageheap_unmapped_bytes", &currUnmappedBytes);
		if(currUnmappedBytes > oldUnmappedBytes){
			size_t releaseBytes = currUnmappedBytes - oldUnmappedBytes;
			QWORD costTimeInUsec = 0;
			if(endTimeInUsec > startTimeInUsec){
				costTimeInUsec = endTimeInUsec - startTimeInUsec;
				_release_memory_bytes_one_time = static_cast<DWORD>(0.6 * releaseBytes / costTimeInUsec * _max_release_memory_cost_time_in_usec);
				if(costTimeInUsec > _max_release_memory_cost_time_in_usec){
					_max_release_memory_bytes_one_time = _release_memory_bytes_one_time;
					baseServer->fatal("释放内存时间超出限制[%llu,%u][%lu]", costTimeInUsec, _max_release_memory_cost_time_in_usec, releaseBytes);
				}
				else if(_release_memory_bytes_one_time > _max_release_memory_bytes_one_time){
					_max_release_memory_bytes_one_time = _release_memory_bytes_one_time;
				}
				if(_release_memory_bytes_one_time < _max_release_memory_bytes_one_time){
					_release_memory_bytes_one_time = _max_release_memory_bytes_one_time;
				}
			}
			else if(_release_memory_bytes_one_time < _max_release_memory_bytes_one_time){
				_release_memory_bytes_one_time = _max_release_memory_bytes_one_time;
			}

			baseServer->info("releaseMemory 释放[%f]M 消耗时间[%llu]US releaseMemoryOneTime=[%f]M", (1.0 * (currUnmappedBytes - oldUnmappedBytes) / 1024 / 1024), costTimeInUsec, (1.0 * _release_memory_bytes_one_time / 1024 / 1024));
		}
	}

	void nBaseMonitorService::verifyMemory()
	{
		MallocExtension::instance()->VerifyAllMemory();
	}

	void nBaseMonitorService::getUptimeInfo(char *statusInfo, const DWORD maxSize) const
	{
		struct rusage self_ru, c_ru;
		getrusage(RUSAGE_SELF, &self_ru);
		getrusage(RUSAGE_CHILDREN, &c_ru);
		snprintf(statusInfo, maxSize, 
				"uptime_in_sec:%lu\n"
				"used_cpu_sys_in_sec:%.2f\n"
				"used_cpu_user_in_sec:%.2f\n"
				"used_cpu_sys_children_in_sec:%.2f\n"
				"used_cpu_user_children_in_sec:%.2f\n",
				baseServer->time().sec() - baseServer->getStartTimeInSec(),
				(float)self_ru.ru_stime.tv_sec + (float)self_ru.ru_stime.tv_usec/1000000,
				(float)self_ru.ru_utime.tv_sec + (float)self_ru.ru_utime.tv_usec/1000000,
				(float)c_ru.ru_stime.tv_sec + (float)c_ru.ru_stime.tv_usec/1000000,
				(float)c_ru.ru_utime.tv_sec + (float)c_ru.ru_utime.tv_usec/1000000
				);
	}

	void nBaseMonitorService::dealSendCommand(nServerConn *serverConn, const struct CommandCtl *commandCtl, const void *nullCommand, const DWORD cmdLen)
	{
	}

	void nBaseMonitorService::dealParseCommand(nServerConn *serverConn, const struct CommandCtl *commandCtl, const void *nullCommand, const DWORD cmdLen)
	{
	}

	void nBaseMonitorService::addCompressPacketData(const BYTE zipMethod, const DWORD compressSize, const DWORD compressedSize)
	{
		_packet_zip_statistics_data_array[zipMethod].addZipPacketData(_need_packet_compress_statistics, compressSize, compressedSize);
	}

	void nBaseMonitorService::addPacket(const BYTE zipMethod, const DWORD packetSize)
	{
		_packet_zip_statistics_data_array[zipMethod].addPacket(packetSize);
	}

	bool nBaseMonitorService::addAsyncLogger(Tidy::nLogger *logger)
	{
		return _logger_thread->addLogger(logger);
	}

	bool nBaseMonitorService::removeAsyncLogger(Tidy::nLogger *logger)
	{
		return _logger_thread->removeLogger(logger);
	}

	void nBaseMonitorService::dealCore()
	{
		_logger_thread->flushAll();
	}

	DWORD nBaseMonitorService::getMaxLogWriteBufferSize() const
	{
		return _max_log_write_buffer_size;
	}

	void nBaseMonitorService::startProfile(const char *outputFileDir)
	{
		char outputFileName[1024];
		char timeBuffer[64];
		baseServer->time().format(timeBuffer, sizeof(timeBuffer), ".%y%m%d-%H-%M");
		snprintf(outputFileName, sizeof(outputFileName), "%s%s_%s_%u.prof", outputFileDir, baseServer->getServerName(), timeBuffer, baseServer->getServerID());
		if(ProfilerStartWithOptions(outputFileName, NULL) == 0){
			baseServer->info("开始性能分析失败");
			return;
		}
		ProfilerRegisterThread();
		baseServer->info("开始性能分析成功[%s]", outputFileName);
	}

	void nBaseMonitorService::stopProfile()
	{
		_is_in_profile = false;
		ProfilerStop();
		baseServer->info("结束性能分析成功");
	}

	void nBaseMonitorService::setNeedProfile(const bool needProfile, const char *outputFileDir)
	{
		if(_is_in_profile == needProfile){
			return;
		}
		_is_in_profile = needProfile;
		if(_is_in_profile == true){
			startProfile(outputFileDir);
		}
		else{
			stopProfile();
		}
	}

	void nBaseMonitorService::resetPacketZipMin(BYTE zipMethod, const DWORD packetZipMin)
	{
		if(MIN_PACKET_ZIP_SIZE[zipMethod] == packetZipMin){
			baseServer->info("重置消息包压缩最小阈值未变化[%u,%u]", zipMethod, packetZipMin);
			return;
		}
		baseServer->info("重置消息包压缩最小阈值[%u,%u]", zipMethod, packetZipMin);
		MIN_PACKET_ZIP_SIZE[zipMethod] = packetZipMin;
		_packet_zip_statistics_data_array[zipMethod].reset();
	}

	void nBaseMonitorService::setNeedPacketStatistics(bool needPacketCompressStatistics)
	{
		if(_need_packet_compress_statistics == needPacketCompressStatistics){
			return;
		}
		_need_packet_compress_statistics = needPacketCompressStatistics;
		if(_need_packet_compress_statistics == false){
			for(BYTE i = 0; i < eZipMethod_Max; ++ i){
				_packet_zip_statistics_data_array[i].logZipStatus(i);
				_packet_zip_statistics_data_array[i].reset();
			}
		}
	}

	void nBaseMonitorService::setZlibCompressLevel(const int level)
	{
		ZLIB_COMPRESS_LEVEL = level;
	}

	PacketZipStatisticsData::PacketZipStatisticsData() : 
		_all_packet_size_sum(0),
		_compress_packet_size_sum(0),
		_compressed_packet_size_sum(0)
	{
	}

	void PacketZipStatisticsData::reset()
	{
		_all_packet_size_sum = 0;
		_compress_packet_size_sum = 0;
		_compressed_packet_size_sum = 0;
		_compress_packet_size_map.clear();
	}

	void PacketZipStatisticsData::addZipPacketData(const bool needPacketSizeStatistics, const DWORD compressSize, const DWORD compressedSize)
	{
		_compress_packet_size_sum += compressSize;
		_compressed_packet_size_sum += compressedSize;
		if(needPacketSizeStatistics == true){
			PacketZipSizeData *data = _compress_packet_size_map.fetch(compressSize/16 * 16);
			data->zipSize += compressSize;
			data->zippedSize += compressedSize;
			if(_compress_packet_size_map.size() > maxCompressPacketSizeMapSize){
				baseServer->info("超出压缩统计最大数量,重置统计数据,避免数据量太大导致日志量太大阻塞[%u]", maxCompressPacketSizeMapSize);
				reset();
			}
		}
	}

	void PacketZipStatisticsData::addPacket(const DWORD packetSize)
	{
		_all_packet_size_sum += packetSize;
	}

	void PacketZipStatisticsData::logZipStatus(const BYTE zipMethod)
	{
		if(_compress_packet_size_sum == 0){
			return;
		}
		baseServer->info("消息压缩统计[%u][%llu, %llu, %llu], [%f,%f,%f]", zipMethod, _all_packet_size_sum, _compress_packet_size_sum, _compressed_packet_size_sum, (1.0 * _compress_packet_size_sum / _all_packet_size_sum), (1.0 * _compressed_packet_size_sum/_compress_packet_size_sum), (1.0 * (_all_packet_size_sum - _compress_packet_size_sum + _compressed_packet_size_sum)/_all_packet_size_sum));
		for(Tidy::map<DWORD, PacketZipSizeData>::const_iterator iter = _compress_packet_size_map.begin(); iter != _compress_packet_size_map.end(); ++ iter){
			const PacketZipSizeData &data = iter->second;
			baseServer->info("压缩大小[%u,%f][%f]", data.zipSize, (1.0 * data.zipSize / _compress_packet_size_sum), (1.0 * data.zippedSize / data.zipSize));
		}
	}
};
