#include "base/nObj.h"
#include "base/Tidy.h"
#include "base/nLogger.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	nObj::nObj() :
		_id(0) 
	{
	}

	nObj::nObj(const DWORD id, const char* name) :
		_id(id),
		_name(name)
	{
	}

	nObj::~nObj() 
	{
	}

	const DWORD nObj::getID() const
	{
		return _id;
	}

	const char* nObj::getName() const
	{
		return _name.c_str();
	}

	DWORD nObj::getNameLength() const
	{
		return _name.length();
	}

	const bool nObj::isSame(const nObj* other) const
	{
		return this == other;
	}

	nLogger* nObj::getLogger() const 
	{
		return baseServer->getLogger();
	}

	const DWORD nObj::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const 
	{
		return snprintf(headerBuffer, bufferSize, "[%u,%s]", getID(), getName());
	}

	void nObj::setID(DWORD id)
	{
		_id = id;
	}

	void nObj::setName(const char *name)
	{
		_name = name;
	}

	void nObj::setName(const std::string &name)
	{
		_name = name;
	}
};
