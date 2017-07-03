#ifndef _ANTIBOT_CLIENT_H_
#define _ANTIBOT_CLIENT_H_

#include <common_lib/global_header.h>

#include "antibot_def.h"

#ifdef _DEBUG
#undef _DEBUG
#include <third_party/antibot/client/sdk/AntiBotImport.h>
#include <third_party/antibot/client/sdk/DProto.h>
#define _DEBUG
#else
#include <third_party/antibot/client/sdk/AntiBotImport.h>
#include <third_party/antibot/client/sdk/DProto.h>
#endif

using namespace AntiBot;

class CAntibotClient : public IAntibotClient, public IClientSink, public IDPClientSink
{
	enum{NOT_INIT,INIT_OK,INIT_FAIL};

public:
	CAntibotClient();
	virtual ~CAntibotClient();

	int Init();

public:
	virtual void SetConnection(IPacketConnection* conn);
	virtual int OnSend(pipe_pack_t*& pack);
	virtual int OnReceive(pipe_pack_t*& pack);
	virtual void Update();

public:
	//IClientSink antibot
	virtual int SendDataToSvr(BYTE *pbyData, DWORD dwSize);
	//IDPClientSink dp
	virtual int SendDataToServer( BYTE *pbyData, DWORD dwSize );

protected:
	int SendData(int type, BYTE *pbyData, DWORD dwSize);
	int GetCmdID(pipe_pack_t* pack);

protected:
	IAntiBot* m_antibot;
	IDProto* m_dproto;
	pipe_token_t m_token;
	IPacketConnection* m_conn;
	CDynamicStreamBuf m_buf;
	int m_init;
};


#endif//_ANTIBOT_CLIENT_H_