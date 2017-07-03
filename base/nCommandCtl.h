#ifndef _N_COMMAND_CTL_H_
#define _N_COMMAND_CTL_H_

#include "base/nType.h"
#include "base/nByteBuffer.h"
#include "base/nZip.h"

namespace Tidy
{
	class nEncrypt;
    class nSocket;

	struct CommandCtl
	{
		/// @brief 返回数据包包头最小长度
		DWORD (*getMinPacketSize)();
		/// @brief 返回整个数据包的长度
		int (*getPacketSize)(const BufferCmdQueue &dataQueue, const DWORD skipHeadSize);
		int (*unpackCmd)(nSocket *socket, BufferCmdQueue &in, BufferCmdQueue* out, void* cmdBuffer, const DWORD cmdBufferLen);
		int (*encdecData)(nEncrypt *encData, void *data, unsigned int nLen, bool enc);
		DWORD (*putPacket)(const BYTE zipMethod, const void* data, const DWORD len, char* buffer, const DWORD bufferSize, const bool isCmd, DWORD *lastPacketSize);
		DWORD (*getCmdBufferSize)(const DWORD len);
		DWORD (*getBufferSize)(const DWORD len);
		std::string (*getCommandID)(const void* data, const DWORD len);
	};

#pragma pack(1)
	struct HttpRequest{
		BYTE requestType;// eHttpRequestType
		WORD locationBegin;
		WORD locationEnd;
		WORD locationParamBegin;
		WORD locationParamEnd;
		WORD postDataBegin;
		WORD postDataEnd;
		WORD num;
		char data[0];
		HttpRequest() : 
			locationBegin(0),
			locationEnd(0),
			locationParamBegin(0),
			locationParamEnd(0),
			postDataBegin(0),
			postDataEnd(0)
		{
		}
		bool hasLocationParam() const{
			return locationParamBegin != locationParamEnd;
		}
		const char* getLocationParamBegin() const{
			return &data[locationParamBegin];
		}
		const char* getLocationParamEnd() const{
			return &data[locationParamEnd];
		}
		bool hasPostData() const{
			return postDataBegin != postDataEnd;
		}
		const char* getPostDataBegin() const{
			return &data[postDataBegin];
		}
		const char* getPostDataEnd() const{
			return &data[postDataEnd];
		}
		DWORD allSize() const{
			return sizeof(*this) + num * sizeof(data[0]);
		}
	};
#pragma pack()
	enum eHttpRequestType{
		eHttpRequestType_Get,
		eHttpRequestType_Post,
		eHttpRequestType_Delete,
	};

	extern CommandCtl baseCommandCtl;
	extern CommandCtl telnetCommandCtl;
	extern CommandCtl httpCommandCtl;
	extern CommandCtl qqCloudCommandCtl;

	// 启用消息包压缩的最小阈值
	extern DWORD MIN_PACKET_ZIP_SIZE[eZipMethod_Max];
};
#endif
