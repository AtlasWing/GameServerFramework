#ifndef _ANTIBOT_SERVER_H_
#define _ANTIBOT_SERVER_H_

#include <common_lib/global_header.h>

#include "antibot_def.h"
#include <third_party/antibot/server/sdk/include/iantibotinterface.h>

class CAntibotServer : public IAntibotServer, public IBusiness
{
public:
	CAntibotServer();
	virtual ~CAntibotServer();

	int Init();
	void Update( void );

public:
	//IAntibotServer
	virtual int AddClient(AntibotClientInfo* info);
	virtual int DelClient(AntibotClientInfo* info);
	virtual int OnSend(AntibotClientInfo* info, pipe_pack_t*& pack);
	virtual int OnReceive(AntibotClientInfo* info, pipe_pack_t*& pack);

public:
	//IBusiness
	virtual int SendBuffertoClient( int iType, int iGameID, int iRoomID, unsigned int uiUid, unsigned char * abySendBuffer, short shSize );
	virtual int PunishClient( int iGameID, int iRoomID, unsigned int uiUid, EPUNISHMODE enPunishMode, AntibotPunishInfo * pstAntibotPunishInfo, char * szPunishReason );
	virtual int ProcessProtectMsg( SSafeModePkg * pstSecSafeMsg );

protected:
	AntibotClientInfo* FindClient(unsigned int qq);
	int GetCmdID(pipe_pack_t* pack);

protected:
	IAntibotInterface* m_antibot;
	bool m_antibot_enable;
	bool m_antibot_punish;
	int m_gameid;
	CDynamicStreamBuf m_buf;

	typedef std::map<unsigned int, AntibotClientInfo*> ClientMap;
	ClientMap m_user_map;

	pipe_token_t m_token;

};



#endif//_ANTIBOT_SERVER_H_