#ifndef GAME_NSCRIPTOBJ_H_
#define GAME_NSCRIPTOBJ_H_

#include "base/nBaseScriptObj.h"
#include "base/nTimer.h"

#include "game/nGameServer.h"

namespace Game
{
	//tolua_begin
	enum eActionClearType{
		eActionClearType_None,
		eActionClearType_MAX,
	};
	//tolua_end

	struct ActionData{
		ActionData(const QWORD nextFireTime, const DWORD intervalTimeInMsec, const DWORD curTimes, const DWORD maxTimes, const DWORD id) :
			nextFireTimeInMsec(nextFireTime),
			intervalInMsec(intervalTimeInMsec),
			curFireTimes(curTimes),
			maxFireTimes(maxTimes),
			actionID(id)
		{
		}

		QWORD nextFireTimeInMsec;
		DWORD intervalInMsec;
		DWORD curFireTimes;
		DWORD maxFireTimes;
		DWORD actionID;
	};

	template<typename OwnerT, typename ImpT>
		class nScriptObj : public Tidy::nBaseScriptObj
	{
		protected:
			nScriptObj(OwnerT *ownerObj, void *pThis, Tidy::nLuaState* luaState, const char* typeName) :
				Tidy::nBaseScriptObj(pThis, luaState, typeName),
				owner(ownerObj),
				_action_data_container(NULL),
				_script_tick_timer(300)
		{
		}
			virtual ~nScriptObj(){
				SAFE_DELETE(_action_data_container);
			}

		public:
			bool registerScriptAction(const DWORD actionClearType, const DWORD delayTimeInMsec, const DWORD intervalInMsec, const DWORD maxTimes, const DWORD actionID);
			bool addActionDelay(const DWORD actionID, const DWORD addTimes);
			bool callScriptAction(const DWORD actionID, const bool needRemove);

		public:
			virtual bool doScriptTick();

		public:
			void dispose()
			{
				Tidy::nBaseScriptObj::dispose();
				owner = NULL;
			}

			bool isNormal() const
			{
				return owner != NULL;
			}

		public:
			OwnerT *owner;

		private:
			Tidy::vector<ActionData> *_action_data_container;

		private:
			Tidy::nTimer _script_tick_timer;
	};

	template<typename OwnerT, typename ImpT>
		bool nScriptObj<OwnerT, ImpT>::registerScriptAction(const DWORD actionClearType, const DWORD delayTimeInMsec, const DWORD intervalInMsec, const DWORD maxFireTimes, const DWORD actionID)
		{
			assert_debug(intervalInMsec != 0 || maxFireTimes == 1, "");
			if(actionClearType >= eActionClearType_MAX) {
				assert_fail("actionClearType >= eActionClearType_MAX");
				return false;
			}
			if(_action_data_container == NULL) {
				_action_data_container = new Tidy::vector<ActionData>();
			}

			_action_data_container->push_back(ActionData((gameServer->time().msec() + delayTimeInMsec), intervalInMsec, 0, maxFireTimes, actionID));
			return true;
		}

	template<typename OwnerT, typename ImpT>
		bool nScriptObj<OwnerT, ImpT>::addActionDelay(const DWORD actionID, const DWORD addDelayTimeInMsec)
		{
			if(_action_data_container == NULL) {
				return true;
			}

			for(DWORD i = 0; i < _action_data_container->size(); ++ i) {
				ActionData& data = (*_action_data_container)[i];
				if(data.actionID == actionID){
					data.nextFireTimeInMsec = data.nextFireTimeInMsec + addDelayTimeInMsec;
					return true;
				}
			}

			return true;
		}

	template<typename OwnerT, typename ImpT>
		bool nScriptObj<OwnerT, ImpT>::doScriptTick()
		{
			if(_action_data_container == NULL) {
				return true;
			}

			if(_script_tick_timer(gameServer->time()) == false) {
				return true;
			}
			for(DWORD i = 0; i < _action_data_container->size();) {
				if(isNormal() == false){
					return true;
				}
				assert_debug(this->owner != NULL, "");
				ActionData& data = (*_action_data_container)[i];
				if(data.nextFireTimeInMsec <= gameServer->time().msec()) {
					data.nextFireTimeInMsec += data.intervalInMsec;
					if(data.nextFireTimeInMsec <= gameServer->time().msec()){
						data.nextFireTimeInMsec = gameServer->time().msec() + data.intervalInMsec;
					}
					++ data.curFireTimes;
					if(data.curFireTimes >= data.maxFireTimes && data.maxFireTimes > 0) {
						assert_debug(this->owner != NULL, "");
						callLuaFuncBool("l_fireAction", data.actionID, true);
						_action_data_container->removeByIndex(i);
						continue;
					}
					else {
						assert_debug(this->owner != NULL, "");
						if(callLuaFuncBool("l_fireAction", data.actionID, false) == false) {
							_action_data_container->removeByIndex(i);
							continue;
						}
					}
				}

				++ i;
			}

			return true;
		}

#if 0
	template<typename OwnerT, typename ImpT>
		void nScriptObj<OwnerT, ImpT>::final()
		{
			assert_debug(isNormal() == true, "已经失效");
			this->callLuaFuncVoid("l_final");
		}
#endif

	template<typename OwnerT, typename ImpT>
		bool nScriptObj<OwnerT, ImpT>::callScriptAction(const DWORD actionID, const bool needRemove)
		{
			return this->callLuaFuncBool("l_fireAction", actionID, needRemove);
		}
};
#endif
