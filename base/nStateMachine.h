#ifndef _N_STATE_MACHINE_H_
#define _N_STATE_MACHINE_H_

#include "base/nState.h"
#include "base/nEventObserver.h"
#include "base/nEventCheck.h"
#include "base/nSTL.h"
#include "base/Tidy.h"
#include "base/nRandom.h"
#include "base/nBaseServer.h"

namespace Tidy
{
#define STATE_EVENT_MAX_PROBABILITY 100.0f

	struct nStateTransit {
		nStateTransit() : 
			state(NULL),
			probability(0) {
			}
		nStateTransit(nState* st, float pro):
			state(st),
			probability(pro) {
				assert_debug(state != NULL, "state不能为NULL");
			}
		nState* state;
		float probability; // 触发事件的几率，不能为0，最大是100
	};

	class nStateTable
	{
		public:
			/// @brief	构造函数
			nStateTable(DWORD &tableId, char* name) : _init_state(NULL)
		{
			this->id = tableId;
			_global_transit_table.clear();
			_transit_table.clear();
		}
			/// @brief	析构函数
			~nStateTable()
			{
				_global_transit_table.clear();
				_transit_table.clear();
			}

		public:
			/// @brief	初始化所有的状态事件
			static void initAllStateEvent();
			/// @brief	根据枚举值获得状态对象
			static nState* getStateByID(int stateID)
			{
				return _all_state[stateID];
			}

		public:
			/// @brief	设置初始状态
			inline bool setInitState(DWORD &state)
			{
				assert_debug(_init_state == NULL, "");

				_init_state = _all_state[state];
				return true;
			}
			/// @brief	全局转换,所有的状态共享
			inline bool addGlobalTransit(DWORD &event, DWORD &out, float probability = STATE_EVENT_MAX_PROBABILITY)
			{
				assert_debug(event != 0 && out != 0, "");
				assert_debug(probability > 0 && probability <= STATE_EVENT_MAX_PROBABILITY, "概率值不合法，必须>0且<=100");

				nStateTransit transit(_all_state[out], probability);
				_global_transit_table[event] =  transit; // _all_state[out];
				return true;
			}
			/// @brief	添加状态转换
			inline bool addTransit(DWORD &state, DWORD &event, DWORD &out, float probability = STATE_EVENT_MAX_PROBABILITY)
			{
				assert_debug( _transit_table[_all_state[state]].find(event) == _transit_table[_all_state[state]].end() , "");
				assert_debug(probability > 0 && probability <= STATE_EVENT_MAX_PROBABILITY, "概率值不合法，必须>0且<=100");

				nStateTransit transit(_all_state[out], probability);
				_transit_table[ _all_state[state] ][event] = transit; //_all_state[out];

				_state_event_table[ _all_state[state] ].push_back(event);
				return true;
			}
			/// @brief	事件查询.如果这个事件不属于这个状态关心的事件或几率随机失败就返回当前状态。
			inline nState* getNextState(nState *state, const DWORD &event)
			{
				TransitTable::const_iterator transitTableIter = _transit_table.find(state);
				bool needCheckGlobal = false;
				TransitTableItem::const_iterator iter;
				if(transitTableIter == _transit_table.end()){
					needCheckGlobal = true;
				}
				else{
					const TransitTableItem &table = transitTableIter->second;
					iter = table.find(event);
					if(iter == table.end()) {
						needCheckGlobal = true;
					}
				}
				if(needCheckGlobal == true){
					iter = _global_transit_table.find(event);
					if(iter == _global_transit_table.end()) {
						return state;
					}
				}
				const nStateTransit &stateTransit = iter->second;
				if(stateTransit.probability < STATE_EVENT_MAX_PROBABILITY){
					float rand = Tidy::randBetween(0.0f, STATE_EVENT_MAX_PROBABILITY);
					if(rand > stateTransit.probability) {
						// 没有触发事件
						return state;
					}
				}
				return stateTransit.state;
			}
			/// @brief  事件检测，返回这个事件是否是这个行为关心的事件
			const bool checkStateConcernedEvent(nState *state, const DWORD &event)
			{
				TransitTable::const_iterator transitTableIter = _transit_table.find(state);
				if(transitTableIter == _transit_table.end()){
					return false;
				}
				const TransitTableItem &table = transitTableIter->second;
				TransitTableItem::const_iterator iter = table.find(event);
				if(iter == table.end()) {
					return false;
				}
				return true;
			}
			/// @brief	注册某个状态的所有事件.收集事件.
			//			因为在注册事件的时候,有些事件已经丢失,比如进入九屏的事件.在收集事件的过程中,有可能会导致状态转换,
			//			所以需要每次收集之后检查是状态机的当前状态是否发生了变化,如果有变化就不要继续执行了.
			//			这里的顺序不要轻易改动
			/// @param	state 当前状态.每次collectOutEvent之后都可能变化
			template<typename StateMachineT>
				inline bool regStateEvent(const nEventObserver *observer, StateMachineT *stateMachine, nState *state)
				{
					StateEventTable::iterator stateEventTableIter = _state_event_table.find(state);
					if(stateEventTableIter == _state_event_table.end()) {
						return false;
					}

					const StateEventList &eventList = stateEventTableIter->second;
					nEventCheck* eventCheck = NULL;
					for(StateEventList::const_iterator iter = eventList.begin(); iter != eventList.end(); ++ iter) {
						eventCheck = _all_event[*iter];
						eventCheck->collectOutEvent((nEventObserver*)observer);
						if(stateMachine->getNextState() != stateMachine->getCurrentState() ||
                                stateMachine->getCurrentState() != state)
						{
							return true;
						}
						eventCheck->regMe((nEventObserver*)observer);
					}
					return true;
				}
			/// @brief	撤销注册某个状态的所有事件
			inline bool unregStateEvent(const nEventObserver *observer, nState *state)
			{
				StateEventTable::iterator stateEventTableIter = _state_event_table.find(state);
				if(stateEventTableIter == _state_event_table.end())
				{
					return false;
				}
				StateEventList &eventList = stateEventTableIter->second;
				for(StateEventList::iterator iter = eventList.begin(); iter != eventList.end(); ++ iter)
				{
					_all_event[*iter]->unregMe((nEventObserver*)observer);
				}
				return true;
			}
			/// @brief	注册全局事件,这些事件只有切换状态表时需要注销
			inline bool regGlobalEvent(const nEventObserver *observer)
			{
				for(TransitTableItem::const_iterator iter = _global_transit_table.begin(); iter != _global_transit_table.end(); ++ iter)
				{
					_all_event[iter->first]->regMe((nEventObserver*)observer);
				}
				return true;
			}
			/// @brief	撤销注册全局事件
			inline bool unregGlobalEvent(const nEventObserver *observer)
			{
				for(TransitTableItem::iterator iter = _global_transit_table.begin(); iter != _global_transit_table.end(); ++ iter)
				{
					_all_event[iter->first]->unregMe((nEventObserver*)observer);
				}
				return true;
			}
			/// @brief	获得初始状态.如果没有指定初始状态就以状态表的第一个为初始状态,这个并不可靠。
			inline nState* getInitState()
			{
				assert_debug(_init_state != NULL, "");
				if(_init_state == NULL)
				{
					TransitTable::iterator iter = _transit_table.begin();
					_init_state = iter->first;	
				}

				return _init_state;
			}

		public:
			inline const DWORD getID() const
			{
				return id;
			}

		public:
			DWORD id;

		private:
			/// @brief	事件对应的新状态
			typedef Tidy::hash_map<DWORD, nStateTransit>	TransitTableItem;
			/// @brief	事件列表,按照优先级排序
			typedef Tidy::list<DWORD> StateEventList;
			/// @brief	状态转换表
			typedef Tidy::hash_map<nState*, TransitTableItem> TransitTable;
			/// @brief	状态-事件,事件顺序是优先级顺序
			typedef Tidy::hash_map<nState*, StateEventList> StateEventTable;

			/// @brief	全局转换表,所有的状态都共享的
			TransitTableItem _global_transit_table;
			/// @brief	状态转换表
			TransitTable _transit_table;
			/// @brief	事件表
			StateEventTable _state_event_table;
			/// @brief	初始状态
			nState *_init_state;

		public:
			/// @brief	所有的状态对象
			static Tidy::vector<nState*> _all_state;
			/// @brief	所有的事件对象
			static Tidy::vector<nEventCheck*> _all_event;

	};

	/// @brief	状态机.
	/// @param	StateTableT 状态表类型.继承在nStateTable
	/// @param	OwnerT 所有者类型.继承在nObj和nEventObserver
	template<typename StateTableT, typename OwnerT, typename StateT>
		class nStateMachine
		{
			public:
				/// @brief	构造函数
				nStateMachine(const OwnerT *owner) : _transit_table(NULL), _cur_state(NULL), _pre_state(NULL), _next_state(NULL), _owner(owner),_debug(false), _is_in_set_current(false), _is_in_update(false)
			{
			}
				/// @brief	构造函数
				nStateMachine(const nEventObserver *owner, StateTableT *table) : _transit_table(table), _owner(owner)
			{
				_cur_state = _transit_table->getInitState();
				_next_state = _cur_state;
				_pre_state = NULL;
				_owner = NULL;
			}
				/// @brief	析构函数
				virtual ~nStateMachine()
				{
                    if(_cur_state != NULL) {
						_cur_state->exitState((nObj*)_owner, NULL);
                    }
				}

			public:
				/// @brief	得到当前状态
				inline StateT* getCurrentState() const
				{
					return _cur_state;
				}

				inline StateT* getNextState() const
				{
					return _next_state;
				}

				inline StateTableT* getCurrentStateTable() const
				{
					return _transit_table;
				}

				/// @brief	得到前一个状态
				inline const StateT* getPreviousState() const
				{
					return _pre_state;
				}
			protected:
				virtual void onStateChange()
				{
				}
				virtual void onStateNonChange()
				{
				}

			public:
				/// @brief	动态切换状态表
				/// @param	table 为新的状态表
				/// @return	
				void setStateTable(StateTableT *table)
				{
					assert_debug(table != NULL, "");
					if(this->_transit_table != NULL) {
						this->_transit_table->unregGlobalEvent(_owner);
						this->_transit_table->unregStateEvent(_owner, _cur_state);
						this->_cur_state->exitState((nObj*)_owner, NULL);
						_transit_table = NULL;
						this->_cur_state = NULL;
						this->_next_state = NULL;
						this->_pre_state = NULL;
					}

					if(table != NULL) {
						this->_transit_table = table;
						this->_cur_state = static_cast<StateT*>(this->_transit_table->getInitState());
						this->_next_state = this->_cur_state;
						this->_pre_state = NULL;

						_transit_table->regGlobalEvent(_owner);
						_cur_state->enterState((nObj*)_owner, NULL);
						_transit_table->regStateEvent(_owner, this, _cur_state);
					}
				}

				void changeSubStateTable(StateTableT *table)
				{
					assert_debug(table != NULL, "");
					if(this->_transit_table != NULL) {
						this->_transit_table->unregGlobalEvent(_owner);
						this->_transit_table->unregStateEvent(_owner, _cur_state);
						this->_cur_state->exitState((nObj*)_owner, NULL);
						_transit_table = NULL;
						this->_cur_state = NULL;
						this->_next_state = NULL;
						this->_pre_state = NULL;
					}

					if(table != NULL) {
						this->_transit_table = table;
						this->_cur_state = static_cast<StateT*>(this->_transit_table->getInitState());
						this->_next_state = this->_cur_state;
						this->_pre_state = NULL;

						_transit_table->regGlobalEvent(_owner);
						_cur_state->enterState((nObj*)_owner, NULL);
						_transit_table->regStateEvent(_owner, this, _cur_state);
					}
				}
				void resumeStateTable(StateTableT *table, StateT *state)
				{
					assert_debug(table != NULL, "");
					if(this->_transit_table != NULL) {
						this->_transit_table->unregGlobalEvent(_owner);
						this->_transit_table->unregStateEvent(_owner, _cur_state);
						this->_cur_state->exitState((nObj*)_owner, NULL);
						_transit_table = NULL;
						this->_cur_state = NULL;
						this->_next_state = NULL;
						this->_pre_state = NULL;
					}

					if(table != NULL) {
						this->_transit_table = table;
						this->_cur_state = state;
						this->_next_state = this->_cur_state;
						this->_pre_state = NULL;

						_transit_table->regGlobalEvent(_owner);
						_transit_table->regStateEvent(_owner, this, _cur_state);
					}
				}
				/// @brief	状态机状态更新接口
				void updateState()
				{
					if(_debug == true)
					{
						_owner->debug("UPDATE [%s]", _cur_state->getName());
					}
					assert_debug(_next_state == _cur_state, "");

					_is_in_update = true;
					_cur_state->updateState((nObj*)_owner);
					_is_in_update = false;
					if(_next_state != _cur_state)
					{
						setCurrentState(_next_state);
					}
				}

				/// @brief	事件触发状态转换
				/// @param	event 事件
				/// @return	是否成功
				bool sendEvent(const DWORD &event)
				{
					StateT *nextState = static_cast<StateT*>(_transit_table->getNextState(_next_state, event));
					if(_cur_state != nextState)
					{
						StateT* oldCurState = _next_state;

						if(_debug == true)
						{
							_owner->debug("%s --> %s --> %s", oldCurState->getName(), _transit_table->_all_event[event]->name, nextState->getName());
							char ownerLog[32];
							snprintf(ownerLog, sizeof(ownerLog), "[%s,%u]", _owner->getName(), _owner->getID());
							_owner->printAllEventCheck(ownerLog);
						}
						setCurrentState(nextState);
					}
					else 
					{
						onStateNonChange();
					}
					return true;
				}
				/// @brief  检查事件是否是当前行为关心的
				/// @param  event 事件
				/// @return 是否关心
				const bool checkCurrentStateConcernedEvent(const DWORD &event)
				{
					return _transit_table->checkStateConcernedEvent(getCurrentState(), event); 
				}
				/// @brief	设置/清除debug
				/// @param	debug 是否debug
				void setDebug(bool debug)
				{
					_debug = debug;
				}
				/// @brief	是否设置了debug
				bool isDebug()
				{
					return _debug;
				}

			private:
				/// @brief	设置当前状态
				inline void setCurrentState(StateT* nextState)
				{
					if(_is_in_set_current == true || _is_in_update == true)
					{
						_next_state = nextState;
						return;
					}

					_next_state = nextState;
#ifdef _DEBUG
					StateT* oldCurState = _cur_state;
#endif
					_is_in_set_current = true;
					StateTableT *oldTransitTable = _transit_table;
#ifdef _DEBUG
					DWORD loopTimes = 0;
#endif
					while(_cur_state != _next_state) {
						_transit_table->unregStateEvent(_owner, _cur_state);
						_cur_state->exitState((nObj*)_owner, _next_state);

						onStateChange();

						_pre_state = _cur_state;
						assert_debug(_pre_state != NULL, "");
						_cur_state = _next_state;

						_cur_state->enterState((nObj*)_owner, _pre_state);
						if(oldTransitTable == _transit_table){
							_transit_table->regStateEvent(_owner, this, _cur_state);
						}
						else{
							oldTransitTable = _transit_table;
							//break;
						}
#ifdef _DEBUG
						//死循环检测
						if(oldCurState == _next_state){
							assert_debug(loopTimes < 5, "循环两次就是死循环了");
							baseServer->debug("loopTimes == %u", loopTimes);
							//assert_debug(loopTimes == 0, "循环");
							++ loopTimes;
						}
#endif
					}
					assert_debug(_cur_state == _next_state, "");
					//assert_debug((oldTransitTable != _transit_table) || _pre_state != NULL, "");
					//assert_debug(_pre_state != NULL, "");
					_is_in_set_current = false;
				}

			protected:
				/// @brief	状态表
				StateTableT *_transit_table;
				/// @brief	当前状态
				StateT* _cur_state;
				/// @brief	前一个状态
				StateT* _pre_state;
				/// @brief	下一个状态
				StateT* _next_state;
				/// @brief	主人,_owner同时是nObj和nEventObserver的子类
				const OwnerT *_owner;
				/// @brief	是否debug状态
				bool _debug;
				bool _is_in_set_current;
				bool _is_in_update;
		};
};

#endif
