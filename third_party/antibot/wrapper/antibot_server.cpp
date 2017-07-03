#include "antibot_server.h"
#include <platform/platform_shared/frame/xml_config_reader.h>
#include <platform/logic_frame/include/module_base/pipe_app_interface.h>
#include <platform/logic_frame/include/module_base/module_base_interface.h>
#include <platform/logic_frame/include/interfaces/zone_player_manager_interface.h>

CAntibotServer::CAntibotServer() : m_antibot(NULL), m_antibot_enable(true), m_antibot_punish(false), m_gameid(-1)
{
	m_buf.ExpandTo(1024 * 1024);
}

CAntibotServer::~CAntibotServer()
{

}

int CAntibotServer::Init()
{
	m_token = GetRegistry()->GetApplication()->GetToken();

	IXmlConfigReader* reader = LoadXmlConfigFile( "../resources/config/server/tencent/tencent.xml" );
	if(reader)
	{
		reader->Get(m_antibot_enable, "antibot_enable");
		reader->Get(m_antibot_punish, "antibot_punish");
		//reader->Get(m_gameid, "antibot_gameid");
		reader->Release();
	}

	sscanf(TOKEN_DESC(m_token).c_str(), "GW%d", &m_gameid);

	if(m_antibot_enable)
	{
		m_antibot = CreateAntiServerObj();
		if(m_antibot)
		{
			if(m_antibot->Initialize(this, (char*)ItoA(m_gameid).c_str()) == 0)
			{
				LogInfo("Antibot ", m_gameid, " created.");
				return 0;
			}
		}
	}
	LogError("Antibot is not created.");

	return -1;
}

int CAntibotServer::SendBuffertoClient( int iType, int iGameID, int iRoomID, unsigned int uiUid, unsigned char * abySendBuffer, short shSize )
{
	AntibotClientInfo* info = FindClient(uiUid);
	if(info)
	{
		pipe_pack_t* pack = (pipe_pack_t*)m_buf.GetBytes();
		pack->type = (iType == 0) ? PACK_CONN_ANTIBOT : PACK_CONN_DPROTO;
		pack->src = m_token;
		pack->dst = info->token;
		pack->len = shSize;
		m_buf.Put(abySendBuffer, sizeof(pipe_pack_t), shSize);
		info->conn->SendPacket(BackThread(), net_pack_t(m_buf.GetBytes(), sizeof(pipe_pack_t) + shSize));
		return 0;
	}
	else
	{
		LogDebug("Antibot SendBuffertoClient can not find client qq: ", uiUid, " DeleteClient.");
		m_antibot->DeleteClient(m_gameid, 0, uiUid);
		return -1;
	}
	return -1;
}

int CAntibotServer::PunishClient( int iGameID, int iRoomID, unsigned int uiUid, EPUNISHMODE enPunishMode, AntibotPunishInfo * pstAntibotPunishInfo, char * szPunishReason )
{
	LogInfo("CAntibotServer::PunishClient ", uiUid, " mode: ", enPunishMode, " reason: ", szPunishReason);

	if(!FindClient(uiUid))
	{
		LogDebug("Antibot PunishClient can not find client qq: ", uiUid, " DeleteClient.");
		m_antibot->DeleteClient(m_gameid, 0, uiUid);
		return -1;
	}

	if(m_antibot_punish)
	{
		PunishInfo info;
		info.accid = uiUid;

		switch(enPunishMode)
		{
		case KICKOUT:
			{
				info.type = PUNISH_TYPE_ROOM_KICK;
			}
			break;
		case CLOSECLIENT:
			{
				info.type = PUNISH_TYPE_LOGOUT;
			}
			break;
		}

		if(info.type != PUNISH_TYPE_NONE)
		{
			CStubPtr<IZonePlayerManagerService> stub(__zone);
			stub->PunishPlayer(info);
		}
	}

	return 0;
}

int CAntibotServer::ProcessProtectMsg( SSafeModePkg * pstSecSafeMsg )
{
	return 0;
}

void CAntibotServer::Update( void )
{
	m_antibot->ProcessAntiBotInput();
}

int CAntibotServer::AddClient( AntibotClientInfo* info )
{
	TUserInfo user_info;
	user_info.szPlayerName[0] = '\0';
	user_info.iUserID = -1;
	user_info.uiCltIp = info->cli_ip;
	user_info.uiSvrIp = info->svr_ip;

	//双登时 两个连接对应同一个qq和key，消息会错乱，游戏可能不正常
	int key = m_antibot->AddClient(m_gameid, 0, info->qq, info->cli_ver, &user_info);
	if(key >= 0)
	{
		if(FindClient(info->qq))
		{
			LogError("Antibot duplicated AddClient qq: ", info->qq, " key: ", key);
		}

		LogDebug("Antibot AddClient qq: ", info->qq, " key: ", key, " ver: ", info->cli_ver);
		info->key = key;
		m_user_map[info->qq] = info;
	}
	return 0;
}

int CAntibotServer::DelClient( AntibotClientInfo* info )
{
	if(FindClient(info->qq) == info)
	{
		LogDebug("Antibot DelClient qq: ", info->qq, " key: ", info->key);
		m_antibot->DeleteClient(m_gameid, 0, info->qq);
		m_user_map.erase(info->qq);
	}
	return 0;
}

int CAntibotServer::OnSend( AntibotClientInfo* info, pipe_pack_t*& pack )
{
	if(info->key >= 0)
	{
		int cmd_id = GetCmdID(pack);
		if(cmd_id)
		{
			if(m_antibot->IsCmdInDownList(info->key, cmd_id))
			{
				pipe_pack_t* send_pack = (pipe_pack_t*)m_buf.GetBytes();
				unsigned int len = m_buf.Size() - sizeof(pipe_pack_t);

				//连信道头一起加密
				if(m_antibot->RecvBufferForEncrypt(info->key, (unsigned char*)pack, pack->len + sizeof(pipe_pack_t), (unsigned char*)send_pack->body, len) == 0)
				{
					LogDebug("Antibot send encrypt qq: ", info->qq, " cmd_id: ", cmd_id);

					send_pack->type = PACK_CONN_DPROTO;
					send_pack->src = m_token;
					send_pack->dst = info->token;
					send_pack->len = len;
					pack = send_pack;
					return 0;
				}
			}
		}
	}
	return 0;
}

int CAntibotServer::OnReceive( AntibotClientInfo* info, pipe_pack_t*& pack )
{
	if(info->key >= 0)
	{
		switch(pack->type)
		{
		case PACK_CONN_DATA:
			{
				int cmd_id = GetCmdID(pack);
				if(cmd_id)
				{
					if(m_antibot->AntiCheatPacket(info->key, cmd_id) != 0)
					{
						LogDebug("Antibot AntiCheatPacket fail qq: ", info->qq, " cmd_id: ", cmd_id);
						return -1;
					}
				}
				return 0;
			}
			break;
		case PACK_CONN_ANTIBOT:
			{
				LogDebug("Antibot OnRecvBufferFromClient qq: ", info->qq);
				m_antibot->CheckTimer(m_gameid, 0, info->qq);
				m_antibot->OnRecvBufferFromClient(m_gameid, 0, info->qq, (unsigned char*)pack->body, pack->len);
				return -1;
			}
			break;
		case PACK_CONN_DPROTO:
			{
				pipe_pack_t* recv_pack = (pipe_pack_t*)m_buf.GetBytes();
				unsigned short len = (m_buf.Size() > 0xFFFF) ? 0xFFFF : m_buf.Size();
				int ret = m_antibot->RecvBufferForDecrypt(info->key, (unsigned char*)pack->body, pack->len, (unsigned char*)recv_pack, len);
				if(ret == 0)
				{
					int cmd_id = GetCmdID(recv_pack);
					if(cmd_id > 0)
					{
						if(m_antibot->IsCmdInUpList(info->key, cmd_id))
						{
							LogDebug("Antibot dproto decrypt ok qq: ", info->qq);
							pack = recv_pack;
							return 0;
						}
					}
					LogDebug("Antibot dproto decrypt cmd_id not in uplist qq: ", info->qq);
					return -1;
				}
				LogDebug("Antibot dproto decrypt return ", ret, " qq: ", info->qq);
				return -1;
			}
			break;
		}
	}
	return 0;
}

AntibotClientInfo* CAntibotServer::FindClient( unsigned int qq )
{
	ClientMap::iterator it = m_user_map.find(qq);
	if(it != m_user_map.end())
	{
		return it->second;
	}
	return NULL;
}

int CAntibotServer::GetCmdID( pipe_pack_t* pack )
{
	if(pack->type == PACK_CONN_DATA && pack->len > sizeof(int))
	{
		return *(int*)pack->body;
	}
	return 0;
}
