#ifndef _N_CLIENT_CONN_H_
#define _N_CLIENT_CONN_H_

#include "base/nTime.h"
#include "base/nTimer.h"
#include "base/nServerConn.h"
#include "base/nConnectServerData.h"

namespace Tidy
{
	class nSocket;
	class nClientConn
	{
		public:
			/// @brief	连接断开类型
			enum eTerminateMethod
			{
				/// @brief	没有结束任务 
				TM_no,
				/// @brief	检测到套接口关闭或者套接口异常 
				TM_sock_error,
				/// @brief	服务器即将关闭 
				TM_service_close,
				eTerminateMethod_NeedClose,
			};

			/// @brief	连接任务状态
			enum eClientConnState {
				/// @brief	连接关闭状态 
				eClientConnState_Close	=	0,
				/// @brief	等待同步状态 
				eClientConnState_Sync	=	1,							
				/// @brief	连接处理阶段 
				eClientConnState_Ok	=	2,
				/// @brief	连接退出状态 
				eClientConnState_Recycle	=	3,
				eClientConnState_Max
			};

			nClientConn(nServerConn* serverConn, const nConnectServerData &connectServerData);
			virtual ~nClientConn();

        public:
			void final();
			/// @brief	判断是否需要关闭连接
			/// @return	true or false
			bool isTerminate() const
			{
				return TM_no != terminate;
			}

			/// @brief	需要主动断开客户端的连接
			/// @param	method 连接断开方式
			void Terminate(const eTerminateMethod method)
			{
				terminate = method;
			}
			void setNeedClose();

			/// @brief	如果是第一次进入主循环处理，需要先处理缓冲中的指令
			/// @return	是否是第一次进入主处理循环
			bool checkFirstMainLoop()
			{
				if (_is_in_main_loop)
					return false;
				else
				{
					_is_in_main_loop = true;
					return true;
				}
			}

			/// @brief	获取连接任务当前状态
			/// @return	状态
			const eClientConnState getState() const
			{
				return _state;
			}

			/// @brief	设置连接任务下一个状态
			/// @param	state 需要设置的状态
			void setState(const eClientConnState state)
			{
				this->_state = state;
			}

			/// @brief	获得状态的字符串描述
			/// @param	state 状态
			/// @return	返回状态的字符串描述
			const char *getStateString(const eClientConnState state);
			/// @brief	添加检测事件到epoll描述符
			/// @param	kdpfd epoll描述符
			/// @param	events 待添加的事件
			/// @param	ptr 额外参数
			void addEpoll(int kdpfd, __uint32_t events, void *ptr);
			/// @brief	从epoll描述符中删除检测事件
			/// @param	kdpfd epoll描述符
			/// @param	events 待添加的事件
			void delEpoll(int kdpfd, __uint32_t events);
			/// @brief	检测某种状态是否验证超时
			/// @param	state 待检测的状态
			/// @param	ct 当前系统时间
			/// @param	timeout 超时时间
			/// @return	检测是否成功
			bool checkStateTimeout(const eClientConnState state, const nTime &ct, const unsigned long timeout);
			/// @return	验证是否成功，1表示成功，可以进入下一步操作，0，表示还要继续等待验证，-1表示等待验证失败，需要断开连接
			int checkRebound();

			bool connectToServer();
			bool connectSocket();

			/// @brief	10分钟一次的探测指令检查
			void checkConn();
			/// @brief	向套接口发送指令
			/// @param	pstrCmd 待发送的指令
			/// @param	nCmdLen 待发送指令的大小
			/// @return	发送是否成功
			bool sendClientCmd(const void *nullCommand, const int len) const;
			/// @brief	从套接口中接受数据，并且拆包进行处理
			//		 	在调用这个函数之前保证已经对套接口进行了轮询
			/// @param	needRecv 是否需要真正从套接口接受数据
			//			false则不需要接收，只是处理缓冲中剩余的指令
			//			true需要实际接收数据，然后才处理
			/// @return	接收是否成功，true表示接收成功，false表示接收失败，可能需要断开连接 
			bool listeningRecv(bool needRecv);
			/// @brief	发送缓冲中的数据到套接口，再调用这个之前保证已经对套接口进行了轮询
			/// @return	发送是否成功，true表示发送成功，false表示发送失败，可能需要断开连接
			bool listeningSend();
			/// @brief	把TCP连接任务交给下一个任务队列，切换状态
			void getNextState();
			/// @brief	重值连接任务状态，回收连接
			void resetState();
			/// @brief	检查是否需要重连
			/// @return	是否加入
			bool needReConn();
			/// @brief	返回连接的端口
			/// @return	返回连接的端口
			const WORD getPort() const;
			/// @brief	返回连接的IP地址
			/// @return	连接的IP地址
			const char *getIP() const;
			/// @brief	得到socket地址
			/// @return	网罗顺序的socket地址
			DWORD getAddr() const;
			/// @brief	检查是否已经加入读事件
			/// @return	是否加入
			bool isReadEventRegged();
			/// @brief	设置加入读事件标志
			/// @param	set 是否加入
			/// @return	是否加入
			bool setReadEventRegged(bool isReadEventRegged);
			nServerConn* castAsServerConn() const;
			/// @brief	实现nTCPClientBase中的虚函数
			/// @param	ptNullCmd 指令
			/// @param	nCmdLen 指令长度
			/// @return	是否识别这个指令，true表示已经识别，false是不可识别
			virtual bool parseClientRawCmd(const void *packet, const DWORD len)=0;
			virtual bool recycleConn() = 0;

		public:
			/// @brief	测试信号已经返回了
			void clearTick()
			{
				_is_in_heart_tick = false;
			}

		protected:
			/// @brief	检查测试信号，如果测试信号在规定时间内返回，那么重新发送测试信号，没有返回的话可能TCP连接已经出错了
			/// @return	true，表示检测成功；false，表示检测失败 
			bool checkHeartTick() const;
            void setInHeartTick();
            bool sendRawDataIM(const void *data, const DWORD len) const;

		private:
			/// @brief	连接状态 
			volatile eClientConnState _state;
			nConnectServerData _connect_server_data;
			/// @brief	底层套接口 
			nSocket *_socket;
			/// @brief	读事件添加标志 
			bool _is_read_event_regged;
			/// @brief	生命期，记录每次状态改变的时间 
			nTime lifeTime;
			/// @brief	是否结束任务 
			eTerminateMethod terminate;

			/// @brief	是否已经进入主处理循环 
			volatile bool _is_in_main_loop;
			/// @brief	心跳定时器
			nTimer _tick_timer;
			/// @brief	定时检测套接口连接状况
			bool _is_in_heart_tick;
			nServerConn* const _server_conn;
	}; 

	class nClientConnNoQueue : public nClientConn, public nServerConnNoQueue
	{
		public :
			nClientConnNoQueue(const nConnectServerData &connectServerData);
			virtual ~nClientConnNoQueue();

		private:
			virtual bool sendCmd(const void *command, const DWORD len) const;
			virtual bool sendRawData(const void *command, const DWORD len) const;
			virtual bool parseClientRawCmd(const void *packet, const DWORD len);
			virtual bool recycleConn();
	};

	class nClientConnQueue : public nClientConn, public nServerConnQueue
	{
		public :
			nClientConnQueue(const nConnectServerData &connectServerData);
			virtual ~nClientConnQueue();

		private:
			virtual bool sendCmd(const void *command, const DWORD len) const;
			virtual bool sendRawData(const void *command, const DWORD len) const;
			virtual bool parseClientRawCmd(const void *packet, const DWORD len);
			virtual bool recycleConn();
	};
};

#endif
