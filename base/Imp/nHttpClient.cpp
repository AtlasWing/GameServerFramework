#include "base/nHttpRequest.h"
#include "base/nHttpClient.h"

namespace Tidy
{
	nHttpClient::nHttpClient():
		nThread("nHttpClient", true),
		_headers(NULL)
	{
	}

	nHttpClient::~nHttpClient()
	{
	}

	bool nHttpClient::init()
	{
		_curl = curl_easy_init();
		// 100-continue 问题
		_headers = curl_slist_append(_headers, "Expect:");
		if(_headers != NULL){
			curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _headers);
		}
		else{
			assert_fail("curl_slist_append 失败");
			return false;
		}
        // 超时时间
		curl_easy_setopt(_curl, CURLOPT_TIMEOUT_MS, 5000);
		curl_easy_setopt(_curl, CURLOPT_NOSIGNAL , 1);
		// 关闭对ssl证书的验证
		curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYHOST, 0L);
		start();
		return true;
	}

	void nHttpClient::final()
	{
		if(_headers != NULL){
			curl_slist_free_all(_headers);
		}
		nThread::join();
		curl_easy_cleanup(_curl);
	}

	bool nHttpClient::doTick()
	{
		if(_completed_request_list.empty() == true){
			return true;
		}

		RequestList requestList;
		_completed_list_lock.lock();
		requestList.swap(_completed_request_list);
		_completed_list_lock.unlock();
        nHttpRequest *request = NULL;
		for(RequestList::iterator iter = requestList.begin(); iter != requestList.end(); ++ iter){
            request = *iter;
			request->completeRequest();
            if(request->checkNeedRetry() == false){
                SAFE_DELETE(request);
            }
            else{
                request->clearNeedRetry();
                request->addRetryTimes();
                addHttpRequest(request);
            }
		}
		return true;
	}

	bool nHttpClient::addHttpRequest(nHttpRequest *request)
	{
		nMutex_scope_lock lock(_runing_request_list_lock);
		_waiting_request_list.push_back(request);
		return true;
	}

	DWORD nHttpClient::getRequestNum() const
	{
		return _waiting_request_list.size() + _runing_request_list.size();
	}

	void nHttpClient::run()
	{
		while(isFinal() == false || _runing_request_list.empty() == false || _waiting_request_list.empty() == false){
			setRunning();
			nThread::msleep(20);

			if(_runing_request_list.empty() == false){
				RequestList requestList;
				_runing_request_list_lock.lock();
				if(_runing_request_list.empty() == false){
					requestList.swap(_runing_request_list);
				}
				_runing_request_list_lock.unlock();
				if(requestList.empty() == false){
					for(RequestList::iterator iter = requestList.begin(); iter != requestList.end(); ++ iter){
						(*iter)->doRequest(_curl);
                        _completed_list_lock.lock();
                        _completed_request_list.push_back(*iter);
                        _completed_list_lock.unlock();
						setRunning();
					}
				}
			}
			if(_waiting_request_list.empty() == false){
				nMutex_scope_lock lock(_runing_request_list_lock);
				if(_waiting_request_list.empty() == false){
					_runing_request_list.swap(_waiting_request_list);
				}
			}
		}
	}
};
