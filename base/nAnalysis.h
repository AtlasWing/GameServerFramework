#ifndef _NANALYSIS_H_
#define _NANALYSIS_H_

#include "base/nMutex.h"
#include "base/nTimer.h"

namespace Tidy
{
	/// @brief	统计函数单次进入花费时间
	struct FunctionTime
	{
		public:
			/// @brief	构造函数
			/// @param	interval	输出日志的最小间隔时间
			/// @param	func		输出日志的函数名称
			/// @param	dis		描述
			/// @param	dis_len	dis可输出的最大长度
			/// @return	
			FunctionTime(const long interval,const char *func=NULL,const char *dis=NULL,const int dis_len=16)
				:_need_log(interval),_fun_name(func),_dis(dis),_dis_len(dis_len)
			{
				clock_gettime(CLOCK_REALTIME, &_tv_1);
			}
			~FunctionTime();

		private:
			/// @brief	调用前时间
			struct timespec _tv_1;
			/// @brief	调用后时间
			struct timespec _tv_2;
			/// @brief	输出日志的时间间隔
			const long _need_log;
			/// @brief	函数名称
			const char *_fun_name;
			/// @brief	描述
			const char *_dis;
			/// @brief	描述内容的长度
			const int _dis_len;
	};

	/// @brief		统计一个函数在指定时间内的调用次数
	//			为了提高效率每个调用需要自己加入一个key值
	//			不能嵌套使用
	struct FunctionTimes
	{
		public:
			/// @brief	构造函数
			/// @param	which 给定的key
			/// @param	dis	描述
			/// @return	
			FunctionTimes(const int which , const char *dis);
			~FunctionTimes();
			/// @brief	时间结构
			struct Times
			{
				/// @brief	构造函数
				Times():_log_timer(600 * 1000L),_times(0),_total_time(0)
				{
					bzero(_dis,sizeof(_dis));
				}
				/// @brief	输出日志的时间间隔
				nTimer _log_timer;
				/// @brief	描述
				char _dis[256];
				/// @brief	掉头用次数
				int _times;
				/// @brief	使用总时间
				int _total_time;
				/// @brief	锁
				nMutex _mutex;
			};
		private:
			/// @brief	字典
			static Times _times[256]; 
			/// @brief	key值
			int _which;
			/// @brief	起始时间
			struct timespec _tv_1;
			/// @brief	结束时间
			struct timespec _tv_2;
	};

	/// @brief	函数的调用时间间隔
	struct FunctionInterval
	{
		public:
			/// @brief	构造函数
			/// @param	interval	间隔时间
			/// @return	
			explicit FunctionInterval(const long interval)
				:_need_log(interval)
			{
				_tv_1.tv_sec=-1;
				_tv_1.tv_nsec=-1;
			}
			/// @brief	执行函数
			/// @param	func	函数名称
			/// @return	
			void interval(const char *func=NULL);
		public:
			/// @brief	起始时间
			struct timespec _tv_1;
			/// @brief	结束时间
			struct timespec _tv_2;
			/// @brief	日志输出间隔
			const long _need_log;
			/// @brief	函数名称
			const char *_fun_name;
	};

	/// @brief	指令统计分析
	struct CmdAnalysis
	{
		/// @brief	构造函数
		CmdAnalysis(const char *disc,const long time_secs):_log_timer(time_secs * 1000L)
		{
			bzero(_disc,sizeof(disc));
			strncpy(_disc,disc,sizeof(_disc)-1);
			bzero(_data,sizeof(_data));
			_switch=false;
		}
		/// @brief	指令数组
		struct  
		{
			DWORD num;
			DWORD size;
		}_data[256][256] ;
		/// @brief	锁
		nMutex _mutex; 
		/// @brief	输出时间
		nTimer _log_timer;
		/// @brief	描述
		char _disc[256];
		/// @brief	打开日志开关
		bool _switch;
		/// @brief	添加一个指令
		/// @param	cmd 一级指令
		/// @param	para 二级指令
		/// @param	size 指令大小
		void add(const BYTE &cmd, const BYTE &para , const DWORD &size);
		/// @brief	结束
		void final();
	};
};
#endif

