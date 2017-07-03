#ifndef _GATEWAY_USER_H_
#define _GATEWAY_USER_H_

#include "protocol/loginCommon.h"
#include "game/nUser.h"

namespace Tidy{
	class nServerConn;
};
class gwUserTaskConn;
class gwRoom;
class gwLoginSession;

namespace protocol{
	namespace login{
		struct UserLogoutData;
		struct LoginUserSerializeData;
	};
	namespace user{
		class LoginSerializeBinary; 
	};
};

#if 0
enum gwUserState{
    gwUserState_Init           =   0,
    gwUserState_WaitSelectChar =   1,
    gwUserState_WaitLoginSuccess=  2,
    gwUserState_ChangeServer   =   3,
    gwUserState_WaitUnregWithoutLogout =   4,
    gwUserState_WaitLogoutReSelectChar=   5,
    gwUserState_Normal         =   6,
    gwUserState_WaitDeleteChar =   7,
    gwUserState_Unreged        =   8,
    gwUserState_Max
};
#endif

enum eGateUserDataFlag{
    eGateUserDataFlag_NeedOfflineProtect    =   0,
    eGateUserDataFlag_IsSyncTimeTick,
    eGateUserDataFlag_IsInPrepareScene,
    eGateUserDataFlag_IsInRestRoom,
    eGateUserDataFlag_HasPassGuideMap,
    eGateUserDataFlag_WaitIntoScene,
    eGateUserDataFlag_IsSceneAudience,
    eGateUserDataFlag_IsSysAudience,
    eGateUserDataFlag_Max,
};
struct SelectedEntityData{
    SelectedEntityData(const DWORD selectedEntityID, const BYTE selectedEntityType) : entityID(selectedEntityID), entityType(selectedEntityType)
    {
    }
    DWORD entityID;
    BYTE entityType;
};

class gwUser : public Game::nUser<gwUser>, public Tidy::nObj
{
    public:
        gwUser(const char *accountName, protocol::login::eLoginState initLoginState );
        virtual ~gwUser();

	public:
		bool initData(const protocol::login::LoginUserSerializeData *userData);
        DWORD getCharID() const;
        const char* getAccountName() const;
        DWORD getSessionID() const;
        void setName(const char *name);
		bool setLevel(const WORD level);
		WORD getLevel() const;
        void doTick();

    public:
        bool reg(const protocol::login::UserLoginData &userData);
		void unreg();
		void dealResumeOnline();

    public:
        const DWORD getLogicConnID() const;
        void setLogicConn(Tidy::nServerConn* serverConn);
        bool syncPingTimeInMsec();
        QWORD getPingTimeInMsec() const;
        bool syncTime();
        bool checkResetLogicConn(Tidy::nServerConn *logicConn);
        void setNeedSyncTime();

    public:
        bool parseProtoUserCommand(Tidy::nServerConn* serverConn, const protocol::base::UserCommandBase* nullCommand, const DWORD len);

    public:
		const char *getIP() const;
        void setIP(const char* ip);
		bool fillLogoutData(protocol::login::UserLogoutData *logoutData) const;

    public:
        bool sendLoginEvent(const BYTE event);
        const bool isRegged() const;
        Tidy::nMutex& getLogoutLock();
        Tidy::nRWLock& getDataLock();

    public:
        const BYTE getLoginState() const;
        const bool isInLoginState(const BYTE state) const;
        protocol::login::eUserLogOutSourceType getLogoutSource() const;
        void setLogoutSource(const protocol::login::eUserLogOutSourceType logoutSource);
        bool isInLogout();
        void setNeedOfflineProtect();
        void clearNeedOfflineProtect();
        bool checkNeedOfflineProtect() const;
        void dealOfflineProtect();
		const gwLoginSession* getLoginSession() const;
		void clearLoginSession();

    public:
        void setSelectedEntity(const DWORD selectedEntityID, const BYTE selectedEntityType);
        const bool checkSelectedEntity(const DWORD entityID, const BYTE entityType) const;
        bool setHasPassGuideMap();
        void setSessionID(const DWORD sessionID);

    private:
        bool transitLoginState(const BYTE newState);
        void setLoginState(const BYTE state);
        void dealLoginSuccess();
        bool sendPingTimeToLogic();
        void setUserDataFlag(eGateUserDataFlag flag, bool value);
    public:
        bool sendCmdToLogic(const protocol::NullProtoCommand *nullCommand, const DWORD len) const;

    private:
        std::string _account_name;
		const gwLoginSession *_login_session;

    private:
        DWORD _screen_index;
        BYTE _login_state;
        BYTE _logout_source;
        DWORD _ping_time_in_msec;
        DWORD _last_sync_time_time_in_sec;
        mutable Tidy::nRWLock _data_lock;
        Tidy::BitSet<eGateUserDataFlag_Max> _user_data_flags_set;
		WORD _level;
        DWORD _session_id;
        Tidy::nServerConn* _logic_conn;
        std::string _ip;

    private:
        SelectedEntityData _selected_entity_data;
};

#endif
