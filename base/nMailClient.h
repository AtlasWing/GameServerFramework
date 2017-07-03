#ifndef BASE_NMAILCLIENT_H_
#define BASE_NMAILCLIENT_H_

#include "base/nSTL.h"

struct curl_slist;

namespace Tidy
{
	typedef void CURL;
	class nMailClient
	{
		public:
			nMailClient(const char* mailUserName, const char* mailUserPassword, const char* mailServer, const char *mailFromAddr);
			~nMailClient();

		public:
			bool init(const char *destAddrList);
			void final();

		public:
			bool sendMessageToUser(const char* message);

		private:
			Tidy::string _login_user_name;
			Tidy::string _login_user_password;
			Tidy::string _mail_server;
			Tidy::string _from_addr;
			CURL* _curl;
			struct curl_slist *_recv_addr_list;
	};
};
#endif//BASE_NMAILCLIENT_H_
