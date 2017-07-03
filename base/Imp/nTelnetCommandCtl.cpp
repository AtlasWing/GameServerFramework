#include "base/nCommandCtl.h"
#include "base/nZlib.h"
#include "base/Tidy.h"
#include "base/nEncrypt.h"
#include "base/nAssert.h"

namespace Tidy
{
	static const DWORD MAX_TELNET_PACKET_SIZE = 16 * 1024;
	static DWORD getMinPacketSize()
	{
		return 3; 
	}

	static int getPacketSize(const BufferCmdQueue &dataQueue, const DWORD skipHeadSize)
	{
		const unsigned char* dataVec = dataQueue.rd_buf() + skipHeadSize;
		for(DWORD i = 0; i < dataQueue.rd_size() - 1; ++ i) {
			if(dataVec[i] == '\r' && dataVec[i+1] == '\n') {
				return i+2;
			}
		}
		if(dataQueue.rd_size() >= MAX_TELNET_PACKET_SIZE){
			assert_fail("数据太大[%u]", dataQueue.rd_size());
			return -1;
		}

		return MAX_TELNET_PACKET_SIZE;
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
		memcpy(cmdBuffer, in.rd_buf(), len-2);
		((char*)cmdBuffer)[len-2] = '\0';
		in.rd_flip(len);
		return len-1;
	}

	static nEncrypt encrypter;
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
		memcpy(buffer, data, len);
		buffer[len] = '\r';
		buffer[len+1] = '\n';
		return len+2;
	}

	CommandCtl telnetCommandCtl = {
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
