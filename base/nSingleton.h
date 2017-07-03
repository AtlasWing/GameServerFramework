#ifndef _N_SINGLETON_H_
#define _N_SINGLETON_H_

#include "base/nSys.h"
#include "base/nMisc.h"

namespace Tidy
{
	template <typename T, bool INIT = true>
		class nSingleton
		{
			DISALLOW_COPY_AND_ASSIGN(nSingleton);
		};
	/// @brief	一个单体的基类实现
	// 		Singleton设计模式
	// 		保证整个进程中有且只有一个类的实例
	template <typename T>
		class nSingleton<T,true>
		{
			protected:
				DISALLOW_COPY_AND_ASSIGN(nSingleton);
				/// @brief	构造函数
				nSingleton() 
				{
				}
				/// @brief	析构函数
				~nSingleton() {}
			public:
				/// @brief	得到单体实例
				/// @return	返回实例引用
				static T& getInstance()
				{
					assert(_instance);
					return *_instance;
				}
				/// @brief	初始化一个单体
				static void newInstance()
				{
					SAFE_DELETE(_instance);
					_instance = new T();
				}
				/// @brief	删除一个单体
				static void delInstance()
				{
					SAFE_DELETE(_instance);
				}
			private:
				/// @brief	单体实例指针
				static T* _instance;
		};
	template <typename T>
		class nSingleton<T,false>
		{
			protected:
				DISALLOW_COPY_AND_ASSIGN(nSingleton);
				/// @brief	构造函数
				nSingleton() 
				{
				}
				/// @brief	析构函数
				~nSingleton() {}
			public:
				/// @brief	得到单体实例
				/// @return	返回实例引用
				static T& getInstance()
				{
					assert(_instance);
					return *_instance;
				}
				/// @brief	得到单体实例
				/// @return	返回实例引用
				template <typename T1>
					static T1& getInstance()
					{
						assert(_instance);
						return *(T1*)_instance;
					}
				/// @brief	初始化一个单体
				template < typename T1>
					static void newInstance()
					{
						SAFE_DELETE(_instance);
						_instance = new T1();
					}
				/// @brief	删除一个单体
				static void delInstance()
				{
					SAFE_DELETE(_instance);
				}
			private:
				/// @brief	单体实例指针
				static T* _instance;
		};

	/// @brief	自动初始化实例
	// 		并且无需在运行时作判断
	//			也没有线程问题
	template <typename T > T* nSingleton<T , true>::_instance = new T();
	template <typename T> T* nSingleton<T , false>::_instance = NULL;
};
#endif

