#ifndef ZONESERVER_SCHEDULESERVICE_H_
#define ZONESERVER_SCHEDULESERVICE_H_

#include "base/nScheduleService.h"
#include "base/nTimer.h"

class Schedule : public Tidy::nSchedule{
    public:
        Schedule(const DWORD configID, const DWORD scheduleServerType, const DWORD scheduleServerID, const DWORD eventID, const std::string &eventParam, const SWORD month, const SWORD monthDay, const SWORD weekDay, const BYTE hour, const BYTE minute, const BYTE sec);
        virtual ~Schedule();

    public:
        DWORD getConfigID() const;
        bool stop();
        bool restart();

    public:
        virtual bool fireEvent();

    private:
        DWORD _server_type;
        DWORD _server_id;
        DWORD _event_id;
        std::string _event_param;
        DWORD _config_id;
        bool _stoped;
};
struct GetConfigIDPolicy {
    typedef Schedule EntityType;
    static DWORD getID(const Schedule *schedule){
        return schedule->getConfigID();
    }
};
class zoScheduleService : public Tidy::nScheduleService
{
    public:
        zoScheduleService();
        ~zoScheduleService();

    public:
        bool initService();
        void finalService();
        bool doTick();
        void dealHupSig();

    public:
        const Schedule* createSchedule(const DWORD configID, const DWORD scheduleServerType, const DWORD scheduleServerID, const DWORD eventID, const std::string &eventParam, const SWORD month, const SWORD monthDay, const SWORD weekDay, const BYTE hour, const BYTE minute, const BYTE sec);
        bool fireSchedule(const DWORD configID);
        bool restartSchedule(const DWORD configID);
        bool stopSchedule(const DWORD configID);

    private:
        bool loadConfig();

    private:
        Tidy::nTimer _check_timer;
        Tidy::vector<const Schedule*> _config_schedule_list;
        Tidy::nManager<false, Tidy::nIndexByID< GetConfigIDPolicy > > _config_schedule_manager;
};
#endif
