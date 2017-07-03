#include "game/nGameScript.h"
#include "game/nGameServer.h"

void c_assertFail(const char* info)
{
	assert_fail(info);
}

const char* c_getRootDir()
{
	std::string dir = (std::string)Tidy::global["scriptDir"];

	if(!dir.empty())
	{
		if(dir.at(dir.length()-1) != '/')
		{
			dir += '/';
		}
		return dir.c_str();
	}
	return NULL;
}

void c_debug(const char *pattern, ...)
{
    char buffer[MSGBUF_MAX];
    getMessage(&buffer[0], sizeof(buffer), pattern);
	Game::gameServer->getLogger()->writeLog(buffer, msgLen);
}

const char* c_getFileListByFolder(const char* folder)
{
#if 0
	std::string dir = (std::string)Tidy::global["scriptDir"];
	if(!dir.empty())
	{
		if(dir.at(dir.length()-1) != '/')
		{
			dir += '/';
		}
	}
	dir += folder;
#endif
    std::string dir = folder;

	std::ostringstream retFileName;
	DIR *pdir = opendir(dir.c_str());
	if( pdir == NULL )
		return NULL;

	dirent* dp;
	while((dp = readdir(pdir)) != NULL)
	{
		if(strcmp(dp->d_name, "." ) == 0 ||
				strcmp(dp->d_name, "..") == 0 ||
				strcmp(dp->d_name, ".svn") == 0 ||
				strchr(dp->d_name, '~') != NULL ||
				strstr(dp->d_name, ".swp") != NULL)
			continue;
		retFileName << dp->d_name <<" ";
	}
	closedir(pdir);

	return retFileName.str().c_str();
}
