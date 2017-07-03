#include "protocol/serverInfo.h"

#include "base/nStringTool.h"
#include "base/nTime.h"

#include "zoneServer/zoMonitorService.h"
#include "zoneServer/zoServer.h"
#include "zoneServer/zoLogicConn.h"
#include "zoneServer/zoGatewayConn.h"
#include "zoneServer/zoUser.h"
#include "zoneServer/zoUserService.h"
#include "zoneServer/zoMessageService.h"

zoMonitorService::zoMonitorService() :
	_reset_conn_user_num_timer(Tidy::global["RESET_CONN_USER_NUM_IN_SEC"].getDword() * ONE_SEC),
	_one_sec_timer(ONE_SEC),
	_notify_close_zone_interval_time_in_sec(Tidy::global["NOTIFY_CLOSE_ZONE_INTERVAL_TIME_IN_MIN"].getDword() * ONE_MIN_IN_SEC),
	_close_zone_time_in_sec(0)
{
}

zoMonitorService::~zoMonitorService()
{
}

const DWORD zoMonitorService::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const
{
    return Tidy::nStringTool::copyStr(headerBuffer, "[Monitor]", bufferSize);
}

Tidy::nLogger* zoMonitorService::getLogger() const
{
    return server->getLogger();
}

bool zoMonitorService::initService()
{
    Tidy::nBaseMonitorService::initService();
	Tidy::nStringTool::splitValue(Tidy::global["NOTIFY_CLOSE_ZONE_LEFT_TIME_IN_MIN"].getString(), ';', _notify_close_zone_left_time_in_sec);
	for(DWORD i = 0; i < _notify_close_zone_left_time_in_sec.size(); ++ i){
		_notify_close_zone_left_time_in_sec[i] *= ONE_MIN_IN_SEC;
	}
    return true;
}

void zoMonitorService::finalService()
{
    Tidy::nBaseMonitorService::finalService();
}

void zoMonitorService::addUser(zoUser *user)
{
}

void zoMonitorService::removeUser(zoUser *user)
{
    Tidy::nServerConn *logicConn = user->getLogicConn();
    if(logicConn != NULL){
        logicConn->getData<zoLogicConn>()->decUserNum();
    }
    else{
        // Logic宕机
    }
    const Tidy::nServerConn *gwConn = user->getGatewayConn();
    if(gwConn != NULL){
        gwConn->getData<zoGatewayConn>()->decUserNum();
    }
    else{
        // Gateway宕机
    }
}

void zoMonitorService::removeUser(const DWORD logicConnID, const DWORD gatewayConnID)
{
	Tidy::nServerConn *conn = NULL;
	if(logicConnID != 0){
		conn = server->getTaskConnByID(logicConnID);
		if(conn != NULL){
			conn->getData<zoLogicConn>()->decUserNum();
		}
	}
	if(gatewayConnID != 0){
		conn = server->getTaskConnByID(gatewayConnID);
		if(conn != NULL){
			conn->getData<zoGatewayConn>()->decUserNum();
		}
	}
}

void zoMonitorService::addRoom(Tidy::nServerConn *logicConn)
{
    logicConn->getData<zoLogicConn>()->incRoomNum();
}

void zoMonitorService::removeRoom(const DWORD logicConnID)
{
    Tidy::nServerConn *logicConn = server->getTaskConnByID(logicConnID);
    if(logicConn == NULL){
        assert_debug(server->getServerEntityByID(logicConnID) != NULL, "没找到服务器");
        // Logic宕机会出现这种情况
        return;
    }
    logicConn->getData<zoLogicConn>()->decRoomNum();
}

namespace{
    struct GetLeastUserNumLogicExec{
        zoLogicConn *_logic_conn;
        GetLeastUserNumLogicExec():_logic_conn(NULL){
        }
        bool exec(Tidy::nServerConn *conn){
            assert_debug(conn->getServerType() == LOGICSERVER, "");
            // TODO 具体策略应该可以更好配置,每个LogicServer配置它的承载数量
            zoLogicConn *logicConn = conn->getData<zoLogicConn>();
            if(_logic_conn == NULL){
                _logic_conn = logicConn;
            }
            else if(_logic_conn->getUserNum() > logicConn->getUserNum()){
                _logic_conn = logicConn;
            }
            return true;
        }
    };
};

Tidy::nServerConn* zoMonitorService::getLeastUserNumLogicConn() const
{
    GetLeastUserNumLogicExec exec;
    server->execTaskConnByType(exec, LOGICSERVER);
    return (exec._logic_conn != NULL) ? exec._logic_conn->baseConn : NULL;
}

Tidy::nServerConn* zoMonitorService::selectLeastUserNumLogicConn(Tidy::nServerConn *logicConn) const
{
    if(logicConn == NULL){
        logicConn = getLeastUserNumLogicConn();
    }
    if(logicConn != NULL){
        assert_debug(logicConn->getServerType() == LOGICSERVER, "[%u]", logicConn->getServerType());
        logicConn->getData<zoLogicConn>()->incUserNum();
    }
    return logicConn;
}

namespace{
    struct GetLeastUserNumGatewayExec{
        zoGatewayConn *_gateway_conn;
        GetLeastUserNumGatewayExec(): 
            _gateway_conn(NULL)
        {
        }
        bool exec(Tidy::nServerConn *conn){
            assert_debug(conn->getServerType() == GATEWAYSERVER, "");
            zoGatewayConn *gwConn = conn->getData<zoGatewayConn>();
            if(_gateway_conn == NULL){
                _gateway_conn = gwConn;
            }
            else if(_gateway_conn->getUserNum() > gwConn->getUserNum()){
                _gateway_conn = gwConn;
            }
            return true;
        }
    };
};

Tidy::nServerConn* zoMonitorService::getLeastUserNumGatewayConn() const
{
    GetLeastUserNumGatewayExec exec;
    server->execTaskConnByType(exec, GATEWAYSERVER);
    return (exec._gateway_conn != NULL) ? exec._gateway_conn->baseConn : NULL;
}

Tidy::nServerConn* zoMonitorService::selectLeastUserNumGatewayConn(Tidy::nServerConn *gatewayConn) const
{
    if(gatewayConn == NULL){
        gatewayConn = getLeastUserNumGatewayConn();
    }
    if(gatewayConn != NULL) {
        assert_debug(gatewayConn->getServerType() == GATEWAYSERVER, "[%u]", gatewayConn->getServerType());
        gatewayConn->getData<zoGatewayConn>()->incUserNum();
    }
    return gatewayConn;
}

namespace{
    struct LogServerLoadExec{
        bool exec(Tidy::nServerConn *taskConn){
            if(taskConn->getServerType() == LOGICSERVER){
                zoLogicConn *logicConn = taskConn->getData<zoLogicConn>();
                server->info("逻辑[%u]人数[%u]", taskConn->getServerID(), logicConn->getUserNum());
            }
            else if(taskConn->getServerType() == GATEWAYSERVER){
                server->info("网关[%u]人数[%u]", taskConn->getServerID(), taskConn->getData<zoGatewayConn>()->getUserNum());
            }
            return true;
        }
    }logServerLoadExec;
};
void zoMonitorService::logServerStatus()
{
    server->info("区负载数据");
    server->execTaskConnByType(logServerLoadExec, LOGICSERVER);
    server->execTaskConnByType(logServerLoadExec, GATEWAYSERVER);
    Tidy::nBaseMonitorService::logServerStatus();
}

void zoMonitorService::dealChangeLogicConn(Tidy::nServerConn *oldLogicConn, Tidy::nServerConn *newLogicConn)
{
	if(oldLogicConn != NULL){
		oldLogicConn->getData<zoLogicConn>()->decUserNum();
	}
	if(newLogicConn != NULL){
		newLogicConn->getData<zoLogicConn>()->incUserNum();
	}
}

bool zoMonitorService::addCommandFilter(const BYTE cmd, const BYTE param)
{
	server->info("[消息屏蔽],屏蔽消息[%u,%u]", cmd, param);
	return _command_filter_set.insert(std::make_pair(cmd, param)).second == true;
}

bool zoMonitorService::removeCommandFilter(const BYTE cmd, const BYTE param)
{
	server->info("[消息屏蔽],取消屏蔽[%u,%u]", cmd, param);
	return _command_filter_set.erase(std::make_pair(cmd, param)) > 0;
}

bool zoMonitorService::clearCommandFilter()
{
	server->info("[消息屏蔽],取消屏蔽所有消息");
	bool empty = _command_filter_set.empty();
	_command_filter_set.clear();
	return empty == false;
}

void zoMonitorService::dealGatewayConnectSuccess(Tidy::nServerConn* conn)
{
	protocol::ServerRun::FilterUserCmdCommand send;
	send.operType = protocol::ServerRun::eCommandFilterOperType_Add;
	for(Tidy::set<std::pair<BYTE, BYTE> >::iterator iter = _command_filter_set.begin(); iter != _command_filter_set.end(); ++ iter){
		send.cmd = (*iter).first;
		send.param = (*iter).second;
		conn->sendCmd(&send, sizeof(send));
	}
}

namespace{
    struct ResetConnUserNumExec{
        bool exec(Tidy::nServerConn *taskConn){
            if(taskConn->getServerType() == LOGICSERVER){
				taskConn->getData<zoLogicConn>()->clearUserNum();
            }
            else if(taskConn->getServerType() == GATEWAYSERVER){
				taskConn->getData<zoGatewayConn>()->clearUserNum();
            }
            return true;
        }
    };
};

bool zoMonitorService::resetConnUserNum()
{
	ResetConnUserNumExec exec;
    server->execTaskConnByType(exec, LOGICSERVER);
    server->execTaskConnByType(exec, GATEWAYSERVER);
	server->userService->resetConnUserNum();
    server->info("重算区负载数据");
    server->execTaskConnByType(logServerLoadExec, LOGICSERVER);
    server->execTaskConnByType(logServerLoadExec, GATEWAYSERVER);
	return true;
}

bool zoMonitorService::doTick()
{
	if(_reset_conn_user_num_timer.isReady() == true){
		resetConnUserNum();
	}
	if(_one_sec_timer.isReady() == true){
		if(_close_zone_time_in_sec > 0){
			checkCloseZone();
		}
	}
	return true;
}

bool zoMonitorService::setConnUserNum(Tidy::nServerConn *taskConn, const DWORD num)
{
	if(taskConn->getServerType() == LOGICSERVER){
		taskConn->getData<zoLogicConn>()->setUserNum(num);
	}
	else if(taskConn->getServerType() == GATEWAYSERVER){
		taskConn->getData<zoGatewayConn>()->setUserNum(num);
	}
	else{
		return false;
	}
	server->info("指令设置负载数值[%u,%u]", taskConn->getID(), num);
	return true;
}

void zoMonitorService::dealHupSig()
{
	_reset_conn_user_num_timer.reset(Tidy::global["RESET_CONN_USER_NUM_IN_SEC"].getDword() * ONE_SEC, server->time());
}

bool zoMonitorService::setCloseZoneTime(const DWORD closeTimeInSec)
{
	_close_zone_time_in_sec = closeTimeInSec;
	_next_notify_close_zone_time_in_sec = server->time().sec();
	_last_notify_close_zone_left_time_in_sec = server->time().sec();
	std::string timeStr = Tidy::nDayTime::getTimeInStr(closeTimeInSec, "%Y-%m-%d %H:%M:%S");
	server->info("[关服操作]设置关服时间[%lu,%u,%s]", server->time().sec(), _close_zone_time_in_sec, timeStr.c_str());
	return true;
}

bool zoMonitorService::clearCloseZoneTime()
{
	if(_close_zone_time_in_sec == 0){
		return false;
	}
	_close_zone_time_in_sec = 0;
	_last_notify_close_zone_left_time_in_sec = 0;
	_next_notify_close_zone_time_in_sec = 0;
	server->info("[关服操作]清除关服时间[%lu]", server->time().sec());
	return true;
}

void zoMonitorService::checkCloseZone()
{
	DWORD curTimeInSec = server->time().sec();
	if(_close_zone_time_in_sec <= curTimeInSec){
		server->info("[关服操作]倒计时结束开始关服[%lu,%u]", server->time().sec(), _close_zone_time_in_sec);
		_close_zone_time_in_sec = 0;
		_last_notify_close_zone_left_time_in_sec = 0;
		_next_notify_close_zone_time_in_sec = 0;
		server->normalCloseZone();
	}
	else{
		if(_next_notify_close_zone_time_in_sec > 0 && _next_notify_close_zone_time_in_sec <= curTimeInSec){
			Tidy::nDayTime dayTime(_close_zone_time_in_sec);
			//server->chatService->sendSysMessageOnTopToWorld(1, command::chat::eMessagePriority_High, 0, false, 10040, dayTime.getYears(), dayTime.getMonths() + 1, dayTime.getMDays(), dayTime.getHours(), dayTime.getMinutes());
			//server->chatService->sendSysMessageToWorld(10040, dayTime.getYears(), dayTime.getMonths() + 1, dayTime.getMDays(), dayTime.getHours(), dayTime.getMinutes());
			_next_notify_close_zone_time_in_sec = curTimeInSec + _notify_close_zone_interval_time_in_sec;
		}
		for(DWORD i = 0; i < _notify_close_zone_left_time_in_sec.size(); ++ i){
			DWORD leftTimeInSec = _notify_close_zone_left_time_in_sec[i];
			if(curTimeInSec + leftTimeInSec >= _close_zone_time_in_sec && _last_notify_close_zone_left_time_in_sec + leftTimeInSec < _close_zone_time_in_sec){
				_last_notify_close_zone_left_time_in_sec = curTimeInSec;
				if(leftTimeInSec > ONE_HOUR_IN_SEC){
					//server->chatService->sendSysMessageOnTopToWorld(1, command::chat::eMessagePriority_High, 0, false, 10041, leftTimeInSec/ONE_HOUR_IN_SEC, (leftTimeInSec%ONE_HOUR_IN_SEC)/ONE_MIN_IN_SEC);
					//server->chatService->sendSysMessageToWorld(10041, leftTimeInSec/ONE_HOUR_IN_SEC, (leftTimeInSec%ONE_HOUR_IN_SEC)/ONE_MIN_IN_SEC);
				}
				else{
					//server->chatService->sendSysMessageOnTopToWorld(1, command::chat::eMessagePriority_High, 0, false, 10042, leftTimeInSec/ONE_MIN_IN_SEC);
					//server->chatService->sendSysMessageToWorld(10042, leftTimeInSec/ONE_MIN_IN_SEC);
				}
			}
		}
	}
}
