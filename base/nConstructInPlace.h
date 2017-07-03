#ifndef _N_CONSTRUCTINPLACE_H_
#define _N_CONSTRUCTINPLACE_H_

#include "base/nSys.h"

namespace Tidy
{
	template<class T> 
		inline	void constructInPlace(T  *_Ptr)
		{	// construct object at _Ptr with value _Val
			new ((void  *)_Ptr) T( );
		}

	template<class _Ty,class _TParam> 
		inline	void constructInPlace(_Ty  *_Ptr,_TParam param)
		{	// construct object at _Ptr with value _Val
			new ((void  *)_Ptr) _Ty( param );
		}

	template<class _Ty,class _TParam1,class _TParam2> 
		inline	void constructInPlace(_Ty  *_Ptr,_TParam1 param1,_TParam2 param2)
		{	// construct object at _Ptr with value _Val
			new ((void  *)_Ptr) _Ty( param1,param2 );
		}

	template<class _Ty,class _TParam1,class _TParam2,class _TParam3> 
		inline	void constructInPlace(_Ty  *_Ptr,_TParam1 param1,_TParam2 param2,_TParam3 param3)
		{	// construct object at _Ptr with value _Val
			new ((void  *)_Ptr) _Ty( param1,param2 ,param3);
		}

	template<class _Ty,class _TParam1,class _TParam2,class _TParam3,class _TParam4> 
		inline	void constructInPlace(_Ty  *_Ptr,_TParam1 param1,_TParam2 param2,_TParam3 param3,_TParam4 param4)
		{	// construct object at _Ptr with value _Val
			new ((void  *)_Ptr) _Ty( param1,param2 ,param3,param4);
		}

	template<class _Ty,class _TParam1,class _TParam2,class _TParam3,class _TParam4,class _TParam5> 
		inline	void constructInPlace(_Ty  *_Ptr,_TParam1 param1,_TParam2 param2,_TParam3 param3,_TParam4 param4,_TParam5 param5)
		{	// construct object at _Ptr with value _Val
			new ((void  *)_Ptr) _Ty( param1,param2 ,param3,param4,param5);
		}

	template<class _Ty,class _TParam1,class _TParam2,class _TParam3,class _TParam4,class _TParam5,class _TParam6> 
		inline	void constructInPlace(_Ty  *_Ptr,_TParam1 param1,_TParam2 param2,_TParam3 param3,_TParam4 param4,_TParam5 param5,_TParam6 param6)
		{	// construct object at _Ptr with value _Val
			new ((void  *)_Ptr) _Ty( param1,param2 ,param3,param4,param5,param6);
		}

	template<class _Ty,class _TParam1,class _TParam2,class _TParam3,class _TParam4,class _TParam5,class _TParam6,class _TParam7 > 
		inline	void constructInPlace(_Ty  *_Ptr,_TParam1 param1,_TParam2 param2,_TParam3 param3,_TParam4 param4,_TParam5 param5,_TParam6 param6,_TParam7 param7)
		{	// construct object at _Ptr with value _Val
			new ((void  *)_Ptr) _Ty( param1,param2 ,param3,param4,param5,param6,param7);
		}

	template <class T>
		inline void destructInPlace(T* p)
		{
			p->~T();
		}

	template<class _T> 
		inline	void constructInPlaceCount(_T  *_Ptr,size_t count)
		{	// construct object at _Ptr with value _Val
			for(size_t i=0;i<count;++i)
			{
				constructInPlace(_Ptr++);
			}
		}

	template <class T>
		inline void destructInPlaceCount(T* p,size_t count)
		{
			for(size_t i=0;i<count;++i)
			{
				destructInPlace(p++);
			}
		}
};

#endif

