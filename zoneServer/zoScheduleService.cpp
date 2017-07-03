#include "protocol/serverRunCommand.h"
#include "protocol/Proto/ServerRunData.pb.h"

#include "base/nXMLParser.h"

#include "zoneServer/zoScheduleService.h"
#include "zoneServer/zoServer.h"
#include "zoneServer/zoMessageService.h"

zoScheduleService::zoScheduleService():
    _check_timer(10 * ONE_SEC)
{
}

zoScheduleService::~zoScheduleService()
{
}

bool zoScheduleService::initService()
{
    if(loadConfig() == false){
        assert_fail("加载配置失败");
        return false;
    }
    return true;
}

void zoScheduleService::finalService()
{
    _config_schedule_manager.clear();
}

Schedule::Schedule(const DWORD configID, const DWORD scheduleServerType, const DWORD scheduleServerID, const DWORD eventID, const std::string &eventParam, const SWORD month, const SWORD monthDay, const SWORD weekDay, const BYTE hour, const BYTE minute, const BYTE sec):
    Tidy::nSchedule(month, monthDay, weekDay, hour, minute, sec),
    _server_type(scheduleServerType),
    _server_id(scheduleServerID),
    _event_id(eventID),
    _event_param(eventParam),
    _config_id(configID),
    _stoped(false)
{
    assert_debug(_server_type < MAX_SERVERTYPE || _server_id != 0, "");
}

Schedule::~Schedule()
{
}

DWORD Schedule::getConfigID() const
{
    return _config_id;
}

bool Schedule::fireEvent()
{
    if(_stoped == true){
        server->info("schedule[%u] 被取消,不执行fireEvent", _config_id);
        return true;
    }
    server->sendServerEvent(_server_type, _server_id, _event_id, _event_param.data(), _event_param.size());
    return true;
}

bool Schedule::stop()
{
    _stoped = true;
    server->info("schedule[%u] 被取消", _config_id);
    return true;
}

bool Schedule::restart()
{
    _stoped = false;
    server->info("schedule[%u] 被恢复", _config_id);
    return true;
}

namespace{
    struct DeleteConfigScheduleExec{
        bool exec(Schedule *schedule){
            server->scheduleService->deleteSchedule(schedule);
            return true;
        }
    }deleteConfigScheduleExec;
};
bool zoScheduleService::loadConfig()
{
    Tidy::nXMLParser xml;
    const char* file = (const char*)(Tidy::global["ScheduleConfig"]);
    if(xml.loadFile(file) == false) {
        assert_fail("加载ScheduleConfig失败");
        return false;
    }
    _config_schedule_manager.execAll(deleteConfigScheduleExec);
    _config_schedule_manager.clear();
    if(xml.setCurNode("Config", "Schedule")) {
        const Schedule *schedule = NULL;
        DWORD configID = 0;
        protocol::ServerRun::ScheduleParamData scheduleParamData;
        while(xml.isNoneNode() == false) {
            if(xml.getNodeAttrInt("id", &configID) == false){
                assert_fail("没有配置id");
                return false;
            }
            scheduleParamData.Clear();
            scheduleParamData.set_configid(configID);
            DWORD  scheduleServerType   = 0;
            DWORD  scheduleServerID     = 0;
            DWORD  eventID  = 0;
            std::string  eventParam;
            SWORD  month    = 0;
            SWORD  monthDay = 0;
            SWORD  weekDay  = 0;
            BYTE   hour     = 0;
            BYTE   minute   = 0;
            BYTE   sec      = 0;

            if(xml.setChildNode("Time") == false){
                assert_fail("无节点");
            }
            if(xml.getNodeAttrInt("month", &month) == false){
                assert_fail("无属性");
            }
            if(xml.getNodeAttrInt("monthDay", &monthDay) == false){
                assert_fail("无属性");
            }
            if(xml.getNodeAttrInt("weekDay", &weekDay) == false){
                assert_fail("无属性");
            }
            if(xml.getNodeAttrInt("hour", &hour) == false){
                assert_fail("无属性");
            }
            if(xml.getNodeAttrInt("minute", &minute) == false){
                assert_fail("无属性");
            }
            if(xml.getNodeAttrInt("sec", &sec) == false){
                assert_fail("无属性");
            }
            xml.setUpNode();

            if(xml.setChildNode("Event") == false){
                assert_fail("无节点");
            }
            if(xml.getNodeAttrInt("serverType", &scheduleServerType, 0) == false){
            }
            if(xml.getNodeAttrInt("id", &eventID) == false){
                assert_fail("无属性");
            }

            xml.setChildNode("Param");
            std::string paramName;
            std::string paramValue;
            while(xml.isNoneNode() == false) {
                if(xml.getNodeAttrStr("name", &paramName) == false){
                    assert_fail("没有找到name属性");
                    return false;
                }
                const char *content = xml.getNodeContentStr();
                if(content != NULL){
                    paramValue = content;
                }
                else{
                    paramValue.clear();
                }
                protocol::ServerRun::ParamData *param = scheduleParamData.add_paramlist();
                param->set_paramname(paramName);
                param->set_paramvalue(paramValue);

                xml.setNextNode();
            }
            xml.setUpNode();

            xml.setUpNode();

            if(scheduleParamData.SerializeToString(&eventParam) == false){
                assert_fail("序列化失败");
                return false;
            }
            schedule = createSchedule(configID, scheduleServerType, scheduleServerID, eventID, eventParam, month, monthDay, weekDay, hour, minute, sec);
            if(schedule == NULL){
                assert_fail("创建schedule失败[%u]", configID);
                return false;
            }

            xml.setNextNode();
        }
    }
    else{
        server->error("加载ScheduleConfig失败:%s", (const char *)Tidy::global["ScheduleConfig"]);
        assert_fail("加载ScheduleConfig失败");
    }
    return true;
}

bool zoScheduleService::doTick()
{
    if(_check_timer(server->time()) == false){
        return true;
    }
    return Tidy::nScheduleService::doTick(server->time());
}

const Schedule* zoScheduleService::createSchedule(const DWORD configID, const DWORD scheduleServerType, const DWORD scheduleServerID, const DWORD eventID, const std::string &eventParam, const SWORD month, const SWORD monthDay, const SWORD weekDay, const BYTE hour, const BYTE minute, const BYTE sec)
{
    Schedule *schedule = new Schedule(configID, scheduleServerType, scheduleServerID, eventID, eventParam, month, monthDay, weekDay, hour, minute, sec);
    if(configID != 0){
        if(_config_schedule_manager.add(schedule) == false){
            assert_fail("添加失败");
            SAFE_DELETE(schedule);
            return false;
        }
    }
    if(addSchedule(schedule) == false){
        if(configID != 0){
            _config_schedule_manager.remove(schedule);
        }
        assert_fail("添加失败");
        SAFE_DELETE(schedule);
        return schedule;
    }
    server->info("创建Schedule 成功 [%u,%u,%u,%u,%d,%d,%d,%u,%u,%u]", configID, scheduleServerType, scheduleServerID, eventID, month, monthDay, weekDay, hour, minute, sec);
    return schedule;
}

void zoScheduleService::dealHupSig()
{
    //loadConfig();
}

bool zoScheduleService::fireSchedule(const DWORD configID)
{
    Schedule *schedule = _config_schedule_manager.getByID(configID);
    if(schedule == NULL){
        return false;
    }
    return schedule->fireEvent();
}

bool zoScheduleService::restartSchedule(const DWORD configID)
{
    Schedule *schedule = _config_schedule_manager.getByID(configID);
    if(schedule == NULL){
        return false;
    }
    return schedule->restart();
}

bool zoScheduleService::stopSchedule(const DWORD configID)
{
    Schedule *schedule = _config_schedule_manager.getByID(configID);
    if(schedule == NULL){
        return false;
    }
    return schedule->stop();
}
