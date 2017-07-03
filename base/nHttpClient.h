#ifndef BASE_NHTTPCLIENT_H
#define BASE_NHTTPCLIENT_H

#include "base/nSTL.h"
#include "base/nThread.h"

namespace Tidy
{
	class nHttpRequest;
	class nHttpClient : public nThread
	{
		public:
			nHttpClient();
			~nHttpClient();

		public:
			bool init();
			void final();
			bool doTick();

		public:
			bool addHttpRequest(nHttpRequest *request);
			DWORD getRequestNum() const;

		private:
			virtual void run();

		private:
			typedef Tidy::vector<nHttpRequest*> RequestList;
			RequestList _runing_request_list;
			RequestList _waiting_request_list;
			nMutex _runing_request_list_lock;
			RequestList _completed_request_list;
			nMutex _completed_list_lock;
			CURL* _curl;
			struct curl_slist *_headers;
	};
};
#endif
