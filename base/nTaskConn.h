#ifndef BASE_NTASKCONN_H_
#define BASE_NTASKCONN_H_

#include "base/nTimer.h"
#include "base/nObj.h"
#include "base/nServerConn.h"
#include "base/nCmdQueue.h"

namespace Tidy
{
	class nSocket;
	class nServerConn;
	class SocketPortData;

	class nTaskConn
	{
		public:
			enum eVerifyState{
				eVerifyState_Fail    =   0,
				eVerifyState_None    =   1,
				eVerifyState_Success =   2,
			};
			enum eSyncState{
				eSyncState_Fail      =   0,
				eSyncState_None      =   1,
				eSyncState_Success   =   2,
			};
			nTaskConn(nServerConn* serverConn, const int sock,const struct sockaddr *addr, const SocketPortData* listenPortData);
			virtual ~nTaskConn();

		public:
			/// @brief	添加检测事件到epoll描述符
			/// @param	kdpfd epoll描述符
			/// @param	events 待添加的事件
			/// @param	ptr 额外参数
			void addEpoll(int kdpfd, __uint32_t events, void *ptr);
			/// @brief	从epoll描述符中删除检测事件
			/// @param	kdpfd epoll描述符
			/// @param	events 待添加的事件
			void delEpoll(int kdpfd, __uint32_t events);
			/// @brief	检测是否验证超时
			/// @param	ct 当前系统时间
			/// @param	interval 超时时间，毫秒
			/// @return	检测是否成功
			bool checkVerifyTimeout(const nTime &ct, const unsigned long interval = 10000L) const;
			/// @brief	检查是否已经加入读事件
			/// @return	是否加入
			bool isReadEventRegged() const;
			/// @brief	设置加入读事件标志
			/// @return	是否加入
			bool setReadEventRegged();
			/// @brief	验证虚函数
			/// @return	验证是否成功
			//			1,验证成功
			//			0,超时,需要继续等待
			//			-1,失败,需要关闭连接
			virtual int verifyConn();

			/// @brief	等待其它线程同步验证这个连接，有些线程池不需要这步，所以不用重载这个函数，缺省始终返回成功
			/// @return		等待是否成功
			const nTaskConn::eSyncState waitSync();
			/// @brief	回收是否成功，回收成功以后，需要删除这个TCP连接相关资源
			/// @return		回收是否成功
			virtual bool recycleConn() = 0;
			/// @brief	判断是否需要关闭连接
			/// @return	true or false
			bool checkNeedClose() const;
			/// @brief	需要主动断开客户端的连接
			/// @param	method 连接断开方式
			void dealNetError();
			void setNeedClose();
            bool sendTaskCmd(const void *command, const DWORD len) const;
            bool sendRawDataIM(const void *data, const DWORD len) const;

			/// @brief	从套接口中接受数据，并且拆包进行处理，在调用这个函数之前保证已经对套接口进行了轮询
			/// @param	needRecv 是否需要真正从套接口接受数据
			//			false则不需要接收，只是处理缓冲中剩余的指令
			//			true需要实际接收数据，然后才处理
			/// @return	接收是否成功，true表示接收成功，false表示接收失败，可能需要断开连接 
			bool listeningRecv(bool needRecv);

			/// @brief	发送缓冲中的数据到套接口，再调用这个之前保证已经对套接口进行了轮询
			/// @return	发送是否成功，true表示发送成功，false表示发送失败，可能需要断开连接
			bool listeningSend();

			/// @brief	连接任务状态
			enum eTaskConnState
			{
				/// @brief	连接关闭状态
				TaskConnState_NotUse		=	0,
				/// @brief	连接验证状态
				TaskConnState_Verify		=	1,
				/// @brief	等待来自其它服务器的验证信息同步
				TaskConnState_Sync		=	2,
				/// @brief	连接处理阶段，验证通过了，进入主循环
				TaskConnState_Okay		=	3,
				/// @brief	连接退出状态，回收
				TaskConnState_Recycle		=	4,
				TaskConnState_Max
			};

			/// @brief	获取连接任务当前状态
			/// @return	状态
			const eTaskConnState getState() const;
			/// @brief	设置连接任务状态
			/// @param	state 需要设置的状态
			void setState(const eTaskConnState state);
			/// @brief	把TCP连接任务交给下一个任务队列，切换状态
			void getNextState();

			/// @brief	重值连接任务状态，回收连接
			void resetState();
			/// @brief	获得状态的字符串描述
			/// @param	state 状态
			/// @return	返回状态的字符串描述
			const char *getStateString(const eTaskConnState state) const;
			/// @brief	返回连接的端口
			/// @return	返回连接的端口
			const WORD getPort() const;
			/// @brief	返回连接的IP地址
			/// @return	连接的IP地址
			const char *getIP() const;
			/// @brief	是否发送网络连接链路测试信号
			/// @return	true or false
			inline const bool needCheckTick() const;
			void setNeedNowCheckTick();
			/// @brief	检测测试信号发送间隔
			/// @return	检测是否成功
			bool checkHeartTickInterval(const nTime &ct);
			/// @brief	检查测试信号，如果测试信号在规定时间内返回，那么重新发送测试信号，没有返回的话可能TCP连接已经出错了
			/// @return	true，表示检测成功；false，表示检测失败 
			bool checkTick() const;
			/// @brief	测试信号已经返回了
			void clearTick();
			/// @brief	发送测试信号成功
			void setTick(const nTime &ct);
			DWORD getPingTimeInUsec() const;
			/// @brief	信号检测
			void checkSignal(const nTime &ct);
			/// @brief	检查是否已经验证通过
			bool checkVerifyOK();
			void setVerifyState(const eVerifyState verifyState);

		public:
			virtual bool parseTaskRawCmd(const void *packet, const DWORD len) = 0;

		public:
			nServerConn* castAsServerConn() const;
			nSocket& getSocket();

		private:
			/// @brief	发送指令是否缓冲
			bool _buffered;
			/// @brief	连接状态
			eTaskConnState _state;
			/// @brief	读事件添加标志
			bool _is_read_event_regged;
			/// @brief	连接创建时间记录
			nTime _start_time;

			/// @brief	是否发送链路检测信号
			const bool _need_check_heart_tick;
			/// @brief	心跳定时器
			nTimer _tick_timer;
			/// @brief	定时检测套接口连接状况
			bool _in_tick;
			bool _need_now_check_tick;
			QWORD _start_tick_time_in_usec;
			DWORD _ping_time_in_usec;
			/// @brief	连接验证状态
			//			0等待验证,1验证成功,-1验证失败
			SWORD _verify_state;

		protected:
			int _recycle_time;
			//缓存,避免了使用虚函数每次进行转换
			nServerConn* const _server_conn;
			bool _is_net_error;
			/// @brief	底层套接口
			nSocket *_socket;
	};

	class nTaskConnNoQueue : public nTaskConn, public nServerConnNoQueue
	{
		public:
			nTaskConnNoQueue(const int sock, const struct sockaddr *addr, const SocketPortData* listenPortData);
			~nTaskConnNoQueue();

		public:
			virtual bool sendCmd(const void *command, const DWORD nCmdLen) const;
			virtual bool sendRawData(const void *command, const DWORD len) const;
			virtual bool parseTaskRawCmd(const void *packet, const DWORD len);

		private:
			virtual bool recycleConn();
	};

	class nTaskConnQueue : public nTaskConn, public nServerConnQueue
	{
		public:
			enum
			{
				Recycle_NONE,
				Recycle_Wait,
				Recycle_Delete,
			};

		public:
			nTaskConnQueue(const int sock, const struct sockaddr *addr, const SocketPortData* listenPortData);
			virtual ~nTaskConnQueue();

		public:
			virtual bool sendCmd(const void *command, const DWORD nCmdLen) const;
			virtual bool sendRawData(const void *command, const DWORD len) const;

		private:
			virtual bool recycleConn();
			virtual bool parseTaskRawCmd(const void *packet, const DWORD len);
	};


	inline const bool nTaskConn::needCheckTick() const
	{
		return _need_check_heart_tick == true || _need_now_check_tick == true;
	}

};

#endif
