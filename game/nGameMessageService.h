#ifndef GAME_NGAMEMESSAGESERVICE_H_
#define GAME_NGAMEMESSAGESERVICE_H_

#include "protocol/baseCommand.h"
#include "protocol/forwardCommand.h"
#include "protocol/adminCommand.h"
#include "protocol/netCommand.h"

#include "base/nBaseMessageService.h"
#include "base/nServerConn.h"
#include "base/nTaskConn.h"
#include "base/nClientConn.h"

#include "game/nGameServer.h"
#include "game/nBaseGameMessageService.h"

namespace Game
{
	template<typename ImpMessageServiceT>
		class nGameMessageService : public Tidy::nBaseMessageService, public nBaseGameMessageService
	{
		protected:
			nGameMessageService() : 
				Tidy::nBaseMessageService(ImpMessageServiceT::checkTaskTickCommand, ImpMessageServiceT::sendTaskTickCommand, ImpMessageServiceT::parseClientTickCommand, ImpMessageServiceT::sendClientTickCommand),
				_data_server_conn(NULL)
		{
		}
			virtual ~nGameMessageService()
			{
			}

		protected:
			virtual bool init()
			{
				for(WORD i = 2; i <= 255; ++ i){
					for(WORD j = 0; j <= 255; ++ j){
						if(protocol::base::ProtoCommandSizeData::protoCommandDataMap[i][j].num > 1){
							assert_fail("proto消息重复定义[%u, %u]", i, j);
							return false;
						}
					}
				}
				return true;
			}

		private:
			virtual bool parseCommand(Tidy::nServerConn* serverConn, const void *nullCommand, const DWORD len)
			{
				return static_cast<ImpMessageServiceT*>(this)->parseServerCommand(serverConn, reinterpret_cast<const protocol::NullProtoCommand *>(nullCommand), len);
			}

		protected:
			static bool checkTaskTickCommand(Tidy::nTaskConn *taskConn, const char *cmdBuffer, const DWORD cmdLen)
			{
                const protocol::base::NetTickCommandBase *nullCommand = reinterpret_cast<const protocol::base::NetTickCommandBase *>(    cmdBuffer);
                if(protocol::base::NetTickCommandBase::CMD != nullCommand->cmd){
                    return false;
                }
                if(protocol::net::ServerTick::PARAM == nullCommand->param){
                    taskConn->clearTick();
                }
                else if(protocol::net::ClientTick::PARAM == nullCommand->param){
                    taskConn->sendTaskCmd(nullCommand,cmdLen);
                }
                return true;
			}

			static bool sendTaskTickCommand(Tidy::nTaskConn *taskConn, const Tidy::nTime &ct)
			{
                protocol::net::ServerTick send;
                return taskConn->sendTaskCmd(&send, sizeof(send));
			}

			static bool sendClientTickCommand(Tidy::nClientConn *clientConn)
			{
                protocol::net::ClientTick send;
                return clientConn->sendClientCmd(&send, sizeof(send));
			}

			static bool parseClientTickCommand(Tidy::nClientConn *clientConn, const char* cmdBuffer, const DWORD cmdLen)
			{
                const protocol::base::NetTickCommandBase *nullCommand = reinterpret_cast<const protocol::base::NetTickCommandBase*>(cmdBuffer);
                if(protocol::base::NetTickCommandBase::CMD != nullCommand->cmd){
                    return false;
                }
                if(protocol::net::ServerTick::PARAM == nullCommand->param){
                    if (clientConn->sendClientCmd(cmdBuffer, cmdLen) == false){
                        return true;
                    }
                }
                else if(protocol::net::ClientTick::PARAM == nullCommand->param){
                    clientConn->clearTick();
                }
                return true;
			}

        public:
            /*
            bool sendUserCmdToWorldExcept(const DWORD *exceptIDList, const BYTE exceptIDListSize, IEvent* event) const
            {
                if(exceptIDListSize > 2){
                    assert_fail("最大人只能是2[%u]", exceptIDListSize);
                    return false;
                }
                BUFFER_PROTO_CMD(protocol::forward::ForwardAllUserExceptUserListCommand, send);
                CDynamicStreamBuf buff;
                COStream to(buff);
                event->SaveStreamNoThrow(to);
                send->num = to.GetBuf()->Size();
                if(PROTO_CMD_CAPACITY(protocol::forward::ForwardAllUserExceptUserListCommand) < send->num){
                    assert_fail("消息长度不足[%u]", send->num);
                    return false;
                }
                send->exceptIDListSize = exceptIDListSize;
                for(BYTE i = 0; i < exceptIDListSize; ++ i){
                    send->exceptIDList[i] = exceptIDList[i];
                }
                send->clsID = event->GetCLSID();
                bcopy(to.GetBuf()->GetBytes(), send->data, send->num);
                return sendCmdToGatewayAll(send, send->size());
            }TODO 
            */ 
			bool sendCmdToServer(const Tidy::nServerConn *serverConn, const protocol::NullProtoCommand *send, const DWORD len) const
			{
				assert_debug(serverConn != NULL, "");
				return serverConn->sendCmd(send, len);
			}
			bool sendCmdToZone(const protocol::NullProtoCommand *cmd, const DWORD len) const
			{
				return gameServer->sendCmdToZone(cmd, len);
			}
			bool sendCmdToData(const protocol::NullProtoCommand *cmd, const DWORD len) const
			{
				if(_data_server_conn == NULL){
					return false;
				}
				return _data_server_conn->sendCmd(cmd, len);
			}
			bool sendCmdToLoop(const protocol::NullProtoCommand *cmd, const DWORD len) const
			{
				return gameServer->sendCmdToLoop(cmd, len);
			}
			bool sendProtoCmdToOtherZoneServer(const protocol::NullProtoCommand* cmd, const DWORD len, const DWORD destZoneID, const DWORD destServerID, const WORD destServerType) const
			{
				BUFFER_PROTO_CMD(protocol::forward::ForwardOtherZoneServerCommand, send);
				if(PROTO_CMD_CAPACITY(protocol::forward::ForwardOtherZoneServerCommand) < len){
					assert_fail("越界");
					return false;
				}
				send->destZoneID = destZoneID;
				send->destServerID = destServerID;
				send->destServerType = destServerType;
				send->num = len;
				memcpy(send->data, cmd, len);
				return sendCmdToZone(send, send->allSize());
			}
			bool sendProtoCmdToGlobalServer(const protocol::NullProtoCommand* cmd, const DWORD len, const DWORD destGlobalServerID, const DWORD destGlobalServerType) const
			{
				BUFFER_PROTO_CMD(protocol::forward::ForwardGlobalServerCommand, send);
				if(PROTO_CMD_CAPACITY(protocol::forward::ForwardGlobalServerCommand) < len){
					assert_fail("越界");
					return false;
				}
				send->destGlobalServerID = destGlobalServerID;
				send->destGlobalServerType = destGlobalServerType;
				send->num = len;
				memcpy(send->data, cmd, len);
				return sendCmdToZone(send, send->allSize());
			}
			bool sendCmdToAdminServer(const protocol::base::AdminCommandBase* cmd, const DWORD len) const
			{
				if(PROTO_CMD_CAPACITY(protocol::forward::ForwardAdminServerCommand) < len){
					assert_fail("越界");
					return false;
				}
				BUFFER_PROTO_CMD(protocol::forward::ForwardAdminServerCommand, send);
				send->num = len;
				memcpy(send->data, cmd, len);
				return sendCmdToZone(send, send->allSize());
			}
			bool sendCmdToMonitorServer(const DWORD destServerID, const protocol::base::MonitorCommandBase* cmd, const DWORD len) const
			{
                return true;
				//return static_cast<const ImpMessageServiceT*>(this)->sendProtoCmdToGlobalServer(cmd, len, destServerID, MONITORSERVER);
			}
			bool sendCmdToGatewayByID(DWORD serverID, const protocol::NullProtoCommand* cmd, const unsigned int len)const
			{
				return gameServer->sendCmdToTaskConnByID(serverID, cmd, len);
			}

			bool sendCmdToGatewayAll(const protocol::NullProtoCommand* cmd, const unsigned int len)const
			{
                return gameServer->sendCmdToTaskConnByType(GATEWAYSERVER, cmd, len);
			}
			bool sendCmdToGatewayAllExceptOne(const DWORD serverID, const protocol::NullProtoCommand* cmd, const unsigned int len)const
			{
                return gameServer->sendCmdToTaskConnByTypeExceptOne(GATEWAYSERVER, serverID, cmd, len);
            }

            /*
            bool sendUserCmdToWorld(IEvent* event) const
            {
                BUFFER_PROTO_CMD(protocol::forward::ForwardAllUserCommand, sendCmd);
                CDynamicStreamBuf buff;
                COStream to(buff);
                event->SaveStreamNoThrow(to);
                sendCmd->num = to.GetBuf()->Size();
                sendCmd->clsID = event->GetCLSID();
                bcopy(to.GetBuf()->GetBytes(), sendCmd->data, sendCmd->num);
                if(PROTO_CMD_CAPACITY(protocol::forward::ForwardAllUserCommand) < sendCmd->num){
                    assert_fail("消息长度不足[%u]", sendCmd->num);
                    return false;
                }
                return sendCmdToGatewayAll(sendCmd, sendCmd->allSize());
            }

            bool sendUserCmdToClub(const DWORD clubID, IEvent *event) const
            {   
                BUFFER_PROTO_CMD(protocol::forward::ForwardClubUserCommand, sendCmd);
                CDynamicStreamBuf buff;
                COStream to(buff);
                event->SaveStreamNoThrow(to);
                if(PROTO_CMD_CAPACITY(protocol::forward::ForwardClubUserCommand) < to.GetBuf()->Size()){
                    assert_fail("消息长度不足[%u]", to.GetBuf()->Size());
                    return false;
                }
                sendCmd->clubID = clubID;
                sendCmd->num = to.GetBuf()->Size();
                sendCmd->clsID = event->GetCLSID();
                bcopy(to.GetBuf()->GetBytes(), sendCmd->data, sendCmd->num);
                return sendCmdToGatewayAll(sendCmd, sendCmd->allSize());
            }   

            bool sendUserCmdToClubExceptOne(const DWORD clubID, const DWORD exceptUserID, IEvent *event) const
            {   
                BUFFER_PROTO_CMD(protocol::forward::ForwardClubUserExceptOneCommand, sendCmd);
                CDynamicStreamBuf buff;
                COStream to(buff);
                event->SaveStreamNoThrow(to);
                if(PROTO_CMD_CAPACITY(protocol::forward::ForwardClubUserExceptOneCommand) < to.GetBuf()->Size()){
                    assert_fail("消息长度不足[%u]", to.GetBuf()->Size());
                    return false;
                }   
                sendCmd->num = to.GetBuf()->Size();
                sendCmd->clubID = clubID;
                sendCmd->exceptUserID = exceptUserID;
                sendCmd->clsID = event->GetCLSID();
                bcopy(to.GetBuf()->GetBytes(), sendCmd->data, sendCmd->num);
                return sendCmdToGatewayAll(sendCmd, sendCmd->allSize());
            }
            TODO 消息转发到不同的网关@
            */

		public:
			bool hasDataServerConn() const
			{
				return _data_server_conn != NULL;
			}
			Tidy::nServerConn* getDataServerConn() const
			{
				return _data_server_conn;
			}

		protected:
			void dealConnectSuccess(Tidy::nServerConn *serverConn)
			{
				switch(serverConn->getServerType()){
					case DATASERVER:
						{
							_data_server_conn = serverConn;
						}
						break;
					default:
						{
						}
				}
			}

			void dealConnectClose(Tidy::nServerConn *serverConn)
			{
				switch(serverConn->getServerType()){
					case DATASERVER:
						{
							_data_server_conn = NULL;
							gameServer->info("DataServer连接关闭");
						}
						break;
					default:
						{
						}
				}
			}

		private:
			virtual bool sendCmdToMonitorServerFromZone(const protocol::base::MonitorCommandBase *cmd, const DWORD len) const
			{
				return this->sendCmdToMonitorServer(0, cmd, len);
			}

		private:
			Tidy::nServerConn *_data_server_conn;
			Tidy::nServerConn *_session_server_conn;
	};
};
#endif
