#ifndef ZONESERVER_SERELATIONUSERSERVICE_H_
#define ZONESERVER_SERELATIONUSERSERVICE_H_

#include "base/nTimer.h"

#include "zoneServer/Relation/zoRelationManager.h"

class zoRelationUser;
namespace Tidy{
	class nServerConn;
};
class zoUser;

namespace protocol{
    namespace base{
        struct RelationCommandBase;
    };
    namespace login{
        struct CreateCharacterData;
    };
};
struct CreateRelationUserData;
class zoRelationUserService : public zoRelationManager<zoRelationUser>
{
	public:
		zoRelationUserService();
		~zoRelationUserService();

    public:
		bool init();
		bool doTick();
		void dealCore();

    public:
		zoRelationUser *getUserByID(DWORD userID);
        zoRelationUser *getUserByName(const char* userName);

    public:
        bool initAllRelationUser();
        bool loadRelationUser(const DWORD charID, zoRelationUser *user);
		zoRelationUser* createRelationUser(const char *accountName, const CreateRelationUserData &relationData);
		bool addRelationUser(zoRelationUser *user);
        void removeRelationUser(zoRelationUser *user);
        bool freshRelationUser(const DWORD charID);
        bool deleteRelationUser(const DWORD charID);

    public:
        bool parseProtoRelationCommand(Tidy::nServerConn* conn, const protocol::base::RelationCommandBase* nullCommand, const DWORD len);

	public:
		bool addNeedSerializeUser(zoRelationUser *user);

	private:
		bool saveRelationUserData();

    private:
        Tidy::map<DWORD, zoRelationUser*> _robot_user_manager;
		Tidy::vector<zoRelationUser*> _need_serialize_user_list;
		Tidy::nTimer _save_relation_user_data_timer;
};

#endif

