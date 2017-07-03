#include "protocol/serverRunCommand.h"

#include "zoneServer/zoUser.h"
#include "zoneServer/zoServer.h"
#include "zoneServer/zoTaskConn.h"
#include "zoneServer/zoMessageService.h"
#include "zoneServer/zoScheduleService.h"
#include "zoneServer/zoUserService.h"

bool zoServer::parseServerRunCommand(Tidy::nServerConn* serverConn, const protocol::base::ServerRunCommandBase *nullCommand, const unsigned int len)
{
    using namespace protocol::ServerRun;

    switch(nullCommand->param)
    {
        case NotifyOtherWaitStartRespondCommand::PARAM:
            {
                const NotifyOtherWaitStartRespondCommand* recvCmd = static_cast<const NotifyOtherWaitStartRespondCommand*>(nullCommand);
                Tidy::nTaskConn* conn = getSyncTaskConn(recvCmd->serverID);
                if(conn == NULL)
                {
                    return false;
                }
                zoTaskConn* taskConn = static_cast<zoTaskConn*>(conn);
                taskConn->addNotifyOtherRespond(serverConn);
                return true;
            }
            break;
        case ServerStartupComplete::PARAM:
            {
                zoTaskConn* task = static_cast<zoTaskConn*>(serverConn);
                assert_debug(task->isInSyncState(zoTaskConn::eServerState_WaitMeStart) == true, "");
                task->setSyncState(zoTaskConn::eServerState_OK);
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
        case AddScheduleEventCommand::PARAM:
            {
                const AddScheduleEventCommand *command = static_cast<const AddScheduleEventCommand*>(nullCommand);
                std::string param(command->eventParam, command->num);
                return scheduleService->createSchedule(0, command->scheduleServerType, command->scheduleServerID, command->eventID, param, command->month, command->monthDay, command->weekDay, command->hour, command->minute, command->sec) != NULL;
            }
            break;
        case ServerDelayLinkedTestCommand::PARAM:
            {
                const ServerDelayLinkedTestCommand *command = static_cast<const ServerDelayLinkedTestCommand*>(nullCommand);
                server->onReturnServerDelayTest(serverConn, command);
                return true;
            }
            break;
        case RequestCloseZoneCommand::PARAM:
            {
                setCloseZone();
                return true;
            }
            break;
        case StopScheduleCommand::PARAM:
            {
                const StopScheduleCommand *command = static_cast<const StopScheduleCommand*>(nullCommand);
                return scheduleService->stopSchedule(command->configID);
            }
            break;
        case NotifyServerEventCommand::PARAM:
            {
                return nGameServer::parseServerRunCommand(serverConn, nullCommand, len);
            }
            break;
        default:
            {
                assert_fail("未识别消息");
				return false;
            }
    }
	return true;
}

