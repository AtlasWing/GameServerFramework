#ifndef __IANTIBOTINTERFACE_H__
#define __IANTIBOTINTERFACE_H__

#if defined(WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#endif

#include "ibusiness.h"

//��Ϸ����Ҫ�������½ṹTUserInfo���Ա�addclient��ʱ���롣
#define USERINFOPLAYERNAMELEN 64
struct TUserInfo
{
    //The role's name of the user
    char szPlayerName[USERINFOPLAYERNAMELEN];
    //The attribute is used to accelerate the access/query of UID for game server. It is the UID's offset in game server.
    //If you don't use it. please fill it with -1.
    int iUserID;

    //The IP or region id of game server.
    unsigned int uiSvrIp;
    //The IP of the user.
    unsigned int uiCltIp;
};

/*QQGame OnNotifyServer�ӿڵĽṹ����
1.��ҵ���֪ͨ uiMainID = 200001    �������
2.�������֪ͨ uiMainID = 200002    �������
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
    int iUserID    ;        //�û�id
#define ROLENAMELENGTH 64
    char szPlayerName[ROLENAMELENGTH] ;   //���״̬ ��Ŀǰ����ȫ����0 
}stChangRole;

class IAntibotInterface
{
public:

    //commment interface
    virtual int Initialize(IBusiness *pstBusiness,char *szIPCPathName) = 0;
    /**
    Function Name: AddClient
    Description :
        When a user login, game server need to call the function.
    Parameters:
        iGameId
            [in] It is used to distinguish the process of the game server.
        iRoomID
            [in] The parameter is used by mini game of tencent. Other game fill it with zero.
        uiUin
            [in] The QQ Number of the user.
        uiCliVer
            [in] The version of the client's executable file.
        pstUserInfo
            [in] The other informations of the user. Please reference the definition of the structure TUserInfo.
    Return Value :
        The offset of the UID in dp. It will be used in the function of dp, example : RecvBufferForEncrypt, IsCmdInUpList.
    **/
    virtual int AddClient(int iGameID,int iRoomID,unsigned int uiUin,unsigned int uiCliVer,TUserInfo *pstUserInfo) = 0;
    virtual int DeleteClient(int iGameID,int iRoomID,unsigned int uiUin) = 0;
    virtual int OnRecvBufferFromClient(int iGameID,int iRoomID,unsigned int uiUin,unsigned char* pabyBuffer,unsigned short nBufferSize) = 0;
    virtual int ProcessAntiBotInput() = 0;
    virtual int CheckTimer(int iGameID,int iRoomID,unsigned int uiUin) = 0;
    virtual void SetLogPath(char *pszLogBasePath) = 0;   
    virtual void ShowInfo() = 0;

    //interface of dp
    virtual int P2PAddClient(unsigned int uiUin, int iUserID, unsigned int uiCliVersion) = 0;
    virtual int P2PDeleteClient(unsigned int uiUin) = 0;
    virtual int RecvBufferForEncrypt(unsigned int uiKey,unsigned char* pabyInBuffer,unsigned int nInSize,unsigned char* pabyOutBuffer,unsigned int& nOutSize) = 0;
    virtual int RecvBufferForDecrypt(unsigned int uiKey,unsigned char* pabyInBuffer,unsigned short nInSize,unsigned char* pabyOutBuffer,unsigned short& nOutSize) = 0;
    
    /**
    Function Name : IsCmdInUpList
    Parameters:
        uiKey
            [in] uiKey is the return value of AddClient, and it is same as that in RecvBufferForEncrypt.
        CmdID
            [in] CmdID is the protocol id of the message.
    **/
    virtual int IsCmdInUpList(unsigned int uiKey, int CmdID) = 0;
    virtual int IsCmdInDownList(unsigned int uiKey, int CmdID) = 0;

    //interface of antibot
    virtual int OnNotifyServer(int iGameID,int iRoomID,unsigned int uiMainId,unsigned char* pBuffer,short nSize) = 0;
    
    /**
    Return Value: 
        -1 indicates the message should be encrypted, but isn't encryted;
        0  indicates all is normal.
    **/
    virtual int AntiCheatPacket(unsigned int uiKey,int CmdID) = 0;

    /**
    Function Name:    QueryProtectPolicy
    Description:
        Caller can query the safe level of the UID.
    Parameters:
        iGameID, iRoomId, uiUin
            [in]    The meaning of these parameters is same as in AddClient.
        pstSecSafeMsg
            [out]    The function will return the information about safe level by pstSecSafeMsg.
    Return Values: 
        1  indicates caller need to process the result
        0  indicates caller don't need to process the result
    **/
    virtual int QueryProtectPolicy(int iGameID, int iRoomId, unsigned int uiUin, SSafeModePkg *pstSecSafeMsg) = 0;
};


#if defined(WIN32) || defined(_WIN64)

#ifdef _DEBUG
#define TERSAFES_DLL "antisvrimportd.dll"
#else
#define TERSAFES_DLL "antisvrimport.dll"
#endif

//! pszSafeDllPath�����ΪNULLʱ������'\'�ָ��
inline IAntibotInterface* CreateAntiServerObj(const char *pszSafeDllPath = NULL)
{
 //   HMODULE hAntiBot = ::GetModuleHandle(TERSAFES_DLL);
 //   if (!hAntiBot)
 //   {
 //       char szPath[MAX_PATH] = {0};
 //       
 //       if (!pszSafeDllPath)
 //       {
 //           ::GetModuleFileName(NULL, szPath, MAX_PATH);
 //           *(strrchr(szPath, '\\') + 1) = 0;
 //       }
 //       else
 //       {
 //           strncpy(szPath, pszSafeDllPath, strlen(pszSafeDllPath));
 //       }
 //       strncat(szPath, TERSAFES_DLL, strlen(TERSAFES_DLL));
 //       hAntiBot = ::LoadLibrary(szPath);
 //   }

	//if(!hAntiBot)
	//{
	//	return NULL;
	//}

 //   typedef IAntibotInterface* (CREATEOBJ)();
 //   CREATEOBJ *pCreateObj = (CREATEOBJ*)::GetProcAddress(hAntiBot, "GetAntiBotInterfaceAll");
 //   
 //   return pCreateObj();
	return NULL;
}

#else

#define TERSAFES_DLL "libantisvrimport.so"

//! pszSafeDllPath�����ΪNULLʱ������'\'�ָ��
inline IAntibotInterface* CreateAntiServerObj(const char *pszSafeDllPath = "./")
{
    char szPath[260] = {0};
    strncat(szPath, pszSafeDllPath, strlen(pszSafeDllPath));
    strncat(szPath, TERSAFES_DLL , strlen(TERSAFES_DLL));
    
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
