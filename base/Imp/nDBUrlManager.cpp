#include "base/nDBUrlManager.h"

#include "base/nRegex.h"
#include "base/nBaseServer.h"
#include "base/nEncrypt.h"

namespace Tidy
{
	nDBUrl::nDBUrl(const DWORD id, const char *url):
		_id(id),
		_url(url),
		_port(0)
	{
		parseUrl();
	}

	nDBUrl::~nDBUrl()
	{}

	bool nDBUrl::checkValid() const
	{
		return _port != 0;
	}

	nDBUrl& nDBUrl::operator =(const nDBUrl& other)
	{
		this->_id = other._id;
		this->_url = other._url;
		this->_hostIP = other._hostIP;
		this->_user = other._user;
		this->_passwd = other._passwd;
		this->_database = other._database;
		this->_port = other._port;

		return *this;
	}

	const DWORD nDBUrl::getID() const
	{
		return _id;
	}

	bool nDBUrl::parseUrl()
	{
		if(_url.empty() == false && nStringTool::contains(_url, "mysql") == false){
			nEncrypt encrypt;
			encrypt.setEncMethod(eEncMethod_Private);
			eEncdecResult decResult = encrypt.encdec((void*)_url.c_str(), _url.size(), false, &_url);
			if(decResult != eEncdecResult_OK){
				assert_fail("解析失败[%d]", decResult);
				return false;
			}
		}
		nRegex re;
		if(re.compile("mysql://(.+):(.+)@(.+):(.+)/(.+)") && re.match(_url.c_str()))
		{
			std::string port_str;
			re.getSub(_user, 1);
			re.getSub(_passwd, 2);
			re.getSub(_hostIP, 3);
			re.getSub(port_str, 4);
			_port = atoi(port_str.c_str());
			re.getSub(_database, 5);
			baseServer->debug("[%s, %s, %s, %s, %s]", 
					_user.c_str(), _passwd.c_str(), _hostIP.c_str(), port_str.c_str(), _database.c_str());
			return true;
		}
		return false;
	}

	const std::string& nDBUrl::getUrl() const
	{
		return _url;
	}

	const std::string& nDBUrl::getDatabase()const
	{
		return _database;
	}

	const std::string& nDBUrl::getHostIP()const
	{
		return _hostIP;
	}

	const std::string& nDBUrl::getUser()const
	{
		return _user;
	}

	const std::string& nDBUrl::getPasswd()const
	{
		return _passwd;
	}

	WORD nDBUrl::getPort()const
	{
		return _port;
	}

	nDBUrlManager::nDBUrlManager()
	{}

	nDBUrlManager::~nDBUrlManager()
	{}

	bool nDBUrlManager::addDBUrl( nDBUrl *url )
	{
		bool bret = false;
		rwlock.wrlock();
		bret = add(url);
		rwlock.unlock();
		return bret;
	}

	void nDBUrlManager::removeDBUrl( nDBUrl *url )
	{
		rwlock.wrlock();
		remove(url);
		rwlock.unlock();
	}

	nDBUrl* nDBUrlManager::getDBUrlByID( const DWORD urlID )
	{
		nDBUrl *ret=NULL;
		rwlock.rdlock();
		ret= static_cast<nDBUrl*>(getByID(urlID));
		rwlock.unlock();
		return ret;
	}
};
