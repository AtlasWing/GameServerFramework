#include "base/nDBConnectionPool.h"
#include "base/nDBConnection.h"
#include "base/nBinaryQueue.h"
#include "base/nMemoryMappedFile.h"
#include "base/nAsyncSqlThread.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	nAsyncSqlData::nAsyncSqlData():
		version(1),
		id(0),
		connectionID(0),
		callback(NULL),
		sqlLen(0)
	{
		reservedFieldList[0] = 0;
		reservedFieldList[1] = 0;
		reservedFieldList[2] = 0;
		reservedFieldList[3] = 0;
	}

	DWORD nAsyncSqlData::allSize() const
	{
		return sizeof(*this) + sqlLen * sizeof(sql[0]);
	}

	DWORD nAsyncSqlThread::sqlID = 0;
	nAsyncSqlThread::nAsyncSqlThread(const std::string &sqlMemoryMappedFile, const DWORD sqlMemoryMappedFileSize):
		nThread("nAsyncSqlThread", true),
		_input_sql_list_index(NULL),
		_need_sync(false),
		_db_conn_manager(false),
		_sql_memory_mapped_file(sqlMemoryMappedFile),
		_map_file(NULL),
		_sql_memory_mapped_file_size(sqlMemoryMappedFileSize),
		_log_timer(10 * ONE_SEC),
		_last_log_sql_id(0)
	{
	}

	nAsyncSqlThread::~nAsyncSqlThread()
	{
		assert_debug(_map_file == NULL, "没有final");
	}

	bool nAsyncSqlThread::init()
	{
		_map_file = new nMemoryMappedFile();
		bool needParse = _map_file->exists(_sql_memory_mapped_file.c_str());
		if(_map_file->initFile(_sql_memory_mapped_file.c_str(), _sql_memory_mapped_file_size + sizeof(*_input_sql_list_index)) == false){
			SAFE_DELETE(_map_file);
			assert_fail("初始化文件失败");
			return false;
		}
		const DWORD queueDataSpaceSize = _sql_memory_mapped_file_size / 2;
		char *fileData = _map_file->getData();
		_input_sql_list_index = reinterpret_cast<BYTE*>(fileData);
		fileData += sizeof(*_input_sql_list_index);
		char *queueDataSpace_0 = fileData;
		char *queueDataSpace_1 = fileData + _sql_memory_mapped_file_size / 2;
		_sql_queue[0] = new nBinaryQueue();
		_sql_queue[1] = new nBinaryQueue();
		if(needParse == true){
			_sql_queue[0]->parseFromBinary(queueDataSpace_0, queueDataSpaceSize);
			_sql_queue[1]->parseFromBinary(queueDataSpace_1, queueDataSpaceSize);
		}
		else{
			_sql_queue[0]->setSpace(queueDataSpace_0, queueDataSpaceSize);
			_sql_queue[1]->setSpace(queueDataSpace_1, queueDataSpaceSize);
			*_input_sql_list_index = 0;
		}

		nBinaryQueue *ouputSqlList = _sql_queue[((*_input_sql_list_index) + 1) % 2];
		if(exeSqlList(ouputSqlList, false) == false){
			assert_fail("执行sql失败");
			return false;
		}
		ouputSqlList = _sql_queue[(*_input_sql_list_index)];
		if(exeSqlList(ouputSqlList, false) == false){
			assert_fail("执行sql失败");
			return false;
		}
		return true;
	}

	void nAsyncSqlThread::final()
	{
		if(_need_sync == true){
			_map_file->sync();
		}
		SAFE_DELETE(_sql_queue[0]);
		SAFE_DELETE(_sql_queue[1]);
		SAFE_DELETE(_map_file);
	}

	nDBConnectionPool& nAsyncSqlThread::getDBConnPool()
	{
		return _db_conn_manager;
	}

	bool nAsyncSqlThread::addSql(const DWORD dbConnID, const std::string &sql, ExecSqlCallback *callback)
	{
		assert_debug(_input_sql_list_index != NULL, "没有初始化");
		const DWORD BUFFER_SIZE = MAX_BIG_PROTO_CMD_LEN + 1024;
		if(sql.size() > BUFFER_SIZE){
			assert_fail("溢出[%u]", sql.size());
			if(callback != NULL){
				callback->exec(false);
				SAFE_DELETE(callback);
			}
			return false;
		}
		BUFFER_DATA(nAsyncSqlData, asyncSqlData, BUFFER_SIZE);
		asyncSqlData->connectionID = dbConnID;
		asyncSqlData->callback = callback;
		asyncSqlData->sqlLen = sql.size();
		memcpy(asyncSqlData->sql, sql.c_str(), asyncSqlData->sqlLen);
		nMutex_scope_lock scopeLock(_change_list_lock);
		++ sqlID;
		asyncSqlData->id = sqlID;
		if(_sql_queue[(*_input_sql_list_index)]->pushBack(reinterpret_cast<const char*>(asyncSqlData), asyncSqlData->allSize()) == false){
			assert_fail("无法添加异步sql[%u]", asyncSqlData->allSize());
			baseServer->terminate(eServerCloseReason_Normal);
			return false;
		}

		_need_sync = true;
		return true;
	}

	void nAsyncSqlThread::run()
	{
		nBinaryQueue *ouputSqlList = _sql_queue[((*_input_sql_list_index) + 1) % 2];
		while(isFinal() == false){
			setRunning();
			nThread::msleep(20);

			if(ouputSqlList->empty() == true && _sql_queue[(*_input_sql_list_index)]->empty() == false){
				_change_list_lock.lock();
				if(_sql_queue[(*_input_sql_list_index)]->empty() == false){
					(*_input_sql_list_index) = ((*_input_sql_list_index) + 1) % 2;
				}
				_change_list_lock.unlock();
			}
			if(_sql_queue[(*_input_sql_list_index)] != ouputSqlList){
				// 没有改变
				nThread::msleep(20);
			}
			else{
				ouputSqlList = _sql_queue[((*_input_sql_list_index) + 1) % 2];
				if(exeSqlList(ouputSqlList, true) == false){
					//不能break,需要等服务器关闭,再退出
				}
			}
			if(_need_sync == true){
				_map_file->sync();
				_need_sync = false;
				if(_log_timer(baseServer->time()) == true){
					baseServer->info("异步sql=[%u]", (nAsyncSqlThread::sqlID - _last_log_sql_id));
					_last_log_sql_id = nAsyncSqlThread::sqlID;
				}
			}
		}

		if(_sql_queue[(*_input_sql_list_index)]->empty() == false){
			if(exeSqlList(_sql_queue[(*_input_sql_list_index)], true) == false){
				//assert_fail("执行sql失败");
			}
		}

		if(_need_sync == true){
			_map_file->sync();
			if(_log_timer(baseServer->time()) == true){
				baseServer->info("异步sql=[%u]", (nAsyncSqlThread::sqlID - _last_log_sql_id));
				_last_log_sql_id = nAsyncSqlThread::sqlID;
			}
		}
	}

	bool nAsyncSqlThread::exeSqlList(nBinaryQueue *list, bool needExeCallback)
	{
		_need_sync = (list->empty() == false);

		DWORD len = 0;
		const nAsyncSqlData *asyncSqlData = NULL;
		nDBConnection *conn = NULL;
		bool result = true;
		while(list->empty() == false){
			setRunning();
			asyncSqlData = reinterpret_cast<const nAsyncSqlData*>(list->getFront(&len));
			result = true;
			// 支持空sql,只执行callback
			if(asyncSqlData->sqlLen > 0){
				assert_debug(asyncSqlData->allSize() == len, "长度错误");
				conn = _db_conn_manager.getConnection(asyncSqlData->connectionID);
				if(conn == NULL) {
					assert_fail("没有数据库连接");
					return false;
				}
				nDBQuery query = conn->getQuery();
				result = query.exeQuery(asyncSqlData->sql, asyncSqlData->sqlLen);
				if(result == false){
					if(query.checkDBClosed() == eCheckDBClosedResult_Closed){
						return false;
					}
					else{
						//正常失败,还是要允许
						baseServer->fatal("执行sql错误[%u][%s]", asyncSqlData->id, asyncSqlData->sql);
					}
				}
				else{
				}
			}
			list->popFront();
			if(needExeCallback == true && asyncSqlData->callback != NULL){
				asyncSqlData->callback->exec(result);
			}
		}

		return true;
	}
};
