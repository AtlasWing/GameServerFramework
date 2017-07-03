#ifndef BASE_NBASEMONITORSERVICE_H_
#define BASE_NBASEMONITORSERVICE_H_

#include "base/nServerTickTimer.h"
#include "base/nZip.h"

namespace Tidy
{
	class nThread;
	class nThreadManager;
	class nThreadMonitor;
	class nLoggerThread;
	struct CommandCtl;

	struct CommandData{
		CommandData() : sendSizeSum(0), recentSendSizeSum(0){
		}
		DWORD sendSizeSum;
		DWORD recentSendSizeSum;
	};
	struct ServerCommandData{
		Tidy::map<std::string, CommandData> commandDataMap;
	};
	struct FuncData{
		FuncData() : timeSum(0), recentTimeSum(0){
		}
		DWORD timeSum;
		DWORD recentTimeSum;
	};
	struct PacketZipSizeData{
		PacketZipSizeData() : zipSize(0), zippedSize(0) {
		}
		DWORD zipSize;
		DWORD zippedSize;
	};
	class PacketZipStatisticsData{
		public:
			PacketZipStatisticsData();

		public:
			void reset();
			void addZipPacketData(const bool needPacketSizeStatistics, const DWORD compressSize, const DWORD compressedSize);
			void logZipStatus(const BYTE zipMethod);
			void addPacket(const DWORD packetSize);

		private:
			QWORD _all_packet_size_sum;
			QWORD _compress_packet_size_sum;
			QWORD _compressed_packet_size_sum;
			Tidy::map<DWORD, PacketZipSizeData> _compress_packet_size_map;
	};
	class nBaseMonitorService
	{
		public:
			nBaseMonitorService();
			virtual ~nBaseMonitorService();

		public:
			virtual bool initService();
			virtual void finalService();
			virtual void dealHupSig();

		public:
			bool doTick();

		public:
			void addThread(nThread *thread);
			void removeThread(nThread *thread);
			void logMemStatus(const char *info);
			void getMemStatus(char *statusInfo, const DWORD maxSize) const;
			void releaseMemory(bool needFreeAll);
			void verifyMemory();
			void getUptimeInfo(char *statusInfo, const DWORD maxSize) const;
			bool addAsyncLogger(Tidy::nLogger *logger);
			bool removeAsyncLogger(Tidy::nLogger *logger);
			void dealCore();
			DWORD getMaxLogWriteBufferSize() const;
			void setNeedProfile(const bool needProfile, const char *outputFileDir);
			void resetPacketZipMin(BYTE zipMethod, const DWORD packetZipMin);
			void setNeedPacketStatistics(bool needCacketCompressStatistics);
			void setZlibCompressLevel(const int level);

		public:
			void dealSendCommand(nServerConn *serverConn, const struct CommandCtl *commandCtl, const void *nullCommand, const DWORD cmdLen);
			void dealParseCommand(nServerConn *serverConn, const struct CommandCtl *commandCtl, const void *nullCommand, const DWORD cmdLen);
			void addCompressPacketData(const BYTE zipMethod, const DWORD compressSize, const DWORD compressedSize);
			void addPacket(const BYTE zipMethod, const DWORD packetSize);

		protected:
			virtual void logServerStatus();

		private:
			bool loadConfig();
			void startProfile(const char *outputFileName);
			void stopProfile();

		private:
			nThreadManager *_thread_manager;
			nThreadMonitor *_thread_monitor;
			nLoggerThread *_logger_thread;
			nServerTickTimer _log_status_timer;
			DWORD _release_memory_bytes_one_time;
			DWORD _max_release_memory_bytes_one_time;
			DWORD _max_release_memory_cost_time_in_usec;
			DWORD _max_log_write_buffer_size;
			bool _is_in_profile;
			bool _need_packet_compress_statistics;
			PacketZipStatisticsData _packet_zip_statistics_data_array[eZipMethod_Max];
	};
};
#endif
