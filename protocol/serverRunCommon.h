#ifndef _PROTOCOL_SERVER_RUN_COMMON_H_
#define _PROTOCOL_SERVER_RUN_COMMON_H_

#include "commonType.h"
#include <string.h>

#pragma pack(1)

namespace protocol {
    namespace ServerRun {
        const DWORD gameProtoVersion = 2011122701;
        struct ServerListenData
        {
            ServerListenData():
                port(0),
                netType(0),
                taskConnType(0)
            {
                ip[0] = '\0';
                url[0] = '\0';
            }
            char ip[MAX_IP_LENGTH];
            char url[MAX_URL_LENGTH];
            WORD port;
            BYTE netType;
            DWORD taskConnType;
        };
        const DWORD MAX_SERVER_LISTEN_DATA_NUM = 6;
        struct ServerEntity
        {
            WORD serverID;
            DWORD serverType;
            char pstrName[MAX_NAMESIZE];
            char internalIP[MAX_IP_LENGTH];
            WORD internalPort;
            BYTE zipMethod;
            bool checkHeartTick;
            DWORD heartTickTimerInMsec;
            bool needQuickAck;
            bool hasUnixSocket;
            DWORD lastServerCloseTimeInSec;
            WORD rankInSameType;
            WORD serverNumInSameType;
            bool hasClosed;
            DWORD listenDataNum;
            ServerListenData listenDataList[MAX_SERVER_LISTEN_DATA_NUM];
            ServerEntity(const WORD serverIDData, const DWORD serverTypeData, const char *nameData, const char *ipData, const WORD portData, const BYTE zipMethodData, const bool checkHeartTickData, const DWORD heartTickTimerInMsecData, const bool needQuickAckData, const bool hasUnixSocketData, const DWORD lastServerCloseTimeInSecData, const WORD rankInSameTypeData, const bool hasClosedData) : 
                serverID(serverIDData),
                serverType(serverTypeData),
                internalPort(portData),
                zipMethod(zipMethodData),
                checkHeartTick(checkHeartTickData),
                heartTickTimerInMsec(heartTickTimerInMsecData),
                needQuickAck(needQuickAckData),
                hasUnixSocket(hasUnixSocketData),
                lastServerCloseTimeInSec(lastServerCloseTimeInSecData),
                rankInSameType(rankInSameTypeData),
                serverNumInSameType(0),
                hasClosed(hasClosedData),
                listenDataNum(0)
            {
                strncpy(pstrName, nameData, sizeof(pstrName));
                strncpy(internalIP, ipData, sizeof(internalIP));
            }
            ServerEntity() : 
                serverID(0),
                serverType(0),
                internalPort(0),
                zipMethod(0),
                checkHeartTick(true),
                heartTickTimerInMsec(1000),
                needQuickAck(false),
                hasUnixSocket(false),
                lastServerCloseTimeInSec(0),
                rankInSameType(0),
                serverNumInSameType(0),
                hasClosed(false),
                listenDataNum(0)
            {
                pstrName[0] = '\0';
                internalIP[0] = '\0';
            }
            ServerEntity(const ServerEntity& other)
            {
                *this = other;
            }
        };

        enum eServerEvent{
            eServerEvent_None               =   0,
            eServerEvent_AdminConnectSucc   =   1,      // 连接adminServer完成
            eServerEvent_AllGameServerStartComplete=2,  // 所有服务器启动完成
            eServerEvent_ReloadConfig       =   3,      // 重新加载配置
            eServerEvent_MonthFirstDay      =   4,      // 每月1号
            eServerEvent_WeekFirstDay       =   5,      // 每周一
            eServerEvent_ZeroClocker        =   6,      // 0点
            eServerEvent_Analyze            =   7,      // 分析时间超时 
            eServerEvent_MysteryShop        =   8,      // 神秘商店 
            eServerEvent_VitalityTaskBegin  =   9,      // 体力任务 
            eServerEvent_VitalityTaskEnd    =   10,     // 体力任务 
            eServerEvent_SequenceEloReward  =   11,     // 天梯榜奖励 
            eServerEvent_SequenceEloReset   =   12,     // 天梯榜重置 
        };

		//屏蔽消息操作类型定义
		enum eCommandFilterOperType{
			eCommandFilterOperType_None,
			eCommandFilterOperType_Add,			///< 添加一个屏蔽消息
			eCommandFilterOperType_Remove,		///< 取消一个屏蔽消息
			eCommandFilterOperType_Clear,		///< 取消所有屏蔽消息
			eCommandFilterOperType_Max
		};
		//用户消息统计操作类型定义
		enum eCheckUserCommandOperType{
			eCheckUserCommandOperType_None,
			eCheckUserCommandOperType_Add,			///< 添加一个屏蔽消息
			eCheckUserCommandOperType_Remove,		///< 取消一个屏蔽消息
			eCheckUserCommandOperType_Clear,		///< 取消所有屏蔽消息
			eCheckUserCommandOperType_Max
		};
        struct ServerDelayTestInfo
        {
            DWORD fromServerID;
            DWORD fromServerType;
            DWORD toServerID;
            DWORD toServerType;
            DWORD delayTimeInMsec;
        };
    };
};

#pragma pack()
#endif

