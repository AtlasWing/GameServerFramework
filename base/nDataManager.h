#ifndef _NDATAMANAGER_H
#define _NDATAMANAGER_H

#include "base/nSys.h"
#include "base/nAny.h"

namespace Tidy
{
	class nDataManager
	{
		public:
			nDataManager();
			~nDataManager();

		public:
			void                                        setInt( DWORD key, int data );
			void                                        setWord( DWORD key, WORD data );
			void                                        setDword( DWORD key, DWORD data );
			void                                        setQword( DWORD key, QWORD data );
			void                                        setCString( DWORD key, const char* data );
			void                                        setFloat( DWORD key, float data );
			void                                        setDouble( DWORD key, double data );
			void                                        setUserData( DWORD key, const char* data, DWORD size );

			bool                                        getInt( DWORD key, int* dst ) const;
			bool                                        getWord( DWORD key, WORD* dst ) const;
			bool                                        getDword( DWORD key, DWORD* dst ) const;
			bool                                        getQword( DWORD key, QWORD* dst ) const;
			bool                                        getFloat( DWORD key, float* dst ) const;
			bool                                        getDouble( DWORD key, double* dst ) const;

			const char*                                 getCString( DWORD key ) const;
			const char*                                 getUserData( DWORD key ) const;
			bool                                        copyCString( DWORD key, char* dst, DWORD dstsize ) const;
			bool                                        copyUserData( DWORD key, char* dst, DWORD dstsize ) const;

			void                                        removeData( DWORD key );

		private:
			const nAny*                                 getData( DWORD key ) const; 

		private:
			//__gnu_cxx::hash_map<DWORD, nAny*>           _data_map;
			typedef std::map<DWORD, nAny*> DataMap;
			DataMap           _data_map;
	};
};
#endif
