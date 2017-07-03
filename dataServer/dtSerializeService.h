#ifndef DATASERVER_DTSERIALIZESERVICE_H_
#define DATASERVER_DTSERIALIZESERVICE_H_

#include "base/nObjLogger.h"
#include "base/nAsyncSqlThread.h"
#include "base/nTimer.h"

#include "game/nMiscDataSerialize.h"

namespace Tidy{
	class nServerConn;
};
class dtUser;
namespace protocol {
    namespace base {
        struct SerializeCommandBase;
        struct RelationCommandBase;
    };
};

struct AsyncSqlCallback : public Tidy::ExecSqlCallback{
    public:
        bool getResult() const;
        virtual bool dealExecResult(bool result) = 0;

    private:
        virtual bool exec(bool result);

    private:
        bool _result;
};
class dtSerializeService : public Game::nMiscDataSerialize, public Tidy::nObjLogger<dtSerializeService>
{
    public:
        dtSerializeService();
        ~dtSerializeService();

    public:
        bool initService();
        bool finalService();
        bool doTick();

    public:
        const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;
        Tidy::nLogger* getLogger() const;

    public:
        bool parseProtoSerializeCommand(Tidy::nServerConn* conn, const protocol::base::SerializeCommandBase* nullCommand, const DWORD len);
        bool parseProtoRelationCommand(Tidy::nServerConn* conn, const protocol::base::RelationCommandBase* nullCommand, const DWORD len);

    public:
        bool exeAsyncSql(const DWORD dbConnID, const std::string &sql, AsyncSqlCallback *callback);
        bool addAsyncSqlCallback(AsyncSqlCallback *callback);
		std::string getLastLoginIP(const DWORD charID, const char *name);
        std::string getAccountName(const char *name);
        std::string getName(const char *accountName);
        bool dealRemoveUser(const DWORD userID, const bool isRobot);
        bool dealDeleteAllRobot();
        bool saveMiscData(const DWORD miscDataType, const DWORD dataKey, const char* data, const DWORD num);

    private:
        bool saveUserData(const DWORD charID, const char *data, const DWORD num);
        bool saveUserRelationData(const DWORD charID, const char *data, const DWORD num);

    private:
        Tidy::nAsyncSqlThread *_sql_thread;
        Tidy::vector<AsyncSqlCallback*> _async_callback_result_vec;
        Tidy::nMutex _async_callback_result_vec_mutex;
        Tidy::nTimer _hundred_msec_timer;
};
#endif
