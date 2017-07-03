#include "base/nCommandCtl.h"
#include "base/Tidy.h"
#include "base/nAssert.h"
#include "base/nBaseServer.h"
#include "base/nBaseMonitorService.h"
#include "base/Imp/nSocket.h"

namespace Tidy
{
    // 例子 tgw_l7_forward\r\nHost: app12345.qzoneapp.com:80\r\n\r\n
    static char FIRST_PACKAGE_HEAD_START[] = "tgw_l7_forward\r\nHost: ";
    static char FIRST_PACKAGE_HEAD_END[] = "\r\n\r\n";
	static int getFirstPacketSize(const BufferCmdQueue &dataQueue)
    {
        const char *dataStr = (const char*)dataQueue.rd_buf();
        // 只处理了数据包头一次收取完的情况,如果分成两次就没有当做数据包头来处理,这种情况应该是很少,忽略了
        if(dataQueue.rd_size() >= (sizeof(FIRST_PACKAGE_HEAD_START) - 1 + sizeof(FIRST_PACKAGE_HEAD_END) - 1)){
            if(memcmp(dataStr, FIRST_PACKAGE_HEAD_START, (sizeof(FIRST_PACKAGE_HEAD_START) - 1)) == 0){
                for(DWORD i = (sizeof(FIRST_PACKAGE_HEAD_START) - 2); i < (dataQueue.rd_size() - sizeof(FIRST_PACKAGE_HEAD_END + 1)); ++ i){
                    if(memcmp(&dataStr[i], FIRST_PACKAGE_HEAD_END, (sizeof(FIRST_PACKAGE_HEAD_END) - 1)) == 0){
                        return i + 4;
                    }
                }
            }
        }

        return 0;
    }

	static int getPacketSize(const BufferCmdQueue &dataQueue, const DWORD skipHeadSize)
    {
        int firstPacketSize = getFirstPacketSize(dataQueue);
        return baseCommandCtl.getPacketSize(dataQueue, firstPacketSize) + firstPacketSize;
    }

	static int unpackCmd(nSocket *socket, BufferCmdQueue &in, BufferCmdQueue* out, void* cmdBuffer, const DWORD cmdBufferLen)
    {
        int firstPacketLen = getFirstPacketSize(in);
        if(firstPacketLen != 0){
			in.rd_flip(firstPacketLen);
            socket->changeCommandCtl(&baseCommandCtl);
        }
        return baseCommandCtl.unpackCmd(socket, in, out, cmdBuffer, cmdBufferLen);
    }

	CommandCtl qqCloudCommandCtl = {
		baseCommandCtl.getMinPacketSize,
		getPacketSize,
		unpackCmd,
		baseCommandCtl.encdecData,
		baseCommandCtl.putPacket,
		baseCommandCtl.getCmdBufferSize,
		baseCommandCtl.getBufferSize,
		NULL,
	};
};

