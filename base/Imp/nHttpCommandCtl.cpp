#include "base/nCommandCtl.h"
#include "base/nZlib.h"
#include "base/Tidy.h"
#include "base/nEncrypt.h"
#include "base/nAssert.h"
#include "base/nRegex.h"
#include "base/nConstructInPlace.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	static const DWORD MAX_HTTP_PACKET_SIZE = 16 * 1024 * 1024;
	static DWORD getMinPacketSize()
	{
		//包括 "GET \r\n\r\n",至少8个字符
		return 8; 
	}

	static int getPacketSize(const BufferCmdQueue &dataQueue, const DWORD skipHeadSize)
	{
		do{
			const char *dataStr = (const char*)dataQueue.rd_buf();
			if(dataQueue.rd_size() >=3 && strncmp(dataStr, "GET", 3) == 0){
				for(DWORD i = 0; i < dataQueue.rd_size() - 3; ++ i) {
					if(dataStr[i] == '\r' && dataStr[i+1] == '\n' && dataStr[i+2] == '\r' && dataStr[i+3] == '\n') {
						return i+4;
					}
				}
			}
			else if(dataQueue.rd_size() >= 4 && strncmp(dataStr, "POST", 4) == 0){
				bool dataBegin = false;
				DWORD num = 0;
				for(num = 0; num < dataQueue.rd_size() - 3; ++ num) {
					if(dataStr[num] == '\r' && dataStr[num+1] == '\n' && dataStr[num+2] == '\r' && dataStr[num+3] == '\n') {
						dataBegin = true;
						break;
					}
				}
				if(dataBegin == false){
					break;
				}
				num += 4;
				char key[] = "Content-Length:";
				const char *beginContentLengthStr = strstr(dataStr, key);
				if(beginContentLengthStr != NULL){
					beginContentLengthStr += sizeof(key);
					char *endContentLengthStr = NULL;
					SDWORD contentLength = strtol(beginContentLengthStr, &endContentLengthStr, 10);
					if(dataQueue.rd_size() < (num + contentLength)){
						break;
					}
					num += contentLength;
					return num;
				}
			}
			else{
				assert_fail("无法处理的请求");
				return -1;
			}
		}while(0);

		if(dataQueue.rd_size() >= MAX_HTTP_PACKET_SIZE){
			assert_fail("数据太多了[%u]", dataQueue.rd_size());
			return -1;
		}
		return MAX_HTTP_PACKET_SIZE;
	}

	static int unpackCmd(nSocket *socket, BufferCmdQueue &in, BufferCmdQueue* out, void* cmdBuffer, const DWORD cmdBufferLen)
	{
		int len = getPacketSize(in, 0);
		if(len == -1){
			return -1;
		}
		else if(static_cast<DWORD>(len) > cmdBufferLen) {
			return len;
		}
		HttpRequest *request = reinterpret_cast<HttpRequest*>(cmdBuffer);
		constructInPlace(request);
		request->num = 0;
		if(cmdBufferLen < (sizeof(request) + len)){
			return len;
		}

		const char *dataStr = (const char*)in.rd_buf();
		if(strncmp(dataStr, "GET", 3) == 0){
			request->requestType = eHttpRequestType_Get;
		}
		else if(strncmp(dataStr, "POST", 4) == 0){
			request->requestType = eHttpRequestType_Post;
		}
		else{
			assert_fail("不支持协议");
			return len;
		}

		if(request->requestType == eHttpRequestType_Get){
			const char *locationBegin = strchr(dataStr, '/');
			const char *locationEnd = strchr(locationBegin, ' ');
			request->locationBegin = request->num;
			request->num += (locationEnd - locationBegin);
			strncpy(request->data, locationBegin, locationEnd - locationBegin);
			request->data[request->num] = '\0';
			++ request->num;
			request->locationEnd = request->num;
			assert_debug(locationBegin != NULL && locationEnd != NULL, "路径错误");

			char *locationParamBegin = strchr(&request->data[request->locationBegin], '?');
			if(locationParamBegin != NULL){
				(*locationParamBegin) = '\0';
				request->locationParamBegin = request->locationBegin + (locationParamBegin - &request->data[request->locationBegin]) + 1;
				request->locationParamEnd = request->locationEnd;
				request->locationEnd = request->locationParamBegin - 1;
			}
		}

		if(request->requestType == eHttpRequestType_Post){
			char key[] = "Content-Length:";
			const char *beginContentLengthStr = strstr(dataStr, key);
			if(beginContentLengthStr != NULL){
				beginContentLengthStr += sizeof(key);
				char *endContentLengthStr = NULL;
				SDWORD contentLength = strtol(beginContentLengthStr, &endContentLengthStr, 10);
				assert_debug(contentLength > 0, "不会负数");
				if(contentLength > 0){
					request->postDataBegin = request->num;
					strncpy(&request->data[request->num], dataStr + len - contentLength, contentLength);
					request->num += contentLength;
					request->data[request->num] = '\0';
					++ request->num;
					request->postDataEnd = request->num;
				}
			}
		}
		//nRegex regex;
		//regex.compile("(POST)|(GET)|(DELETE)\\s([/\\S]+).*Content-Length:\\s*(\\<\\d?\\>)");
		//regex.match((const char*)in.rd_buf());
		in.rd_flip(len);
		return request->allSize();
	}

	static int encdecData(nEncrypt *encData, void *data, unsigned int nLen, bool enc)
	{
		return nLen;
	}

	static DWORD getCmdBufferSize(const DWORD len)
	{
		return len+2;
	}

	static DWORD getBufferSize(const DWORD len)
	{
		return len+2;
	}

	static DWORD putPacket(const BYTE zipMethod, const void* data, const DWORD len, char* buffer, const DWORD bufferSize, const bool isCmd, DWORD *lastPacketSize)
	{
		assert_debug(isCmd == false || getCmdBufferSize(len) <= bufferSize, "");
		std::string content(reinterpret_cast<const char*>(data), len);
		nStringTool::replace(content, "\n", "<br>");
		const DWORD headSize = snprintf(buffer, bufferSize, 
				"HTTP/1.1 200 OK\r\n"
				"Content-Length: %lu\r\n"
				"Content-Type: text/html\r\n\r\n%s"
				, content.length(), content.c_str());
		return headSize + 1;
	}

	CommandCtl httpCommandCtl = {
		getMinPacketSize,
		getPacketSize,
		unpackCmd,
		encdecData,
		putPacket,
		getCmdBufferSize,
		getBufferSize,
		NULL
	};
};
