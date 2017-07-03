#include "antibot_client.h"
#include <platform/logic_frame/include/module_base/pipe_app_interface.h>

#if _PLATFORM_WIN32_

CAntibotClient::CAntibotClient() : m_antibot(NULL), m_conn(NULL), m_dproto(NULL), m_init(NOT_INIT)
{
	m_buf.ExpandTo(1024 * 1024);
	memset(m_buf.GetBytes(), 0, m_buf.Size());
}

CAntibotClient::~CAntibotClient()
{
	if(m_antibot)
	{
		m_antibot->Release();
	}
	if(m_dproto)
	{
		m_dproto->Release();
	}
}

int CAntibotClient::Init()
{
	if(m_init == NOT_INIT)
	{
		m_token = GetRegistry()->GetApplication()->GetToken();

		m_antibot = (IAntiBot*)CreateAntiObj(ANTI_OBJ_ANTIBOT);
		m_dproto = (IDProto*)CreateAntiObj(ANTI_OBJ_DPROTO);

		if(!m_antibot || !m_dproto)
		{
			LogError("$HI_YELLOW$Create antibot or dproto fail.");
			m_init = INIT_FAIL;
			return m_init;
		}

		LogInfo("$HI_YELLOW$Create antibot ok");
		m_init = INIT_OK;
	}


	return m_init;
}

void CAntibotClient::SetConnection( IPacketConnection* conn )
{
	if(Init() != INIT_OK)
	{
		return;
	}

	lightAssert(m_antibot);
	lightAssert(m_dproto);

	m_conn = conn;

	if(conn)
	{
		LogInfo("$HI_YELLOW$SendInitData for antibot & dproto");

		DWORD ver = GetRegistry()->GetExeVer();
		LogInfo("$HI_YELLOW$Exe ver for antibot: ", ver);
		m_antibot->SetUserInfo(ver, (DWORD)m_token.pstid, this);
		m_dproto->Init(ver, this);

		m_antibot->SendInitData();
		m_dproto->SendInitData();
	}
}

//antibot
int CAntibotClient::SendDataToSvr( BYTE *pbyData, DWORD dwSize )
{
	LogDebug("$HI_YELLOW$Antibot SendDataToSvr data len: ", dwSize);
	return SendData(PACK_CONN_ANTIBOT, pbyData, dwSize);
}


//dp
int CAntibotClient::SendDataToServer( BYTE *pbyData, DWORD dwSize )
{
	LogDebug("$HI_YELLOW$DProto SendDataToServer data len: ", dwSize);
	return SendData(PACK_CONN_DPROTO, pbyData, dwSize);
}

int CAntibotClient::SendData( int type, BYTE *pbyData, DWORD dwSize )
{
	if(m_conn)
	{
		pipe_pack_t* pack = (pipe_pack_t*)m_buf.GetBytes();
		pack->type = type;
		pack->src = m_token;
		pack->len = dwSize;
		m_buf.Put(pbyData, sizeof(pipe_pack_t), dwSize);
		m_conn->SendPacket(BackThread(), net_pack_t(m_buf.GetBytes(), sizeof(pipe_pack_t) + dwSize));
		return 0;
	}
	return -1;
}

int CAntibotClient::OnSend( pipe_pack_t*& pack )
{
	if(Init() != INIT_OK)
	{
		return 0;
	}

	int cmd_id = GetCmdID(pack);
	if(cmd_id)
	{
		if(m_dproto->NeedEncrypt(cmd_id))
		{
			if(m_dproto->Encrypt((PBYTE)pack, sizeof(pipe_pack_t) + pack->len))
			{
				LogDebug("$HI_YELLOW$DProto encrypt for cmd: ", cmd_id);

				PBYTE pData = NULL;
				DWORD dwDataLen = 0;
				m_dproto->GetUPVMDataBuf(pData, dwDataLen);

				pipe_pack_t* send_pack = (pipe_pack_t*)m_buf.GetBytes();
				send_pack->type = PACK_CONN_DPROTO;
				send_pack->src = m_token;
				send_pack->len = dwDataLen;
				m_buf.Put(pData, sizeof(pipe_pack_t), dwDataLen);

				pack = send_pack;
			}
		}
	}
	return 0;
}

int CAntibotClient::OnReceive( pipe_pack_t*& pack )
{
	if(Init() != INIT_OK)
	{
		return 0;
	}

	switch(pack->type)
	{
	case PACK_CONN_DATA:
		{
			int cmd_id = GetCmdID(pack);
			if(cmd_id)
			{
				if(!m_dproto->CheckProtoStatus(cmd_id))
				{
					LogError("$HI_YELLOW$DProto CheckProtoStatus fail for cmd: ", cmd_id);
					return -1;
				}
			}
			return 0;
		}
		break;
	case PACK_CONN_ANTIBOT:
		{
			LogDebug("$HI_YELLOW$Antibot OnRcvAntiData data len: ", pack->len);
			m_antibot->OnRcvAntiData((BYTE*)pack->body, pack->len);
			return -1;
		}
		break;
	case PACK_CONN_DPROTO:
		{
			int ret = m_dproto->OnGetData((PBYTE)pack->body);
			if(ret == IDProto::GetCryptedData)
			{
				LogDebug("$HI_YELLOW$DProto GetCryptedData");

				PBYTE pData = NULL;
				DWORD dwDataLen = 0;
				m_dproto->GetVMDataBuf(pData, dwDataLen);

				pipe_pack_t* recv_pack = (pipe_pack_t*)m_buf.GetBytes();
				m_buf.Put(pData, 0, dwDataLen);
				pack = recv_pack;
				return 0;
			}
			else if(ret == IDProto::AnalyseOK)
			{
				LogDebug("$HI_YELLOW$DProto AnalyseOK");
				return -1;
			}
			else if(ret == IDProto::AnalyseFail)
			{
				LogDebug("$HI_YELLOW$DProto AnalyseFail");
			}
			else if(ret == IDProto::NeedByteCode)
			{
				LogDebug("$HI_YELLOW$DProto NeedByteCode");
			}
			return -1;
		}
		break;
	}
	return 0;
}

int CAntibotClient::GetCmdID( pipe_pack_t* pack )
{
	if(pack->type == PACK_CONN_DATA && pack->len > sizeof(int))
	{
		return *(int*)pack->body;
	}
	return 0;
}

void CAntibotClient::Update()
{
	MSG stMsg = {0};

	if (::PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&stMsg);
		::DispatchMessage(&stMsg);
	}
}


#endif
