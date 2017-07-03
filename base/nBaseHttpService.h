#ifndef BASE_NBASEHTTPSERVICE_H
#define BASE_NBASEHTTPSERVICE_H

#include "base/nObjLogger.h"
#include "base/nSTL.h"

namespace Tidy{
	class nHttpRequest;
	class nHttpClient;

	class nBaseHttpService : public Tidy::nObjLogger<nBaseHttpService>{
		public:
			nBaseHttpService();
			~nBaseHttpService();

		public:
			const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;
			Tidy::nLogger* getLogger() const;

		public:
			bool initService(const DWORD maxClientPoolNum, const DWORD maxRequestNumPerClient);
			void finalService();
			bool doTick();

		public:
			bool addRequest(nHttpRequest *request);

		protected:
			void setHttpClientPoolNum(const DWORD maxClientPoolNum, const DWORD maxRequestNumInOneClient);

		private:
			nHttpClient* createClient();

		private:
			DWORD _max_client_pool_num;
			DWORD _max_request_num_in_one_client;
			Tidy::vector<nHttpClient*> _client_pool;
	};
};
#endif
