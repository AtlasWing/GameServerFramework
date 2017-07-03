#ifndef BASE_NANY_H
#define BASE_NANY_H

#include "base/nSys.h"

namespace Tidy
{
	enum eAnyDataType {
		eAnyDataType_None = 0,

		eAnyDataType_Int = 1,
		eAnyDataType_INT = 1,
		eAnyDataType_Word = 2,
		eAnyDataType_WORD = 2,
		eAnyDataType_Dword = 3,
		eAnyDataType_DWORD = 3,
		eAnyDataType_Qword = 4,
		eAnyDataType_QWORD = 4,
		eAnyDataType_Float = 5,
		eAnyDataType_FLOAT = 5,
		eAnyDataType_Double = 6,
		eAnyDataType_DOUBLE = 6,
		eAnyDataType_CharP = 7,
		eAnyDataType_UserData = 8,
		eAnyDataType_bool = 9,
		eAnyDataType_BYTE = 10,

		eAnyDataType_Max
	};

	class nAny
	{
		public:
			nAny();
			nAny( int data );
			nAny( DWORD data );
			nAny( QWORD data );
			nAny( float data );
			nAny( double data );
			nAny( const char* data );
			nAny( const char* data, DWORD size );
			nAny( const std::string &str );
			~nAny();

		public:
			bool empty() const;
			BYTE                        getByte() const;
			int                         setInt( int data ) const;
			int                         getInt() const;

			WORD                        setWord( WORD data ) const;
			WORD                        getWord() const;

			DWORD                       setDword( DWORD data ) const;
			DWORD                       getDword() const;

			QWORD                       setQword( QWORD data ) const;
			QWORD                       getQword() const;

			float                       setFloat( float data ) const;
			float                       getFloat() const;

			double                      setDouble( double data ) const;
			double                      getDouble() const;

			const char*                 setCString( const char* data ) const;
			const char*                 getCString() const;
			void                        copyCString( char* dst, DWORD dstsize ) const;

			const char*                 setUserData( const char* data, DWORD size ) const;
			const char*                 getUserData() const;
			void                        copyUserData( char* dst, DWORD dstsize ) const;

			const std::string&          setString(const std::string &data) const;
			const std::string&          getString() const;

			DWORD                       getSize() const;
			void                        clear();

		public:
			operator BYTE() const;
			operator WORD() const;
			operator DWORD() const;
			operator SDWORD() const;
			operator QWORD() const;
			operator float() const;
			operator double() const;
			operator const char*() const;
			operator bool() const;

		private:
			mutable BYTE               _data_type;
			mutable std::string         _data_buf;
	};
};

#endif
