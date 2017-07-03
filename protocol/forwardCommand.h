#ifndef _PROTOCOL_FORWARD_COMMAND_H_
#define _PROTOCOL_FORWARD_COMMAND_H_

#include "baseCommand.h"

#pragma pack(1)
namespace protocol
{
    namespace forward
    {
        /// @brief 用户消息转发给客户端
        struct ForwardClientUserCommand : public NullProtoCommandBase(base::ForwardCommand, 1)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD charID;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 服务器之间转发用户消息
        struct ForwardServerUserCommand : public NullProtoCommandBase(base::ForwardCommand, 2)
            DWORD allSize() const {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            QWORD sendTimeInMsec;
            DWORD charID;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息给周围用户
        struct ForwardAroundUserCommand : public NullProtoCommandBase(base::ForwardCommand, 3)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD roomID;
            WORD screenIndex;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息给周围用户,不发给自己
        struct ForwardAroundUserExceptOneCommand : public NullProtoCommandBase(base::ForwardCommand, 4)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD roomID;
            WORD screenIndex;
            DWORD exceptCharID;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息给指定方向的周围用户
        struct ForwardAroundUserInDirectCommand : public NullProtoCommandBase(base::ForwardCommand, 5)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD roomID;
            WORD screenIndex;
            BYTE direct;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息给反向的周围用户
        struct ForwardAroundUserInReverseDirectCommand : public NullProtoCommandBase(base::ForwardCommand, 6)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD roomID;
            WORD screenIndex;
            BYTE direct;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        struct ForwardToAllPlatformServerCommand : public NullProtoCommandBase(base::ForwardCommand, 7)
            DWORD clsID;
            DWORD num;
            char data[0];
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
        };

        struct ForwardPlatformServerCommand : public NullProtoCommandBase(base::ForwardCommand, 8)
            DWORD clsID;
            DWORD num;
            char data[0];
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
        };

        struct ForwardToPlatformServerCommand : public NullProtoCommandBase(base::ForwardCommand, 9)
            DWORD platformID;
            DWORD clsID;
            DWORD num;
            char data[0];
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
        };

        /// @brief 转发客户端消息到当前场景所有用户
        struct ForwardRoomUserCommand : public NullProtoCommandBase(base::ForwardCommand, 10)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD roomID;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息到服务器上的所有用户
        struct ForwardAllUserCommand : public NullProtoCommandBase(base::ForwardCommand, 11)
            DWORD allSize() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息到服务器上的除去exceptID之外的所有用户
        struct ForwardAllUserExceptUserListCommand : public NullProtoCommandBase(base::ForwardCommand, 12)
			ForwardAllUserExceptUserListCommand() : exceptIDListSize(0){
			}
            DWORD size() const {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD exceptIDList[2];
			BYTE  exceptIDListSize;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息给周围用户
        struct ForwardAroundUserWithLogicConnIDCommand : public NullProtoCommandBase(base::ForwardCommand, 13)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD logicConnID;
            DWORD roomID;
            WORD screenIndex;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息给周围用户,不发给自己
        struct ForwardAroundUserWithLogicConnIDExceptOneCommand : public NullProtoCommandBase(base::ForwardCommand, 14)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD logicConnID;
            DWORD roomID;
            WORD screenIndex;
            DWORD exceptCharID;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息给指定方向的周围用户
        struct ForwardAroundUserWithLogicConnIDInDirectCommand : public NullProtoCommandBase(base::ForwardCommand, 15)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD logicConnID;
            DWORD roomID;
            WORD screenIndex;
            DWORD clsID;
            BYTE direct;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息给反向的周围用户
        struct ForwardAroundUserWithLogicConnIDInReverseDirectCommand : public NullProtoCommandBase(base::ForwardCommand, 16)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD logicConnID;
            DWORD roomID;
            WORD screenIndex;
            BYTE direct;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息给周围用户,通过charID,必须通过逻辑服务器或者AI服务器
        struct ForwardAroundUserWithCharIDCommand : public NullProtoCommandBase(base::ForwardCommand, 17)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD charID;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息给周围用户,通过charID,必须通过逻辑服务器或者AI服务器
        struct ForwardAroundUserWithCharIDExceptMeCommand : public NullProtoCommandBase(base::ForwardCommand, 18)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD charID;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        struct ForwardRoomUserWithLogicConnIDCommand : public NullProtoCommandBase(base::ForwardCommand, 19)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD logicConnID;
            DWORD roomID;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息给指定方向包括当前所在屏的周围用户
        struct ForwardAroundUserInDirectWithCentorCommand : public NullProtoCommandBase(base::ForwardCommand, 20)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD roomID;
            WORD screenIndex;
            BYTE direct;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息给指定方向包括当前所在屏的周围用户,除去指定玩家
        struct ForwardAroundUserInDirectWithCentorExceptOneCommand : public NullProtoCommandBase(base::ForwardCommand, 21)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD roomID;
            WORD screenIndex;
            BYTE direct;
            DWORD exceptCharID;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发消息到在某个场景的所有网关,除去某个网关
        struct ForwardAllGatewayInRoomExceptOneCommand : public NullProtoCommandBase(base::ForwardCommand, 22)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD roomID;
            DWORD exceptGatewayID;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        struct ForwardVerifyServerCommand : public NullProtoCommandBase(base::ForwardCommand, 23)
            DWORD clsID;
            DWORD num;
            char data[0];
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
        };

        struct ForwardToVerifyServerCommand : public NullProtoCommandBase(base::ForwardCommand, 24)
            DWORD platformID;
            DWORD clsID;
            DWORD num;
            char data[0];
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
        };

        /// @brief 通过Session服务器转发用户消息
        struct ForwardUserCommandViaSessionCommand : public NullProtoCommandBase(base::ForwardCommand, 25)
            DWORD allSize() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD charID;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息到除去一个玩家之外的当前场景所有用户
        struct ForwardRoomUserExceptOneCommand : public NullProtoCommandBase(base::ForwardCommand, 26)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD exceptCharID;
            DWORD roomID;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息到部分状态的所有用户
        struct ForwardUserInLoginStateCommand : public NullProtoCommandBase(base::ForwardCommand, 27)
            DWORD size() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD loginState;
            DWORD clsID;
            WORD num;
            char data[0];
        };
        // 转发其他区的服务器
        struct ForwardOtherZoneServerCommand : public NullProtoCommandBase(base::ForwardCommand, 28)
            ForwardOtherZoneServerCommand() : num(0){
            }
            DWORD allSize() const{
                return sizeof(*this) + num * sizeof(data[0]);
            }
            WORD destServerType;
            DWORD destServerID;
            DWORD destZoneID;
            DWORD clsID;
            WORD num;
            char data[0];
        };
        // 转发消息到adminServer
        struct ForwardAdminServerCommand : public NullProtoCommandBase(base::ForwardCommand, 29)
            ForwardAdminServerCommand() : num(0){
            }
            DWORD allSize() const{
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD clsID;
            WORD num;
            char data[0];
        };
        // 发送消息给机器人
        struct ForwardRobotUserCommand : public NullProtoCommandBase(base::ForwardCommand, 30)
            DWORD allSize() const
            {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD charID;
            DWORD clsID;
            DWORD num;
            char data[0];
        };
        /// @brief 转发用户消息到所有服务器，本消息有生命期
        struct ForwardSysMessageOnTopToWorldCommand : public NullProtoCommandBase(base::ForwardCommand, 31)
            ForwardSysMessageOnTopToWorldCommand() : num(0){
            }
            DWORD allSize() const {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD clsID;
            DWORD num;
            char data[0];
        };
        // 广播所有打开了某个窗口的玩家
        struct ForwardClientUserOpenWindowCommand : public NullProtoCommandBase(base::ForwardCommand, 32)
            DWORD allSize() const {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            BYTE  windowType;
            DWORD clsID;
            DWORD num;
            char data[0];
        };
        // 广播所有打开了某个窗口的玩家,除去某个玩家
        struct ForwardClientUserOpenWindowExceptOneCommand : public NullProtoCommandBase(base::ForwardCommand, 33)
            DWORD allSize() const {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            BYTE  windowType;
            DWORD exceptUserID;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息到当前场景所有某个阵营的用户
        struct ForwardRoomCampUserCommand : public NullProtoCommandBase(base::ForwardCommand, 34)
            DWORD allSize() const {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD roomID;
            BYTE campType;
            DWORD clsID;
            DWORD num;
            char data[0];
        };
        // 转发其他区的服务器
        struct ForwardGlobalServerCommand : public NullProtoCommandBase(base::ForwardCommand, 35)
            ForwardGlobalServerCommand() : num(0){
            }
            DWORD allSize() const{
                return sizeof(*this) + num * sizeof(data[0]);
            }
            WORD destGlobalServerID;
            DWORD destGlobalServerType;
            DWORD clsID;
            WORD num;
            char data[0];
        };

        /// @brief 转发客户端消息到某个游戏频道的在某个状态的玩家
        struct ForwardUserInLoginStateOrWindowTypeByChannelCommand : public NullProtoCommandBase(base::ForwardCommand, 36)
            DWORD allSize() const {
                return sizeof(*this) + num * sizeof(data[0]);
            }
			bool needSendRobot;
			BYTE gameChannel;
            DWORD loginState;
			BYTE windowType;
            DWORD clsID;
            WORD num;
            char data[0];
        };
		/// @brief 转发到任意一个Platform
		struct ForwardToOnePlatformServerCommand : public NullProtoCommandBase(base::ForwardCommand, 37)
            DWORD clsID;
            DWORD num;
            char data[0];
            DWORD size() const {
                return sizeof(*this) + num * sizeof(data[0]);
            }
        };
		/// @brief 转发大喇叭聊天消息
		struct ForwardLabaMessageCommand : public NullProtoCommandBase(base::ForwardCommand, 38)
			DWORD charID;
            DWORD clsID;
            DWORD num;
            char data[0];
            DWORD size() const {
                return sizeof(*this) + num * sizeof(data[0]);
            }
        };
        // 广播所有俱乐部成员
        struct ForwardClubUserCommand : public NullProtoCommandBase(base::ForwardCommand, 39)
            DWORD allSize() const {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD clubID;
            DWORD clsID;
            DWORD num;
            char data[0];
        };
        // 广播所有俱乐部成员出去一个
        struct ForwardClubUserExceptOneCommand : public NullProtoCommandBase(base::ForwardCommand, 40)
            DWORD allSize() const {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD clubID;
			DWORD exceptUserID;
            DWORD clsID;
            DWORD num;
            char data[0];
        };

        /// @brief 转发客户端消息到服务器上的所有不在战斗中的用户
        struct ForwardAllNotInContestUserCommand : public NullProtoCommandBase(base::ForwardCommand, 41)
            DWORD allSize() const {
                return sizeof(*this) + num * sizeof(data[0]);
            }
            DWORD clsID;
            DWORD num;
            char data[0];
        };

    };
};
#pragma pack()
#endif
