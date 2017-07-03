#pragma once

#include <windows.h>

#define NS_USING_ANTIBOT using namespace AntiBot;

#ifdef USE_ANTIHLP	//�Ƿ�ʹ�ù��߽ӿ�
#include "AntiHlpImport.h"
#endif//USE_ANTIHLP

#ifdef USE_STYPE	//�Ƿ�ʹ�ð�ȫ��������
#include "AntiSDTImport.h"
#endif//USE_STYPE

#ifdef USE_SINPUT//! SafeInput�ӿ�
#include "AntiSInputImport.h"
#endif//USE_SINPUT

#ifdef USE_DPROTO	//! ��̬Э��ջsdk�ӿ�
#include "DProto.h"
#endif//USE_DPROTO


namespace AntiBot {


//! ��Ϸ�ͻ�����Ҫʵ�ֵĻص��ӿڣ�ʵ�ְ����ݷ��͵���������
//! ���������copy���øú�����������
class IClientSink
{
public:
	virtual int SendDataToSvr(BYTE *pbyData, DWORD dwSize) = 0;
};

class IAntiBot
{
public:
	//! �ͻ�����������������Gamever��uin�ͷ�����svr�Ļص��ӿ�
	virtual void SetUserInfo(DWORD dwGameVer, DWORD dwUin, IClientSink *pSink) = 0;
	//! ��Ϸ�������ʼ�����ϱ����ز������ݸ�antibotsvr
	virtual void SendInitData() = 0;

	//! �յ�antibot������ݺ���ã��������ͷ�pbyData�ڴ�
	virtual void OnRcvAntiData(BYTE *pbyData, DWORD dwSize) = 0;
	
	//! ֹͣ�����������
	virtual void StopLocalCheck() = 0;

	//! gameclient����������ʼһ�μ��
	virtual void StartLocalOnceCheck() = 0;

	//! �رտͻ���ǰ����ͷ�AntiBot�ڴ�
	virtual void Release() = 0;
};
//! ����AntiBot�ӿڵ�һʵ�������صĶ�����Ҫʹ��Release()�ͷ�
IAntiBot* CreateAntiBot();


#ifdef USE_DYNA_LIB

enum
{
	ANTI_OBJ_ERROR = 0,
	ANTI_OBJ_ANTIBOT = 1,
	ANTI_OBJ_ANTIBOT2 = 2,
	ANTI_OBJ_ANTIBOT3 = 3,
	ANTI_OBJ_CLIENTCHECKER = 4,
	ANTI_OBJ_SMOUSE = 5,
	ANTI_OBJ_SKBD = 6,
	ANTI_OBJ_DPROTO = 7,
	ANTI_OBJ_HELPER = 8,
	ANTI_OBJ_STYPE_ARITH = 9
};
#ifdef _DEBUG
#define TERSAFE_DLL "TerSafed.dll"
#else
#define TERSAFE_DLL	"TerSafe.dll"
#endif
//! pszSafeDllPath�����ΪNULLʱ������'\'�ָ��
inline void* CreateAntiObj(BYTE byObjIdx, const char *pszSafeDllPath = NULL)
{
	HMODULE hAntiBot = ::GetModuleHandle(TERSAFE_DLL);
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
			strncpy(szPath, pszSafeDllPath, sizeof(szPath)-1);
		}

		unsigned int cRemaining = (sizeof szPath - strlen(szPath)) - 1;
		strncat(szPath, TERSAFE_DLL, cRemaining);
		hAntiBot = ::LoadLibrary(szPath);
		// ASSERT(hAntiBot);
	}

	typedef void* (CREATEOBJ)(BYTE byObjIdx);
	CREATEOBJ *pCreateObj = (CREATEOBJ*)::GetProcAddress(hAntiBot, "CreateObj");
	
	return pCreateObj(byObjIdx);
}

#else//USE_DYNA_LIB

#ifdef INJECT_CODE

#ifdef _DEBUG
#pragma comment(lib, "AntiBotSdkjd.lib")
#else
#pragma comment(lib, "AntiBotSdkj.lib")
#endif

#else//INJECT_CODE

#ifdef USE_DPROTO
#ifdef _DEBUG
#pragma comment(lib, "VMCoreD.lib")
#else
#pragma comment(lib, "VMCore.lib")
#endif//_DEBUG
#endif//USE_DPROTO


#endif//INJECT_CODE

#endif//USE_DYNA_LIB

}//end namespace antibot