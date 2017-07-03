#ifndef _BASE_IMP_NSOCKET_H_
#define _BASE_IMP_NSOCKET_H_

#include "base/Tidy.h"
#include "base/nTime.h"
#include "base/nZlib.h"
#include "base/nMutex.h"
#include "base/nEncrypt.h"
#include "base/nByteBuffer.h"
#include "base/nCommandCtl.h"

namespace Tidy
{
	/// @brief 封装套接口底层函数，提供一个比较通用的接口
	class nSocket
	{
		public:
			/// @brief 读取超时的毫秒数
			static const int T_RD_MSEC					=	2100;
			/// @brief 发送超时的毫秒数
			static const int T_WR_MSEC					=	5100;					
			/// @brief 上次对套接口进行读取操作没有读取完全的标志
			static const unsigned int INCOMPLETE_READ	=	0x00000001;
			/// @brief 上次对套接口进行写入操作没有写入完毕的标志
			static const unsigned int INCOMPLETE_WRITE	=	0x00000002;

		public:
			DISALLOW_COPY_AND_ASSIGN(nSocket);
			nSocket(const int sock, const struct sockaddr *addr, const CommandCtl* ctl, const BYTE zipMethod, const bool needQuickAck);
			~nSocket();

		public:
			static int createTcpSocket();
			static int createUnixSocket();
			static int bindTcpServer(const std::string &ip, WORD port);
			static int bindUnixServer(const char *path);
			static nSocket* connectTcpServer(const char *ip, int port, const CommandCtl *commandCtl, const BYTE zipMethod);
			static nSocket* connectUnixServer(const char *destUnixSocketPath, const CommandCtl *commandCtl, const BYTE zipMethod);

		public:
			/// @brief 更换Socket描述符
			const int getSocket()
			{
				return _sock;
			}
            const struct CommandCtl* getCommandCtl() const;
            void changeCommandCtl(const CommandCtl* commandCtl);
			/// @brief 接收指令到缓冲区
			/// @param pstrCmd 指令缓冲区
			/// @param nCmdLen 指令缓冲区的大小
			/// @param wait 当套接口数据没有准备好的时候，是否需要等待
			/// @return 实际接收的指令大小
			//			返回-1，表示接收错误
			//			返回0，表示接收超时
			//			返回整数，表示实际接收的字节数
			DWORD recvToCmd(void *pstrCmd, const DWORD nCmdLen, const bool wait);
			bool sendCmd(const void *command, const DWORD len, const bool buffer = false);
			/// @brief 定时检测并发送一次已经缓冲好的数据
			/// @return 发送是否成功
			bool sync();
			/// @brief 	强制发送一次已经缓冲好的数据
			//			主要用在套接口关闭时需要处理所有剩余的数据
			void force_sync();

			/// @brief 检查套接口准备好读取操作
			/// @return
			//			-1，表示操作失败
			//			0，表示操作超时
			//			1，表示等待成功，套接口已经有数据准备读取
			int checkIOForRead();
			/// @brief 等待套接口准备好写入操作
			/// @return
			//			-1，表示操作失败
			//			0，表示操作超时
			//			1，表示等待成功，套接口已经可以写入数据
			int checkIOForWrite();
			/// @brief 	接收数据到缓冲区，保证在调用这个函数之前套接口准备好了接收，也就是使用poll轮询过
			//			如果是加密包需要先解密到解密缓冲区
			/// @return 	实际接收字节数
			//			返回-1，表示接收错误
			//			返回0，表示接收超时
			//			返回整数，不加密时表示实际接收的字节数,加密时返回解密到的字节数
			int recvToBuf_NoPoll();
			/// @brief 接收指令到缓冲区，不从套接口接收指令，只是把接收缓冲的数据解包
			/// @param pstrCmd 指令缓冲区
			/// @param nCmdLen 指令缓冲区的大小
			/// @return 实际接收的指令大小
			//			返回-1，表示接收错误
			//			返回0，表示接收超时
			//			返回整数，表示实际接收的字节数
			DWORD recvToCmd_NoPoll(void *pstrCmd, const DWORD nCmdLen);

			/// @brief 获取套接口对方的地址
			/// @return IP地址
			inline const char *getIP() const { return _peer_ip_str.c_str(); }

			/// @brief 获取套接口对方端口
			/// @return 端口
			inline const unsigned short getPort() const { return _peer_port; }

			/// @brief 获取套接口本地的地址
			/// @return IP地址
			inline const char *getLocalIP() const { return _local_ip_str.c_str(); }

			/// @brief 获取套接口本地端口
			/// @return 端口
			inline const DWORD getLocalPort() const { return _local_port; }

			/// @brief 设置读取超时
			/// @param msec 超时，单位毫秒 
			/// @return 
			inline void setReadTimeout(const int msec) { rd_msec = msec; }

			/// @brief 设置写入超时
			/// @param msec 超时，单位毫秒 
			/// @return 
			inline void setWriteTimeout(const int msec) { wr_msec = msec; }

			/// @brief 添加检测事件到epoll描述符
			/// @param kdpfd epoll描述符
			/// @param events 待添加的事件
			/// @param ptr 额外参数
			void addEpoll(int kdpfd, __uint32_t events, void *ptr);
			/// @brief 从epoll描述符中删除检测事件
			/// @param kdpfd epoll描述符
			/// @param events 待添加的事件
			void delEpoll(int kdpfd, __uint32_t events);
			/// @brief 设置加密方式
			/// @param m	加密方式
			/// @return 
			inline void setEncMethod(eEncMethod method) { getEncData()->setEncMethod(method); }
			/// @brief 设置des加密方式
			/// @param key	des密钥
			/// @return 
			inline void set_key_des(const_DES_cblock *key) { getEncData()->setDesKey(key); }
			// 当前所有缓存的发送数据的大小
			DWORD getSendBufferSize() const;
			/// @brief 当前接收buf中的字节数
			/// @return 当前接收buf中的字节数
			inline unsigned int rcv_raw_size() const { return _rcv_raw_size; }

			bool heartTick() const;
			void setNeedClose();
			bool checkNeedClose() const;
			/// @brief 向套接口发送原始数据，没有打包的数据，一般发送数据的时候需要加入额外的包头
			/// @param pBuffer 待发送的原始数据
			/// @param nSize 待发送的原始数据大小
			/// @return 实际发送的字节数
			//			返回-1，表示发送错误
			//			返回0，表示发送超时
			//			返回整数，表示实际发送的字节数
			int sendRawData(const void *pBuffer, const DWORD nSize);

		private:
			/// @brief 设置某个标志位
			/// @param _f 需要设置的标志位
			/// @return 
			inline void set_flag(unsigned int _f) { _bit_mask |= _f; }
			/// @brief 检测是否设置某个标志位
			/// @param _f 需要检测的标志位
			/// @return 是否设置
			inline bool isset_flag(unsigned int _f) const { return _bit_mask & _f; }
			/// @brief 清除一个已经设置的标志位
			/// @param _f 已经设置的标志位
			inline void clear_flag(unsigned int _f) { _bit_mask &= ~_f; }
			/// @brief 设置套接口为非阻塞模式
			/// @return 操作是否成功
			bool setNonblock();

			/// @brief 发送指定字节数的原始数据，忽略超时，无限制时间的发送，直到发送完毕或者发送失败
			/// @param pBuffer 待发送的原始数据
			/// @param nSize 待发送的原始数据大小
			/// @return 发送数据是否成功
			inline bool sendRawDataIM(const void *pBuffer, const DWORD nSize);
			/// @brief 向套接口发送原始数据，没有打包的数据，一般发送数据的时候需要加入额外的包
			// 头，保证在调用这个函数之前套接口准备好了接收，也就是使用poll轮询过
			/// @param pBuffer 待发送的原始数据
			/// @param nSize 待发送的原始数据大小
			/// @return 实际发送的字节数
			//			返回-1，表示发送错误
			//			返回0，表示发送超时
			//			返回整数，表示实际发送的字节数
			int sendRawData_NoPoll(const void *pBuffer, const DWORD nSize);
			/// @brief 等待套接口准备好读取操作
			/// @return 是否成功
			//			-1，表示操作失败
			//			0，表示操作超时
			//			1，表示等待成功，套接口已经有数据准备读取
			inline int waitForRead();
			/// @brief 等待套接口准备好写入操作
			/// @return 是否成功
			//			-1，表示操作失败
			//			0，表示操作超时
			//			1，表示等待成功，套接口已经可以写入数据
			inline int waitForWrite();
			/// @brief 	接收数据到缓冲区
			//			如果是加密包需要解密到解密缓冲区
			/// @return 	实际接收字节数
			//			返回-1，表示接收错误
			//			返回0，表示接收超时
			//			返回整数，不加密包表示实际接收的字节数,加密包返回解密后实际可用的字节数
			inline int recvToBuf();

			/// @brief 				对数据进行加密
			/// @param cmd_queue		待加密的数据，输入输出
			/// @param current_cmd	最后一个指令长度
			/// @param offset		待加密数据的偏移
			/// @return 				返回加密以后真实数据的大小
			template<typename buffer_type>
				inline DWORD packetPackEnc(buffer_type &cmd_queue, const DWORD current_cmd, const DWORD offset = 0);
			void keepQuickAck();
			nEncrypt* getEncData();
			inline bool checkNeedZip() const;

		private:
			DWORD _bit_mask;
			/// @brief 套接口
			int _sock;
			std::string _peer_ip_str;
			WORD _peer_port;
			std::string _local_ip_str;
			WORD _local_port;
			/// @brief 读取超时，毫秒
			int rd_msec;
			/// @brief 写入超时，毫秒 
			int wr_msec;
			/// @brief 接收缓冲指令队列
			BufferCmdQueue _rcv_queue;
			/// @brief 解压后的消息队列
			BufferCmdQueue _unpack_cmd_queue;
			/// @brief 接收缓冲解密数据大小
			DWORD _rcv_raw_size;
			/// @brief 等待加密的消息报队列
			BufferCmdQueue _wait_zip_packet_queue;
			/// @brief 缓冲指令队列
			//			存放待加密数据
			BufferCmdQueue _snd_queue;
			/// @brief 加密缓冲指令队列
			//			存放已经加密的数据
			BufferCmdQueue _enc_queue;
			/// @brief 最后一个放入缓冲队列中的指令位置
			//			为加密的8字节填充做准备
			//			修改消息头大小为8字节对齐的
			DWORD _current_packet;
			/// @brief 锁
			nMutex _mutex;

			const CommandCtl* _command_ctl;
			BYTE _zip_method;
			bool _need_enc;
			/// @brief 加密方式
			nEncrypt *_enc_data;
			bool _need_close;
			bool _need_quick_ack;
	};

	inline bool nSocket::checkNeedZip() const
	{
		return _zip_method != eZipMethod_None;
	}

	//================================template function =======================================
	template<typename buffer_type>
		inline DWORD nSocket::packetPackEnc(buffer_type &cmd_queue, const DWORD current_cmd, const DWORD offset)
		{
			DWORD mod = (cmd_queue.rd_size() - offset) % 8;
			if (0!=mod)
			{
				mod = 8 - mod;
				(*(DWORD *)(&(cmd_queue.rd_buf()[cmd_queue.rd_size() - current_cmd]))) += mod;
				cmd_queue.wr_reserve(mod);
				cmd_queue.wr_flip(mod);
			}

			//加密动作
			_command_ctl->encdecData(_enc_data, (void*)(&cmd_queue.rd_buf()[offset]), cmd_queue.rd_size() - offset, true);

			return cmd_queue.rd_size();
		}
};

#endif
