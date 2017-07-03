#ifndef _N_RTX_CLIENT_H_
#define _N_RTX_CLIENT_H_

#include "base/nSTL.h"

namespace Tidy
{
	typedef void CURL;

	class nRTXClient
	{
		public:
			nRTXClient(const char* serverUserName, const char* serverUserPwd, const char* serverIPStr, const WORD serverPort);
			~nRTXClient();

		public:
			bool sendMessageToUser(const char* receiver, const char* message);
			bool sendMessageToUser(const Tidy::vector<const char*> receiver, const char* message);

		private:
			const Tidy::string _server_user_name;
			const Tidy::string _server_user_pwd;
			const Tidy::string _server_ip_str;
			const WORD _server_port;
			CURL* _curl;
	};
};
#endif
