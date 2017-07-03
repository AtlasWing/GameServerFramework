#ifndef _N_MISC_DATA_SERIALIZE_H_
#define _N_MISC_DATA_SERIALIZE_H_

#include "protocol/serializeCommand.h"

#include "base/Tidy.h"
#include "base/nSTL.h"

namespace Tidy{
	class nDBConnectionPool;
};

namespace Game
{
	class nGameServer;
	extern nGameServer *gameServer;
	struct MiscDataBase
	{
		MiscDataBase(const DWORD dataKey, const DWORD saveTimeInSec, const char* data, const DWORD dataLength) :
			dataKey(dataKey),
			saveTimeInSec(saveTimeInSec),
			data(data, dataLength)
		{
		}
		DWORD dataKey;
		DWORD saveTimeInSec;
		Tidy::string data;
	};

	class nMiscDataSerialize
	{
		public:
			nMiscDataSerialize(Tidy::nDBConnectionPool* connPool) : _conn_pool(connPool)
		{
		}

		public:
			bool init();

		public:
			template<typename ServerType>
				bool requestSaveMiscData(const DWORD miscDataType, const DWORD dataKey, const char* data, const DWORD num);
			template<typename ServerType>
				bool requestLoadMiscData(const DWORD miscDataType, const DWORD dataKey, const bool needClear);
			bool load(const protocol::serialize::eMiscDataSerializeType type, const DWORD dataKey, DWORD *saveTimeInSec, Tidy::string *data);
			bool load(const protocol::serialize::eMiscDataSerializeType type, Tidy::vector<MiscDataBase> *dataVec);

		public:
			// 以下为同步接口,慎用,尽量使用requestSaveMiscData
			bool save(const protocol::serialize::eMiscDataSerializeType dataType, const DWORD dataKey, const char* data, const DWORD size);
			bool save(const protocol::serialize::eMiscDataSerializeType dataType, const DWORD dataKey, const Tidy::string& data);
			bool clear(const protocol::serialize::eMiscDataSerializeType dataType, const DWORD dataKey);

		private:
			Tidy::nDBConnectionPool* _conn_pool;
	};

	template<typename ServerType>
		bool nMiscDataSerialize::requestSaveMiscData(const DWORD miscDataType, const DWORD dataKey, const char* data, const DWORD num)
		{
			if(num >= BIG_PROTO_CMD_CAPACITY(protocol::serialize::SaveMiscDataCommand)){
				assert_fail("溢出[%u,%u,%u]", miscDataType, dataKey, num);
				return true;
			}
			BUFFER_BIG_PROTO_CMD(protocol::serialize::SaveMiscDataCommand, send);
			send->miscDataSerializeType = miscDataType;
			send->dataKey = dataKey;
			send->num = num;
			memcpy(send->data, data, num);
			return static_cast<ServerType*>(gameServer)->messageService->sendCmdToData(send, send->size());
		}

	template<typename ServerType>
		bool nMiscDataSerialize::requestLoadMiscData(const DWORD miscDataType, const DWORD dataKey, const bool needClear)
		{
			protocol::serialize::RequestMiscDataCommand send;
			send.miscDataSerializeType = miscDataType;
			send.dataKey = dataKey;
			send.needClear = needClear;
			return static_cast<ServerType*>(gameServer)->messageService->sendCmdToData(&send, sizeof(send));
		}

};
#endif
