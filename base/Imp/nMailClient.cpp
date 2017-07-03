#include "base/nMailClient.h"
#include "base/nStringTool.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	nMailClient::nMailClient(const char* mailUserName, const char* mailUserPassword, const char* mailServer, const char *mailFromAddr):
		_login_user_name(mailUserName),
		_login_user_password(mailUserPassword),
		_mail_server(mailServer),
		_from_addr(mailFromAddr),
		_curl(NULL),
		_recv_addr_list(NULL)
	{
		_from_addr.insert(0, "<");
		_from_addr.append(">");
	}

	nMailClient::~nMailClient()
	{
		assert_debug(_curl == NULL && _recv_addr_list == NULL, "");
	}

	bool nMailClient::init(const char *destAddrList)
	{
		Tidy::vector<Tidy::string> receiverList;
		nStringTool::splitValue(destAddrList, ';', receiverList);
		if(receiverList.empty() == true) {
			return true;
		}

		_curl = curl_easy_init();
		if(_curl == NULL) {
			return false;
		}

		for(DWORD i = 0; i < receiverList.size(); ++ i) {
			receiverList[i].insert(0, "<");
			receiverList[i].append(">");
		}

		for(DWORD i = 0; i < receiverList.size(); ++ i) {
			_recv_addr_list = curl_slist_append(_recv_addr_list, receiverList[i].c_str());  //接收者
		}

		if(_recv_addr_list == NULL) {
			curl_easy_cleanup(_curl);
			return false;
		}

		return true;
	}

	void nMailClient::final()
	{
		if(_recv_addr_list != NULL) {
			curl_slist_free_all(_recv_addr_list);
			_recv_addr_list = NULL;
		}
		curl_easy_cleanup(_curl);
		_curl = NULL;
	}

	static size_t sendMessage(void *ptr, size_t size, size_t nmemb, void *userData)
	{
		char *message = reinterpret_cast<char*>(userData);
		DWORD messageLen = strlen(message);
		if(messageLen > 0) {
			nStringTool::copyStr(reinterpret_cast<char*>(ptr), message, size * nmemb);
			message[0] = '\0';
		}

		return messageLen;
	}

	bool nMailClient::sendMessageToUser(const char* message)
	{
		char buffer [1024];
		DWORD offset = snprintf(buffer, sizeof(buffer), "Subject: %s", message);
		snprintf(buffer + offset, sizeof(buffer) - offset, "%s ", message);
		baseServer->time().format(buffer + offset, sizeof(buffer) - offset, "[%d/%m %T]");

		curl_easy_setopt(_curl, CURLOPT_URL, _mail_server.c_str());
		curl_easy_setopt(_curl, CURLOPT_MAIL_FROM, _from_addr.c_str());    //发送者
		curl_easy_setopt(_curl, CURLOPT_MAIL_RCPT, _recv_addr_list);
		curl_easy_setopt(_curl, CURLOPT_USERNAME, _login_user_name.c_str());
		curl_easy_setopt(_curl, CURLOPT_PASSWORD, _login_user_password.c_str());
		curl_easy_setopt(_curl, CURLOPT_READDATA, buffer);
		curl_easy_setopt(_curl, CURLOPT_READFUNCTION, sendMessage);
		// 不设置超时,curl会发生宕机,使用的alarm信号
		//curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT_MS, 5000);
#ifdef _DEBUG
		baseServer->debug("开始发邮件!");
#endif
		CURLcode  res = curl_easy_perform(_curl);
		if(res != CURLE_OK) {
			baseServer->info("Failed Send Mail[%d]:\n %s", res, buffer);
		}
#ifdef _DEBUG
		baseServer->debug("结束发邮件!");
#endif

		return true;
	}
};
