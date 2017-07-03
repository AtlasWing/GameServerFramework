#include "base/nCommandCtl.h"
#include "base/nZip.h"
#include "base/Tidy.h"
#include "base/nEncrypt.h"
#include "base/nAssert.h"
#include "base/nBaseServer.h"
#include "base/nBaseMonitorService.h"

namespace Tidy
{
	// 消息包长度的掩码
	static const unsigned int PACKET_MASK			=	0xffffff;
	static const DWORD MAX_PACKET_SIZE              =   64 * 1024 * 1024;

	// 压缩消息的压缩之前的长度的掩码,用于区分压缩的方法
	static const unsigned int ZIP_CMD_LEN_MASK		=	0x3ffffff;
	static const unsigned int SNAPPY_METHOD_MASK	=	0x8000000;
#pragma pack(1)
	struct PacketData
	{
		union
		{
			DWORD packetMask;
			DWORD cmdSize;
		};
		char cmd[0];
		void putCmd(const void* cmd, const DWORD len)
		{
			cmdSize = len;
			memcpy(this->cmd, cmd, len);
		}
		const DWORD allSize() const
		{
			return getCmdSize() + sizeof(*this);
		}
		const DWORD getCmdSize() const
		{
			return cmdSize & PACKET_MASK;
		}
		static const DWORD getPacketSize(const DWORD cmdLen)
		{
			return (cmdLen & PACKET_MASK) + sizeof(PacketData);
		}
	};
#pragma pack()

	static const unsigned int PH_LEN            =   sizeof(unsigned int);
	//static const unsigned int trunkSize = 64 * 1024;
	/// @brief 数据包最大长度，包括包头4字节
	static const unsigned int MAX_DATABUFFERSIZE	=	PACKET_MASK;
	static const DWORD MAX_PACKET_LEN =	(MAX_DATABUFFERSIZE - PH_LEN);
	static const unsigned int PACKET_ZIP		=	0x40000000;
	DWORD MIN_PACKET_ZIP_SIZE[eZipMethod_Max] = {0, 96, 4 * 1024};

	static DWORD getMinPacketSize()\
	{ 
		return PH_LEN; 
	}

	static int getPacketSize(const BufferCmdQueue &dataQueue, const DWORD skipHeadSize)
	{
        const char *rdBuff = reinterpret_cast<const char*>(dataQueue.rd_buf()) + skipHeadSize;
		const PacketData* packet = reinterpret_cast<const PacketData*>(rdBuff);
		assert_debug(packet->allSize() == (PH_LEN + ((*((unsigned int *)rdBuff)) & PACKET_MASK)), "");
		const DWORD allSize = packet->allSize();
		if(allSize > MAX_PACKET_SIZE){
			return -1;
		}
		return static_cast<int>(allSize);
	}

	static int unpackCmd(nSocket *socket, BufferCmdQueue &in, BufferCmdQueue* out, void* cmdBuffer, const DWORD cmdBufferLen)
	{
		const PacketData* packet = reinterpret_cast<const PacketData*>(in.rd_buf());
		DWORD cmdSize = packet->getCmdSize();
		if(cmdSize == 0 || cmdSize > MAX_PACKET_SIZE){
			assert_fail("外挂,不存在长度为[%u]的消息", cmdSize);
			return -1;
		}
		assert_debug(packet->allSize() <= in.rd_size(), "数据不足");
		DWORD zipMask = (packet->packetMask & (~PACKET_MASK));
		//数据包压缩过
		if (zipMask == PACKET_ZIP) {
			assert_debug(out != NULL, "");
			//获得解压后的大小
			DWORD unzipLen = *(reinterpret_cast<const DWORD*>(packet->cmd));
			// 默认是zlib,兼容旧的压缩实现
			eZipMethod zipMethod = eZipMethod_ZLIB;
			if((unzipLen & SNAPPY_METHOD_MASK) > 0){
				zipMethod = eZipMethod_Snappy;
			}
			unzipLen = (unzipLen & ZIP_CMD_LEN_MASK);
			// 限制最大64M的解压缩空间
			if(unzipLen > MAX_PACKET_SIZE){
				assert_waigua("解压缩的空间[%u]大于64", unzipLen);
				return -1;
			}
			if(cmdSize <= sizeof(unzipLen)){
				assert_waigua("消息大小不足[%u]", cmdSize);
				return -1;
			}
			cmdSize -= sizeof(unzipLen);
			uLong nUnzipLen = unzipLen;
			out->wr_reserve(unzipLen+1);
			if(unZip(zipMethod, &(packet->cmd[sizeof(unzipLen)]), cmdSize, out->wr_buf(), &nUnzipLen) == false){
				return -1;
			}
			in.rd_flip(packet->allSize());
			out->wr_flip(nUnzipLen);
			assert_debug(nUnzipLen == unzipLen, "");
			//返回得到的字节数,尾递归
			return unpackCmd(socket, *out, NULL, cmdBuffer, cmdBufferLen);
		}
		else if(zipMask == 0){
			if(cmdSize <= cmdBufferLen) {
				bcopy(packet->cmd, cmdBuffer, cmdSize);
				in.rd_flip(packet->allSize());
				return cmdSize;
			}
			else {
				return cmdSize;
			}
		}
		else{
			assert_fail("掩码错误");
			return -1;
		}

		return 0;
	}

	static int encdecData(nEncrypt *encData, void *data, unsigned int nLen, bool enc)
	{
		return encData->encdec(data, nLen, enc, NULL);
	}

	static DWORD getBufferSize(const DWORD len)
	{
		return getMaxZipBufferSize(len);
	}

	static DWORD getCmdBufferSize(const DWORD len)
	{
		return PacketData::getPacketSize(getBufferSize(len));
	}

	static DWORD putPacket(const BYTE zipDataMethod, const void* data, const DWORD len, char* buffer, const DWORD bufferSize, const bool isCmd, DWORD *lastPacketSize)
	{
		eZipMethod zipMethod = static_cast<eZipMethod>(zipDataMethod);
		assert_debug(len <= bufferSize && (isCmd == false || getCmdBufferSize(len) <= bufferSize), "消息缓冲区不足");
		if(zipMethod != eZipMethod_None){
			baseServer->baseMonitorService->addPacket(zipDataMethod, len);
		}
		PacketData* packet = reinterpret_cast<PacketData*>(buffer);
		if(zipMethod != eZipMethod_None && len >= MIN_PACKET_ZIP_SIZE[zipDataMethod] && len < MAX_PACKET_SIZE ) {
			size_t zippedLen = bufferSize - sizeof(PacketData);
			DWORD* unzipLen = reinterpret_cast<DWORD*>(packet->cmd);

			if(isCmd == true) {
				*unzipLen = PacketData::getPacketSize(len);
				if((*unzipLen) < PACKET_ZIP_BUFFER) {
					StackCmdQueue cmdBuffer;
					cmdBuffer.wr_reserve(*unzipLen);

					PacketData* packetCmd = reinterpret_cast<PacketData*>(cmdBuffer.wr_buf());
					memcpy(packetCmd->cmd, data, len);
					packetCmd->cmdSize = len;
					cmdBuffer.wr_flip(packetCmd->allSize());
					if(zip(zipMethod, (const char*)cmdBuffer.rd_buf(), cmdBuffer.rd_size(), &packet->cmd[sizeof(*unzipLen)], &zippedLen) == false){
						return 0;
					}
				}
				else {
					BufferCmdQueue cmdBuffer;
					cmdBuffer.wr_reserve(*unzipLen);
					PacketData* packetCmd = reinterpret_cast<PacketData*>(cmdBuffer.wr_buf());
					memcpy(packetCmd->cmd, data, len);
					packetCmd->cmdSize = len;
					cmdBuffer.wr_flip(packetCmd->allSize());
					if(zip(zipMethod, (const char*)cmdBuffer.rd_buf(), cmdBuffer.rd_size(), &packet->cmd[sizeof(*unzipLen)], &zippedLen) == false){
						return 0;
					}
				}
			}
			else {
				*unzipLen = len;
				if(zip(zipMethod, (const char*)data, len, &packet->cmd[sizeof(*unzipLen)], &zippedLen) == false){
					return 0;
				}
			}
			if(zipMethod == eZipMethod_Snappy){
				(*unzipLen) |= SNAPPY_METHOD_MASK;
			}
			assert_debug(zippedLen < MAX_PACKET_SIZE, "[%u]", zippedLen);
			packet->cmdSize = zippedLen + sizeof(*unzipLen);
			packet->packetMask |= PACKET_ZIP;
			baseServer->baseMonitorService->addCompressPacketData(zipDataMethod, len, zippedLen);
		}
		else {
			if(isCmd == true) {
				memcpy(packet->cmd, data, len);
				packet->cmdSize = len;
			}
			else {
				memcpy(packet, data, len);
				//只是数据包从一个缓冲区拷到另外一个缓冲区,lastPacketSize 不变,长度不变,不能取packet->allSize()
				return len;
			}
		}

		*lastPacketSize = packet->allSize();
		return packet->allSize();
	}

	CommandCtl baseCommandCtl = {
		getMinPacketSize,
		getPacketSize,
		unpackCmd,
		encdecData,
		putPacket,
		getCmdBufferSize,
		getBufferSize,
		NULL,
	};
};
