#ifndef _N_DB_CONNECTION_H_
#define _N_DB_CONNECTION_H_

#include "base/Tidy.h"
#include "base/nObj.h"
#include "base/nDBUrlManager.h"

namespace Tidy
{
	class nDBConnectionPool;

	class nDBQueryResult : public mysqlpp::StoreQueryResult
	{
		public:
			nDBQueryResult(const mysqlpp::StoreQueryResult & res);

		public:
			const bool empty() const;
			const DWORD size() const;
			template<typename TableT>
				inline std::vector<DWORD> getResultFieldIndexMap(const DWORD tableFieldNum) const;
	};

	enum eResultSortOrder{
		eResultSortOrder_None   =   0,
		eResultSortOrder_Asc,
		eResultSortOrder_Desc,
	};
	enum eDBTableLockLevel{
		eDBTableLockLevel_Read,
		eDBTableLockLevel_Write,
	};
	enum eCheckDBClosedResult{
		eCheckDBClosedResult_Closed,
		eCheckDBClosedResult_ReconnectSucc,
		eCheckDBClosedResult_OtherErr,
	};
	class nDBConnection;
	class nDBQuery : public mysqlpp::Query
	{
		public:
			nDBQuery(const nDBQuery &query);
			nDBQuery(const mysqlpp::Query &query, nDBConnection *conn);
			nDBQuery(nDBConnection *conn);
			~nDBQuery();

		public:
			bool exeQuery(const char* str,const int len);
			bool exeQuery( const std::string& str );

			/// @brief 查询
			nDBQueryResult exeSelect();
			template<typename Ssqls, typename WhereT>
				nDBQueryResult exeSelect(const Ssqls& ssqls, std::vector<bool>& flag, const WhereT& where, const DWORD limitNum);
			template<typename T1, typename T2>
				nDBQueryResult exeSelect(const char* tableName, const T1& column, const T2& where, const DWORD limitNum);
			template<typename T1, typename T2, typename T3>
				nDBQueryResult exeSelect(const char* tableName, const T1& column, const T2& where, const DWORD limitNum, const eResultSortOrder order = eResultSortOrder_None, const T3 *orderByColumn = NULL, const char *extendParam = NULL);
			template<typename T1>
				nDBQueryResult exeSelect(const char* tableName, const T1& column);

			/// @brief 如果有auto_increment，返回自增值,失败返回(DWORD)-1
			DWORD exeInsert();
			template<typename T1, typename T2>
				DWORD exeInsert(const char* tableName, const T1& field, const T2& value);
			template<typename T1,typename T2, typename T3>
				DWORD exeInsert(const char* tableName, const T1& field, const T2& value, const T3 &onDuplicateUpdateValue);
			template<typename Ssqls>
				DWORD exeInsert(const Ssqls& ssqls, std::vector<bool>& flag);

			template<typename T1, typename T2>
				inline DWORD exeReplace(const char* tableName, const T1& field, const T2& value);
			template<typename T1, typename T2>
				inline std::string getReplaceSql(const char* tableName, const T1& field, const T2& value);
			template<typename Ssqls>
				std::string getReplaceSql(const Ssqls& ssqls, std::vector<bool>& flag);
			template<typename T1, typename T2>
				inline std::string getInsertSql(const char* tableName, const T1& field, const T2& value);
			template<typename Ssqls>
				inline std::string getInsertSql(const Ssqls& ssqls, std::vector<bool>& flag);
			template<typename T1,typename T2, typename T3>
				inline std::string getInsertSql(const char* tableName, const T1& field, const T2& value, const T3 &onDuplicateUpdateValue);

			/// @brief 返回受影响的条数，失败返回(DWORD)-1
			DWORD exeDelete();
			template<typename T>
				DWORD exeDelete(const char* tableName, const T& where, const DWORD limitNum);

			template<typename Ssqls, typename WhereT>
				inline std::string getDeleteSql(const Ssqls& ssqls, const WhereT& where, const DWORD limitNum);

			/// @brief 返回受影响的条数，失败返回(DWORD)-1
			DWORD exeUpdate();
			template<typename T1, typename T2>
				DWORD exeUpdate(const char* tableName, const T1& column, const T2& where, const DWORD limitNum);
			template<typename Ssqls, typename WhereT>
				DWORD exeUpdate(const Ssqls& ssqls, std::vector<bool>& flag, const WhereT& where, const DWORD limitNum);
			template<typename Ssqls, typename WhereT>
				std::string getUpdateSql(const Ssqls& ssqls, std::vector<bool>& flag, const WhereT& where, const DWORD limitNum);
			template<typename T1, typename T2>
				std::string getUpdateSql(const char* tableName, const T1& column, const T2& where, const DWORD limitNum);
			bool exeLockTable(const char *tableName, const eDBTableLockLevel lockLevel);
			bool exeUnlockTable();


			const char* getSql();

		public:
			eCheckDBClosedResult checkDBClosed();

		public:
			static const DWORD ERROR = static_cast<DWORD>(-1);

		private:
			nDBConnection *_connection;
			bool _need_unlock_table;
	};

	enum eDBConnState{
		eDBConnState_None,
		eDBConnState_Connected,
		eDBConnState_Disconnected,
		eDBConnState_MAX
	};
	class nDBConnection : public mysqlpp::Connection
	{
		public:
			nDBConnection(const nDBUrl &url, const nDBUrl *slaveUrl, nDBConnectionPool *pool);
			~nDBConnection();

		public:
			bool connectMaster();
			bool connectSlave();
			bool reConnect();
			nDBQuery getQuery();
			const nDBUrl* getUrl() const;
			const nDBUrl* getSlaveUrl() const;
			void setConnState(const eDBConnState state);
			bool checkConnected() const;
			bool checkDisconnected() const;
			DWORD getConnectionID() const;
			bool setUsed(bool used);
			bool isUsed() const;

		private:
			bool connect(const nDBUrl &url);

		private:
			nDBUrl _url;
			nDBUrl *_slave_url;
			nDBConnectionPool *_pool;
			eDBConnState _conn_state;
			bool _used;
	};

	//======================================================================

	template<typename Ssqls, typename WhereT>
		inline nDBQueryResult nDBQuery::exeSelect(const Ssqls& ssqls, std::vector<bool>& flag, const WhereT& where, const DWORD limitNum)
		{
			reset();

			*this << "SELECT " << ssqls.field_list(",", &flag) << " FROM `" << ssqls.table() << "` WHERE " << where;
			if(limitNum > 0){
				*this << " LIMIT " << limitNum;
			}

			return exeSelect();
		}

	template<typename T1, typename T2>
		inline nDBQueryResult nDBQuery::exeSelect(const char* tableName, const T1& column, const T2& where, const DWORD limitNum)
		{
			reset();

			*this << "SELECT " << column << " FROM `" << tableName << "` WHERE " << where;
			if(limitNum > 0){
				*this << " LIMIT " << limitNum;
			}

			return exeSelect();
		}

	template<typename T1, typename T2, typename T3>
		inline nDBQueryResult nDBQuery::exeSelect(const char* tableName, const T1& column, const T2& where, const DWORD limitNum, const eResultSortOrder order, const T3 *orderByColumn, const char *extendParam)
		{
			reset();

			*this << "SELECT " << column << " FROM `" << tableName << "` WHERE " << where;
			if(order == eResultSortOrder_Desc){
				assert_debug(orderByColumn != NULL, "");
				*this << " ORDER BY " << orderByColumn << " DESC";
			}
			else if(order == eResultSortOrder_Asc){
				assert_debug(orderByColumn != NULL, "");
				*this << " ORDER BY " << orderByColumn << " ASC";
			}
			if(limitNum > 0){
				*this << " LIMIT " << limitNum;
			}
			if(extendParam != NULL){
				*this << extendParam;
			}

			return exeSelect();
		}

	template<typename T1>
		inline nDBQueryResult nDBQuery::exeSelect(const char* tableName, const T1& column)
		{
			reset();

			*this << "SELECT " << column << " FROM `" << tableName;

			return exeSelect();
		}

	template<typename T1,typename T2 >
		inline DWORD nDBQuery::exeInsert(const char* tableName, const T1& field, const T2& value)
		{
			reset();

			*this << "INSERT INTO " << tableName << " ( " << field << " ) VALUES (" << value << " )" ;

			return exeInsert();
		}

	template<typename T1,typename T2, typename T3>
		inline DWORD nDBQuery::exeInsert(const char* tableName, const T1& field, const T2& value, const T3 &onDuplicateUpdateValue)
		{
			reset();

			*this << "INSERT INTO " << tableName << " ( " << field << " ) VALUES (" << value << " ) ON DUPLICATE KEY UPDATE " << onDuplicateUpdateValue;

			return exeInsert();
		}

	template<typename T1, typename T2>
		inline DWORD nDBQuery::exeReplace(const char* tableName, const T1& field, const T2& value)
		{
			reset();
			*this << "REPLACE INTO " << tableName << " ( " << field << " ) VALUES (" << value << " )" ;

			return exeUpdate();
		}

	template<typename T1, typename T2>
		inline std::string nDBQuery::getReplaceSql(const char* tableName, const T1& field, const T2& value)
		{
			reset();
			*this << "REPLACE INTO " << tableName << " ( " << field << " ) VALUES (" << value << " )" ;

			return this->str();
		}

	template<typename Ssqls>
		inline std::string nDBQuery::getReplaceSql(const Ssqls& ssqls, std::vector<bool>& flag)
		{
			reset();
			*this << "REPLACE INTO " << ssqls.table() << " ( " << ssqls.field_list(&flag) << " ) VALUES (" << ssqls.value_list(&flag)<< " )" << '\0';

			return this->str();
		}

	template<typename T1, typename T2>
		inline std::string nDBQuery::getInsertSql(const char* tableName, const T1& field, const T2& value)
		{
			reset();
			*this << "INSERT INTO " << tableName << " ( " << field << " ) VALUES (" << value << " )" ;

			return this->str();
		}

	template<typename T1,typename T2, typename T3>
		inline std::string nDBQuery::getInsertSql(const char* tableName, const T1& field, const T2& value, const T3 &onDuplicateUpdateValue)
		{
			reset();
			*this << "INSERT INTO " << tableName << " ( " << field << " ) VALUES (" << value << " ) ON DUPLICATE KEY UPDATE " << onDuplicateUpdateValue;
			return this->str();
		}

	template<typename Ssqls>
		inline DWORD nDBQuery::exeInsert(const Ssqls& ssqls, std::vector<bool>& flag)
		{
			reset();
			*this << "INSERT INTO " << ssqls.table() << " ( " << ssqls.field_list(&flag) << " ) VALUES (" << ssqls.value_list(&flag)<< " )" ;

			return exeInsert();
		}

	template<typename Ssqls>
		inline std::string nDBQuery::getInsertSql(const Ssqls& ssqls, std::vector<bool>& flag)
		{
			reset();
			*this << "INSERT INTO " << ssqls.table() << " ( " << ssqls.field_list(&flag) << " ) VALUES (" << ssqls.value_list(&flag)<< " )" << '\0';

			return this->str();
		}

	template<typename T>
		inline DWORD nDBQuery::exeDelete(const char* tableName, const T& where, const DWORD limitNum)
		{
			reset();

			*this << "DELETE FROM `" << tableName << "` WHERE " << where; 
			if(limitNum > 0){
				*this << " LIMIT " << limitNum;
			}
			*this << '\0';

			return exeDelete();
		}

	template<typename T1, typename T2>
		inline DWORD nDBQuery::exeUpdate(const char* tableName, const T1& column, const T2& where, const DWORD limitNum)
		{
			reset();

			*this << "UPDATE `"  << tableName << "` SET " << column << " WHERE " << where;
			if(limitNum > 0){
				*this << " LIMIT " << limitNum;
			}
			*this << '\0';
			return exeUpdate();
		}

	template<typename Ssqls, typename WhereT>
		inline DWORD nDBQuery::exeUpdate(const Ssqls& ssqls, std::vector<bool>& flag, const WhereT& where, const DWORD limitNum)
		{
			reset();

			*this << "UPDATE `"  << ssqls.table() << "` SET " << ssqls.equal_list(",", &flag) << " WHERE " << where;
			if(limitNum > 0){
				*this << " LIMIT " << limitNum;
			}

			return exeUpdate();
		}

	template<typename Ssqls, typename WhereT>
		inline std::string nDBQuery::getUpdateSql(const Ssqls& ssqls, std::vector<bool>& flag, const WhereT& where, const DWORD limitNum)
		{
			reset();

			*this << "UPDATE `"  << ssqls.table() << "` SET " << ssqls.equal_list(",", &flag) << " WHERE " << where;
			if(limitNum > 0){
				*this << " LIMIT " << limitNum;
			}
			*this << '\0';

			return this->str();
		}

	template<typename T1, typename T2>
		inline std::string nDBQuery::getUpdateSql(const char* tableName, const T1& column, const T2& where, const DWORD limitNum)
		{
			reset();

			*this << "UPDATE `"  << tableName << "` SET " << column << " WHERE " << where;
			if(limitNum > 0){
				*this << " LIMIT " << limitNum;
			}
			*this << '\0';
			return this->str();
		}

	template<typename Ssqls, typename WhereT>
		inline std::string nDBQuery::getDeleteSql(const Ssqls& ssqls, const WhereT& where, const DWORD limitNum)
		{
			reset();

			*this << "DELETE FROM `"  << ssqls.table() << "` WHERE " << where;
			if(limitNum > 0){
				*this << " LIMIT " << limitNum;
			}
			*this << '\0';

			return this->str();
		}

	template<typename TableT>
	inline std::vector<DWORD> nDBQueryResult::getResultFieldIndexMap(const DWORD tableFieldNum) const
	{
		std::vector<DWORD> fieldIndexVec;
		int index = 0;
		for(DWORD i = 0; i < tableFieldNum; ++ i){
			index = field_num(TableT::names[i]);
			if(index == -1 || index >= num_fields()){
				fieldIndexVec.push_back((DWORD)-1);
			}
			else{
				fieldIndexVec.push_back(index);
			}
		}
		return fieldIndexVec;
	}
};

#endif
