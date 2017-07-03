#include "base/nSTL.h"
#include "base/nDBConnection.h"
#include "base/nDBConnectionPool.h"

#include "game/nMiscDataSerialize.h"
#include "game/nSsqls.h"
#include "game/nGameServer.h"

namespace Game
{
	bool nMiscDataSerialize::init()
	{
		return true;
	}

	bool nMiscDataSerialize::load(const protocol::serialize::eMiscDataSerializeType dataType, const DWORD dataKey, DWORD *saveTimeInSec, Tidy::string *data)
	{
		Tidy::nDBConnection *conn = _conn_pool->getConnection(eDBConnectionID_ZoneMain);
		if(!conn) {
			gameServer->error("不能从数据库连接池获取连接句柄 [%s]",__PRETTY_FUNCTION__);
			return false;
		}
        SqlMiscDataSave miscDatSaveTable;
        miscDatSaveTable.dataType = dataType;
        miscDatSaveTable.dataKey = dataKey;
		Tidy::nDBQuery query = conn->getQuery();
		Tidy::nDBQueryResult result = query.exeSelect(miscDatSaveTable.table(), miscDatSaveTable.field_list(SqlMiscDataSave_saveTimeInSec, SqlMiscDataSave_allBinary), miscDatSaveTable.equal_list(" AND ", SqlMiscDataSave_dataType, SqlMiscDataSave_dataKey), 0);
		if(result.empty()) {
			return true;
		}
		if(result[0][SqlMiscDataSave::names[SqlMiscDataSave_allBinary]].is_null() == true){
			return true;
		}

		*saveTimeInSec = static_cast<DWORD>(result[0][SqlMiscDataSave::names[SqlMiscDataSave_saveTimeInSec]]);
		if(data == NULL){
            return true;
        }
        data->assign((const char*)result[0][SqlMiscDataSave::names[SqlMiscDataSave_allBinary]], result[0][SqlMiscDataSave::names[SqlMiscDataSave_allBinary]].size());

		return true;
	}

	bool nMiscDataSerialize::load(const protocol::serialize::eMiscDataSerializeType dataType, Tidy::vector<MiscDataBase> *dataVec)
	{
		Tidy::nDBConnection *conn = _conn_pool->getConnection(eDBConnectionID_ZoneMain);
		if(!conn) {
			assert_fail("无法建立数据库连接");
			return false;
		}
		Tidy::nDBQuery query = conn->getQuery();
        SqlMiscDataSave miscDatSaveTable;
        miscDatSaveTable.dataType = dataType;
		Tidy::nDBQueryResult result = query.exeSelect(miscDatSaveTable.table(), miscDatSaveTable.field_list(SqlMiscDataSave_dataKey, SqlMiscDataSave_saveTimeInSec, SqlMiscDataSave_allBinary), miscDatSaveTable.equal_list(SqlMiscDataSave_dataType), 0);
		if(result.empty()) {
			return true;
		}

		for(WORD i=0; i < result.size(); i++) {
			Tidy::string data((const char*)result[i][SqlMiscDataSave::names[SqlMiscDataSave_allBinary]], result[i][SqlMiscDataSave::names[SqlMiscDataSave_allBinary]].size());
			dataVec->push_back(MiscDataBase(static_cast<DWORD>((result[i][SqlMiscDataSave::names[SqlMiscDataSave_dataKey]])), static_cast<DWORD>((result[i][SqlMiscDataSave::names[SqlMiscDataSave_saveTimeInSec]])), (const char*)result[i][SqlMiscDataSave::names[SqlMiscDataSave_allBinary]], result[i][SqlMiscDataSave::names[SqlMiscDataSave_allBinary]].size()));
		}

		return true;
	}

	bool nMiscDataSerialize::save(const protocol::serialize::eMiscDataSerializeType dataType, const DWORD dataKey, const char* data, const DWORD size)
	{
		Tidy::nDBConnection *conn = _conn_pool->getConnection(eDBConnectionID_ZoneMain);
		if(!conn) {
			assert_fail("无法建立数据库连接");
			return false;
		}

		SqlMiscDataSave saveData;
		saveData.dataType = dataType;
		saveData.dataKey = dataKey;
		saveData.saveTimeInSec = gameServer->time().sec();
		saveData.allBinary.assign(data, size);
		Tidy::nDBQuery query = conn->getQuery();
		if(query.exeReplace(saveData.table(), saveData.field_list(), saveData.value_list()) == Tidy::nDBQuery::ERROR) {
			return false;
		}

		return true;
	}

	bool nMiscDataSerialize::save(const protocol::serialize::eMiscDataSerializeType dataType, const DWORD dataKey, const Tidy::string& data)
	{
		return save(dataType, dataKey, data.data(), data.size());
	}

	bool nMiscDataSerialize::clear(const protocol::serialize::eMiscDataSerializeType dataType, const DWORD dataKey)
	{
		Tidy::nDBConnection *conn = _conn_pool->getConnection(eDBConnectionID_ZoneMain);
		if(!conn)
		{
			assert_fail("无法建立数据库连接");
			return false;
		}

		SqlMiscDataSave miscData;
		miscData.dataType = dataType;
		miscData.dataKey = dataKey;
		Tidy::nDBQuery query = conn->getQuery();
		if(query.exeDelete(miscData.table(), miscData.equal_list(), 0) == Tidy::nDBQuery::ERROR) {
			return false;
		}

		return true;
	}
};
