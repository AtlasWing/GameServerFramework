/******************************************************************************
  文件名          : mainctrlbusiness.h
  版本号          : 1.0
  作者            : allanxu
  生成日期        : 2006-2-20
  修改历史        : 
  1 日期          : 2006-2-20
    作者          : allanxu
    修改内容      : 生成
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
