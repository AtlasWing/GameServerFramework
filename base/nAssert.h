#ifndef _N_ASSERT_H_
#define _N_ASSERT_H_
extern void assertLog(const bool needCore, const char *funcName, const char *fileName, const int line, const char *expr, const char * desc, ...);
/// @brief	assert_debug的等级分为core down,log,none.要求必须至少带一个变长参数,比如assert_debug(v == 0, "desc v=%u", v)
#if defined _HAS_ASSERT_CORE_DOWN
#define assert_debug(expr, ...)\
	do{ \
		if((expr) == false){ \
			assertLog(true, __PRETTY_FUNCTION__, __FILE__, __LINE__, #expr, ##__VA_ARGS__); \
		} \
	}while(0)
#elif defined _HAS_ASSERT_LOG
#define assert_debug(expr, ...) \
	do{ \
		if((expr) == false){ \
			assertLog(false, __PRETTY_FUNCTION__, __FILE__, __LINE__, #expr, ##__VA_ARGS__); \
		} \
	}while(0)
#else
#define assert_debug(expr, ...) ((void)(expr))
#endif

#define assert_fail(...) assert_debug(false, __VA_ARGS__)
#define assert_crash() assert_debug(false, "")
#define assert_waigua(...) assert_debug(false, __VA_ARGS__)


/// @brief	编译期Assert, The assert_compile macro can be used to verify that a compile time expression is true.
/// @param	expr 表达式
#define JOIN( X, Y ) DO_JOIN( X, Y )
#define DO_JOIN( X, Y ) DO_JOIN2(X,Y)
#define DO_JOIN2( X, Y ) X##Y
template <bool>
struct CompileAssert {};
#undef assert_compile
#define assert_compile(expr) \
	typedef CompileAssert<(bool(expr))> JOIN(compile_assert_occur_at_line_,__LINE__)[bool(expr) ? 1 : -1];

#undef assert_todo
#ifdef _DEBUG_TEMP
#define assert_todo(expr)
#else
#define assert_todo(expr) TODO(expr)
#endif

// 整型,浮点之间转换的溢出检查
#define ASSERT_CAST(type, value) \
	({__typeof__(value) temp = (value);type resultTemp = static_cast<type>(temp);assert_debug(static_cast<__typeof__(temp)>(resultTemp) == temp || static_cast<__typeof__(temp)>(resultTemp) == trunc(temp), "溢出");resultTemp;})

// 只能用于Class转换,且obj的类型必须有虚函数才可以用
#define CLASS_CAST(type, obj) \
	({__typeof__(obj) temp = (obj); assert_debug(dynamic_cast<type*>(temp) != NULL, "类型错误"); static_cast<type*>(temp);})


#endif

