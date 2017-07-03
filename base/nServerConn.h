#ifndef _N_SERVER_CONN_H_
#define _N_SERVER_CONN_H_

#include "base/nObj.h"
#include "base/nCmdQueue.h"
#include "base/nAssert.h"

namespace Tidy
{
	class nServerConn : public nObj
	{
		protected:
			nServerConn();
			nServerConn(const DWORD serverID, const DWORD serverType);
			nServerConn(const DWORD serverID, const char* serverName, const DWORD serverType);

		public:
			virtual ~nServerConn();

		public:
			virtual bool sendCmd(const void *command, const DWORD len) const = 0;
			virtual bool doTimeTick() = 0;
			virtual bool sendRawData(const void *data, const DWORD len) const = 0;

		public:
			void setServerConnInfo(const DWORD serverID, const DWORD serverType, const char *serverName);
			void setServerName(const char* name);
			void setServerID(DWORD serverID);
			DWORD getServerID() const;
			void setServerType(DWORD serverType);
			DWORD getServerType() const;
			void setAddedToManager(bool added);
			bool isAddedToManager() const;

		public:
			template<typename T>
				inline T* getData() const
				{
					return (T*)_data;
				}
			template<typename T>
				void newData()
				{
					assert_debug(_data == NULL, "");
					T* dataObj = new T(this);
					_data = dataObj;
				}

			template<typename T>
				void setData(T *dataObj)
				{
					assert_debug(_data == NULL, "");
					_data = dataObj;
				}

			template<typename T>
				void deleteData()
				{
					const T* dataObj = reinterpret_cast<const T*>(_data);
					_data = NULL;
					SAFE_DELETE(dataObj);
				}

			void clearData()
			{
				_data = NULL;
			}

		private:
			DWORD _server_type;
			bool _is_added_to_manager;

		private:
			const void* _data;
	};

	class nServerConnQueue : public nServerConn, public nCmdQueue
	{
		public:
			enum eConnRecycleState
			{
				eConnRecycleState_None                   =   0,
				eConnRecycleState_WaitRemoveFromManager  =   1,
				eConnRecycleState_WaitDelete             =   2,
			};

		public:
			nServerConnQueue();
			nServerConnQueue(const DWORD serverID, const DWORD serverType);
			nServerConnQueue(const DWORD serverID, const char* serverName, const DWORD serverType);
			virtual ~nServerConnQueue();

		public:
			virtual bool doTimeTick();

		protected:
			bool parseRawCmd(const void *packet, const DWORD len);

		public:
			bool checkRecycle()
			{
				return _recycle_state == eConnRecycleState_WaitRemoveFromManager;
			}

			void setRecycleState(const eConnRecycleState recycleState);

			const eConnRecycleState getRecycleState() const
			{
				return static_cast<eConnRecycleState>(_recycle_state);
			}
		private:
			BYTE _recycle_state;
	};

	class nServerConnNoQueue : public nServerConn
	{
		public:
			nServerConnNoQueue();
			nServerConnNoQueue(const DWORD serverID, const DWORD serverType);
			nServerConnNoQueue(const DWORD serverID, const char* serverName, const DWORD serverType);
			virtual ~nServerConnNoQueue();

		public:
			virtual bool doTimeTick();

		protected:
			bool parseRawCmd(const void *packet, const DWORD len);
	};
};
#endif
