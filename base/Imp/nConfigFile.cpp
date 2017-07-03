#include "base/Tidy.h"
#include "base/nLogger.h"
#include "base/nConfigFile.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	nConfigFile::nConfigFile(const char *filename)
	{
		this->_filename = filename;
	}

	nConfigFile::~nConfigFile()
	{
		_parser.freeFile();
	}

	bool nConfigFile::parseShare(Tidy::nParamMap *configMap)
	{
		if(!_parser.setCurNode("Config", "Share")){
			baseServer->fatal("加载全局配置失败");
			return false;
		}
		_parser.setChildNode();
		return this->parseNormal(configMap);
	}

	bool nConfigFile::parseNormal(Tidy::nParamMap *configMap)
	{
		char buf[64*1024];
		while(!_parser.isNoneNode()) {
			if(_parser.getNodeContentStr(buf, sizeof(buf))) {
				configMap->set(_parser.getNodeName(), buf);
			}
			_parser.setNextNode();
		}
		return true;
	}

	bool nConfigFile::parseNode(const char *name,bool ignore_false, Tidy::nParamMap *configMap)
	{
		if(!_parser.setCurNode("Config",name, false)) {
			if(!ignore_false) {
				baseServer->fatal("加载配置节点:%s失败",name);
				return false;
			}
			else {
				return true;
			}
		}
		_parser.setChildNode();
		this->parseNormal(configMap);
		return true;
	}

	bool nConfigFile::parse(const char *name, Tidy::nParamMap *configMap)
	{
		if(!_parser.loadFile(_filename.c_str()))
		{
			return false;
		}
		if(!parseShare(configMap))
		{
			return false;
		}
		if(!this->parseNode(name, true, configMap))
		{
			return false;
		}

		return true;
	}
};
