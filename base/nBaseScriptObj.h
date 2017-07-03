#ifndef BASE_NBASESCRIPTOBJ_H_
#define BASE_NBASESCRIPTOBJ_H_

#include "base/nLuaState.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	class nBaseScriptObj
	{
		protected:
			DISALLOW_COPY_AND_ASSIGN(nBaseScriptObj);
			nBaseScriptObj( void *pThis, nLuaState *luaState, const char* typeName ) :
				_lua_state(luaState),
				_ref(LUA_NOREF),
				_type_name(typeName)
		{
			assert_debug(_lua_state->getHandle() != NULL, "");
			tolua_pushusertype_and_takeownership(_lua_state->getHandle(), pThis, typeName );
			_ref = lua_ref( _lua_state->getHandle(), true );
			assert( (_ref != LUA_REFNIL) && (_ref != LUA_NOREF) );
		}

			void dispose()
			{
				assert( (_ref != LUA_NOREF) && (_ref != LUA_REFNIL) );
				lua_unref( _lua_state->getHandle(), _ref );
			}

			inline int getRef() const
			{
				return _ref;
			}

		public:
			bool callLuaFuncBool( const char* func );
			template < typename P1 >
				bool callLuaFuncBool( const char* func, const P1 &p1 );
			template<typename P1, typename P2>
				bool callLuaFuncBool(const char* func, const P1 &p1, const P2 &p2);
			template<typename P1, typename P2, typename P3>
				bool callLuaFuncBool(const char* func, const P1 &p1, const P2 &p2, const P3 &p3);
			template<typename P1, typename P2, typename P3, typename P4>
				bool callLuaFuncBool(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4);
			template< typename P1, typename P2, typename P3, typename P4, typename P5>
				bool callLuaFuncBool(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5);
			template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
				bool callLuaFuncBool(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6);
			template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
				bool callLuaFuncBool(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6, const P7 &p7);
			template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
				bool callLuaFuncBool(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6, const P7 &p7, const P8 &p8);

			void callLuaFuncVoid( const char* func );
			template < typename P1 >
				void callLuaFuncVoid( const char* func, const P1 &p1 );
			template<typename P1, typename P2>
				void callLuaFuncVoid(const char* func, const P1 &p1, const P2 &p2);
			template<typename P1, typename P2, typename P3>
				void callLuaFuncVoid(const char* func, const P1 &p1, const P2 &p2, const P3 &p3);
			template<typename P1, typename P2, typename P3, typename P4>
				void callLuaFuncVoid(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4);
			template< typename P1, typename P2, typename P3, typename P4, typename P5>
				void callLuaFuncVoid(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5);
			template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
				void callLuaFuncVoid(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6);
			template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
				void callLuaFuncVoid(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6, const P7 &p7);
			template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
				void callLuaFuncVoid(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6, const P7 &p7, const P8 &p8);
		public:
			template <typename Ret0, typename Ret1>
				bool callLuaFuncMultiRet_2(Ret0 *ret_0, Ret1 *ret_1, const char* func);
			template <typename Ret0, typename Ret1, typename P0>
				bool callLuaFuncMultiRet_2(Ret0 *ret_0, Ret1 *ret_1, const char* func, const P0 &p_0);
			template <typename P0, typename Ret0>
				bool callLuaFuncMultiRet_1(Ret0 *ret_0, const char* func, const P0 &p_0);

		protected:
			nLuaState* _lua_state;
			int _ref;
			std::string _type_name;

		private:
			friend class nLuaState;
	};

	//==================
	inline bool nBaseScriptObj::callLuaFuncBool( const char* func)
	{
		lua_State *L = this->_lua_state->getHandle();
		nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

		assert_debug(lua_checkstack(L, 10) == 1, "");

		lua_getref( L, this->_ref );
		lua_getfield( L, -1, func );
		lua_insert( L, -2 );

		if(lua_pcall( L, 1, LUA_MULTRET, this->_lua_state->getErrorFunc() )) {
			_lua_state->dealError(_type_name.c_str(), func);
			lua_pop(L,1);//弹出错误消息
			return false;
		}
		if(lua_gettop(_lua_state->getHandle()) <= this->_lua_state->getErrorFunc()){
			assert_fail("调用脚本没有返回值[%s]", func);
			return false;
		}
		bool ret = static_cast<bool>( lua_toboolean(L,-1) );
		lua_pop(L,1);//弹出返回值
		return ret;
	}

	template < typename P1 >
		inline bool nBaseScriptObj::callLuaFuncBool( const char* func, const P1 &p1 )
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p1 );

			if(lua_pcall( L, 2, LUA_MULTRET, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
				return false;
			}
			if(lua_gettop(_lua_state->getHandle()) <= this->_lua_state->getErrorFunc()){
				assert_fail("调用脚本没有返回值[%s]", func);
				return false;
			}
			bool ret = static_cast<bool>( lua_toboolean(L,-1) );
			lua_pop(L,1);//弹出返回值
			return ret;
		}

	template < typename P1, typename P2 >
		inline bool nBaseScriptObj::callLuaFuncBool( const char* func, const P1 &p1, const P2 &p2 )
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p1 );
			this->_lua_state->pushValue( p2 );

			if(lua_pcall( L, 3, LUA_MULTRET, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
				return false;
			}
			if(lua_gettop(_lua_state->getHandle()) <= this->_lua_state->getErrorFunc()){
				assert_fail("调用脚本没有返回值[%s]", func);
				return false;
			}
			bool ret = static_cast<bool>( lua_toboolean(L,-1) );
			lua_pop(L,1);
			return ret;
		}

	template < typename P1, typename P2, typename P3 >
		inline bool nBaseScriptObj::callLuaFuncBool( const char* func, const P1 &p1, const P2 &p2, const P3 &p3 )
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");
			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p1 );
			this->_lua_state->pushValue( p2 );
			this->_lua_state->pushValue( p3 );

			if(lua_pcall( L, 4, LUA_MULTRET, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
				return false;
			}
			if(lua_gettop(_lua_state->getHandle()) <= this->_lua_state->getErrorFunc()){
				assert_fail("调用脚本没有返回值[%s]", func);
				return false;
			}
			bool ret = static_cast<bool>( lua_toboolean(L,-1) );
			lua_pop(L,1);//弹出返回值
			return ret;
		}

	template < typename P1, typename P2, typename P3, typename P4 >
		inline bool nBaseScriptObj::callLuaFuncBool( const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4 )
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p1 );
			this->_lua_state->pushValue( p2 );
			this->_lua_state->pushValue( p3 );
			this->_lua_state->pushValue( p4 );

			if(lua_pcall( L, 5, LUA_MULTRET, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
				return false;
			}
			if(lua_gettop(_lua_state->getHandle()) < this->_lua_state->getErrorFunc()){
				assert_fail("调用脚本没有返回值[%s]", func);
				return false;
			}
			bool ret = static_cast<bool>( lua_toboolean(L,-1) );
			lua_pop(L,1);//弹出返回值
			return ret;
		}

	template < typename P1, typename P2, typename P3, typename P4, typename P5 >
		inline bool nBaseScriptObj::callLuaFuncBool( const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5 )
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p1 );
			this->_lua_state->pushValue( p2 );
			this->_lua_state->pushValue( p3 );
			this->_lua_state->pushValue( p4 );
			this->_lua_state->pushValue( p5 );

			if(lua_pcall( L, 6, LUA_MULTRET, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
				return false;
			}
			if(lua_gettop(_lua_state->getHandle()) <= this->_lua_state->getErrorFunc()){
				assert_fail("调用脚本没有返回值[%s]", func);
				return false;
			}
			bool ret = static_cast<bool>( lua_toboolean(L,-1) );
			lua_pop(L,1);//弹出返回值
			return ret;
		}

	template < typename P1, typename P2, typename P3, typename P4, typename P5 , typename P6 >
		inline bool nBaseScriptObj::callLuaFuncBool( const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6 )
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p1 );
			this->_lua_state->pushValue( p2 );
			this->_lua_state->pushValue( p3 );
			this->_lua_state->pushValue( p4 );
			this->_lua_state->pushValue( p5 );
			this->_lua_state->pushValue( p6 );

			if(lua_pcall( L, 7, LUA_MULTRET, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
				return false;
			}
			if(lua_gettop(_lua_state->getHandle()) <= this->_lua_state->getErrorFunc()){
				assert_fail("调用脚本没有返回值[%s]", func);
				return false;
			}
			bool ret = static_cast<bool>( lua_toboolean(L,-1) );
			lua_pop(L,1);//弹出返回值
			return ret;
		}

	template < typename P1, typename P2, typename P3, typename P4, typename P5 , typename P6 , typename P7>
		inline bool nBaseScriptObj::callLuaFuncBool( const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6, const P7 &p7 )
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p1 );
			this->_lua_state->pushValue( p2 );
			this->_lua_state->pushValue( p3 );
			this->_lua_state->pushValue( p4 );
			this->_lua_state->pushValue( p5 );
			this->_lua_state->pushValue( p6 );
			this->_lua_state->pushValue( p7 );

			if(lua_pcall( L, 8, LUA_MULTRET, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
				return false;
			}
			if(lua_gettop(_lua_state->getHandle()) <= this->_lua_state->getErrorFunc()){
				assert_fail("调用脚本没有返回值[%s]", func);
				return false;
			}
			bool ret = static_cast<bool>( lua_toboolean(L,-1) );
			lua_pop(L,1);//弹出返回值
			return ret;
		}

	template < typename P1, typename P2, typename P3, typename P4, typename P5 , typename P6 , typename P7, typename P8>
		inline bool nBaseScriptObj::callLuaFuncBool( const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6, const P7 &p7 ,const P8 &p8)
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p1 );
			this->_lua_state->pushValue( p2 );
			this->_lua_state->pushValue( p3 );
			this->_lua_state->pushValue( p4 );
			this->_lua_state->pushValue( p5 );
			this->_lua_state->pushValue( p6 );
			this->_lua_state->pushValue( p7 );
			this->_lua_state->pushValue( p8 );

			if(lua_pcall( L, 9, LUA_MULTRET, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
				return false;
			}
			if(lua_gettop(_lua_state->getHandle()) <= this->_lua_state->getErrorFunc()){
				assert_fail("调用脚本没有返回值[%s]", func);
				return false;
			}
			bool ret = static_cast<bool>( lua_toboolean(L,-1) );
			lua_pop(L,1);//弹出返回值
			return ret;
		}

	inline void nBaseScriptObj::callLuaFuncVoid( const char* func)
	{
		lua_State *L = this->_lua_state->getHandle();
		nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

		assert_debug(lua_checkstack(L, 10) == 1, "");

		lua_getref( L, this->_ref );
		lua_getfield( L, -1, func );
		lua_insert( L, -2 );

		if(lua_pcall( L, 1, 0, this->_lua_state->getErrorFunc() )) {
			_lua_state->dealError(_type_name.c_str(), func);
			lua_pop(L,1);//弹出错误消息
		}
	}

	template < typename P1 >
		inline void nBaseScriptObj::callLuaFuncVoid( const char* func, const P1 &p1 )
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p1 );

			if(lua_pcall( L, 2, 0, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
			}

		}

	template < typename P1, typename P2 >
		inline void nBaseScriptObj::callLuaFuncVoid( const char* func, const P1 &p1, const P2 &p2 )
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p1 );
			this->_lua_state->pushValue( p2 );

			if(lua_pcall( L, 3, 0, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
			}

		}

	template < typename P1, typename P2, typename P3 >
		inline void nBaseScriptObj::callLuaFuncVoid( const char* func, const P1 &p1, const P2 &p2, const P3 &p3 )
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p1 );
			this->_lua_state->pushValue( p2 );
			this->_lua_state->pushValue( p3 );

			if(lua_pcall( L, 4, 0, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
			}
		}

	template < typename P1, typename P2, typename P3, typename P4 >
		inline void nBaseScriptObj::callLuaFuncVoid( const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4 )
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p1 );
			this->_lua_state->pushValue( p2 );
			this->_lua_state->pushValue( p3 );
			this->_lua_state->pushValue( p4 );

			if(lua_pcall( L, 5, 0, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
			}
		}

	template < typename P1, typename P2, typename P3, typename P4, typename P5 >
		inline void nBaseScriptObj::callLuaFuncVoid( const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5 )
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p1 );
			this->_lua_state->pushValue( p2 );
			this->_lua_state->pushValue( p3 );
			this->_lua_state->pushValue( p4 );
			this->_lua_state->pushValue( p5 );

			if(lua_pcall( L, 6, 0, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
			}
		}

	template < typename P1, typename P2, typename P3, typename P4, typename P5 , typename P6 >
		inline void nBaseScriptObj::callLuaFuncVoid( const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6 )
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p1 );
			this->_lua_state->pushValue( p2 );
			this->_lua_state->pushValue( p3 );
			this->_lua_state->pushValue( p4 );
			this->_lua_state->pushValue( p5 );
			this->_lua_state->pushValue( p6 );

			if(lua_pcall( L, 7, 0, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
			}
		}

	template < typename P1, typename P2, typename P3, typename P4, typename P5 , typename P6 , typename P7>
		inline void nBaseScriptObj::callLuaFuncVoid( const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6, const P7 &p7 )
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p1 );
			this->_lua_state->pushValue( p2 );
			this->_lua_state->pushValue( p3 );
			this->_lua_state->pushValue( p4 );
			this->_lua_state->pushValue( p5 );
			this->_lua_state->pushValue( p6 );
			this->_lua_state->pushValue( p7 );

			if(lua_pcall( L, 8, 0, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
			}
		}

	template < typename P1, typename P2, typename P3, typename P4, typename P5 , typename P6 , typename P7, typename P8>
		inline void nBaseScriptObj::callLuaFuncVoid( const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6, const P7 &p7 ,const P8 &p8)
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p1 );
			this->_lua_state->pushValue( p2 );
			this->_lua_state->pushValue( p3 );
			this->_lua_state->pushValue( p4 );
			this->_lua_state->pushValue( p5 );
			this->_lua_state->pushValue( p6 );
			this->_lua_state->pushValue( p7 );
			this->_lua_state->pushValue( p8 );

			if(lua_pcall( L, 9, 0, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
			}
		}

	template <typename Ret0, typename Ret1>
		inline bool nBaseScriptObj::callLuaFuncMultiRet_2(Ret0 *ret_0, Ret1 *ret_1, const char* func)
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			if(lua_pcall( L, 1, LUA_MULTRET, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
				return false;
			}
			if(lua_gettop(_lua_state->getHandle()) <= (this->_lua_state->getErrorFunc() + 1)){
				assert_fail("调用脚本没有返回值[%s]", func);
				return false;
			}
			*ret_0 = static_cast<Ret0>( lua_tonumber(L,-2) );
			*ret_1 = static_cast<Ret1>( lua_tonumber(L,-1) );
			lua_pop(L,2);//弹出返回值
			return true;
		}

	template <typename Ret0, typename Ret1, typename P0>
		inline bool nBaseScriptObj::callLuaFuncMultiRet_2(Ret0 *ret_0, Ret1 *ret_1, const char* func, const P0 &p_0)
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  

			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p_0 );
			if(lua_pcall( L, 2, 2, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
				return false;
			}
			if(lua_gettop(_lua_state->getHandle()) <= (this->_lua_state->getErrorFunc() + 1)){
				assert_fail("调用脚本没有返回值[%s]", func);
				return false;
			}
			*ret_0 = static_cast<Ret0>( lua_tonumber(L,-2) );
			*ret_1 = static_cast<Ret1>( lua_tonumber(L,-1) );
			lua_pop(L,2);//弹出返回值
			return true;
		}

	template <typename P0, typename Ret0>
		inline bool nBaseScriptObj::callLuaFuncMultiRet_1(Ret0 *ret_0, const char* func, const P0 &p_0)
		{
			lua_State *L = this->_lua_state->getHandle();
			nLuaErrCallbackSetScopeObj autoSetErrCallback(_lua_state);  
			assert_debug(lua_checkstack(L, 10) == 1, "");

			lua_getref( L, this->_ref );
			lua_getfield( L, -1, func );
			lua_insert( L, -2 );

			this->_lua_state->pushValue( p_0 );

			if(lua_pcall( L, 2, 1, this->_lua_state->getErrorFunc() )) {
				_lua_state->dealError(_type_name.c_str(), func);
				lua_pop(L,1);//弹出错误消息
				return false;
			}
			if(lua_gettop(_lua_state->getHandle()) <= this->_lua_state->getErrorFunc()){
				assert_fail("调用脚本没有返回值[%s]", func);
				return false;
			}
			assert_debug(lua_isnumber(L,-1), "");
			*ret_0 = static_cast<Ret0>( lua_tonumber(L,-1) );
			lua_pop(L,1);//弹出返回值
			return true;
		}
};

#endif
