#ifndef GAME_NUSER_H_
#define GAME_NUSER_H_

#include "base/nObj.h"
#include "base/Tidy.h"

#include "game/nGameServer.h"

namespace Game
{
	/// @brief	角色定义基类,只包含必要的网络属性
	template<typename ImpUserT>
		class nUser
		{
			protected:
				nUser() : _is_offline_protect(false)
			{
				++ _online_num; 
				Tidy::baseServer->debug("本服在线人数:[%u],新角色指针:[%p]", _online_num, this);
			}
				~nUser()
				{
					assert_debug(_online_num > 0, "");
					-- _online_num;
					Tidy::baseServer->debug("本服在线人数:[%u],回收指针:[%p]", _online_num, this);
				}

			public:
				void setOfflineProtect(bool offlineProtect){
					_is_offline_protect = offlineProtect;
				}
				bool isOfflineProtect() const{
					return _is_offline_protect;
				}
				bool isOnline() const{
					return _is_offline_protect == false;
				}

			public:
				/// @brief	在线人数
				static DWORD _online_num;
				bool _is_offline_protect;
		};

	template<typename ImpUserT>
		DWORD nUser<ImpUserT>::_online_num = 0;
};

#endif
