/******************************************************************************
  文件名          : antibotinterface.h
  版本号          : 1.0
  作者            : leochen
  生成日期        : 2007-2-27
  功能描述        : 反外挂统一对外接口
  修改历史        : 
  1 日期          : 2007-2-27
    作者          : leochen
    修改内容      : 生成
*******************************************************************************/

#ifndef __IANTIBOTINTERFACE_H__
#define __IANTIBOTINTERFACE_H__

#ifdef WIN32
#include <Windows.h>
#else
/*#include <dlfcn.h>
#include <string.h>
#include <stdio.h>*/
#endif

class IBusiness;

//游戏方需要声明如下结构TUserInfo，以便addclient的时候传入。
namespace Antibot_Interface
{

	#define USERINFOPLAYERNAMELEN 64
	struct TUserInfo
	{
		char szPlayerName[USERINFOPLAYERNAMELEN];
		int iUserID;//如果游戏需要UserID的话，该参数是一个有意义的值，否则该参数为－1
		//待后续增加...
	};
}
/*QQGame OnNotifyServer接口的结构定义
1.玩家掉线通知 uiMainID = 200001	玩家离线
2.玩家重连通知 uiMainID = 200002	玩家重连
typedef tagPlayerState
{
	unsigned int uiUin;
	int iGameID;
	int iRoomID;
}PLAYERSTATE,*PPLAYERSTATE
*/

// 改变 角色的时候许要的 参数 .  
typedef struct stChangRole
{
	int iUin ;              //玩家的Uin
	int iUserID	;		//用户id
#define ROLENAMELENGTH 64
	char szPlayerName[ROLENAMELENGTH] ;   //玩家状态 ，目前可以全部填0 
}stChangRole;


class IAntibotInterface
{
public:

	//共有接口
	virtual int Initialize(IBusiness *pstBusiness,char *szIPCPathName) = 0;
	virtual int AddClient(int iGameID,int iRoomID,unsigned int uiUin,unsigned int uiCliVer,Antibot_Interface::TUserInfo *pstUserInfo) = 0;
	virtual int DeleteClient(int iGameID,int iRoomID,unsigned int uiUin) = 0;
	virtual int OnRecvBufferFromClient(int iGameID,int iRoomID,unsigned int uiUin,unsigned char* pabyBuffer,unsigned short nBufferSize) = 0;
	virtual int ProcessAntiBotInput() = 0;
	virtual int CheckTimer(int iGameID,int iRoomID,unsigned int uiUin) = 0;
	virtual void SetLogPath(char *pszLogBasePath) = 0;   
    virtual void ShowInfo() = 0;

	//动态协议栈的接口
	virtual int P2PAddClient(unsigned int uiUin, int iUserID, unsigned int uiCliVersion) = 0;
	virtual int P2PDeleteClient(unsigned int uiUin) = 0;
	virtual int RecvBufferForEncrypt(unsigned int uiKey,unsigned char* pabyInBuffer,unsigned short nInSize,unsigned char* pabyOutBuffer,unsigned short& nOutSize) = 0;
	virtual int RecvBufferForDecrypt(unsigned int uiKey,unsigned char* pabyInBuffer,unsigned short nInSize,unsigned char* pabyOutBuffer,unsigned short& nOutSize) = 0;
	virtual int IsCmdInUpList( unsigned int uiKey,int iCmdID) = 0;
	virtual int IsCmdInDownList( unsigned int uiKey,int iCmdID) = 0;

	//动态代码的接口
        virtual int OnNotifyServer(int iGameID,int iRoomID,unsigned int uiMainId,unsigned char* pBuffer,short nSize) = 0;
	
	//-1 表示应该需要加密的包而没有加密，0 表示正常
	virtual int AntiCheatPacket(unsigned int uiKey,int iCmdID) = 0;
};


#ifdef WIN32

#ifdef _DEBUG
#define TERSAFES_DLL "antisvrimportd.dll"
#else
#define TERSAFES_DLL "antisvrimport.dll"
#endif

//! pszSafeDllPath如果不为NULL时最后需带'\'分割符
inline IAntibotInterface* CreateAntiServerObj(const char *pszSafeDllPath = NULL)
{
	HMODULE hAntiBot = ::GetModuleHandle(TERSAFES_DLL);
	if (!hAntiBot)
	{
		char szPath[MAX_PATH] = {0};
		
		if (!pszSafeDllPath)
		{
			::GetModuleFileName(NULL, szPath, MAX_PATH);
			*(strrchr(szPath, '\\') + 1) = 0;
		}
		else
		{
			strcpy(szPath, pszSafeDllPath);
		}
		strcat(szPath, TERSAFES_DLL);
		hAntiBot = ::LoadLibrary(szPath);
	}

	typedef IAntibotInterface* (CREATEOBJ)();
	CREATEOBJ *pCreateObj = (CREATEOBJ*)::GetProcAddress(hAntiBot, "GetAntiBotInterfaceAll");
	
	return pCreateObj();
}

#else

#define TERSAFES_DLL "libantisvrimport.so"

//! pszSafeDllPath如果不为NULL时最后需带'\'分割符
inline IAntibotInterface* CreateAntiServerObj(const char *pszSafeDllPath = "./")
{
	char szPath[260] = {0};
	strcat(szPath, pszSafeDllPath);
	strcat(szPath, TERSAFES_DLL);

	
	void *pDllHandle = dlopen(szPath,RTLD_NOW);
	char *pcError = dlerror();
    if( pcError )
    {
		printf("Open Dll %s failed, %s\n", TERSAFES_DLL, pcError);
        return NULL;
    }

	typedef IAntibotInterface* (CREATEOBJ)();
	CREATEOBJ *pCreateObj = (CREATEOBJ*)dlsym(pDllHandle,"GetAntiBotInterfaceAll");
	pcError = dlerror();
    if( pcError )
    {
		dlclose(pDllHandle);
		printf("dlsym failed, %s\n", pcError);
        return NULL;
    }
	return (*pCreateObj)();
}

#endif


#endif /* __ANTIBOTINTERFACE_H__ */
