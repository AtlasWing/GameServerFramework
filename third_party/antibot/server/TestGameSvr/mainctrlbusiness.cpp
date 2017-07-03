
#include "stdafx.h"
#include "ibusiness.h"
#include "mainctrlbusiness.h"
//#include "iantibotchecker.h"
#include "TestGameSvrSocket.h"


//using namespace ANTIBOT_SYS_PROCESS_NS;

//typedef Singleton<CStatistic> CStatisticSingleton;
extern CTestGameSvrSocket *g_pSvrListenSocket;

CMainCtrlBusiness::CMainCtrlBusiness()
{

}

int CMainCtrlBusiness::Initialize()
{

	return 0;
}

int CMainCtrlBusiness::SendBuffertoClient(int iType,int iGameID,int iRoomID,unsigned int iUin , unsigned char* abySendBuffer,short shSize)
{
	GAMESVR_SOCKET_INFO *pSocketInfo = g_pSvrListenSocket->GetSvrSocket(iUin);
	if(pSocketInfo == NULL)
	{return 0;}
	ASSERT(pSocketInfo);

	BYTE *pbyGameProtoAntiBotData = NULL;
	DWORD dwGameProtoAntiBotSize = 0;
	
	short nSize = shSize;
	if(iType==0)
	{
	pSocketInfo->pSocket->EncodeGameProto_AntiBot(abySendBuffer, nSize, &pbyGameProtoAntiBotData, dwGameProtoAntiBotSize);
	}
	else
	{
	pSocketInfo->pSocket->EncodeGameProto_DynamicProto(abySendBuffer, nSize, &pbyGameProtoAntiBotData, dwGameProtoAntiBotSize);
	}

	pSocketInfo->pSocket->SendData(pbyGameProtoAntiBotData, dwGameProtoAntiBotSize);
	delete[] pbyGameProtoAntiBotData;
	return 0;
}


int CMainCtrlBusiness::PunishClient(int iGameID,int iRoomID,unsigned int iUin, EPUNISHMODE enPunishMode,AntibotPunishInfo* pstAntibotPunishInfo,char* szPunishReason)
{
	//int i;
	return 0 ;
}


