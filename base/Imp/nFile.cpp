#include "base/nFile.h"
#include "base/nSys.h"

namespace Tidy
{
	nFile::nFile()
	{
	}

	nFile::~nFile()
	{
	}

	bool nFile::exists(const std::string &fileName)
	{
		struct stat buf;
		return stat(fileName.c_str(), &buf) > -1;
	}
};
