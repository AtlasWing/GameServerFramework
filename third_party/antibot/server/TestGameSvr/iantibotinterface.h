/******************************************************************************
  �ļ���          : antibotinterface.h
  �汾��          : 1.0
  ����            : leochen
  ��������        : 2007-2-27
  ��������        : �����ͳһ����ӿ�
  �޸���ʷ        : 
  1 ����          : 2007-2-27
    ����          : leochen
    �޸�����      : ����
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

//��Ϸ����Ҫ�������½ṹTUserInfo���Ա�addclient��ʱ���롣
namespace Antibot_Interface
{

	#define USERINFOPLAYERNAMELEN 64
	struct TUserInfo
	{
		char szPlayerName[USERINFOPLAYERNAMELEN];
		int iUserID;//�����Ϸ��ҪUserID�Ļ����ò�����һ���������ֵ������ò���Ϊ��1
		//����������...
	};
}
/*QQGame OnNotifyServer�ӿڵĽṹ����
1.��ҵ���֪ͨ uiMainID = 200001	�������
2.�������֪ͨ uiMainID = 200002	�������
typedef tagPlayerState
{
	unsigned int uiUin;
	int iGameID;
	int iRoomID;
}PLAYERSTATE,*PPLAYERSTATE
*/

// �ı� ��ɫ��ʱ����Ҫ�� ���� .  
typedef struct stChangRole
{
	int iUin ;              //��ҵ�Uin
	int iUserID	;		//�û�id
#define ROLENAMELENGTH 64
	char szPlayerName[ROLENAMELENGTH] ;   //���״̬ ��Ŀǰ����ȫ����0 
}stChangRole;


class IAntibotInterface
{
public:

	//���нӿ�
	virtual int Initialize(IBusiness *pstBusiness,char *szIPCPathName) = 0;
	virtual int AddClient(int iGameID,int iRoomID,unsigned int uiUin,unsigned int uiCliVer,Antibot_Interface::TUserInfo *pstUserInfo) = 0;
	virtual int DeleteClient(int iGameID,int iRoomID,unsigned int uiUin) = 0;
	virtual int OnRecvBufferFromClient(int iGameID,int iRoomID,unsigned int uiUin,unsigned char* pabyBuffer,unsigned short nBufferSize) = 0;
	virtual int ProcessAntiBotInput() = 0;
	virtual int CheckTimer(int iGameID,int iRoomID,unsigned int uiUin) = 0;
	virtual void SetLogPath(char *pszLogBasePath) = 0;   
    virtual void ShowInfo() = 0;

	//��̬Э��ջ�Ľӿ�
	virtual int P2PAddClient(unsigned int uiUin, int iUserID, unsigned int uiCliVersion) = 0;
	virtual int P2PDeleteClient(unsigned int uiUin) = 0;
	virtual int RecvBufferForEncrypt(unsigned int uiKey,unsigned char* pabyInBuffer,unsigned short nInSize,unsigned char* pabyOutBuffer,unsigned short& nOutSize) = 0;
	virtual int RecvBufferForDecrypt(unsigned int uiKey,unsigned char* pabyInBuffer,unsigned short nInSize,unsigned char* pabyOutBuffer,unsigned short& nOutSize) = 0;
	virtual int IsCmdInUpList( unsigned int uiKey,int iCmdID) = 0;
	virtual int IsCmdInDownList( unsigned int uiKey,int iCmdID) = 0;

	//��̬����Ľӿ�
        virtual int OnNotifyServer(int iGameID,int iRoomID,unsigned int uiMainId,unsigned char* pBuffer,short nSize) = 0;
	
	//-1 ��ʾӦ����Ҫ���ܵİ���û�м��ܣ�0 ��ʾ����
	virtual int AntiCheatPacket(unsigned int uiKey,int iCmdID) = 0;
};


#ifdef WIN32

#ifdef _DEBUG
#define TERSAFES_DLL "antisvrimportd.dll"
#else
#define TERSAFES_DLL "antisvrimport.dll"
#endif

//! pszSafeDllPath�����ΪNULLʱ������'\'�ָ��
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

//! pszSafeDllPath�����ΪNULLʱ������'\'�ָ��
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
