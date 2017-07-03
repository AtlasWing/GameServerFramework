#ifndef PROTOCOL_SERVER_INFO_H_
#define PROTOCOL_SERVER_INFO_H_
#include "protocol/commonType.h"

/// @brief 服务器类型
enum ServerType {
    UNKNOWNSERVER	=	0,		///< 未知服务器类型
    ZONESERVER	    =	1,		///< 管理器
    DATASERVER	    =	2,		///< 数据服务器
    LOGICSERVER		=	5,		///< 逻辑服务器
    GATEWAYSERVER	=	7,		///< 网关服务器
    PLATFORMSERVER  =	19,		///< 登陆服务器
    ADMINSERVER     =	20,		///< 验证服务器

    USERCLIENT      =   100,    ///< 玩家客户端连接
    TELNETCLIENT    =	101,    ///< Telnet客户端连接
    HTTPCLIENT      =	102,    ///< HTTP客户端连接
    MONITORCLIENT   =	103,    ///< Monitor客户端连接

    MAX_SERVERTYPE              ///< 服务器类型的最大值
};

// 是否是游戏服务器,一个区内的服务器,去除平台,admin等服务器
inline bool isGameServer(const BYTE serverType)
{
    return serverType == DATASERVER ||
        serverType == LOGICSERVER ||
        serverType == ZONESERVER ||
        serverType == GATEWAYSERVER;
}

inline bool isTempTaskConn(const BYTE serverType)
{
    return serverType == TELNETCLIENT ||
        serverType == HTTPCLIENT;
}

struct ServerTypeStruct
{
    ServerType type;
    char typeStr[MAX_NAMESIZE];
    char name[MAX_NAMESIZE];
};
// 服务器关闭顺序
static DWORD closeSequenceData[] = 
{
    GATEWAYSERVER, LOGICSERVER, DATASERVER, ZONESERVER 
};
static ServerTypeStruct ServerTypeString[] =
{
    {UNKNOWNSERVER, "UNKNOWNSERVER", "未知服务器"},
    {ZONESERVER,    "ZONESERVER",    "管理服务器"},
    {DATASERVER,    "DATASERVER",    "数据服务器"},
    {LOGICSERVER,   "LOGICSERVER",   "逻辑服务器"},
    {GATEWAYSERVER, "GATEWAYSERVER", "网关服务器"},
    {USERCLIENT,    "USERCLIENT",    "客户端连接"},
    {PLATFORMSERVER,"PLATFORMSERVER","平台服务器"},
    {TELNETCLIENT,  "TELNETCLIENT",  "TELNET连接"},
    {HTTPCLIENT,    "HTTPCLIENT",    "HTTP连接"},
};

template<typename ServerTypeT>
inline const char *getServerTypeString(ServerTypeT type)
{
    (void)closeSequenceData;
    int count = sizeof(ServerTypeString)/sizeof(ServerTypeStruct);
    for(int i=0 ; i < count ; i ++)
    {
        if(ServerTypeString[i].type == static_cast<int>(type))
        {
            return ServerTypeString[i].name;
        }
    }
    // 断言
    count = *((DWORD*)0);
    return ServerTypeString[0].name;
};

enum eNetType{
    eNetType_None       =   0,
    eNetType_CU         =   1,
    eNetType_CT         =   2,
    eNetType_MAX
};
// 数据库ID枚举
enum eDBConnectionID
{
    // 区主数据库
    eDBConnectionID_ZoneMain   =   1,
    // 验证数据库
    eDBConnectionID_Admin      =   2,
    // 好友数据库
    eDBConnectionID_Friend     =   3,
    // 仙盟数据库
    eDBConnectionID_Club       =   4,
};

#endif
