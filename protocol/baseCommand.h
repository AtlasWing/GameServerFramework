#ifndef _PROTOCOL_BASE_COMMAND_H_
#define _PROTOCOL_BASE_COMMAND_H_

#include "commonType.h"

#include <strings.h>

#include "serverInfo.h"

#pragma pack(1)

namespace protocol{ 
    /// @brief 空指令
    struct NullProtoCommand
    {
        BYTE	cmd;	///< 指令代码
        BYTE	param;	///< 指令代码子编号
        NullProtoCommand(const BYTE cmdValue, const BYTE paramValue) : cmd(cmdValue), param(paramValue)
        {
        }
    };

    inline DWORD hashZoneID(const WORD gameType, const WORD gameZoneID)
    {
        return (gameType << 16) + gameZoneID;
    }
    inline WORD getGameZoneIDFromZoneID(const DWORD  zoneID)
    {
        return static_cast<WORD>(zoneID & 0xffff);
    }
    namespace base{
        struct CommandData{
            CommandData():size(0), num(0){
            }
            size_t size;
            BYTE   num;
        };
        struct ProtoCommandSizeData{
            static CommandData protoCommandDataMap[256][256];
            ProtoCommandSizeData(BYTE cmd, BYTE param, size_t v){
                protoCommandDataMap[cmd][param].size = v;
                ++ protoCommandDataMap[cmd][param].num;
            }
        };
        template<BYTE cmd, BYTE param, size_t size>
            struct CommandSizeBase{
                CommandSizeBase(){
                }
                static ProtoCommandSizeData c;
            };
        template<BYTE cmd, BYTE param, size_t size> ProtoCommandSizeData CommandSizeBase<cmd, param, size>::c(cmd, param, size);
        inline bool initProtoCommand(BYTE *cmd, BYTE *param){
            for(WORD i = 0; i <= 255; ++ i){
                for(WORD j = 0; j <= 255; ++ j){
                    if(protocol::base::ProtoCommandSizeData::protoCommandDataMap[i][j].num > 1){
                        return false;
                    }
                }
            }
            return true;
        }
        inline bool checkProtoCommand(const NullProtoCommand *command, const DWORD len){
            const DWORD minLen = protocol::base::ProtoCommandSizeData::protoCommandDataMap[command->cmd][command->param].size;
            return minLen > 0 && minLen <= len;
        }

#define NullProtoCommandBase(CommandBase, Param)\
        CommandBase<Param>\
        {\
            void _func_force_instance_template(){\
                void(protocol::base::CommandSizeBase<CommandBase<Param>::CMD, Param, sizeof(*this)>::c.protoCommandDataMap);\
            }

#define DEFINE_COMMAND_BASE(typeName, cmdValue)\
        struct typeName##Base : public protocol::NullProtoCommand\
        {\
            static const BYTE CMD = cmdValue;\
            typeName##Base(const BYTE paramValue) : protocol::NullProtoCommand(cmdValue, paramValue)\
            {\
            }\
	};\
	template<int ParamValue>\
	struct typeName : public typeName##Base\
	{\
		static const BYTE PARAM = ParamValue;\
		typeName() : typeName##Base(ParamValue)\
		{\
		}\
	};
	///////////////////////////////////////////////////////////////////////////////
	/// 服务器内部一级命令定义
	///////////////////////////////////////////////////////////////////////////////

	DEFINE_COMMAND_BASE(LoginCommand,				    2 );		///< 玩家登陆
	DEFINE_COMMAND_BASE(ForwardCommand,				    3 );		///< 转发消息
	DEFINE_COMMAND_BASE(UserCommand,	                4 );		///< user消息,主要用来同步
	DEFINE_COMMAND_BASE(SerializeCommand,	            5 );		///< 序列化数据消息 
	DEFINE_COMMAND_BASE(GMCommand,					    6 );		///< 管理消息 
	DEFINE_COMMAND_BASE(Platform_0_Command,				7 );		///< 0号平台消息 
	DEFINE_COMMAND_BASE(MonitorCommand,				    8 );		///< 监控消息
	DEFINE_COMMAND_BASE(ServerRunCommand,				9 );		///< 服务器管理相关
	DEFINE_COMMAND_BASE(NetTickCommand,				    10 );		///< 网络相关
	DEFINE_COMMAND_BASE(ItemCommand,				    11 );		///< 道具系统消息
	DEFINE_COMMAND_BASE(MailCommand,				    12 );		///< 邮件系统消息
	DEFINE_COMMAND_BASE(RelationCommand,				13 );		///< 社交系统消息
	
	///////////////////////////////////////////////////////////////////////////////
#undef DEFINE_COMMAND_BASE
}}
#pragma pack()

#endif
