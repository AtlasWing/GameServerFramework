#ifndef __IBUSINESS_H__
#define __IBUSINESS_H__

// safe mode struct or union define -- start
enum EProtectInfoMsgType
{
    SS_SAFEMODE_RSP_UPDATE = 18, // safe mode response update
    SS_SAFEMODE_RSP_ACK = 20, // safe mode response ack 
};

#pragma pack(push, 1)
struct SSafeModeHead
{
    unsigned short MsgType; // value use EProtectInfoMsgType
    unsigned int Uin;
};

struct SSafeModeAck
{
    unsigned short IPCDays;
    signed char ProtStatLevel;
    signed char HasVerifyMB;
    unsigned short EventType;
    signed char Reserve[32];
};

struct SSafeModeUpdate
{
    unsigned short IPCDays;
    signed char ProtStatLevel;
    signed char HasVerifyMB;
    unsigned short EventType;
    signed char Reserve[32];
};

union SSafeModeBody
{
    // valid when Head.MsgType == SS_SAFEMODE_RSP_UPDATE
    SSafeModeUpdate RspSafeModeUpdate;

    // valid when Head.MsgType == SS_SAFEMODE_RSP_ACK
    SSafeModeAck RspSafeModeAck;
};

struct SSafeModePkg
{
    SSafeModeHead Head;
    SSafeModeBody Body;
};
#pragma pack(pop)
// safe mode struct or union define----end

typedef struct tagAntibotPunishInfo
{
    int iGameID;
    int iBanTimeSecond;

    int iVerifyTimeout;
    int iVerifyMaxErrorCount;
    int iVerifyLevel;
}AntibotPunishInfo;

typedef enum 
{
    NORMAL          = 0,
    WRITELOG        = 1,
    KICKOUT         = 2,    // kick user to channel
    BANPLAYER       = 3,
    TROJAN_DETECT   = 4,    // prompt client for trojan-detecting
    CLOSECLIENT     = 5,    // close the client of the user
    MEMRECOVER      = 6,    
    CLIENTMSGTIP    = 7,    // deliver msg to client, and pop a dialog
    POP_VERIFYCODE  = 8,    // pop verify code, such as captcha
    BAN_CHATTING    = 9,    // ban user from chatting
    MODIFY_ROOM_NAME= 10,   // modify user create room name to default name.
}EPUNISHMODE;

class IBusiness
{
public:
    /*iType = 0 packet of Antibot*/
    /*iType = 1 packet of DP*/
    virtual int SendBuffertoClient(int iType, int iGameID, int iRoomID, unsigned int uiUid, unsigned char * abySendBuffer, short shSize) = 0;
    virtual int PunishClient(int iGameID, int iRoomID, unsigned int uiUid, EPUNISHMODE enPunishMode, AntibotPunishInfo * pstAntibotPunishInfo, char * szPunishReason) = 0;

    /* safe mode interface */
    virtual int ProcessProtectMsg(SSafeModePkg * pstSecSafeMsg) = 0;
};

#endif /* __BUSINESS_H__ */
