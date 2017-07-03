#ifndef _N_LUA_STATE_H_
#define _N_LUA_STATE_H_

#include "base/Tidy.h"
#include "base/nBaseServer.h"
#include <lua.h>

namespace Tidy
{
	class nBaseScriptObj;
	struct lua_TValue;
	typedef lua_TValue TValue;
	extern nBaseScriptObj *luaNullParam;

	class nLuaState
	{
		protected:
			nLuaState();
			~nLuaState();

		public:
			inline lua_State* getHandle() {
				return _state;
			}
			bool doFile(const char* filename);
			bool doString(const char* buffer);
			bool doBuffer(const char* buffer, size_t size);

		public:
			bool callLuaFuncBool(const char *func);
			template<typename ParamT>
				bool callLuaFuncBool(const char* func, ParamT *paramList, const DWORD size);
			template<typename P1>
				bool callLuaFuncBool(const char* func, const P1 &p1);
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

			void callLuaFuncVoid(const char* func);
			template<typename P1>
				void callLuaFuncVoid(const char* func, const P1 &p1);
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
			void error(const char *errInfo);
			bool checkExistFunction( const char* name );
			void dealError(const char *typeName, const char *funcName);
			void setNeedLock();
			void clearNeedLock();
			bool isInStack() const;
			bool getStack(std::string *stackStr) const;
			int  getErrorFunc() const;

		private:
			/// @brief 重新加载脚本
			bool reset();
			void openLuaLibs();
			void final();

			void pushValue(int param);
			void pushValue(short param); 
			void pushValue(char param); 
			void pushValue(long param); 
			void pushValue(unsigned int param); 
			void pushValue(unsigned short param); 
			void pushValue(unsigned char param); 
			void pushValue(unsigned long param); 
			void pushValue(QWORD param); 
			void pushValue(float param); 
			void pushValue(double param); 
			void pushValue(long double param); 
			void pushValue(const char* param); ///< 必须以0结尾
			void pushValue(const std::string &param); ///< 必须以0结尾
			void pushValue(bool param);
			template<typename ParamT>
				void pushValue(ParamT* param);
			template<typename KeyT, typename ValueT>
				void pushValue(const std::map<KeyT, ValueT> &paramMap);
			template<typename ValueT>
				void pushValue(const std::list<ValueT> &paramList);
			template<typename ValueT>
				void pushValue(const std::vector<ValueT> &paramList);
			template<typename ValueT>
				void pushValue(const std::set<ValueT> &paramList);
			template<typename ValueT>
				void pushValue(const Array<ValueT> &paramList);

		public:
			static bool getCallStack(lua_State *_state, std::string *stackStr);

		private:
			void pushErrCallback();
			void popErrCallback();
			void setInStack(bool isInStack);
			void setErrorFunc(int errorFuncIndex);

		protected:
			lua_State *_state;
			int _parse_status;

		private:
			nMutex _lock;
			bool _need_lock;
			int _error_func;
			bool _is_in_stack;

		private:
			friend class nBaseScriptObj;
			friend class nLuaErrCallbackSetScopeObj;
	};

	class nLuaErrCallbackSetScopeObj
	{
		public:
			nLuaErrCallbackSetScopeObj(nLuaState *state);
			~nLuaErrCallbackSetScopeObj();

		private:
			nLuaState *_state;
			bool _is_in_stack;
			int _old_error_func;
	};
	extern int errCallbackFunc(lua_State *L);

	/// === 模板函数
	template< typename P1>
		inline bool nLuaState::callLuaFuncBool(const char* func, const P1 &p1)
		{
			nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
			lua_getglobal(_state,func);
			pushValue(p1);
			assert_debug(lua_checkstack(_state,10) > 0, "");
			if(lua_pcall(_state, 1, 1, getErrorFunc())) {
				dealError("", func);
				lua_pop(_state,1); /* 弹出错误信息 */
				return false;
			}
			if(lua_gettop(_state) <= getErrorFunc()) {
				assert_fail("调用脚本没有返回值[%s]", func);
				return false;
			}
			bool ret = static_cast<bool>( lua_toboolean(_state,-1) );
			lua_pop(_state,1);//弹出返回值
			return ret;
		}

	template< typename P1, typename P2>
		inline bool nLuaState::callLuaFuncBool(const char* func, const P1 &p1, const P2 &p2)
		{
			nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
			lua_getglobal(_state,func);
			pushValue(p1);
			pushValue(p2);
			assert_debug(lua_checkstack(_state,10) > 0, "");
			if(lua_pcall(_state, 2, 1, getErrorFunc())) {
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

	template< typename P1, typename P2, typename P3>
		inline bool nLuaState::callLuaFuncBool(const char* func, const P1 &p1, const P2 &p2, const P3 &p3)
		{
			nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
			lua_getglobal(_state,func);
			pushValue(p1);
			pushValue(p2);
			pushValue(p3);
			assert_debug(lua_checkstack(_state,10) > 0, "");
			if(lua_pcall(_state, 3, 1, getErrorFunc())) {
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

	template< typename P1, typename P2, typename P3, typename P4>
		inline bool nLuaState::callLuaFuncBool(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4)
		{
			nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
			lua_getglobal(_state,func);
			pushValue(p1);
			pushValue(p2);
			pushValue(p3);
			pushValue(p4);
			assert_debug(lua_checkstack(_state,10) > 0, "");
			if(lua_pcall(_state, 4, 1, getErrorFunc())) {
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

	template< typename P1, typename P2, typename P3, typename P4, typename P5>
		inline bool nLuaState::callLuaFuncBool(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5)
		{
			nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
			lua_getglobal(_state,func);
			pushValue(p1);
			pushValue(p2);
			pushValue(p3);
			pushValue(p4);
			pushValue(p5);
			assert_debug(lua_checkstack(_state,10) > 0, "");
			if(lua_pcall(_state, 5, 1, getErrorFunc())) {
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

	template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		inline bool nLuaState::callLuaFuncBool(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6)
		{
			nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
			lua_getglobal(_state,func);
			pushValue(p1);
			pushValue(p2);
			pushValue(p3);
			pushValue(p4);
			pushValue(p5);
			pushValue(p6);
			assert_debug(lua_checkstack(_state,10) > 0, "");
			if(lua_pcall(_state, 6, 1, getErrorFunc())) {
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

	template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		inline bool nLuaState::callLuaFuncBool(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6, const P7 &p7)
		{
			nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
			lua_getglobal(_state,func);
			pushValue(p1);
			pushValue(p2);
			pushValue(p3);
			pushValue(p4);
			pushValue(p5);
			pushValue(p6);
			pushValue(p7);
			assert_debug(lua_checkstack(_state,10) > 0, "");
			if(lua_pcall(_state, 7, 1, getErrorFunc())) {
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

	template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		inline bool nLuaState::callLuaFuncBool(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6, const P7 &p7, const P8 &p8)
		{
			nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
			lua_getglobal(_state,func);
			pushValue(p1);
			pushValue(p2);
			pushValue(p3);
			pushValue(p4);
			pushValue(p5);
			pushValue(p6);
			pushValue(p7);
			pushValue(p8);
			assert_debug(lua_checkstack(_state,10) > 0, "");
			if(lua_pcall(_state, 8, 1, getErrorFunc())) {
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

	template< typename P1>
		inline void nLuaState::callLuaFuncVoid(const char* func, const P1 &p1)
		{
			nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
			lua_getglobal(_state,func);
			pushValue(p1);
			assert_debug(lua_checkstack(_state,10) > 0, "");
			if(lua_pcall(_state, 1, 0, getErrorFunc())) {
				dealError("", func);
				lua_pop(_state,1); /* 弹出错误信息 */
				return;
			}
		}

	template< typename P1, typename P2>
		inline void nLuaState::callLuaFuncVoid(const char* func, const P1 &p1, const P2 &p2)
		{
			nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
			lua_getglobal(_state,func);
			pushValue(p1);
			pushValue(p2);
			assert_debug(lua_checkstack(_state,10) > 0, "");
			if(lua_pcall(_state, 2, 0, getErrorFunc())) {
				dealError("", func);
				lua_pop(_state,1); /* 弹出错误信息 */
				return;
			}
		}

	template< typename P1, typename P2, typename P3>
		inline void nLuaState::callLuaFuncVoid(const char* func, const P1 &p1, const P2 &p2, const P3 &p3)
		{
			nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
			lua_getglobal(_state,func);
			pushValue(p1);
			pushValue(p2);
			pushValue(p3);
			assert_debug(lua_checkstack(_state,10) > 0, "");
			if(lua_pcall(_state, 3, 0, getErrorFunc())) {
				dealError("", func);
				lua_pop(_state,1); /* 弹出错误信息 */
				return;
			}
		}

	template< typename P1, typename P2, typename P3, typename P4>
		inline void nLuaState::callLuaFuncVoid(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4)
		{
			nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
			lua_getglobal(_state,func);
			pushValue(p1);
			pushValue(p2);
			pushValue(p3);
			pushValue(p4);
			assert_debug(lua_checkstack(_state,10) > 0, "");
			if(lua_pcall(_state, 4, 0, getErrorFunc())) {
				dealError("", func);
				lua_pop(_state,1); /* 弹出错误信息 */
				return;
			}
		}

	template< typename P1, typename P2, typename P3, typename P4, typename P5>
		inline void nLuaState::callLuaFuncVoid(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5)
		{
			nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
			lua_getglobal(_state,func);
			pushValue(p1);
			pushValue(p2);
			pushValue(p3);
			pushValue(p4);
			pushValue(p5);
			assert_debug(lua_checkstack(_state,10) > 0, "");
			if(lua_pcall(_state, 5, 0, getErrorFunc())) {
				dealError("", func);
				lua_pop(_state,1); /* 弹出错误信息 */
				return;
			}
		}

	template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		inline void nLuaState::callLuaFuncVoid(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6)
		{
			nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
			lua_getglobal(_state,func);
			pushValue(p1);
			pushValue(p2);
			pushValue(p3);
			pushValue(p4);
			pushValue(p5);
			pushValue(p6);
			assert_debug(lua_checkstack(_state,10) > 0, "");
			if(lua_pcall(_state, 6, 0, getErrorFunc())) {
				dealError("", func);
				lua_pop(_state,1); /* 弹出错误信息 */
				return;
			}
			assert_debug(lua_gettop(_state) == 0, "%d", lua_gettop(_state));
		}

	template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		inline void nLuaState::callLuaFuncVoid(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6, const P7 &p7)
		{
			nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
			lua_getglobal(_state,func);
			pushValue(p1);
			pushValue(p2);
			pushValue(p3);
			pushValue(p4);
			pushValue(p5);
			pushValue(p6);
			pushValue(p7);
			assert_debug(lua_checkstack(_state,10) > 0, "");
			if(lua_pcall(_state, 7, 0, getErrorFunc())) {
				dealError("", func);
				lua_pop(_state,1); /* 弹出错误信息 */
				return;
			}
			assert_debug(lua_gettop(_state) == 0, "%d", lua_gettop(_state));
		}

	template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		inline void nLuaState::callLuaFuncVoid(const char* func, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6, const P7 &p7, const P8 &p8)
		{
			nLuaErrCallbackSetScopeObj autoSetErrCallback(this);  
			lua_getglobal(_state,func);
			pushValue(p1);
			pushValue(p2);
			pushValue(p3);
			pushValue(p4);
			pushValue(p5);
			pushValue(p6);
			pushValue(p7);
			pushValue(p8);
			assert_debug(lua_checkstack(_state,10) > 0, "");
			if(lua_pcall(_state, 8, 0, getErrorFunc())) {
				dealError("", func);
				lua_pop(_state,1); /* 弹出错误信息 */
				return;
			}
			assert_debug(lua_gettop(_state) == 0, "%d", lua_gettop(_state));
		}

	template<typename ParamT>
		void nLuaState::pushValue(ParamT* param)
		{
			if(param == NULL)
				lua_pushnil(_state);
			else
				lua_getref(_state, param->getRef());
		}

	template<typename KeyT, typename ValueT>
		void nLuaState::pushValue(const std::map<KeyT, ValueT> &paramMap)
		{
			lua_createtable(_state, 0, paramMap.size());
			for(typename std::map<KeyT, ValueT>::const_iterator iter = paramMap.begin();  iter != paramMap.end(); ++ iter){
				pushValue(iter->first);
				pushValue(iter->second);
				lua_settable(_state, -3);
			}
			assert_debug(lua_checkstack(_state,10) > 0, "");
		}

	template<typename ValueT>
		void nLuaState::pushValue(const std::list<ValueT> &paramList)
		{
			lua_createtable(_state, 0, 0);
			DWORD index = 1;
			for(typename std::list<ValueT>::const_iterator iter = paramList.begin();  iter != paramList.end(); ++ iter){
				pushValue(index);
				pushValue(*iter);
				lua_settable(_state, -3);
				++ index;
			}
			assert_debug(lua_checkstack(_state,10) > 0, "");
		}

	template<typename ValueT>
		void nLuaState::pushValue(const std::vector<ValueT> &paramList)
		{
			lua_createtable(_state, paramList.size(), 0);
			DWORD index = 1;
			for(typename std::vector<ValueT>::const_iterator iter = paramList.begin();  iter != paramList.end(); ++ iter){
				pushValue(index);
				pushValue(*iter);
				lua_settable(_state, -3);
				++ index;
			}
			assert_debug(lua_checkstack(_state,10) > 0, "");
		}

	template<typename ValueT>
		void nLuaState::pushValue(const Array<ValueT> &paramList)
		{
			lua_createtable(_state, paramList.size, 0);
			for(DWORD i = 0; i < paramList.size; ++ i){
				pushValue(i + 1);
				pushValue(paramList.data[i]);
				lua_settable(_state, -3);
			}
		}

	template<typename ValueT>
		void nLuaState::pushValue(const std::set<ValueT> &paramList)
		{
			lua_createtable(_state, paramList.size(), 0);
			DWORD index = 1;
			for(typename std::vector<ValueT>::const_iterator iter = paramList.begin();  iter != paramList.end(); ++ iter){
				pushValue(index);
				pushValue(*iter);
				lua_settable(_state, -3);
				++ index;
			}
			assert_debug(lua_checkstack(_state,10) > 0, "");
		}

	inline void nLuaState::dealError(const char *typeName, const char *funcName)
	{
		const char *errLuaMsg =  lua_tostring(_state, -1);
		(void)errLuaMsg;
		assert_fail("脚本错误：[%s,%s]%s\n", typeName, funcName, errLuaMsg);
	}
};

#endif
