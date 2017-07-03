#ifndef _TENCENT_EVENT_H_
#define _TENCENT_EVENT_H_

#include <common_lib/global_header.h>
#include <platform/platform_shared/frame/Interfaces.h>
#include <platform/logic_frame/include/module_base/id_base.h>
#include <platform/platform_shared/frame/router_interface.h>

struct AntibotClientInfo
{
	IPacketConnection* conn;
	pipe_token_t token;
	unsigned int key;
	unsigned int qq;
	unsigned int cli_ver;
	unsigned int svr_ip;
	unsigned int cli_ip;

	AntibotClientInfo() : conn(NULL), key(-1), qq(0), cli_ver(0), svr_ip(0), cli_ip(0)
	{
	}
};

interface IAntbotBase
{
	virtual ~IAntbotBase(){}
};

interface IAntibotServer : IAntbotBase
{
	virtual int AddClient(AntibotClientInfo* info) = 0;
	virtual int DelClient(AntibotClientInfo* info) = 0;

	//pack有可能被改写，返回0，继续处理，返回-1，丢弃该pack
	virtual int OnSend(AntibotClientInfo* info, pipe_pack_t*& pack) = 0;
	virtual int OnReceive(AntibotClientInfo* info, pipe_pack_t*& pack) = 0;
};

interface IAntibotClient : IAntbotBase
{
	virtual void SetConnection(IPacketConnection* conn) = 0;
	virtual int OnSend(pipe_pack_t*& pack) = 0;
	virtual int OnReceive(pipe_pack_t*& pack) = 0;
	virtual void Update() = 0;
};

#endif//_TENCENT_EVENT_H_
