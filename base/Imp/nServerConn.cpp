#include "base/nServerConn.h"
#include "base/nBaseServer.h"
#include "base/nDebugger.h"

namespace Tidy
{
	nServerConn::nServerConn(): _server_type(0), _is_added_to_manager(false), _data(NULL)
	{
	}

	nServerConn::nServerConn(const DWORD serverID, const DWORD serverType) : 
		nObj(serverID, ""),
		_server_type(serverType),
		_is_added_to_manager(false),
		_data(NULL)
	{
	}

	nServerConn::nServerConn(const DWORD serverID, const char* serverName, const DWORD serverType) : 
		nObj(serverID, serverName),
		_server_type(serverType),
		_is_added_to_manager(false),
		_data(NULL)
	{
	}

	nServerConn::~nServerConn()
	{
		assert_debug(_data == NULL, "");
	}

	void nServerConn::setServerConnInfo(const DWORD serverID, const DWORD serverType, const char *serverName)
	{
#ifdef _DEBUG_SZH
        baseServer->debug("设置服务器信息，堆栈:\n[%s]", Tidy::nDebugger::backTrace().c_str());
#endif
		setID(serverID);
		_server_type = serverType;
		setName(serverName);
	}

	void nServerConn::setServerID(DWORD serverID)
	{
		setID(serverID);
	}

	DWORD nServerConn::getServerID() const
	{
		return getID();
	}

	void nServerConn::setServerName(const char* serverName)
	{
		setName(serverName);
	}

	void nServerConn::setServerType(DWORD serverType)
	{
		_server_type = serverType;
	}

	DWORD nServerConn::getServerType() const
	{
		return _server_type;
	}

	void nServerConn::setAddedToManager(bool added)
	{
		_is_added_to_manager = added;
	}

	bool nServerConn::isAddedToManager() const
	{
		return _is_added_to_manager;
	}

	nServerConnQueue::nServerConnQueue() : _recycle_state(0)
	{
	}

	nServerConnQueue::nServerConnQueue(const DWORD serverID, const DWORD serverType) : nServerConn(serverID, serverType), _recycle_state(eConnRecycleState_None)
	{
	}

	nServerConnQueue::nServerConnQueue(const DWORD serverID, const char* serverName, const DWORD serverType) : nServerConn(serverID, serverName, serverType), _recycle_state(eConnRecycleState_None)
	{
#ifdef _DEBUG
		debug("nServerConnQueue构造");
#endif
	}

	nServerConnQueue::~nServerConnQueue()
	{
		assert_debug(getCmd() == NULL, "还剩下消息没有处理");
#ifdef _DEBUG
		debug("nServerConnQueue析构");
#endif
	}

	bool nServerConnQueue::doTimeTick()
	{
		const nCmdData *cmdData = getCmd();
		while(cmdData != NULL) {
			baseServer->parseCommand(this, cmdData->data, cmdData->len);
			popCmd();
			cmdData = getCmd();
		}
		return true;
	}

	bool nServerConnQueue::parseRawCmd(const void *packet, const DWORD nCmdLen)
	{
		return pushCmd(packet, nCmdLen);
	}

	void nServerConnQueue::setRecycleState(const eConnRecycleState recycleState)
	{
		assert_debug(recycleState != eConnRecycleState_WaitDelete || isAddedToManager() == false, "");
		_recycle_state = recycleState;
#ifdef _DEBUG
		debug("_recycle_state=[%u]", _recycle_state);
#endif
	}

	nServerConnNoQueue::nServerConnNoQueue()
	{
	}

	nServerConnNoQueue::nServerConnNoQueue(const DWORD serverID, const DWORD serverType) : nServerConn(serverID, serverType)
	{
	}

	nServerConnNoQueue::nServerConnNoQueue(const DWORD serverID, const char* serverName, const DWORD serverType) : nServerConn(serverID, serverName, serverType)
	{
	}

	nServerConnNoQueue::~nServerConnNoQueue()
	{
	}

	bool nServerConnNoQueue::doTimeTick()
	{
		return true;
	}

	bool nServerConnNoQueue::parseRawCmd(const void *packet, const DWORD len)
	{
		return baseServer->parseCommand(this, packet, len);
	}
};
