/******************************************************************************
  �ļ���          : mainctrlbusiness.h
  �汾��          : 1.0
  ����            : allanxu
  ��������        : 2006-2-20
  �޸���ʷ        : 
  1 ����          : 2006-2-20
    ����          : allanxu
    �޸�����      : ����
*******************************************************************************/

#ifndef __MAINCTRLBUSINESS_H__
#define __MAINCTRLBUSINESS_H__

#include "ibusiness.h"

class CMainCtrlBusiness:public IBusiness
{
public:
	CMainCtrlBusiness();
	int Initialize();
	int SendBuffertoClient(int iType,int iGameID,int iRoomID,unsigned int iUin , unsigned char* abySendBuffer,short shSize);
	int PunishClient(int iGameID,int iRoomID,unsigned int iUin, EPUNISHMODE enPunishMode,AntibotPunishInfo* pstAntibotPunishInfo,char* szPunishReason);
};

#endif /* __MAINCTRLBUSINESS_H__ */
