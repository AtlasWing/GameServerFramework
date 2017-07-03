#include "base/nAny.h"
#include "base/nStringTool.h"
#include "base/nAssert.h"

namespace Tidy
{
	nAny::nAny() : _data_type( eAnyDataType_None )
	{
	}

	nAny::nAny( int data ) : _data_type( eAnyDataType_Int )
	{
		_data_buf.reserve( sizeof(data) );
		*((int*)_data_buf.c_str()) = data;
	}

	nAny::nAny( DWORD data ) : _data_type( eAnyDataType_Dword )
	{
		_data_buf.reserve( sizeof(data) );
		*((DWORD*)_data_buf.c_str()) = data;
	}

	nAny::nAny( QWORD data ) : _data_type( eAnyDataType_Qword )
	{
		_data_buf.reserve( sizeof(data) );
		*((QWORD*)_data_buf.c_str()) = data;
	}

	nAny::nAny( float data ) : _data_type( eAnyDataType_Float )
	{
		_data_buf.reserve( sizeof(data) );
		*((float*)_data_buf.c_str()) = data;
	}

	nAny::nAny( double data ) : _data_type( eAnyDataType_Double )
	{
		_data_buf.reserve( sizeof(data) );
		*((double*)_data_buf.c_str()) = data;
	}

	nAny::nAny( const char* data ) : _data_type( eAnyDataType_CharP )
	{
		_data_buf = data;
	}

	nAny::nAny( const char* data, DWORD size ) : _data_type( eAnyDataType_UserData )
	{
		if( size <= 0 )
			size = 1;
		_data_buf.assign(data, size);
	}

	nAny::nAny( const std::string &str ) : _data_type( eAnyDataType_CharP )
	{
		_data_buf.assign(str);
	}

	nAny::~nAny()
	{
	}

	bool nAny::empty() const
	{
		return _data_type == eAnyDataType_None || _data_buf.empty();
	}

	BYTE nAny::getByte() const
	{
		return ASSERT_CAST(BYTE, getDword());
	}

	int nAny::setInt( int data ) const
	{
		_data_type = eAnyDataType_Int;
		_data_buf.reserve( sizeof(data) );
		*((int*)_data_buf.c_str()) = data;
		return data;
	}

	int nAny::getInt() const
	{
		if( _data_type == eAnyDataType_Int ){
			return *((int*)_data_buf.c_str());
		}
		switch( _data_type )
		{
			case eAnyDataType_CharP:
				return setInt( atoi( _data_buf.c_str() ) );
			case eAnyDataType_Float:
				return setInt( ASSERT_CAST(int, *( reinterpret_cast<const float*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Double:
				return setInt( ASSERT_CAST(int, *( reinterpret_cast<const double*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Qword:
				return setInt( ASSERT_CAST(int, *( reinterpret_cast<const QWORD*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Dword:
				return setInt( ASSERT_CAST(int, *( reinterpret_cast<const DWORD*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_None:
				return 0;
			default:
				{
					assert_fail( "cannot process data type!" );
				}
				break;
		}
		return 0;
	}


	WORD nAny::setWord( WORD data ) const
	{
		_data_type = eAnyDataType_Word;
		_data_buf.reserve( sizeof(data) );
		*((WORD*)_data_buf.c_str()) = data;
		return data;
	}

	WORD nAny::getWord() const
	{
		if( _data_type == eAnyDataType_Word ){
			return *((WORD*)_data_buf.c_str());
		}
		switch( _data_type )
		{
			case eAnyDataType_CharP:
				return setWord( atoi( _data_buf.c_str() ) );
			case eAnyDataType_Float:
				assert_debug(*( reinterpret_cast<const float*>(_data_buf.c_str())) >= 0, "不为负值");
				return setWord( ASSERT_CAST(WORD,  *( reinterpret_cast<const float*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Double:
				assert_debug(*( reinterpret_cast<const double*>(_data_buf.c_str())) >= 0, "不为负值");
				return setWord( ASSERT_CAST(WORD,  *( reinterpret_cast<const double*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Qword:
				return setWord( ASSERT_CAST(WORD,  *( reinterpret_cast<const QWORD*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Int:
				assert_debug(*( reinterpret_cast<const int*>(_data_buf.c_str())) >= 0, "不为负值");
				return setWord( ASSERT_CAST(WORD,  *( reinterpret_cast<const int*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Dword:
				return setWord( ASSERT_CAST(WORD,  getDword() ) );
			case eAnyDataType_None:
				return 0;
			default:
				{
					assert_fail( "cannot process data type!" );
				}
				break;
		}
		return 0;
	}

	DWORD nAny::setDword( DWORD data ) const
	{
		_data_type = eAnyDataType_Dword;
		_data_buf.reserve( sizeof(data) );
		*((DWORD*)_data_buf.c_str()) = data;
		return data;
	}

	DWORD nAny::getDword() const
	{
		if( _data_type == eAnyDataType_Dword ){
			return *((DWORD*)_data_buf.c_str());
		}
		switch( _data_type )
		{
			case eAnyDataType_CharP:
				return setDword( atoi( _data_buf.c_str() ) );
			case eAnyDataType_Float:
				return setDword( ASSERT_CAST(DWORD, *( reinterpret_cast<const float*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Double:
				return setDword( ASSERT_CAST(DWORD, *( reinterpret_cast<const double*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Qword:
				return setDword( ASSERT_CAST(DWORD, *( reinterpret_cast<const QWORD*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Int:
				assert_debug(*( reinterpret_cast<const int*>(_data_buf.c_str())) >= 0, "不为负值");
				return setDword( ASSERT_CAST(DWORD, *( reinterpret_cast<const int*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Word:
				return setDword( ASSERT_CAST(DWORD, getWord() ) );
			case eAnyDataType_None:
				return 0;
			default:
				{
					assert_fail( "cannot process data type!" );
				}
				break;
		}
		return 0;
	}

	QWORD nAny::setQword( QWORD data ) const
	{
		_data_type = eAnyDataType_Qword;
		_data_buf.reserve( sizeof(data) );
		*((QWORD*)_data_buf.c_str()) = data;
		return data;
	}

	QWORD nAny::getQword() const
	{
		if( _data_type == eAnyDataType_Qword ){
			return *((QWORD*)_data_buf.c_str());
		}
		switch( _data_type )
		{
			case eAnyDataType_CharP:
				return setQword( atoi( _data_buf.c_str() ) );
			case eAnyDataType_Float:
				return setQword( ASSERT_CAST(QWORD, *( reinterpret_cast<const float*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Double:
				return setQword( ASSERT_CAST(QWORD, *( reinterpret_cast<const double*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Dword:
				return setQword( ASSERT_CAST(QWORD, *( reinterpret_cast<const DWORD*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Int:
				assert_debug(*( reinterpret_cast<const int*>(_data_buf.c_str())) >= 0, "不为负值");
				return setQword( ASSERT_CAST(QWORD, *( reinterpret_cast<const int*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_None:
				return 0;
			default:
				{
					assert_fail( "cannot process data type!" );
				}
				break;
		}
		return 0;
	}

	float nAny::setFloat( float data ) const
	{
		_data_type = eAnyDataType_Float;
		_data_buf.reserve( sizeof(data) );
		*((float*)_data_buf.c_str()) = data;
		return data;
	}

	float nAny::getFloat() const
	{
		if( _data_type == eAnyDataType_Float ){
			return *((float*)_data_buf.c_str());
		}
		switch( _data_type )
		{
			case eAnyDataType_CharP:
				return setFloat( atof( _data_buf.c_str() ) );
			case eAnyDataType_Double:
				return setFloat( ASSERT_CAST(float, *( reinterpret_cast<const double*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Dword:
				return setFloat( ASSERT_CAST(float, *( reinterpret_cast<const DWORD*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Qword:
				return setFloat( ASSERT_CAST(float, *( reinterpret_cast<const QWORD*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_Int:
				return setFloat( ASSERT_CAST(float, *( reinterpret_cast<const int*>(_data_buf.c_str()) ) ) );
			case eAnyDataType_None:
				return 0.0f;
			default:
				{
					assert_fail( "cannot process data type!" );
				}
				break;
		}
		return 0.0f;
	}

	double nAny::setDouble( double data ) const
	{
		_data_type = eAnyDataType_Double;
		_data_buf.reserve( sizeof(data) );
		*((double*)_data_buf.c_str()) = data;
		return data;
	}

	double nAny::getDouble() const
	{
		if( _data_type == eAnyDataType_Double ){
			return *((double*)_data_buf.c_str());
		}
		switch( _data_type )
		{
			case eAnyDataType_CharP:
				{
					return setDouble( atof( _data_buf.c_str() ) );
				}
				break;
			case eAnyDataType_Float:
				{
					return setDouble( ASSERT_CAST(double, *( reinterpret_cast<const float*>(_data_buf.c_str()) ) ) );
				}
				break;
			case eAnyDataType_Dword:
				{
					return setDouble( ASSERT_CAST(double, *( reinterpret_cast<const DWORD*>(_data_buf.c_str()) ) ) );
				}
				break;
			case eAnyDataType_Qword:
				{
					return setDouble( ASSERT_CAST(double, *( reinterpret_cast<const QWORD*>(_data_buf.c_str()) ) ) );
				}
				break;
			case eAnyDataType_Int:
				{
					return setDouble( ASSERT_CAST(double, *( reinterpret_cast<const int*>(_data_buf.c_str()) ) ) );
				}
				break;
			case eAnyDataType_None:
				{
					return 0.0f;
				}
				break;
			default:
				{
					assert_fail( "cannot process data type!" );
				}
				break;
		}
		return 0.0f;
	}

	const char* nAny::setCString(const char* data) const
	{
        // 不传长度的原因是,避免把\0也算作length
		_data_type = eAnyDataType_CharP;
		_data_buf.assign(data);
		return _data_buf.c_str();
	}

	const char* nAny::getCString() const
	{
		if( _data_type == eAnyDataType_CharP ){
			return _data_buf.c_str();
		}

		char tempBuf[1024];
		DWORD len = 0;
		switch( _data_type )
		{
			case eAnyDataType_Int:
				{
					len = snprintf( tempBuf, sizeof(tempBuf), "%d", *((int*)_data_buf.c_str()) ) + 1;
				}
				break;
			case eAnyDataType_Dword:
				{
					len = snprintf( tempBuf, sizeof(tempBuf), "%u", *((DWORD*)_data_buf.c_str()) ) + 1;
				}
				break;
			case eAnyDataType_Qword:
				{
					len = snprintf( tempBuf, sizeof(tempBuf), "%llu", *((QWORD*)_data_buf.c_str()) ) + 1;
				}
				break;
			case eAnyDataType_Float:
				{
					len = snprintf( tempBuf, sizeof(tempBuf), "%.8f", *((float*)_data_buf.c_str()) ) + 1;
				}
				break;
			case eAnyDataType_Double:
				{
					len = snprintf( tempBuf, sizeof(tempBuf), "%.16f", *((double*)_data_buf.c_str()) ) + 1;
				}
				break;
			case eAnyDataType_None:
				{
					static const char *nullStr = "";
					return nullStr;
				}
				break;
			default:
				{
					assert_fail( "cannot process data type!" );
				}
				break;
		}
		if(len == 0){
			return NULL;
		}
		return setCString(tempBuf);
	}

	void nAny::copyCString( char* dst, DWORD dstsize ) const
	{
		if( _data_type == eAnyDataType_UserData )
		{
			assert_fail( false );
			return;
		}
		getCString();
		nStringTool::copyStr( dst, _data_buf.c_str(), dstsize );
	}

	const std::string& nAny::setString(const std::string &data) const
	{
		_data_buf = data;
		return _data_buf;
	}

	const std::string& nAny::getString() const
	{
		getCString();
		return _data_buf;
	}

	const char* nAny::setUserData( const char* data, DWORD size ) const
	{
		if( size <= 0 )
			size = 1;
		_data_type = eAnyDataType_CharP;
		_data_buf.assign(data, size);
		return data;
	}

	const char* nAny::getUserData() const
	{
		if( _data_type == eAnyDataType_UserData ){
			return (const char*)_data_buf.c_str();
		}

		return setUserData( (const char*)_data_buf.c_str(), _data_buf.size() );
	}

	void nAny::copyUserData( char* dst, DWORD dstsize ) const
	{
		if( _data_type != eAnyDataType_UserData ){
			setUserData( (const char*)_data_buf.c_str(), _data_buf.size() );
		}
		_data_buf.copy( dst, dstsize, 0 );
	}

	DWORD nAny::getSize() const
	{
		return _data_buf.size();
	}

	void nAny::clear()
	{
		_data_type = eAnyDataType_None;
		_data_buf.clear();
	}

	nAny::operator BYTE() const
	{
		return ASSERT_CAST(BYTE, getDword());
	}

	nAny::operator WORD() const
	{
		return ASSERT_CAST(WORD, getDword());
	}

	nAny::operator DWORD() const
	{
		return ASSERT_CAST(DWORD, getDword());
	}

	nAny::operator SDWORD() const
	{
		return ASSERT_CAST(SDWORD, getDword());
	}

	nAny::operator QWORD() const
	{
		return ASSERT_CAST(QWORD, getDword());
	}

	nAny::operator float() const
	{
		return getFloat();
	}

	nAny::operator double() const
	{
		return getDouble();
	}

	nAny::operator const char*() const
	{
		return getCString();
	}

	nAny::operator bool() const
	{
		return getDword() > 0;
	}
};
