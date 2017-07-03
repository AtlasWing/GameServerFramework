#ifndef _N_DBURL_MANAGER_H_
#define _N_DBURL_MANAGER_H_

#include "base/nRWLock.h"
#include "base/nManager.h"

namespace Tidy
{
	class nDBUrl
	{
		public:
			nDBUrl(const DWORD id, const char *url);
			~nDBUrl();

		public:
			bool checkValid() const;
			nDBUrl& operator =(const nDBUrl& other);
			bool setUrl( const std::string &url );
			bool parseUrl();
			const std::string& getUrl() const;
			const std::string& getDatabase()const;
			const std::string& getHostIP()const;
			const std::string& getUser()const;
			const std::string& getPasswd()const;
			WORD getPort()const;

		public:
			const DWORD getID() const;

		private:
			DWORD _id;
			std::string _url;
			std::string _hostIP;
			std::string _user;
			std::string _passwd;
			std::string _database;
			WORD _port;
	};

	class nDBUrlManager : public nManager<true, nIndexByID< CommonGetIDPolicy<nDBUrl> > >
	{
		public:
			nDBUrlManager();
			~nDBUrlManager();

		public:
			bool addDBUrl( nDBUrl* url );
			void removeDBUrl( nDBUrl *url );
			nDBUrl* getDBUrlByID( const DWORD urlID );
			template <class ExecT>
				bool execEveryDBUrl(ExecT &exec)
				{
					rwlock.rdlock();
					bool ret = execAll(exec);
					rwlock.unlock();
					return ret;
				}

		private:
			nRWLock rwlock;
	};
};

#endif

