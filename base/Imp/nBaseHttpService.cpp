#include "base/nBaseHttpService.h"
#include "base/nBaseServer.h"
#include "base/nHttpClient.h"
#include "base/nHttpRequest.h"

namespace Tidy
{
	nBaseHttpService::nBaseHttpService()
	{
	}

	nBaseHttpService::~nBaseHttpService()
	{
	}

	const DWORD nBaseHttpService::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const
	{
		return copyStr(headerBuffer, "HTTP ", bufferSize) - 1;
	}

	Tidy::nLogger* nBaseHttpService::getLogger() const
	{
		return baseServer->getLogger();
	}

	bool nBaseHttpService::initService(const DWORD maxClientPoolNum, const DWORD maxRequestNumInOneClient)
	{
		if(maxClientPoolNum == 0 || maxRequestNumInOneClient == 0){
			assert_fail("线程配置数量 maxClientPoolNum,maxRequestNumInOneClient 为0");
			return false;
		}
		_max_client_pool_num = maxClientPoolNum;
		_max_request_num_in_one_client = maxRequestNumInOneClient;
		if(_max_client_pool_num > 0){
			if(createClient() == false){
				return false;
			}
		}
		return true;
	}

	void nBaseHttpService::finalService()
	{
		for(DWORD i = 0; i < _client_pool.size(); ++ i){
			_client_pool[i]->stop();
			_client_pool[i]->join();
			SAFE_DELETE(_client_pool[i]);
		}
	}

	bool nBaseHttpService::doTick()
	{
		for(DWORD i = 0; i < _client_pool.size(); ++ i){
			_client_pool[i]->doTick();
		}
		return true;
	}

	bool nBaseHttpService::addRequest(nHttpRequest *request)
	{
		nHttpClient *client = NULL;
		for(DWORD i = 0; i < _client_pool.size(); ++ i){
			if(client == NULL || _client_pool[i]->getRequestNum() < client->getRequestNum()){
				client = _client_pool[i];
			}
		}
		if(_client_pool.size() < _max_client_pool_num && (client == NULL || client->getRequestNum() > _max_request_num_in_one_client)){
			client = createClient();
			if(client == NULL){
				assert_fail("创建客户端线程失败");
				return false;
			}
			info("创建新的连接线程,当前线程数[%lu]", _client_pool.size());
		}

		assert_debug(client != NULL, "[%lu]", _client_pool.size());
		return client->addHttpRequest(request);
	}

	nHttpClient* nBaseHttpService::createClient()
	{
		nHttpClient *client = new nHttpClient();
		if(client->init() == false){
			SAFE_DELETE(client);
			return NULL;
		}
		_client_pool.push_back(client);
		return client;
	}

	void nBaseHttpService::setHttpClientPoolNum(const DWORD maxClientPoolNum, const DWORD maxRequestNumInOneClient)
	{
		_max_client_pool_num = maxClientPoolNum;
		_max_request_num_in_one_client = maxRequestNumInOneClient;
	}
}
