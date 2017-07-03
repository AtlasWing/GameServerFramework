#include "base/nLuaState.h"
#include "base/nBaseScriptObj.h"

namespace Tidy
{
	nLuaState::nLuaState():
		_state(NULL),
		_parse_status(-1),
		_need_lock(false),
		_error_func(0),
		_is_in_stack(false)
	{
		reset();
	}

	nLuaState::~nLuaState()
	{
		final();
	}

	bool nLuaState::reset()
	{
		final();
		_parse_status = -1;
		_state = lua_open();
		openLuaLibs();
		lua_checkstack(_state, 10240);

		return true;
	}

	void nLuaState::final()
	{
		if(_state != NULL) {
			lua_close(_state);
			_state = NULL;
		}
	}

	void nLuaState::openLuaLibs()
	{
		assert(_state);
		luaL_openlibs(_state);
	}

	bool nLuaState::doFile(const char* filename)
	{
		_parse_status = luaL_dofile(_state, filename);
		if(_parse_status != 0) {
			baseServer->debug("加载脚本文件:[%s]失败", filename);
			baseServer->error("脚本错误:[%s][%s]\n", filename, lua_tostring(_state,-1));
			assert_fail("加载失败");
			return false;
		}
#ifdef _DEBUG
		baseServer->debug("加载脚本文件:[%s]成功",filename);
#endif
		return true;
	}

	bool nLuaState::doString(const char* buffer)
	{
		return doBuffer(buffer, strlen(buffer));
	}

	bool nLuaState::doBuffer(const char* buffer, size_t size)
	{
		if(_need_lock == true){
			_lock.lock();
		}
		_parse_status = luaL_dostring(_state, buffer);
		if(_need_lock == true){
			_lock.unlock();
		}
		if(_parse_status != 0) {
			baseServer->error("脚本错误:%s\n", lua_tostring(_state,-1));
			assert_fail("加载失败");
			return false;
		}
		return true;
	}

	bool nLuaState::checkExistFunction(const char* name)
	{
		pushValue(name);
		//需要绕开metatable, 相当于rawget,不使用lua_getglobal(_state, name);
		lua_rawget(_state, LUA_GLOBALSINDEX);
		bool ret = false;
		if(lua_type(_state,-1) == LUA_TFUNCTION){
			ret = true;
		}
		lua_pop(_state,1);
		return ret;
	}

	void nLuaState::callLuaFuncVoid(const char* func)
	{
		nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
		lua_getglobal(_state,func);
		if(lua_pcall(_state, 0, 0, getErrorFunc())) {
			dealError("", func);
			lua_pop(_state,1); 
		}
	}

	bool nLuaState::callLuaFuncBool(const char* func)
	{
		nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
		lua_getglobal(_state,func);
		if(lua_pcall(_state, 0, 1, getErrorFunc())) {
			dealError("", func);
			lua_pop(_state,1); /* 弹出错误信息 */
			return false;
		}
		if(lua_gettop(_state) <= getErrorFunc()){
			assert_fail("调用脚本没有返回值[%s]", func);
			return false;
		}
		bool ret = static_cast<bool>( lua_toboolean(_state,-1) );
		lua_pop(_state,1);//弹出返回值
		return ret;
	}

	void nLuaState::pushValue(int param)
	{
		lua_pushinteger(_state, (lua_Integer)param);
	}

	void nLuaState::pushValue(short param) 
	{
		lua_pushinteger(_state, (lua_Integer)param);
	}

	void nLuaState::pushValue(char param) 
	{
		lua_pushinteger(_state, (lua_Integer)param);
	}

	void nLuaState::pushValue(long param) 
	{
		lua_pushinteger(_state, (lua_Integer)param);
	}

	void nLuaState::pushValue(unsigned int param) 
	{
		lua_pushinteger(_state, (lua_Integer)param);
	}

	void nLuaState::pushValue(unsigned short param) 
	{
		lua_pushinteger(_state, (lua_Integer)param);
	}

	void nLuaState::pushValue(unsigned char param) 
	{
		lua_pushinteger(_state, (lua_Integer)param);
	}

	void nLuaState::pushValue(unsigned long param) 
	{
		lua_pushinteger(_state, (lua_Integer)param);
	}

	void nLuaState::pushValue(QWORD param)
	{
		lua_pushinteger(_state, (lua_Integer)param);
	}

	void nLuaState::pushValue(float param) 
	{
		lua_pushnumber(_state, (lua_Number)param);
	}

	void nLuaState::pushValue(double param) 
	{
		lua_pushnumber(_state, (lua_Number)param);
	}

	void nLuaState::pushValue(long double param) 
	{
		lua_pushnumber(_state, (lua_Number)param);
	}

	void nLuaState::pushValue(const char* param)
	{
		if(param == NULL)
			lua_pushnil(_state);
		else
			lua_pushstring(_state, param);
	}

	void nLuaState::pushValue(const std::string &param)
	{
		lua_pushlstring(_state, param.c_str(), param.length());
	}

	void nLuaState::pushValue(bool param)
	{
		lua_pushboolean(_state, param);
	}

	void nLuaState::setNeedLock()
	{
		_need_lock = true;
	}

	void nLuaState::clearNeedLock()
	{
		_need_lock = false;
	}

	bool nLuaState::getCallStack(lua_State *_state, std::string *stackStr)
	{
		lua_Debug ar;
		int result = -1;
		for(int i = 1; i < LUAI_MAXCSTACK; ++ i){
			result = lua_getstack(_state, i, &ar);
			if(result == 0){
				break;
			}
			if(lua_getinfo(_state, "Sln", &ar) == 0){
				break;
			}
			if(ar.name != NULL){
				stackStr->append(ar.name);
			}
			stackStr->push_back(' ');
			stackStr->append(ar.source);
			stackStr->push_back(':');
			stackStr->append(nAny(ar.currentline).getCString());
			int j = 1;
			const char *localVarName = lua_getlocal(_state, &ar, j);
			while(localVarName != NULL){
				const char *strValue = lua_tostring(_state, -1);
				if(strValue != NULL || lua_isboolean(_state, -1)){
					stackStr->push_back('(');
					stackStr->append(localVarName);
					stackStr->push_back('=');
					if(lua_isboolean(_state, -1) == true){
						stackStr->append((lua_toboolean(_state, -1) > 0) ? "true" : "false");
					}
					else{
						stackStr->append(strValue);
					}
					stackStr->push_back(')');
				}

				lua_pop(_state, 1);
				++ j;
				localVarName = lua_getlocal(_state, &ar, j);
			}
			stackStr->append(">>");
		}
		return true;
	}

	bool nLuaState::getStack(std::string *stackStr) const
	{
		if(_is_in_stack == false){
			return true;
		}
		return getCallStack(const_cast<lua_State*>(_state), stackStr) > 0;
	}

	void nLuaState::pushErrCallback()
	{
		lua_pushcfunction(_state, errCallbackFunc);
		_error_func = lua_gettop(_state);
	}

	void nLuaState::popErrCallback()
	{
		lua_pop(_state, 1);
		_error_func = 0;
	}

	int nLuaState::getErrorFunc() const
	{
		return _error_func;
	}

	bool nLuaState::isInStack() const
	{
		return _is_in_stack;
	}

	void nLuaState::setInStack(bool isInStack)
	{
		assert_debug(_is_in_stack != isInStack, "[%u]", _is_in_stack);
		_is_in_stack = isInStack;
	}

	void nLuaState::error(const char *errInfo)
	{
		lua_pushstring(_state, errInfo);
		lua_error(_state);
	}

	void nLuaState::setErrorFunc(int errorFuncIndex)
	{
		_error_func = errorFuncIndex;
	}

	int errCallbackFunc(lua_State *L)
	{
		std::string stackStr("stack:");
		nLuaState::getCallStack(L, &stackStr);
		lua_pushstring(L, stackStr.c_str());
		lua_concat(L, 2);
		return 1;
	}

	nLuaErrCallbackSetScopeObj::nLuaErrCallbackSetScopeObj(nLuaState *state):
		_state(state),
		_is_in_stack(false),
		_old_error_func(0)
	{
		_is_in_stack = state->isInStack();
		if(_is_in_stack == false){
			state->setInStack(true);
		}
		else{
			_old_error_func = state->getErrorFunc();
		}
		_state->pushErrCallback();
	}

	nLuaErrCallbackSetScopeObj::~nLuaErrCallbackSetScopeObj()
	{
		_state->popErrCallback();
		if(_is_in_stack == false){
			_state->setInStack(false);
		}
		else{
			_state->setErrorFunc(_old_error_func);
		}
	}
};
