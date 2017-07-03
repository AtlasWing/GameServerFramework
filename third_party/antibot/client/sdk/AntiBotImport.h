#pragma once

#include <windows.h>

#define NS_USING_ANTIBOT using namespace AntiBot;

#ifdef USE_ANTIHLP	//是否使用工具接口
#include "AntiHlpImport.h"
#endif//USE_ANTIHLP

#ifdef USE_STYPE	//是否使用安全数据类型
#include "AntiSDTImport.h"
#endif//USE_STYPE

#ifdef USE_SINPUT//! SafeInput接口
#include "AntiSInputImport.h"
#endif//USE_SINPUT

#ifdef USE_DPROTO	//! 动态协议栈sdk接口
#include "DProto.h"
#endif//USE_DPROTO


namespace AntiBot {


//! 游戏客户端需要实现的回调接口，实现把数据发送到服务器，
//! 建议把数据copy后让该函数立即返回
class IClientSink
{
public:
	virtual int SendDataToSvr(BYTE *pbyData, DWORD dwSize) = 0;
};

class IAntiBot
{
public:
	//! 客户端启动后立即设置Gamever、uin和反馈给svr的回调接口
	virtual void SetUserInfo(DWORD dwGameVer, DWORD dwUin, IClientSink *pSink) = 0;
	//! 游戏启动后初始化和上报本地部分数据给antibotsvr
	virtual void SendInitData() = 0;

	//! 收到antibot相关数据后调用，不负责释放pbyData内存
	virtual void OnRcvAntiData(BYTE *pbyData, DWORD dwSize) = 0;
	
	//! 停止本地主动检测
	virtual void StopLocalCheck() = 0;

	//! gameclient主动驱动开始一次检查
	virtual void StartLocalOnceCheck() = 0;

	//! 关闭客户端前最后释放AntiBot内存
	virtual void Release() = 0;
};
//! 创建AntiBot接口单一实例，返回的对象需要使用Release()释放
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
//! pszSafeDllPath如果不为NULL时最后需带'\'分割符
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