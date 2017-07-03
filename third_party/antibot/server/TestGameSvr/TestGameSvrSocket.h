// TestGameSvrSocket.h: interface for the CTestGameSvrSocket class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_TESTGAMESVRSOCKET_H__E2783108_CD10_4608_A3A3_C5F3A55215CB__INCLUDED_)
#define AFX_TESTGAMESVRSOCKET_H__E2783108_CD10_4608_A3A3_C5F3A55215CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "../../../include/TabIpcImport.h"

#pragma warning(disable:4786)
#include <vector>

#pragma pack(push,1)
typedef struct tagGameProtoHead
{
	DWORD dwSize;
	WORD wProtoId;
	DWORD dwCrc;
} GAMEPROTO_HEAD;
#pragma pack(pop)

enum
{
	GAMEPROTO_INIT = 1,
	GAMEPROTO_ANTIBOT = 2,
	GAMEPROTO_GAMESTATUS = 3,
	GAMEPROTO_DPROTO = 999,
	GAMEPROTO_TEST_DP = 5,
	GAMEPROTO_REQUESTDATAFORDP = 6,
	// UDP de
	UDP_CMD_LOGIN_SERVER = 56,
	UDP_CMD_GET_USER = 57,
	UDP_CMD_GAME_DATA = 58,
	UDP_CMD_TRANSMIT = 59,
	//cc cmd
	UDP_CMD_P2P_DATA = 510,
	UDP_CMD_P2P_ACK = 511
};


typedef struct tagUserInfo
{
	DWORD dwUin;
	char szIp[32];
	unsigned int unTcpPort;
	unsigned int unUdpPort;
} USER_INFO, *PUSER_INFO;

class CTestGameSvrSocket;
typedef struct tagGameSvrSokcetInfo
{
	DWORD dwUin;
	char szGameClientIp[16];
	WORD wGameClientPort;
	CTestGameSvrSocket *pSocket;
	DWORD dwLoginTime;
	int iKey;
	USER_INFO stUserInfo;
} GAMESVR_SOCKET_INFO;


#pragma pack(push,1)

typedef struct tagCSUdpDataHead
{
	DWORD dwSize;
	WORD wProtoId;
	DWORD dwCrc;
	DWORD dwUin;
	BYTE byOptSize;
	BYTE abyOption[128];

} CS_UDP_DATA_HEAD;

typedef struct tagCSUdpDataHeadNoOpt
{
	DWORD dwSize;
	WORD wProtoId;
	DWORD dwCrc;
	DWORD dwUin;
	BYTE byOptSize;

} CS_UDP_DATA_HEAD_NO_OPT;
#pragma pack(pop)

typedef struct tagGADataHead
{
	DWORD dwSize;
	DWORD dwCrc;
	DWORD dwUin;
} GADATA_HEAD;

typedef std::vector<GAMESVR_SOCKET_INFO*> VECTOR_GAMESVR_SOCKET;

#define MAX_CACHE_BUF_SIZE		(1024 * 1024)
#define MAX_PKG_SIZE			(32 * 1024)



/////////////////////////////////////////////////////////////////////////////
// CTestGameSvrSocket command target
class  CUdpSrv;


class CTestGameSvrSocket : public CAsyncSocket  
{
	friend CUdpSrv;
public:
	CTestGameSvrSocket();
	virtual ~CTestGameSvrSocket();

public:
	void CreateSvrSocket(WORD wPort);
	
	void DecodeGameProto_Init(BYTE *pbyGameProtoInitData, DWORD dwGameProtoInitSize, DWORD& dwUin, DWORD& dwCliVer);
	void EncodeGameProto_AntiBot(BYTE *pbyAntiBotData, DWORD dwAntiBotSize, BYTE **ppbyGameProtoAntiBotData, DWORD& dwGameProtoAntiBotSize);
	void DecodeGameProto_AntiBot(BYTE *pbyGameProtoAntiBotData, DWORD dwGameProtoAntiBotSize, BYTE **ppbyAntiBotData, DWORD& dwAntiBotSize);
	void DecodeGameProto_GameStatus(BYTE *pbyGameProtoGameStatus, DWORD dwGameProtoGameStatus, BYTE& byGameStatus);
	//void DecodeGameProto_Dynamic(BYTE *pbyGameProtoAntiBotData, DWORD dwGameProtoAntiBotSize, BYTE **ppbyAntiBotData, DWORD& dwAntiBotSize);
	void EncodeGAData(BYTE *pbyAntiBotData, DWORD dwAntiBotSize, BYTE **ppbyGAData, DWORD& dwGASize);
	
	void DecodeGameProto(BYTE *pbyGameProtoData, DWORD dwGameProtoSize, BYTE **ppbyData, DWORD& dwSize);
	void EncodeGameProto(BYTE *pbyGameProtoData, DWORD dwGameProtoSize, WORD wProtoId, BYTE **ppbyData, DWORD& dwSize);
	
	void SendData(BYTE *pbyData, DWORD dwSize);
	void OnRcvData(BYTE *pbyData, DWORD dwSize);
	

	//void DecodeGameProto_Dynamic_ForAntibot(BYTE *pbyGameProtoAntiBotData, DWORD dwGameProtoAntiBotSize, BYTE **ppbyAntiBotData, DWORD& dwAntiBotSize);
	void EncodeGameProto_DynamicProto(BYTE *pbyAntiBotData, DWORD dwAntiBotSize, BYTE **ppbyGameProtoAntiBotData, DWORD& dwGameProtoAntiBotSize);


	void AddSvrSocket(GAMESVR_SOCKET_INFO *pstSocketInfo);
	static void DelSvrSocket(CTestGameSvrSocket *pSocket);
	GAMESVR_SOCKET_INFO* GetSvrSocket(CTestGameSvrSocket *pSocket);
	GAMESVR_SOCKET_INFO* GetSvrSocket(DWORD dwUin);
	static void ClearSvrSocket();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestGameSvrSocket)
public:
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL
	
	// Generated message map functions
	//{{AFX_MSG(CTestGameSvrSocket)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

private:
   
	DWORD m_dwRemainSizeRcv;
	BYTE *m_pbyCacheBufRcv;

	DWORD m_dwRemainSizeSnd;
	BYTE *m_pbyCacheBufSnd;	
#ifdef OLD_VERSION
static VECTOR_GAMESVR_SOCKET m_vecSvrSocket;
#else
static VECTOR_GAMESVR_SOCKET m_vecSvrSocket;
#endif

	//static ITabIpc *m_pTabIpc;
};

//{{AFX_INSERT_LOCATION}}
#endif // !defined(AFX_TESTGAMESVRSOCKET_H__E2783108_CD10_4608_A3A3_C5F3A55215CB__INCLUDED_)
