#ifndef ZONESERVER_ZOMONITORSERVICE_H_
#define ZONESERVER_ZOMONITORSERVICE_H_

#include "base/nSTL.h"
#include "base/nBaseMonitorService.h"

class zoUser;
namespace Tidy{
	class nServerConn;
};
namespace protocol{
    struct NullProtoCommand;
    namespace base{
        struct MonitorCommandBase;
    };
};

class zoMonitorService : public Tidy::nBaseMonitorService
{
    public:
        zoMonitorService();

    private:
        virtual ~zoMonitorService();

    public:
        const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;
        Tidy::nLogger* getLogger() const;

    public:
        void addUser(zoUser *user);
        void removeUser(zoUser *user);
		void removeUser(const DWORD logicConnID, const DWORD gatewayConnID);
        void addRoom(Tidy::nServerConn *logicConn);
        void removeRoom(const DWORD logicConnID);
        Tidy::nServerConn* getLeastUserNumLogicConn() const;
        Tidy::nServerConn* selectLeastUserNumLogicConn(Tidy::nServerConn *logicConn) const;
        Tidy::nServerConn* getLeastUserNumGatewayConn() const;
        Tidy::nServerConn* selectLeastUserNumGatewayConn(Tidy::nServerConn *gatewayConn) const;
        void dealChangeLogicConn(Tidy::nServerConn *oldLogicConn, Tidy::nServerConn *newLogicConn);
		bool addCommandFilter(const BYTE cmd, const BYTE param);
		bool removeCommandFilter(const BYTE cmd, const BYTE param);
		bool clearCommandFilter();
		void dealGatewayConnectSuccess(Tidy::nServerConn* conn);
		bool resetConnUserNum();
		bool doTick();
		bool setConnUserNum(Tidy::nServerConn *taskConn, const DWORD num);
		bool setCloseZoneTime(const DWORD closeTimeInSec);
		bool clearCloseZoneTime();
		void checkCloseZone();

    private:
        virtual bool initService();
        virtual void finalService();
        virtual void logServerStatus();
		virtual void dealHupSig();

	private:
		Tidy::set<std::pair<BYTE, BYTE> > _command_filter_set;
		Tidy::nServerTickTimer _reset_conn_user_num_timer;
		Tidy::nServerTickTimer _one_sec_timer;

		Tidy::vector<DWORD> _notify_close_zone_left_time_in_sec;
		DWORD _notify_close_zone_interval_time_in_sec;
		DWORD _close_zone_time_in_sec;
		DWORD _last_notify_close_zone_left_time_in_sec;
		DWORD _next_notify_close_zone_time_in_sec;
};
#endif
