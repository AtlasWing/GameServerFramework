/******************************************************************************
  File Name       : business.h
  Version         : 2.0
  Author          : leochen
  Created         : 2007-3-7
  Description     : Business类是抽象类，描述了一组跟业务相关的，由antibot调用的接口
  History         : 
  1 Date          : 2006-3-7
    Author        : leochen
    Modification  : Created file
******************************************************************************/

#ifndef __IBUSINESS_H__
#define __IBUSINESS_H__

typedef struct tagAntibotPunishInfo
{
	int iGameID;
	int iBanTimeSecond;
}AntibotPunishInfo;

typedef enum 
{
	NORMAL = 0,
	WRITELOG = 1,
	KICKOUT = 2,
	BANPLAYER = 3 ,
	TROJAN_DETECT = 4	//新增，木马提示
}EPUNISHMODE;


/*iType = 0 packet of Antibot*/
/*iType = 1 packet of VM*/

class IBusiness
{
public:
	virtual int SendBuffertoClient(int iType, int iGameID,int iRoomID,unsigned int uiUin ,unsigned char* abySendBuffer,short shSize) = 0;
	virtual int PunishClient(int iGameID,int iRoomID,unsigned int uiUin, EPUNISHMODE enPunishMode, AntibotPunishInfo* pstAntibotPunishInfo,char* szPunishReason) = 0;
};

#endif /* __BUSINESS_H__ */
