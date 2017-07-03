#include "base/nRTXClient.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	nRTXClient::nRTXClient(const char* serverUserName, const char* serverUserPwd, const char* serverIPStr, const WORD serverPort) : 
		_server_user_name(serverUserName),
		_server_user_pwd(serverUserPwd),
		_server_ip_str(serverIPStr),
		_server_port(serverPort)
	{
		_curl = curl_easy_init();
		curl_easy_setopt(_curl, CURLOPT_TIMEOUT_MS, 5000);
	}

	nRTXClient::~nRTXClient()
	{
		curl_easy_cleanup(_curl);
	}

	bool nRTXClient::sendMessageToUser(const Tidy::vector<const char*> receiverList, const char* message)
	{
		if(receiverList.empty() == true) {
			assert_fail("没有接受者");
			return true;
		}
		DWORD len = 0;
		char receiverListBuffer[MAX_PROTO_CMD_LEN];
		len += snprintf(&receiverListBuffer[len], sizeof(receiverListBuffer) - len, "%s", receiverList[0]);
		for(DWORD i = 1; i < receiverList.size(); ++ i) {
			len += snprintf(&receiverListBuffer[len], sizeof(receiverListBuffer) - len, ";%s", receiverList[i]);
		}
		return sendMessageToUser(receiverListBuffer, message);
	}

	bool nRTXClient::sendMessageToUser(const char* receiverList, const char* message)
	{
		if(_curl == NULL) {
			return false;
		}

		char messageBuffer[2048];
		nStringTool::convertCharset("utf-8", message, strlen(message)+1, "gb2312", messageBuffer, sizeof(messageBuffer));

		char urlBuffer[128];
		snprintf(urlBuffer, sizeof(urlBuffer), "%s:%u/SendIM.cgi", _server_ip_str.c_str(), _server_port);
		char dataBuffer[1024];
		snprintf(&dataBuffer[0], sizeof(dataBuffer), "sender=%s&pwd=%s&receivers=%s&msg=%s&sessionid={%lu}", _server_user_name.c_str(), _server_user_pwd.c_str(), receiverList, messageBuffer, baseServer->time().sec());

		curl_easy_setopt(_curl, CURLOPT_URL, urlBuffer);
		curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, dataBuffer);
		curl_easy_perform(_curl);

		return true;
	}
};
