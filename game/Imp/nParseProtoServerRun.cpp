#include "protocol/serverRunCommand.h"

#include "game/nGameServer.h"

namespace Game
{
	bool nGameServer::parseServerRunCommand(Tidy::nServerConn* serverConn, const protocol::base::ServerRunCommandBase *nullCommand, const unsigned int len)
	{
		using namespace protocol::ServerRun;
		switch(nullCommand->param)
		{
			case ServerSyncOKCommand::PARAM:
				{
					return true;
				}
				break;
			case ServerNotifyMeStartCommand::PARAM:
				{
					const ServerNotifyMeStartCommand* command = static_cast<const ServerNotifyMeStartCommand*>(nullCommand);
					for(WORD i=0; i < command->num; ++ i) {
						addServerEntity(command->data[i]);
					}

                    dealNotifyMeStart();

					for(ServerEntitySetIterator it = _server_entity_list.begin(); it != _server_entity_list.end(); ++it) {
						if((*it).serverType != ZONESERVER && (*it).serverID != getServerID() && isClientConn((*it).serverType) == true) {
							dealServerStartComplete((*it).serverID, (*it).serverType);
						}
					}
					Tidy::baseServer->debug("zoneServer通知开始启动服务器...");
					_check_can_start = true;
					return true;
				}
				break;
			case NotifyOtherWaitStartCommand::PARAM:
				{
					const NotifyOtherWaitStartCommand* command = static_cast<const NotifyOtherWaitStartCommand*>(nullCommand);
					addServerEntity(command->serverEntity);

					NotifyOtherWaitStartRespondCommand send;
					send.serverID = command->serverID;
					return serverConn->sendCmd(&send, sizeof(send));
				}
				break;
			case CloseServerCommand::PARAM:
				{
					debug("开始关闭服务器");
					closeGameServer();
					terminate(Tidy::eServerCloseReason_Normal);
					return true;
				}
				break;
			case AdjustServerTimeCommand::PARAM:
				{
					const AdjustServerTimeCommand* command = static_cast<const AdjustServerTimeCommand*>(nullCommand);
					addAdjustServerTime(command->adjustTimeInSec);
					return true;
				}
				break;
			case NotifyStartupCompleteCommand::PARAM:
				{
					const NotifyStartupCompleteCommand* command = static_cast<const NotifyStartupCompleteCommand*>(nullCommand);
					//如果command->serverInfo的服务器依赖于当前服务器,则这部分数据addServerEntity了两次,但是没关系
					addServerEntity(command->serverInfo);
					gameServer->debug("启动完成 [%u]", command->serverInfo.serverID);
					if(command->serverInfo.serverID == getServerID() && command->serverInfo.serverType == getServerType()){
						if(dealCompleteStartup() == false){
							assert_fail("启动完成处理失败");
							terminate(Tidy::eServerCloseReason_Normal);
							return false;
						}
					}
					dealServerStartComplete(command->serverInfo.serverID, command->serverInfo.serverType);
					return true;
				}
				break;
			case NotifyServerEventCommand::PARAM:
				{
					const NotifyServerEventCommand *command = static_cast<const NotifyServerEventCommand*>(nullCommand);
					parseServerEvent(command->eventID, command->eventParam, command->num);
					return true;
				}
				break;
			case AddZoneServerCommand::PARAM:
				{
					const AddZoneServerCommand *command = static_cast<const AddZoneServerCommand*>(nullCommand);
					addZoneData(command->num, command->data);
					return true;
				}
				break;
			case RemoveZoneServerCommand::PARAM:
				{
					const RemoveZoneServerCommand *command = static_cast<const RemoveZoneServerCommand*>(nullCommand);
					removeZoneData(command->zoneID);
					return true;
				}
				break;
			case ClearZoneDataCommand::PARAM:
				{
					clearAllZoneData();
					return true;
				}
				break;
			case SetZoneInfoCommand::PARAM:
				{
					const SetZoneInfoCommand *command = static_cast<const SetZoneInfoCommand*>(nullCommand);
					setZoneInfo(command->zoneID, command->zoneName);
					return true;
				}
				break;
			case ServerDelayLinkedTestCommand::PARAM:
				{
					const ServerDelayLinkedTestCommand *command = static_cast<const ServerDelayLinkedTestCommand*>(nullCommand);
					return onServerDelayLinkedTest(serverConn, command);
				}
				break;
			case SetGlobalParamCommand::PARAM:
				{
					const SetGlobalParamCommand *command = static_cast<const SetGlobalParamCommand*>(nullCommand);
					std::string value;
					if(command->valueLen > 0){
						value.assign(command->value, command->valueLen);
					}
					Tidy::global[command->key].setString(value);
					return true;
				}
				break;
			default:
				{
					assert_fail("未知消息");
				}
		}

		return false;
	}
};

