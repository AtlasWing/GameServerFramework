#ifndef BASE_NASYNCSQLTHREAD_H_
#define BASE_NASYNCSQLTHREAD_H_

#include "base/nThread.h"
#include "base/nSTL.h"
#include "base/nDBConnectionPool.h"
#include "base/nTimer.h"

namespace Tidy
{
	typedef Tidy::list< std::pair<DWORD, std::string> > SqlList;
	class nDBConnectionPool;
	class nBinaryQueue;
	class nMemoryMappedFile;

	struct ExecSqlCallback{
		virtual ~ExecSqlCallback(){
		}
		virtual bool exec(bool result) = 0;
	};
#pragma pack(1)
	struct nAsyncSqlData{
		nAsyncSqlData();
		BYTE  version;
		DWORD id;
		DWORD connectionID;
		DWORD reservedFieldList[4];// 预留的字段,结合version使用
		ExecSqlCallback *callback;
		DWORD sqlLen;
		char  sql[0];
		DWORD allSize() const;
	};
#pragma pack()
	class nAsyncSqlThread : public nThread
	{
		public:
			nAsyncSqlThread(const std::string &sqlMemoryMappedFile, const DWORD sqlMemoryMappedFileSize);
			~nAsyncSqlThread();

		public:
			bool init();
			void final();
			bool addSql(const DWORD dbConnID, const std::string &sql, ExecSqlCallback *callback);
			nDBConnectionPool& getDBConnPool();

		private:
			virtual void run();

		private:
			bool exeSqlList(nBinaryQueue *list, bool needExeCallback);

		private:
			nBinaryQueue *_sql_queue[2];
			BYTE    *_input_sql_list_index;
			nMutex _change_list_lock;
			bool _need_sync;
			nDBConnectionPool _db_conn_manager;
			std::string _sql_memory_mapped_file;
			nMemoryMappedFile *_map_file;
			DWORD _sql_memory_mapped_file_size;
			Tidy::nTimer _log_timer;
			DWORD _last_log_sql_id;

		public:
			static DWORD sqlID;
	};
};

#endif
