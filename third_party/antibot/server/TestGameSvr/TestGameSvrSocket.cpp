// TestGameSvrSocket.cpp: implementation of the CTestGameSvrSocket class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "TestGameSvrSocket.h"

#include <algorithm>
#include "iantibotinterface.h"
#include "crc32.h"

VECTOR_GAMESVR_SOCKET CTestGameSvrSocket::m_vecSvrSocket;
//ITabIpc* CTestGameSvrSocket::m_pTabIpc;
extern IAntibotInterface * g_pIAntibotChecker ;
extern std::vector< unsigned int> g_vecUin;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTestGameSvrSocket::CTestGameSvrSocket()
{
	m_dwRemainSizeRcv = 0;
	m_pbyCacheBufRcv = new BYTE[MAX_CACHE_BUF_SIZE];
	memset(m_pbyCacheBufRcv, 0, MAX_CACHE_BUF_SIZE);

	m_dwRemainSizeSnd = 0;
	m_pbyCacheBufSnd = new BYTE[MAX_CACHE_BUF_SIZE];
	memset(m_pbyCacheBufSnd, 0, MAX_CACHE_BUF_SIZE);
	//ClearSvrSocket();
}

CTestGameSvrSocket::~CTestGameSvrSocket()
{
	m_dwRemainSizeRcv = 0;
	if (m_pbyCacheBufRcv)
	{
		delete[] m_pbyCacheBufRcv;
		m_pbyCacheBufRcv = NULL;
	}

	m_dwRemainSizeSnd = 0;
	if (m_pbyCacheBufSnd)
	{
		delete[] m_pbyCacheBufSnd;
		m_pbyCacheBufSnd = NULL;
	}
	//ClearSvrSocket();
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CTestGameSvrSocket, CAsyncSocket)
//{{AFX_MSG_MAP(CTestGameSvrSocket)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0



void CTestGameSvrSocket::DecodeGameProto_Init(BYTE *pbyGameProtoInitData, DWORD dwGameProtoInitSize, DWORD& dwUin, DWORD& dwCliVer)
{
	GAMEPROTO_HEAD *pstGameProtoHead = (GAMEPROTO_HEAD*)pbyGameProtoInitData;
	ASSERT(ntohl(pstGameProtoHead->dwSize) == dwGameProtoInitSize - sizeof(GAMEPROTO_HEAD));
	DWORD dwCrc = CCrc32::Crc32(pbyGameProtoInitData + sizeof(GAMEPROTO_HEAD), dwGameProtoInitSize - sizeof(GAMEPROTO_HEAD));
	ASSERT(pstGameProtoHead->dwCrc == dwCrc);

	ASSERT(pstGameProtoHead->wProtoId == (WORD)GAMEPROTO_INIT);

	DWORD dwTmp = *(DWORD*)(pbyGameProtoInitData + sizeof(GAMEPROTO_HEAD));
	dwUin = ntohl(dwTmp);

	dwTmp = *(DWORD*)(pbyGameProtoInitData + sizeof(GAMEPROTO_HEAD) + sizeof(DWORD));
	dwCliVer = ntohl(dwTmp);
}

void CTestGameSvrSocket::EncodeGameProto_AntiBot(BYTE *pbyAntiBotData, DWORD dwAntiBotSize, BYTE **ppbyGameProtoAntiBotData, DWORD& dwGameProtoAntiBotSize)
{
	dwGameProtoAntiBotSize = dwAntiBotSize + sizeof(GAMEPROTO_HEAD);
	*ppbyGameProtoAntiBotData = new BYTE[dwGameProtoAntiBotSize];
	
	memmove(*ppbyGameProtoAntiBotData + sizeof(GAMEPROTO_HEAD), pbyAntiBotData, dwAntiBotSize);
	
	GAMEPROTO_HEAD *pstGameProtoHead = (GAMEPROTO_HEAD*)(*ppbyGameProtoAntiBotData);
	pstGameProtoHead->dwSize = htonl(dwAntiBotSize);
	pstGameProtoHead->wProtoId = (WORD)GAMEPROTO_ANTIBOT;
	pstGameProtoHead->dwCrc = CCrc32::Crc32(*ppbyGameProtoAntiBotData + sizeof(GAMEPROTO_HEAD), dwGameProtoAntiBotSize - sizeof(GAMEPROTO_HEAD));
}
void CTestGameSvrSocket::EncodeGameProto_DynamicProto(BYTE *pbyAntiBotData, DWORD dwAntiBotSize, BYTE **ppbyGameProtoAntiBotData, DWORD& dwGameProtoAntiBotSize)
{
	dwGameProtoAntiBotSize = dwAntiBotSize + sizeof(GAMEPROTO_HEAD);
	*ppbyGameProtoAntiBotData = new BYTE[dwGameProtoAntiBotSize];
	
	memmove(*ppbyGameProtoAntiBotData + sizeof(GAMEPROTO_HEAD), pbyAntiBotData, dwAntiBotSize);
	
	GAMEPROTO_HEAD *pstGameProtoHead = (GAMEPROTO_HEAD*)(*ppbyGameProtoAntiBotData);
	pstGameProtoHead->dwSize = htonl(dwAntiBotSize);
	pstGameProtoHead->wProtoId = (WORD)GAMEPROTO_DPROTO;
	pstGameProtoHead->dwCrc = CCrc32::Crc32(*ppbyGameProtoAntiBotData + sizeof(GAMEPROTO_HEAD), dwGameProtoAntiBotSize - sizeof(GAMEPROTO_HEAD));
}

void CTestGameSvrSocket::EncodeGameProto(BYTE *pbyGameProtoData, DWORD dwGameProtoSize, WORD wProtoId, BYTE **ppbyData, DWORD& dwSize)
{
	dwSize = sizeof(GAMEPROTO_HEAD) + dwGameProtoSize;
	*ppbyData = new BYTE[dwSize];
	
	memmove(*ppbyData + sizeof(GAMEPROTO_HEAD), pbyGameProtoData, dwGameProtoSize);
	
	GAMEPROTO_HEAD *pstGameProtoHead = (GAMEPROTO_HEAD*)(*ppbyData);
	pstGameProtoHead->dwCrc = CCrc32::Crc32(*ppbyData + sizeof(GAMEPROTO_HEAD), dwSize - sizeof(GAMEPROTO_HEAD));
	pstGameProtoHead->dwSize = htonl(dwGameProtoSize);
	pstGameProtoHead->wProtoId = wProtoId;
}

void CTestGameSvrSocket::DecodeGameProto(BYTE *pbyGameProtoData, DWORD dwGameProtoSize, BYTE **ppbyData, DWORD& dwSize)
{
	dwSize = dwGameProtoSize - sizeof(GAMEPROTO_HEAD);
	*ppbyData = new BYTE[dwSize];
	
	GAMEPROTO_HEAD *pstGameProtoHead = (GAMEPROTO_HEAD*)pbyGameProtoData; 
	ASSERT(ntohl(pstGameProtoHead->dwSize) == dwSize);
	
	DWORD dwCrc = CCrc32::Crc32(pbyGameProtoData + sizeof(GAMEPROTO_HEAD), dwGameProtoSize - sizeof(GAMEPROTO_HEAD));
	ASSERT(dwCrc == pstGameProtoHead->dwCrc);
	
	memmove(*ppbyData, pbyGameProtoData + sizeof(GAMEPROTO_HEAD), dwSize);
}

void CTestGameSvrSocket::DecodeGameProto_AntiBot(BYTE *pbyGameProtoAntiBotData, DWORD dwGameProtoAntiBotSize, BYTE **ppbyAntiBotData, DWORD& dwAntiBotSize)
{
	dwAntiBotSize = dwGameProtoAntiBotSize - sizeof(GAMEPROTO_HEAD);
	*ppbyAntiBotData = new BYTE[dwAntiBotSize];
	
	GAMEPROTO_HEAD *pstGameProtoHead = (GAMEPROTO_HEAD*)pbyGameProtoAntiBotData; 
	ASSERT(ntohl(pstGameProtoHead->dwSize) == dwAntiBotSize);
	
	DWORD dwCrc = CCrc32::Crc32(pbyGameProtoAntiBotData + sizeof(GAMEPROTO_HEAD), dwGameProtoAntiBotSize - sizeof(GAMEPROTO_HEAD));
	ASSERT(dwCrc == pstGameProtoHead->dwCrc);
	
	ASSERT(pstGameProtoHead->wProtoId == (WORD)GAMEPROTO_ANTIBOT);
	memmove(*ppbyAntiBotData, pbyGameProtoAntiBotData + sizeof(GAMEPROTO_HEAD), dwAntiBotSize);
}

void CTestGameSvrSocket::DecodeGameProto_GameStatus(BYTE *pbyGameProtoGameStatus, DWORD dwGameProtoGameStatus, BYTE& byGameStatus)
{
	GAMEPROTO_HEAD *pstGameProtoHead = (GAMEPROTO_HEAD*)pbyGameProtoGameStatus;
	
	DWORD dwCrc = CCrc32::Crc32(pbyGameProtoGameStatus + sizeof(GAMEPROTO_HEAD), dwGameProtoGameStatus - sizeof(GAMEPROTO_HEAD));
	ASSERT(dwCrc == pstGameProtoHead->dwCrc);

	ASSERT(pstGameProtoHead->wProtoId == (WORD)GAMEPROTO_GAMESTATUS);

	byGameStatus = *(BYTE*)(pbyGameProtoGameStatus + sizeof(GAMEPROTO_HEAD));

	DebugOut("GameSvr:DecodeGameProto_GameStatus:%u", byGameStatus);
	//
}



void CTestGameSvrSocket::EncodeGAData(BYTE *pbyAntiBotData, DWORD dwAntiBotSize, BYTE **ppbyGAData, DWORD& dwGASize)
{
	dwGASize = dwAntiBotSize + sizeof(GADATA_HEAD);
	*ppbyGAData = new BYTE[dwGASize];
	GADATA_HEAD *pstGAHead = (GADATA_HEAD*)(*ppbyGAData);

	pstGAHead->dwSize = dwAntiBotSize;

	GAMESVR_SOCKET_INFO *pstSocketInfo = GetSvrSocket(this);
	ASSERT(pstSocketInfo);

	pstGAHead->dwUin = pstSocketInfo->dwUin;

	memmove(*ppbyGAData + sizeof(GADATA_HEAD), pbyAntiBotData, dwAntiBotSize);
	pstGAHead->dwCrc = CCrc32::Crc32(*ppbyGAData + sizeof(GADATA_HEAD), dwGASize - sizeof(GADATA_HEAD));
}

void CTestGameSvrSocket::AddSvrSocket(GAMESVR_SOCKET_INFO *pstSocketInfo)
{
	ASSERT(pstSocketInfo);
	m_vecSvrSocket.push_back(pstSocketInfo);
}

void CTestGameSvrSocket::DelSvrSocket(CTestGameSvrSocket *pSocket)
{
	ASSERT(pSocket);

	VECTOR_GAMESVR_SOCKET::iterator iter;

	GAMESVR_SOCKET_INFO *pstSocketInfo = NULL;
	for (iter = m_vecSvrSocket.begin(); iter != m_vecSvrSocket.end(); iter++)
	{
		pstSocketInfo = (GAMESVR_SOCKET_INFO*)(*iter);
		if (pstSocketInfo->pSocket == pSocket)
		{
			g_pIAntibotChecker->DeleteClient(1,1,pstSocketInfo->dwUin);
#ifdef  USEP2P
			g_pIAntibotChecker->P2PDeleteClient(pstSocketInfo->dwUin);
#endif 
			std::vector<unsigned int>::iterator iterUin;
			iterUin =std::find(g_vecUin.begin(), g_vecUin.end(), pstSocketInfo->dwUin);
			if (iterUin != g_vecUin.end())
			{
				g_vecUin.erase(iterUin);
			}


			delete pstSocketInfo->pSocket;
			delete pstSocketInfo;
			pstSocketInfo = NULL;
			m_vecSvrSocket.erase(iter);

			break;
		}
	}
}
void CTestGameSvrSocket::ClearSvrSocket()
{	
	VECTOR_GAMESVR_SOCKET::iterator iter;
	
	GAMESVR_SOCKET_INFO *pstSocketInfo = NULL;
	for (iter = m_vecSvrSocket.begin(); iter != m_vecSvrSocket.end(); iter++)
	{
		pstSocketInfo = (GAMESVR_SOCKET_INFO*)(*iter);
		delete pstSocketInfo->pSocket;
		delete pstSocketInfo;	
	}
	m_vecSvrSocket.clear();
}

GAMESVR_SOCKET_INFO* CTestGameSvrSocket::GetSvrSocket(CTestGameSvrSocket *pSocket)
{
	VECTOR_GAMESVR_SOCKET::iterator iter;
	GAMESVR_SOCKET_INFO *pstSocketInfo = NULL;
	try
	{
		for (iter = m_vecSvrSocket.begin(); iter != m_vecSvrSocket.end(); iter++)
		{
			pstSocketInfo = (GAMESVR_SOCKET_INFO*)(*iter);
			if (pstSocketInfo->pSocket == pSocket)
			{
				return pstSocketInfo;
			}
		}
	}
	catch(...)
	{
	
	}
	return NULL;
}
GAMESVR_SOCKET_INFO* CTestGameSvrSocket::GetSvrSocket(DWORD dwUin)
{
	try
	{
		VECTOR_GAMESVR_SOCKET::iterator iter;
		GAMESVR_SOCKET_INFO *pstSocketInfo = NULL;
		
		for (iter = m_vecSvrSocket.begin(); iter != m_vecSvrSocket.end(); iter++)
		{
			pstSocketInfo = (GAMESVR_SOCKET_INFO*)(*iter);
			if (pstSocketInfo->dwUin == dwUin)
			{
				return pstSocketInfo;
			}
		}
	}
	catch(...)
	{
	
	}
	
	return NULL;
}


void CTestGameSvrSocket::CreateSvrSocket(WORD wPort)
{
	ASSERT(wPort < 65535);

	int nRetCode = false;

//	m_pTabIpc = (ITabIpc*)CreateAntiObj(NULL);
	//ASSERT(m_pTabIpc);

	nRetCode = Create(wPort);
	ASSERT(nRetCode);

	nRetCode = Listen();
	ASSERT(nRetCode);
}

void CTestGameSvrSocket::SendData(BYTE *pbyData, DWORD dwSize)
{
	int nRetCode = false;
	DWORD dwLastError = 0;

	bool bDPData = false;
	
	GAMEPROTO_HEAD *pstGameProtoData = (GAMEPROTO_HEAD*)pbyData;

	BYTE *pbySndData = pbyData;
	DWORD dwSndSize = dwSize;
	
	GAMESVR_SOCKET_INFO *pstSocketInfo = GetSvrSocket(this);

	if ( g_pIAntibotChecker->IsCmdInDownList( pstSocketInfo->iKey, pstGameProtoData->wProtoId) )
	{

		ASSERT(pstSocketInfo);

		static BYTE s_byEncodeBuf[30000];
		static WORD s_wEncodeSize = 30000;
		
		nRetCode = g_pIAntibotChecker->RecvBufferForEncrypt(pstSocketInfo->iKey, 
			pbyData, 
			dwSize,
			s_byEncodeBuf,
			s_wEncodeSize);
		//ASSERT(nRetCode == 0);

		if(nRetCode == 0) 
		{
		EncodeGameProto(s_byEncodeBuf, s_wEncodeSize, GAMEPROTO_DPROTO, &pbySndData, dwSndSize);
		pstGameProtoData =(GAMEPROTO_HEAD*)pbySndData;

		bDPData = true;
		}
	}

	nRetCode = CAsyncSocket::Send(pbySndData, dwSndSize);
	dwLastError = ::GetLastError();
	if (nRetCode == SOCKET_ERROR)
	{
		if (dwLastError == WSAEWOULDBLOCK)
		{
			memmove(m_pbyCacheBufSnd + m_dwRemainSizeSnd, pbySndData, dwSndSize);
			m_dwRemainSizeSnd += dwSndSize;
			DebugOut("GameSvr:SendData SOCKET_ERROR Cached:%x", dwLastError);
		}
		else
		{
			DebugOut("GameSvr:SendData SOCKET_ERROR:%x", dwLastError);
			ASSERT(false);
		}
	}
	else if (nRetCode < dwSndSize)
	{
		memmove(m_pbyCacheBufSnd + m_dwRemainSizeSnd, pbySndData + nRetCode, dwSndSize - nRetCode);
		m_dwRemainSizeSnd += (dwSndSize - nRetCode);
		DebugOut("GameSvr:SendData PART:%u:%u", nRetCode, dwSndSize);
	}
	else if (nRetCode == dwSndSize)
	{
		DebugOut("GameSvr:SendData SUCCESS:%u", dwSndSize);
	}
	else
	{
		DebugOut("GameSvr:SendData ERROR");
		ASSERT(false);
	}

	if (bDPData)
	{
		delete[] pbySndData;
	}
}

void CTestGameSvrSocket::OnRcvData(BYTE *pbyPkgData, DWORD dwPkgSize)
{
	DebugOut("GameSvr:OnRcvData:%u", dwPkgSize);

	int nRetCode = false;
	BYTE s_byRawData[30000];
	WORD s_wRawSize = 30000;

	bool bDPData = false;

	BYTE *pbyRcvData = pbyPkgData;
	DWORD dwRcvSize = dwPkgSize;

	GAMEPROTO_HEAD *pstGameProtoHead = (GAMEPROTO_HEAD*)pbyPkgData;

	GAMESVR_SOCKET_INFO *pstSocketInfo = GetSvrSocket(this);
	WORD wProtoId = pstGameProtoHead->wProtoId;
    g_pIAntibotChecker->AntiCheatPacket(pstSocketInfo->iKey ,pstGameProtoHead->wProtoId);

	if (pstGameProtoHead->wProtoId == GAMEPROTO_DPROTO)
	{

		ASSERT(pstSocketInfo);
		DecodeGameProto(pbyPkgData, dwPkgSize, &pbyRcvData, dwRcvSize);
		DebugOutHex(pbyRcvData, dwRcvSize > 100 ? 100 : dwRcvSize);

		nRetCode = g_pIAntibotChecker->RecvBufferForDecrypt(pstSocketInfo->iKey,
			pbyRcvData,
			dwRcvSize,
			s_byRawData,
			s_wRawSize);
		if(  nRetCode != 1  )
		{
			//ASSERT(nRetCode == 0);
		}

		delete[] pbyRcvData;

		dwRcvSize = s_wRawSize;
		pbyRcvData = new BYTE[dwRcvSize];
		memmove(pbyRcvData, s_byRawData, dwRcvSize);
		pstGameProtoHead = (GAMEPROTO_HEAD*)pbyRcvData;

		bDPData = true;
	}

	if (g_pIAntibotChecker->IsCmdInUpList(pstSocketInfo->iKey,pstGameProtoHead->wProtoId))
	{
		if (wProtoId != GAMEPROTO_DPROTO)
		{
			DebugOut("GameSvr:OnRcvData:IsCmdInUpList:%u", pstGameProtoHead->wProtoId);
			//ASSERT(false);
		}
	}

	switch (pstGameProtoHead->wProtoId) 
	{
	case GAMEPROTO_INIT:
		{
			DWORD dwUin = 0, dwCliVer = 0;

			DecodeGameProto_Init(pbyRcvData, dwRcvSize, dwUin, dwCliVer);
			
			GAMESVR_SOCKET_INFO *pstSocketInfo = GetSvrSocket(this);
			ASSERT(pstSocketInfo);
			pstSocketInfo->dwUin = dwUin;
			pstSocketInfo->stUserInfo.dwUin = dwUin;

#ifdef  USEP2P
			//for (int i = 0 ;i < 200; i++)
			{
				pstSocketInfo->iKey = g_pIAntibotChecker->AddClient(1,1,pstSocketInfo->dwUin, 17320020, NULL);
				g_pIAntibotChecker->P2PAddClient(pstSocketInfo->dwUin,-1,17320020 );
			}
#else 

			pstSocketInfo->iKey = g_pIAntibotChecker->AddClient(1,1,pstSocketInfo->dwUin, 17320020, NULL);
#endif
			g_vecUin.push_back(pstSocketInfo->dwUin);
		}
		break;
	case GAMEPROTO_REQUESTDATAFORDP:
		{
			BYTE *pbyAntiBotData = NULL;
			DWORD dwAntiBotSize = 0;

			DecodeGameProto_AntiBot(pbyRcvData, dwRcvSize, &pbyAntiBotData, dwAntiBotSize);
			
			GAMESVR_SOCKET_INFO *pstSocketInfo = GetSvrSocket(this);
			ASSERT(pstSocketInfo);
			
			nRetCode = g_pIAntibotChecker->OnRecvBufferFromClient(1,1,pstSocketInfo->dwUin,
				pbyAntiBotData, 
				dwAntiBotSize) ;
			ASSERT(nRetCode != -1);
			delete[] pbyAntiBotData;
		}
		break;
	case GAMEPROTO_ANTIBOT:
		{
			BYTE *pbyAntiBotData = NULL;
			DWORD dwAntiBotSize = 0;

			DecodeGameProto_AntiBot(pbyRcvData, dwRcvSize, &pbyAntiBotData, dwAntiBotSize);
			
			GAMESVR_SOCKET_INFO *pstSocketInfo = GetSvrSocket(this);
			ASSERT(pstSocketInfo);
			
			nRetCode = g_pIAntibotChecker->OnRecvBufferFromClient(1,1,pstSocketInfo->dwUin,
				pbyAntiBotData, 
				dwAntiBotSize) ;
			ASSERT(nRetCode != -1);
			delete[] pbyAntiBotData;
		}
		break;
	case GAMEPROTO_DPROTO:
		ASSERT(false);
		break;
	case GAMEPROTO_GAMESTATUS:
		{
			BYTE byGameStatus = 0;
			DecodeGameProto_GameStatus(pbyRcvData, dwRcvSize, byGameStatus);
			DebugOut("OnGameStatus:%u", byGameStatus);
		}
		break;

	case UDP_CMD_GET_USER :
		{
				int i = 0;
				VECTOR_GAMESVR_SOCKET::iterator iter;
				GAMESVR_SOCKET_INFO *pstSocketInfo = NULL;
				USER_INFO *pUserifo;
				int dwGameProtoAntiBotSize = sizeof(USER_INFO)*100 + sizeof(CS_UDP_DATA_HEAD_NO_OPT);
				BYTE *ppbyGameProtoAntiBotData = new BYTE[dwGameProtoAntiBotSize];
				CS_UDP_DATA_HEAD_NO_OPT *pstGameProtoHead = (CS_UDP_DATA_HEAD_NO_OPT*)(*ppbyGameProtoAntiBotData);
				pstGameProtoHead->dwUin= 0;
				pstGameProtoHead->wProtoId= 8;
				for (iter = m_vecSvrSocket.begin(); iter != m_vecSvrSocket.end(); iter++)
				{	
					i++;
					pstSocketInfo = (GAMESVR_SOCKET_INFO*)(*iter);
				
					pUserifo = (USER_INFO *)(ppbyGameProtoAntiBotData+sizeof(CS_UDP_DATA_HEAD_NO_OPT ) + (i-1)*sizeof(USER_INFO) );
					strcpy((char *)pUserifo,(char *)&(pstSocketInfo->stUserInfo));
					pstGameProtoHead->byOptSize = i*sizeof(USER_INFO);
				}
				SendData(ppbyGameProtoAntiBotData,pstGameProtoHead->byOptSize + sizeof(USER_INFO) );
			 //  CS_UDP_DATA_HEAD *pCSUDPDATA = pstGameProtoHead;	
		}
		break;
	case UDP_CMD_TRANSMIT :
		{
			//CS_UDP_DATA_HEAD *pCSUDPDATA = pstGameProtoHead;
		     CS_UDP_DATA_HEAD *pCSUDPDATA = (CS_UDP_DATA_HEAD *)pstGameProtoHead;
			 GAMESVR_SOCKET_INFO *pstSocketInfo = GetSvrSocket(pCSUDPDATA->dwUin);
			 pstSocketInfo->pSocket->SendData(pbyPkgData, dwPkgSize);
			 //PUSER_INFO pUserinfo= (PUSER_INFO)(pstGameProtoHead + sizeof(CS_UDP_DATA_HEAD ) );
			 //strcpy((char *)&(pstSocketInfo->stUserInfo),(char *)pUserinfo);
		}
		break;
	case UDP_CMD_LOGIN_SERVER :
		{
		     CS_UDP_DATA_HEAD *pCSUDPDATA = (CS_UDP_DATA_HEAD *)pstGameProtoHead;
			 GAMESVR_SOCKET_INFO *pstSocketInfo = GetSvrSocket(this);
			 PUSER_INFO pUserinfo= (PUSER_INFO)(pstGameProtoHead + sizeof(CS_UDP_DATA_HEAD ) );
			 strcpy((char *)&(pstSocketInfo->stUserInfo),(char *)pUserinfo);
		}
		break;

	default:
		//ASSERT(false);
		break;
	}

	if (bDPData)
	{
		delete[] pbyRcvData;
	}
}

void CTestGameSvrSocket::OnConnect(int nErrorCode)
{
	DebugOut("GameSvr:OnConnect:%u", nErrorCode);

	CAsyncSocket::OnConnect(nErrorCode);
}

void CTestGameSvrSocket::OnAccept(int nErrorCode)
{
	DebugOut("GameSvr:OnAccept:%u", nErrorCode);

	GAMESVR_SOCKET_INFO *pstSocketInfo = new GAMESVR_SOCKET_INFO;
	pstSocketInfo->pSocket = new CTestGameSvrSocket();
	ASSERT(pstSocketInfo && pstSocketInfo->pSocket);

	int nRetCode = false;

	nRetCode = Accept(*(pstSocketInfo->pSocket));
	ASSERT(nRetCode);
	
	CString strGameClientIp;
	UINT unPort = 0;
	nRetCode = pstSocketInfo->pSocket->GetPeerName(strGameClientIp, unPort);
	ASSERT(nRetCode);
	strcpy(pstSocketInfo->szGameClientIp, strGameClientIp);
	strcpy(pstSocketInfo->stUserInfo.szIp, strGameClientIp);
	pstSocketInfo->wGameClientPort = (WORD)unPort;
	pstSocketInfo->stUserInfo.unTcpPort = unPort;

	
	pstSocketInfo->dwLoginTime = time(NULL);

	AddSvrSocket(pstSocketInfo);
	
	CAsyncSocket::OnAccept(nErrorCode);
}

void CTestGameSvrSocket::OnReceive(int nErrorCode)
{
//	DebugOut("GameSvr:OnReceive:%u", nErrorCode);

	while (true)
	{
		int nRcvSize = Receive(m_pbyCacheBufRcv + m_dwRemainSizeRcv, MAX_CACHE_BUF_SIZE - m_dwRemainSizeRcv);
		if (nRcvSize <= 0)
			break;
		
		int nTotalSize = nRcvSize + m_dwRemainSizeRcv;
		
		//
		int nProcSize = 0;
		bool bNotGetFullPkg = false;
		do 
		{
			BYTE *pbyBuf = m_pbyCacheBufRcv + nProcSize;
			DWORD dwPkgSize = ntohl(*(DWORD*)pbyBuf) + sizeof(GAMEPROTO_HEAD);
			ASSERT(dwPkgSize < MAX_PKG_SIZE);
			if (nProcSize + dwPkgSize >= nTotalSize)
			{
				bNotGetFullPkg = true;
			}
			
			if (nProcSize + dwPkgSize <= nTotalSize)
			{
				BYTE *pbyPkgData = new BYTE[dwPkgSize];
				memmove(pbyPkgData, pbyBuf, dwPkgSize);
				OnRcvData(pbyPkgData, dwPkgSize);
				delete[] pbyPkgData;
				
				nProcSize += dwPkgSize;
			}
		} while(!bNotGetFullPkg);
		
		m_dwRemainSizeRcv = nTotalSize - nProcSize;
		memmove(m_pbyCacheBufRcv, m_pbyCacheBufRcv + nProcSize, m_dwRemainSizeRcv);
	}

	CAsyncSocket::OnReceive(nErrorCode);
}

void CTestGameSvrSocket::OnSend(int nErrorCode)
{
	int nRetCode = false;
	DWORD dwLastError = 0;
	
	while (m_dwRemainSizeSnd > 0)
	{
		nRetCode = Send(m_pbyCacheBufSnd, m_dwRemainSizeSnd);
		dwLastError = ::GetLastError();
		if (nRetCode == SOCKET_ERROR)
		{
			if (dwLastError == WSAEWOULDBLOCK)
			{
				DebugOut("GameSvr:OnSend SOCKET_ERROR WSAEWOULDBLOCK");
				break;
			}
			else
			{
				DebugOut("GameSvr:OnSend ERROR");
				ASSERT(false);
			}
		}
		else
		{
			memmove(m_pbyCacheBufSnd, m_pbyCacheBufSnd + nRetCode, m_dwRemainSizeSnd - nRetCode);
			m_dwRemainSizeSnd -= nRetCode;
			DebugOut("GameSvr:OnSend:%u:%u", nRetCode, m_dwRemainSizeSnd);
		}
	}

	CAsyncSocket::OnSend(nErrorCode);
}

void CTestGameSvrSocket::OnClose(int nErrorCode)
{
	DebugOut("GameSvr:OnClose:%u");

	GAMESVR_SOCKET_INFO *pSockInfo = GetSvrSocket(this);
	g_pIAntibotChecker->DeleteClient(1,1,pSockInfo->dwUin);	
#ifdef  USEP2P
	g_pIAntibotChecker->P2PDeleteClient(pSockInfo->dwUin);
#endif 
	DelSvrSocket(this);

	CAsyncSocket::OnClose(nErrorCode);
}
