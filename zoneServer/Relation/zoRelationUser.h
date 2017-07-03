#ifndef ZONESERVER_RELATIONUSER_H_
#define ZONESERVER_RELATIONUSER_H_

#include "protocol/userCommon.h"
#include "protocol/Proto/UserData.pb.h"

#include "base/Tidy.h"
#include "base/nObj.h"
#include "base/nSTL.h"

class zoUser;
class zoClub;

namespace protocol{
    namespace user{
        struct WarResultData;
		struct TimeEndEffect;
		struct UseTimesEffect;
		struct MatchResultData;
        struct UserRelationSaveData;
    };
    namespace relation{
        struct CommonListData;
    };
	namespace login{
		struct LoginUserSerializeData;
	};
};

// 离线数据,保存在内存中,只有相关关系玩家可以看到的.
struct RelationUserData {
	RelationUserData(){
		reset();
	}
	void reset();
	DWORD 	charID;					    // 角色编号 
	char    charName[MAX_NAMESIZE];     // 角色名字
	BYTE    sex;                        // 性别
	WORD  	level;						// 角色当前等级
	DWORD   exp;                        // 角色当前经验值
	DWORD   totalExp;                   // 角色总经验值
    DWORD  winTimes;                    // 胜利次数
    DWORD  loseTimes;                   // 失败次数
    DWORD  pvpTimes;                    // PVP战斗总次数
    DWORD  runOffTimes;                 // 逃跑次数
	DWORD  diamond;                     // 总钻石数
	DWORD  diamondConsume;              // 消费的钻石数 
	BYTE   gmLevel;                     // gm等级
};

struct CreateRelationUserData
{
	CreateRelationUserData(){}
	void reset(){
		relationUserData.reset();
	}
	RelationUserData relationUserData;
};

enum eRelationUserDataNeedSaveFlag{
    eRelationUserDataNeedSaveFlag_MessageData   =   1,
	eRelationUserDataNeedSaveFlag_MAX
};
class zoRelationUser : public Tidy::nObjLogger<zoRelationUser>
{
	friend class zoUser;
	public:
	zoRelationUser(const char* accountName);
	~zoRelationUser();

    public:
        const char* getName() const;
		const char* getShowName() const;
        void setName(const char *name);
        const DWORD getID() const;
        const DWORD getCharID() const;
        const char* getAccountName() const;
        BYTE  getSex() const;
        Tidy::nLogger* getLogger() const;
        const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;
        bool isGM();
    public:
		bool init(const CreateRelationUserData &createRelationData);
		bool freshRelationData(const RelationUserData &relationData);

    public:
        bool reg(zoUser *user);
        void unreg(zoUser *user);

		/// @brief 逻辑服务器登陆成功回调
        void dealLoginSuccess();
        /// @brief 逻辑服务器重新上线回调
        void dealResumeOnline();

		bool doTick();
		bool saveData();

    public:
		/// @brief 是否在线
		bool isOnline() const;
        DWORD getCommonListDataVersion() const;
		DWORD getExtendListDataVersion() const;
        void setLevel(const WORD level);
        WORD getLevel() const;
        void setRunOffTimes(const DWORD runOffTimes);
        void setDiamond(const DWORD diamond);
        void setDiamondConsume(const DWORD diamondConsume);
        void setExp(const DWORD exp, const DWORD totalExp);
		void fillLoginData(protocol::login::LoginUserSerializeData *userData) const;
        DWORD getLastOfflineTime() const;
        void updateLastOfflineTime(const DWORD lastOfflineTime);
        void setLastLoginState(const BYTE loginState);
        BYTE getLastLoginState() const;

    public:
        void fillCommonListData(protocol::relation::CommonListData *data) const;
		void updateCommonListDataVersion(const bool needNotifyUser);

    private:
        void updateExtendListDataVersion();
		void setNeedSerialize(eRelationUserDataNeedSaveFlag flag);
        bool saveMessageData(protocol::user::UserRelationSaveData *relationSaveData);

	public:
		zoUser *ownerUser;
        RelationUserData relationUserData;

	private:
		const Tidy::string _account_name;
        DWORD _common_list_data_version;
        DWORD _extend_list_data_version;
		Tidy::BitSet<eRelationUserDataNeedSaveFlag_MAX> _relation_user_data_need_save_flag_set;
        Tidy::map<DWORD, std::string> _online_need_sys_message_map;
        BYTE _last_login_state;
};

inline bool zoRelationUser::isOnline() const
{
	return ownerUser != NULL;
}

#endif
