#include "base/nDataManager.h"
#include "base/nStringTool.h"
#include "base/nAssert.h"

namespace Tidy
{
	nDataManager::nDataManager()
	{

	}

	nDataManager::~nDataManager()
	{
		for( DataMap::iterator iter = _data_map.begin(); iter != _data_map.end(); ++iter ){
			delete iter->second;
		}
		_data_map.clear();
	}

	void nDataManager::setInt( DWORD key, int data )
	{
		if( const nAny* anydata = getData( key ) ){
			anydata->setInt( data );
		}
		else{
			_data_map[key] = new nAny( data );
		}
	}

	void nDataManager::setWord( DWORD key, WORD data )
	{
		if( const nAny* anydata = getData( key ) ){
			anydata->setWord( data );
		}
		else{
			_data_map[key] = new nAny( data );
		}
	}

	void nDataManager::setDword( DWORD key, DWORD data )
	{
		if( const nAny* anydata = getData( key ) ){
			anydata->setDword( data );
		}
		else{
			_data_map[key] = new nAny( data );
		}
	}
	void nDataManager::setQword( DWORD key, QWORD data )
	{
		if( const nAny* anydata = getData( key ) ){
			anydata->setQword( data );
		}
		else{
			_data_map[key] = new nAny( data );
		}
	}
	void nDataManager::setCString( DWORD key, const char* data )
	{
		if( const nAny* anydata = getData( key ) ){
			anydata->setCString( data );
		}
		else{
			_data_map[key] = new nAny( data );
		}
	}
	void nDataManager::setFloat( DWORD key, float data )
	{
		if( const nAny* anydata = getData( key ) ){
			anydata->setFloat( data );
		}
		else{
			_data_map[key] = new nAny( data );
		}
	}
	void nDataManager::setDouble( DWORD key, double data )
	{
		if( const nAny* anydata = getData( key ) ){
			anydata->setDouble( data );
		}
		else{
			_data_map[key] = new nAny( data );
		}
	}
	void nDataManager::setUserData( DWORD key, const char* data, DWORD size )
	{
		if( const nAny* anydata = getData( key ) ){
			anydata->setUserData( data, size );
		}
		else{
			_data_map[key] = new nAny( data, size );
		}
	}

	const nAny* nDataManager::getData( DWORD key ) const
	{
		using namespace __gnu_cxx;
		DataMap::const_iterator itr = _data_map.find( key );
		if( itr == _data_map.end() )
			return NULL;

		return itr->second;
	}

	bool nDataManager::getInt( DWORD key, int *dst ) const
	{
		const nAny* data = getData( key );
		if( !data ){
			return false;
		}
		else{
			*dst = data->getInt();
			return true;
		}
	}

	bool nDataManager::getWord( DWORD key, WORD* dst ) const
	{
		const nAny* data = getData( key );
		if( !data ){
			return false;
		}
		else{
			*dst = data->getWord();
			return true;
		}
	}

	bool nDataManager::getDword( DWORD key, DWORD *dst ) const
	{
		const nAny* data = getData( key );
		if( !data ){
			return false;
		}
		else{
			*dst = data->getDword();
			return true;
		}
	}

	bool nDataManager::getQword( DWORD key, QWORD *dst ) const
	{
		const nAny* data = getData( key );
		if( !data ){
			return false;
		}
		else{
			*dst = data->getQword();
			return true;
		}
	}

	bool nDataManager::getFloat( DWORD key, float* dst ) const
	{
		const nAny* data = getData( key );
		if( !data ){
			return false;
		}
		else{
			*dst = data->getFloat();
			return true;
		}
	}

	bool nDataManager::getDouble( DWORD key, double* dst ) const
	{
		const nAny* data = getData( key );
		if( !data ){
			return false;
		}
		else{
			*dst = data->getDouble();
			return true;
		}
	}

	const char* nDataManager::getCString( DWORD key ) const
	{
		const nAny* data = getData( key );
		if( !data ){
			return NULL;
		}
		else{
			return data->getCString();
		}
	}

	const char* nDataManager::getUserData( DWORD key ) const
	{
		const nAny* data = getData( key );
		if( !data ){
			return NULL;
		}
		else{
			return data->getUserData();
		}
	}

	bool nDataManager::copyCString( DWORD key, char* dst, DWORD dstsize ) const
	{
		const nAny* data = getData( key );
		if( !data ){
			return false;
		}
		else{
			data->copyCString( dst, dstsize );
			return true;
		}
	}

	bool nDataManager::copyUserData( DWORD key, char* dst, DWORD dstsize ) const
	{
		const nAny* data = getData( key );
		if( !data ){
			return false;
		}
		else{
			data->copyUserData( dst, dstsize );
			return true;
		}
	}

	void nDataManager::removeData( DWORD key )
	{
		DataMap::iterator itr = _data_map.find( key );
		if( itr == _data_map.end() )
			return;

		delete itr->second;
		_data_map.erase( itr );
	}
};
