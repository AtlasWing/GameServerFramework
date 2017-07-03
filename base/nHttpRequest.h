#ifndef BASE_NHTTPREQUEST_H
#define BASE_NHTTPREQUEST_H

#include "base/nSTL.h"
#include "base/nParamMap.h"

namespace Tidy
{
	enum eHttpRequestMethod{
		eHttpRequestMethod_None =   0,
		eHttpRequestMethod_Post =   1,
		eHttpRequestMethod_Get  =   2,
		eHttpRequestMethod_MAX
	};
	inline const char* getRequestMethodName(eHttpRequestMethod method){
		static const char* methodNameMap[eHttpRequestMethod_MAX] = {"NONE", "POST", "GET"};
		return methodNameMap[method];
	}
	class nHttpRequest
	{
		public:
			nHttpRequest();
			virtual ~nHttpRequest();

		public:
			template<typename ValueType>
				bool addParam(const char *key, ValueType &value){
					nAny data(value);
					_param_data_map.set(key, data);
					return true;
				}

		public:
			bool exec(const char *data);
			bool doRequest(CURL *curl);
			void addResult(const char *data, const size_t size);
			bool completeRequest();
			void setRequestMethod(eHttpRequestMethod method);
            void setRequestUrl(const std::string &url);
            const std::string& getRequestUrl() const;
			const char* getRequestMethodName() const;
			eHttpRequestMethod getRequestMethod() const;
            std::string getParamConcatStr(const char separator) const;
            bool checkReuseConn() const;
            void setReuseConn(bool needReuseConn);
            void setNeedRetry();
            void clearNeedRetry();
            bool checkNeedRetry() const;
            BYTE getRetryTimes() const;
            void addRetryTimes();

		protected:
            const Tidy::nParamMap& getParamMap() const;
            void encodeAllParam();

		private:
			virtual void dealRecieveDataCallback(const bool result, const std::string &resultData) = 0;

		private:
			eHttpRequestMethod _method;
			std::string _url;
			nParamMap _param_data_map;
			std::string _result_data;
			bool _result;
            bool _reuse_conn;
            bool _need_retry;
            BYTE _retry_times;
	};
};
#endif
