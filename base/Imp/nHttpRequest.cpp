#include "base/nHttpRequest.h"
#include "base/nBaseServer.h"
#include "base/nEncoding.h"

namespace Tidy
{
	nHttpRequest::nHttpRequest():
		_method(eHttpRequestMethod_None),
        _result(false),
        _reuse_conn(true),
        _need_retry(false),
        _retry_times(0)
	{
	}

	nHttpRequest::~nHttpRequest()
	{
	}

	void nHttpRequest::addResult(const char *data, const size_t size)
	{
		_result_data.append(data, size);
	}

	namespace{
		int writerData(unsigned char *data, size_t size, size_t nitems, void *outstream)
		{
			nHttpRequest *request = reinterpret_cast<nHttpRequest*>(outstream);
			size_t dataSize = size * nitems;
			request->addResult(reinterpret_cast<const char*>(data), dataSize);
			return dataSize;
		}
	};

	bool nHttpRequest::doRequest(CURL *curl)
	{
		std::string paramData = _param_data_map.concat('&');
		curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
		if(_method == eHttpRequestMethod_Post){
			curl_easy_setopt(curl, CURLOPT_POST, 1);
			if(paramData.empty() == false){
				curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, paramData.size());
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, paramData.c_str());
			}
		}
		else if(_method == eHttpRequestMethod_Get){
			curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
		}
		else{
			assert_fail("无法处理的请求类型");
		}
        if(checkReuseConn() == true){
            curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 0);
        }
        else{
            curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
        }
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writerData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
		CURLcode result = curl_easy_perform(curl);
		if(result != CURLE_OK){
			baseServer->debug("http请求错误[%s, %u]", _url.c_str(), result);
		}
        else{
            _result = true;
        }
		return _result;
	}

	bool nHttpRequest::completeRequest()
	{
		dealRecieveDataCallback(_result, _result_data);
		return true;
	}

	void nHttpRequest::setRequestMethod(eHttpRequestMethod method)
	{
		_method = method;
	}

	void nHttpRequest::setRequestUrl(const std::string &url)
	{
		_url = url;
	}

    const std::string& nHttpRequest::getRequestUrl() const
    {
        return _url;
    }

	const char* nHttpRequest::getRequestMethodName() const
	{
		return Tidy::getRequestMethodName(_method);
	}

	eHttpRequestMethod nHttpRequest::getRequestMethod() const
	{
		return _method;
	}

    const Tidy::nParamMap& nHttpRequest::getParamMap() const
	{
		return _param_data_map;
    }

    namespace{
        struct UrlEncodeParamExec{
            bool exec(const std::string &key, Tidy::nAny &value){
                std::string encodedStr = Tidy::nEncoding::urlEncode(value.getCString(), value.getSize());
                value.setString(encodedStr);
                return true;
            }
        };
    };
    void nHttpRequest::encodeAllParam()
    {
        UrlEncodeParamExec exec;
        _param_data_map.execAllData(exec);
    }

    bool nHttpRequest::checkReuseConn() const
    {
        return _reuse_conn;
    }

    void nHttpRequest::setReuseConn(bool needReuseConn)
    {
        _reuse_conn = needReuseConn;
    }

    std::string nHttpRequest::getParamConcatStr(const char separator) const
    {
        return _param_data_map.concat(separator);
    }

    void nHttpRequest::setNeedRetry()
    {
        _need_retry = true;
        _result_data.clear();
    }

    void nHttpRequest::clearNeedRetry()
    {
        _need_retry = false;
    }

    BYTE nHttpRequest::getRetryTimes() const
    {
        return _retry_times;
    }

    void nHttpRequest::addRetryTimes()
    {
        ++ _retry_times;
    }

    bool nHttpRequest::checkNeedRetry() const
    {
        return _need_retry;
    }
};
