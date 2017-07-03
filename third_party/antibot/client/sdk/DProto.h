#ifndef __H__DPROTO__
#define __H__DPROTO__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////////////////////////////////////////////////////////

namespace AntiBot
{

//! ��Ϸ�ͻ�����Ҫʵ�ֵĻص��ӿڣ�ʵ�ְ����ݷ��͵���������
//! ���������copy���øú�����������
class IDPClientSink
{
public:
	virtual int SendDataToServer(BYTE *pbyData, DWORD dwSize) = 0;
};

class IDProto  
{
public:
	//////////////////////////////////////////////////////////////////////////
    enum
    {
        AnalyseOK = 0,
        AnalyseFail = -1,
        GetCryptedData = 1,
		NeedByteCode = 2,	//��Ҫ�ֽ���
    }DPOnGetDataResult;

	//////////////////////////////////////////////////////////////////////////
	//C\S���
	virtual BOOL NeedEncrypt( DWORD dwCmdId) = 0;
	virtual BOOL Encrypt(PBYTE pData, WORD wDataLen) = 0;
	virtual void GetUPVMDataBuf(PBYTE& pData, DWORD& dwDataLen) = 0;
	virtual void GetVMDataBuf( PBYTE& pData, DWORD& dwDataLen) = 0;

	//////////////////////////////////////////////////////////////////////////
	//P2P���
	virtual BOOL P2PNeedEncrypt( DWORD dwCmdId) = 0;
	virtual BOOL GetRequestPackage(PBYTE pbyBuff, WORD* pwLen, BYTE* pbyGetCodeFlag) = 0;
	virtual void GetP2PSendData( PBYTE& pData, DWORD& dwDataLen) = 0;
	virtual void GetP2PRecvData( PBYTE& pData, DWORD& dwDataLen, PBYTE& pbyHash) = 0;
	virtual int	 OnGetP2PData(PBYTE pbyData, WORD wLen) = 0;
	virtual BOOL P2PEncrypt(PBYTE pbyData, WORD wDataLen) = 0;

	//////////////////////////////////////////////////////////////////////////
	//�ۺ�
	virtual int GetLastError() = 0;
    virtual void Init(DWORD dwCliVersion, IDPClientSink *pDPClientSink) = 0;
    virtual int OnGetData(PBYTE pbyData) = 0;
	virtual void Release() = 0;	//�������ͷŽӿ�
	virtual BOOL GetErrorInfo(PBYTE pbyBuff, WORD* pwLen, BOOL bIsEncrypt, BOOL bIsP2P) = 0;
	
	/////////////////////////////////////////
	virtual void SendInitData() = 0;
	virtual BOOL CheckProtoStatus(DWORD dwCmdId) = 0;
};

IDProto* CreateIDProto();	//ע��! �����ʵ����Ψһ��


};
//////////////////////////////////////////////////////////////////////////
#endif