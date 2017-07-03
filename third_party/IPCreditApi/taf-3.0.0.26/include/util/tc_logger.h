#ifndef __TC_LOGGER_H
#define __TC_LOGGER_H

#include <streambuf>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <time.h>
#include <sys/syscall.h>
#include <iomanip>
#include <ext/hash_map>
#include "util/tc_ex.h"
#include "util/tc_autoptr.h"
#include "util/tc_common.h"
#include "util/tc_file.h"
#include "util/tc_thread.h"
#include "util/tc_monitor.h"
#include "util/tc_thread_pool.h"
#include "util/tc_timeprovider.h"

using namespace std;
using namespace __gnu_cxx;

namespace taf
{
/////////////////////////////////////////////////
/**
 * @file tc_logger.h
 * @brief ��־��.
 *
 * ˵��:����չ����־��,�����Զ���������Ժ�д����.
 *
 *  �ṩ�����ַ�ʽ�Ĺ�����ʽ:
 *
 *  1 ���ļ���С,�ļ���Ŀ����;
 *
 *  2 ��ʱ�����(����/Сʱ/���ӵ�)
 *
 *   ��������Ĭ�ϵĲ�����,����Ĭ��д���ļ���,��û�г�ʼ��,��Ĭ��д�뵽cout
 *
 *  �Զ��������ʽ�Ĳ�������:
 *
 *  1 ʵ���Զ���ģ����,�̳���TC_LoggerRoll,ʵ��roll����,�ú���ʵ��д�߼�,
 *
 *  ����:template<typename WriteT>
 *
 *  class MyRoll : public TC_LoggerRoll, public TC_ThreadMutex
 *
 *  2 ģ���е�WriteT�Ǿ����д���߼�,�ṩȱʡ��:TC_DefaultWriteT
 *
 *  3 �����Զ���ʵ��WriteT, ����:ʵ�ַ��͵�������߼�.
 *
 *  4 ��MyRoll��ʵ���ڲ���RollWrapperI, ���̳�RollWrapperBase<MyRoll<WriteT> >
 *
 *  5 RollWrapperI��Ҫ�����ṩMyRoll�ĳ�ʼ���ӿ�, ��Щ�ӿ�����ֱ�ӷ�Ӧ��TC_Logger��.
 *
 *  6 RollWrapperI����ʱ, ����MyRoll����(��������ָ��),���ҽӿڵĲ���ֱ��ת����MyRoll������.
 *
 *  ʵ����Щ�߼���, ����ģ�巽ʽ���������:
 *
 *  typedef TC_Logger<MyWriteT, MyRoll> MyLogger;
 *
 *  TAF��Զ��logʵ������,���Բμ�src/libtaf/taf_logger.h
 *
 * @author : jarodruan@tencent.com,devinchen@tencent.com,skingfan@tencent.com
 */


/**
* @brief ��־�쳣��
*/
	struct TC_Logger_Exception : public TC_Exception
	{
		TC_Logger_Exception(const string &buffer) : TC_Exception(buffer){};
		TC_Logger_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
		~TC_Logger_Exception() throw(){};
	};

/**
 * @brief д����
 */
	class TC_DefaultWriteT
	{
	public:
		void operator()(ostream &of, const deque<pair<int, string> > &ds)
		{
			deque<pair<int, string> >::const_iterator it = ds.begin();
			while (it != ds.end())
			{
				of << it->second;
				++it;
			}
			of.flush();
		}
	};

	class TC_LoggerThreadGroup;

//////////////////////////////////////////////////////////////////////////////
/**
 * @brief ����д��־����
 */
	class TC_LoggerRoll : public TC_HandleBase
	{
	public:
		/**
		 * @brief ���캯��
		 */
		TC_LoggerRoll() : _pThreadGroup(NULL)
		{
		}

		/**
		 * @brief ʵʱ����־, ���ҹ���.
		 *
		 * ��ͬ�Ĳ�����־��,ʵ�ֲ�ͬ���߼�
		 * @param buffer
		 */
		virtual void roll(const deque<pair<int, string> > &ds) = 0;

		/**
		 * @brief ��װ�߳�.
		 *
		 * @param ltg
		 */
		void setupThread(TC_LoggerThreadGroup *pThreadGroup);

		/**
		 * @brief ȡ���߳�.
		 *
		 */
		void unSetupThread();

		/**
		 * @brief д����־.
		 *
		 * @param ��־����
		 */
		void write(const pair<int, string> &buffer);

		/**
		 * @brief ˢ�»��浽�ļ�
		 */
		void flush();

		/**
		 * @brief ����Ⱦɫ�Ƿ���Ч.
		 *
		 * @param bEnable
		 */
		void enableDyeing(bool bEnable, const string& sKey)
		{
			TC_LockT<TC_ThreadMutex> lock(_mutexDyeing);

			if (bEnable)
			{
				_mapThreadID.insert(std::make_pair(pthread_self(), sKey));
				//_setThreadID.insert(pthread_self());
			}
			else
			{
                _mapThreadID.erase(pthread_self());
				//_setThreadID.erase(pthread_self());
			}

			_bDyeingFlag = (_mapThreadID.size() > 0);
			//_bDyeingFlag = (_setThreadID.size() > 0);
		}

	protected:

		/**
		 * buffer
		 */
		TC_ThreadQueue<pair<int, string> >  _buffer;

		/**
		 * ��
		 */
		TC_ThreadMutex          _mutex;

		/**
		 * �߳���
		 */
		TC_LoggerThreadGroup    *_pThreadGroup;

		/**
		 * �Ƿ��Ѿ�Ⱦɫ�ı�־
		 */
		static bool 			_bDyeingFlag;

		/**
		 * Ⱦɫ����
		 *
		 * @author kevintian (2010-10-9)
		 */
		static TC_ThreadMutex	_mutexDyeing;

		/**
		 * Ⱦɫ���߳�ID����
		 *
		 * @author kevintian (2010-10-9)
		 */
		static hash_map<pthread_t, string> _mapThreadID;
	};

	typedef TC_AutoPtr<TC_LoggerRoll> TC_LoggerRollPtr;

//////////////////////////////////////////////////////////////////////////////
//
/**
 * @brief д��־�߳���.
 *
 * �ؼ���:ע����־��,�ᱣ��ְ��ָ��,��֤��־����һֱ����
 */
	class TC_LoggerThreadGroup : public TC_ThreadLock
	{
	public:
		/**
		 * @brief ���캯��
		 */
		TC_LoggerThreadGroup();

		/**
		 * @brief ��������
		 */
		~TC_LoggerThreadGroup();

		/**
		 * @brief �����߳�.
		 *
		 * @param iThreadNum�߳���
		 */
		void start(size_t iThreadNum);

		/**
		 * @brief ע��logger����.
		 *
		 * @param l TC_LoggerRollPtr����
		 */
		void registerLogger(TC_LoggerRollPtr &l);

		/**
		 * @brief ж��logger����.
		 *
		 * @param l TC_LoggerRollPtr����
		 */
		void unRegisterLogger(TC_LoggerRollPtr &l);

		/**
		 * @brief ˢ�����е�����
		 */
		void flush();

	protected:

		/**
		 * @brief д��־
		 */
		void run();

		/**
		 * @brief ָ��Ƚ�
		 */
		struct KeyComp
		{
			bool operator()(const TC_LoggerRollPtr& p1, const TC_LoggerRollPtr& p2) const
			{
				return p1.get() <  p2.get();
			}
		};

		typedef set<TC_LoggerRollPtr, KeyComp>  logger_set;

	protected:

		/**
		 * ����
		 */
		bool            _bTerminate;

		/**
		 * д�߳�
		 */
		TC_ThreadPool   _tpool;

		/**
		 * logger����
		 */
		logger_set      _logger;

	};

/**
 * @brief �Զ���logger buffer
 */
	class LoggerBuffer : public std::basic_streambuf<char>
	{
	public:
		/**
		 * @brief ��������buffer��С(10M)
		 */
		enum
		{
			MAX_BUFFER_LENGTH = 1024*1024*10,
		};

		/**
		 * @brief ���캯��
		 */
		LoggerBuffer();

		/**
		 * @brief ���캯��.
		 *
		 * @param roll        TC_LoggerRollPtr����
		 * @param buffer_len  buffer��С
		 */
		LoggerBuffer(TC_LoggerRollPtr roll, size_t buffer_len);

		/**
		 * @brief ��������
		 */
		~LoggerBuffer();

	protected:

		/**
		 * @brief ����ռ�.
		 *
		 * @param n
		 */
		void reserve(std::streamsize n);

		/**
		 * @brief ������.
		 *
		 * @param s
		 * @param n
		 *
		 * @return streamsize
		 */
		virtual streamsize xsputn(const char_type* s, streamsize n);

		/**
		 * @brief buffer����, ����д����.
		 *
		 * @param c
		 * @return int_type
		 */
		virtual int_type overflow(int_type c);

		/**
		 * @brief ���ռ�empty��(��ʵ��).
		 *
		 * @return int_type
		 */
		virtual int_type underflow()    { return std::char_traits<char_type>::eof();}

		/**
		 * @brief ����д�߼�.
		 *
		 * @return int
		 */
		virtual int sync();

	protected:
		LoggerBuffer(const LoggerBuffer&);
		LoggerBuffer& operator=(const LoggerBuffer&);

	protected:
		/**
		 * @brief д��־
		 */
		TC_LoggerRollPtr    _roll;

		/**
		 * ������
		 */
		char*                       _buffer;

		/**
		 * ��������С
		 */
		std::streamsize     _buffer_len;
	};

/**
 * @brief ��ʱ��, ������ʱ��д��־
 */
	class LoggerStream
	{
	public:
		/**
		 * @brief ����.
		 *
		 * @param stream
		 * @param mutex
		 */
		LoggerStream(ostream *stream, ostream *estream, TC_ThreadMutex &mutex) : _stream(stream), _estream(estream), _mutex(mutex)
		{
		}

		/**
		 * @brief ����
		 */
		~LoggerStream()
		{
			if (_stream)
			{
				_stream->flush();
				_mutex.unlock();
			}
		}

		/**
		* @brief ����<<
		*/
		template <typename P>
		LoggerStream& operator << (const P &t)  { if (_stream) *_stream << t;return *this;}

		/**
		 * @brief endl,flush�Ⱥ���
		 */
		typedef ostream& (*F)(ostream& os);
		LoggerStream& operator << (F f)         { if (_stream) (f)(*_stream);return *this;}

		/**
		 * @brief  hex��ϵ�к���
		 */
		typedef ios_base& (*I)(ios_base& os);
		LoggerStream& operator << (I f)         { if (_stream) (f)(*_stream);return *this;}

		/**
		 * @brief �ֶ�ת����ostream����.
		 *
		 * @return ostream&
		 */
		operator ostream&()
		{
			if (_stream)
			{
				return *_stream;
			}

			return *_estream;
		}

		//��ʵ��
		LoggerStream(const LoggerStream& lt);
		LoggerStream& operator=(const LoggerStream& lt);

	protected:

		/**
		 * �����
		 */
		std::ostream    *_stream;

		/**
		 *
		 */
		std::ostream    *_estream;

		/**
		 * ��
		 */
		TC_ThreadMutex  &_mutex;
	};

/**
 * @brief ��־����
 */
	template<typename WriteT, template<class> class RollPolicy>
	class TC_Logger : public RollPolicy<WriteT>::RollWrapperI
	{
	public:

		/**
		 * @brief ������ʾ����
		 */
		enum
		{
			HAS_TIME   = 0x01,	/**�뼶��ʱ��*/
			HAS_PID    = 0x02,	/**����ID*/
			HAS_LEVEL  = 0x04,	/**��־�ȼ�*/
			HAS_MTIME  = 0x08	/**���뼶��ʱ��(������ʹ��, ��Ӱ������)*/
		};

		/**
		* @brief ö������,������־�����ֵȼ� .
		*/
		enum
		{
			NONE_LOG    = 1,	/**���е�log����д*/
			ERROR_LOG   = 2,	/**д����log*/
			WARN_LOG    = 3,	/**д����,����log*/
			DEBUG_LOG   = 4,	/**д����,����,����log*/
			INFO_LOG    = 5		/**д����,����,����,Info log*/
		};

		/**
		 * @brief ��־��������
		 */
		static const string LN[6];

		/**
		 * @brief ���캯��
		 */
		TC_Logger()
		: _flag(HAS_TIME)
		, _level(DEBUG_LOG)
		, _buffer(TC_LoggerRollPtr::dynamicCast(this->_roll), 1024)
		, _stream(&_buffer)
		, _ebuffer(NULL, 0)
		, _estream(&_ebuffer)
		,_sSepar("|")
		,_bHasSquareBracket(false)
		{
		}

		/**
		 * @brief ��������
		 */
		~TC_Logger()
		{
		}

		/**
		 * @brief �޸ı���.
		 *
		 * @param flag
		 * @param add
		 */
		void modFlag(int flag, bool add = true)
		{
			if (add)
			{
				_flag |= flag;
			}
			else
			{
				_flag &= ~flag;
			}
		}

		/**
		 * @brief �Ƿ���ĳ��ʾ.
		 *
		 * @return bool
		 */
		bool hasFlag(int flag) const    { return _flag & flag;}

		/**
		 * @brief ��ȡflag
		 * @return flag��ֵ
		 */
		int getFlag() const             { return _flag;}

		/**
		* @brief ��ȡ��־�ȼ�.
		*
		* @return int �ȼ�
		*/
		int getLogLevel() const         { return _level;}

		/**
		* @brief ������־�ȼ�.
		*
		* @param level �ȼ�
		* @return      �ɹ����÷���0�����򷵻�-1
		*/
		int setLogLevel(int level)
		{
			if (!isLogLevelValid(level))
			{
				return -1;
			}

			_level = level;
			return 0;
		}

		/**
		 * @brief ���õȼ�.
		 *
		 * @param level
		 * @param int
		 */
		int setLogLevel(const string &level)
		{
			if (level == "ERROR")
			{
				return setLogLevel(ERROR_LOG);
			}
			else if (level == "WARN")
			{
				return setLogLevel(WARN_LOG);
			}
			else if (level == "DEBUG")
			{
				return setLogLevel(DEBUG_LOG);
			}
			else if (level == "NONE")
			{
				return setLogLevel(NONE_LOG);
			}
			else if (level == "INFO")
			{
				return setLogLevel(INFO_LOG);
			}
			return -1;
		}

		bool IsNeedLog(const string &level)
		{
			if (level == "ERROR")
			{
				return IsNeedLog(ERROR_LOG);
			}
			else if (level == "WARN")
			{
				return IsNeedLog(WARN_LOG);
			}
			else if (level == "DEBUG")
			{
				return IsNeedLog(DEBUG_LOG);
			}
			else if (level == "NONE")
			{
				return IsNeedLog(NONE_LOG);
			}
			else if (level == "INFO")
			{
				return IsNeedLog(INFO_LOG);
			}
			else
			{
				return true;
			}
		}
		bool IsNeedLog(int level)
		{
			return level <= _level;
		}
		/**
		 * @brief ��������ӵ���־����֮��ķָ����Ĭ����"|"
		 * @param str
		 */
		void setSeparator(const string& str) {_sSepar = str;}

		/**
		 * @brief ��������ں�ʱ��֮���Ƿ���Ҫ��������[],��Щͳ������������Ĭ�ϲ���
		 * @param bEnable
		 */
		void enableSqareWrapper(bool bEnable) {_bHasSquareBracket = bEnable;}

		/**
		* @brief DEBUG����־
		*/
		LoggerStream info()     { return stream(INFO_LOG);}

		/**
		* @brief DEBUG����־
		*/
		LoggerStream debug()    { return stream(DEBUG_LOG);}

		/**
		* @brief WARNING����־
		*/
		LoggerStream warn()     { return stream(WARN_LOG);}

		/**
		* @brief ERROR����־
		*/
		LoggerStream error()    { return stream(ERROR_LOG);}

		/**
		* @brief ��������־, ��ȼ��޹�
		*/
		LoggerStream any()      { return stream(0);}

	    LoggerStream log(int level) { return stream(level);}
	protected:

		/**
		 * @brief ��ȡͷ����Ϣ.
		 *
		 * @param c
		 * @param len
		 * @param level
		 */
		void head(char *c, int len, int level)
		{
			size_t n = 0;

			if (hasFlag(TC_Logger::HAS_MTIME))
			{
				struct timeval t;
				TC_TimeProvider::getInstance()->getNow(&t);
				//gettimeofday(&t, NULL);

				tm tt;
				localtime_r(&t.tv_sec, &tt);
				const char *szFormat = (_bHasSquareBracket)?("[%04d-%02d-%02d %02d:%02d:%02d.%03ld]%s"):("%04d-%02d-%02d %02d:%02d:%02d.%03ld%s");
				n += snprintf(c + n, len-n, szFormat,
							  tt.tm_year + 1900, tt.tm_mon+1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, t.tv_usec/1000,_sSepar.c_str());
			}
			else if (hasFlag(TC_Logger::HAS_TIME))
			{
				time_t t = TNOW;
				tm tt;
				localtime_r(&t, &tt);
				const char *szFormat = (_bHasSquareBracket)?("[%04d-%02d-%02d %02d:%02d:%02d]%s"):("%04d-%02d-%02d %02d:%02d:%02d%s");
				n += snprintf(c + n, len-n, szFormat,
							  tt.tm_year + 1900, tt.tm_mon+1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec,_sSepar.c_str());
			}

			if (hasFlag(TC_Logger::HAS_PID))
			{
				n += snprintf(c + n, len - n, "%ld%s", syscall(SYS_gettid), _sSepar.c_str());
			}

			if (hasFlag(TC_Logger::HAS_LEVEL))
			{
				n += snprintf(c + n, len - n, "%s%s", TC_Logger::LN[level].c_str(), _sSepar.c_str());
			}
		}

		/**
		 * @brief ��.
		 *
		 * @param level
		 * @return LoggerStream
		 */
		LoggerStream stream(int level)
		{
			ostream *ost = NULL;

			if (level <= _level)
			{
				char c[128] = "\0";
				head(c, sizeof(c) - 1, level);

				_mutex.lock();
				ost = &_stream;
				_stream.clear();
				_stream << c;
			}

			return LoggerStream(ost, &_estream, _mutex);
		}

		/**
		* @brief ���̵ȼ��Ƿ���Ч.
		*
		* @param level : ���̵ȼ�
		* @return bool,�ȼ��Ƿ���Ч
		*/
		bool isLogLevelValid(int level)
		{
			switch (level)
			{
			case NONE_LOG:
			case ERROR_LOG:
			case WARN_LOG:
			case DEBUG_LOG:
			case INFO_LOG:
				return true;
				break;
			}

			return false;
		}

	protected:
		/**
		 * ��ʾͷ
		 */
		int             _flag;

		/**
		* ��־��ߵȼ�
		*/
		int             _level;

		/**
		 * buffer
		 */
		LoggerBuffer    _buffer;

		/**
		 * logger��ʱ��
		 */
		std::ostream    _stream;

		/**
		 * ��buffer
		 */
		LoggerBuffer   _ebuffer;

		/**
		 * ����
		 */
		std::ostream   _estream;

		/**
		 * ��
		 */
		TC_ThreadMutex  _mutex;
		/**
		 * �ָ���
		 */
		 string        _sSepar;
		/**
		 * ���ڲ����Ƿ����[]
		 */
		 bool 		  _bHasSquareBracket;

	};

	template<typename WriteT, template<class> class RollPolicy>
	const string TC_Logger<WriteT, RollPolicy>::LN[6] = {"ANY", "NONE_LOG", "ERROR", "WARN", "DEBUG", "INFO"};

////////////////////////////////////////////////////////////////////////////////

	class RollWrapperInterface
	{
	public:

		virtual ~RollWrapperInterface() {}

		/**
		* @brief DEBUG����־
		*/
		virtual LoggerStream info() = 0;

		/**
		* @brief DEBUG����־
		*/
		virtual LoggerStream debug() = 0;

		/**
		* @brief WARNING����־
		*/
		virtual LoggerStream warn() = 0;

		/**
		* @brief ERROR����־
		*/
		virtual LoggerStream error() = 0;

		/**
		* @brief ��������־, ��ȼ��޹�
		*/
		virtual LoggerStream any() = 0;

		/**
		 *@brief ���յȼ��������־
		 */
		virtual LoggerStream log(int level)=0;
		/**
		 * @brief ������첽���ã������Ͻ���ˢ��
		 */
		virtual void flush() = 0;
	};


	template<typename RollPolicyWriteT>
	class RollWrapperBase : public RollWrapperInterface
	{
	public:

		typedef TC_AutoPtr<RollPolicyWriteT>     RollPolicyWriteTPtr;

		/**
		 * @brief ����
		 */
		RollWrapperBase()
		{
			_roll = new RollPolicyWriteT;
		}

		/**
		 * @brief ��װ�߳�.
		 *
		 * @param ltg
		 */
		void setupThread(TC_LoggerThreadGroup *ltg) { _roll->setupThread(ltg);}

		/**
		 * @brief ȡ���߳�
		 */
		void unSetupThread()                        { _roll->unSetupThread();}

		/**
		 * @brief ��ȡд����.
		 *
		 * @return WriteT&
		 */
		typename RollPolicyWriteT::T &getWriteT()   { return _roll->getWriteT();}

		/**
		 * @brief д��־.
		 *
		 * @param buffer
		 */
		void roll(const pair<int, string> &buffer)  { _roll->write(buffer);}

		/**
		* @brief ��ȡrollʵ��.
		 *
		 * @return RollPolicyWriteTPtr&
		 */
		RollPolicyWriteTPtr & getRoll()          	{return _roll;}

		/**
		 * @brief �첽ˢ��
		 */
		void flush()								{ _roll->flush(); }

	protected:

		/**
		 * @brief ����д��־������
		 */
		RollPolicyWriteTPtr      _roll;

	};

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief ��־��������, ������־��С����
 */
	template<typename WriteT>
	class TC_RollBySize : public TC_LoggerRoll, public TC_ThreadMutex
	{
	public:
		typedef WriteT T;

		/**
		 * @brief ��װ��(�ṩ�ӿ�)
		 */
		class RollWrapperI : public RollWrapperBase<TC_RollBySize<WriteT> >
		{
		public:
			/**
			 * @brief ��ʼ��.
			 *
			 * @param path
			 * @param iMaxSize, �ֽ�
			 * @param iMaxNum
			 */
			void init(const string &path, int iMaxSize = 5000000, int iMaxNum = 10)
			{
				this->_roll->init(path, iMaxSize, iMaxNum);
			}

			/**
			 * @brief ��ȡ��־·��.
			 *
			 * @return string
			 */
			string getPath()                    { return this->_roll->getPath();}

			/**
			 * @brief �����ļ�·��
			 */
			void setPath(const string &path)    { this->_roll->setPath(path);}

			/**
			 * @brief ��ȡ����С.
			 *
			 * @return int
			 */
			int getMaxSize()                    { return this->_roll->getMaxSize();}

			/**
			 * @brief ��������С.
			 *
			 * @param maxSize
			 */
			void setMaxSize(int maxSize)        { this->_roll->setMaxSize(maxSize);}

			/**
			 * @brief ��ȡ������.
			 *
			 * @return int
			 */
			int getMaxNum()                     { return this->_roll->getMaxNum();}

			/**
			 * @brief �����ļ�����.
			 *
			 * @param maxNum
			 */
			void setMaxNum(int maxNum)          { this->_roll->setMaxNum(maxNum);}
		};

		/**
		 * @brief ���캯��
		 */
		TC_RollBySize() : _maxSize(5000000),_maxNum(10),_iUpdateCount(0), _lt(time(NULL))
		{
		}

		/**
		 * @brief ����
		 */
		~TC_RollBySize()
		{
			if (_of.is_open())
			{
				_of.close();
			}
		}

		/**
		 * @brief ��ʼ��.
		 *
		 * @param path, �ļ���
		 * @param iMaxSize, ����С
		 * @param iMaxNum, ������
		 */
		void init(const string &path, int iMaxSize = 5000000, int iMaxNum = 10)
		{
			TC_LockT<TC_ThreadMutex> lock(*this);

			_path   = path;
			_maxSize= iMaxSize;
			_maxNum = iMaxNum;
		}

		/**
		 * @brief ��ȡ��־·��.
		 *
		 * @return string
		 */
		string getPath()                    { TC_LockT<TC_ThreadMutex> lock(*this); return _path;}

		/**
		 * @brief ����·��.
		 */
		void setPath(const string &path)    { TC_LockT<TC_ThreadMutex> lock(*this); _path = path;}

		/**
		 * @brief ��ȡ����С.
		 *
		 * @return int
		 */
		int getMaxSize()                    { TC_LockT<TC_ThreadMutex> lock(*this); return _maxSize;}

		/**
		 * @brief ��������С.
		 *
		 * @param maxSize
		 * @return void
		 */
		void setMaxSize(int maxSize)        { TC_LockT<TC_ThreadMutex> lock(*this); return _maxSize = maxSize;}

		/**
		 * @brief ��ȡ������.
		 *
		 * @return int
		 */
		int getMaxNum()                     { TC_LockT<TC_ThreadMutex> lock(*this); return _maxNum;}

		/**
		 * @brief ����������.
		 *
		 * @param maxNum
		 */
		void setMaxNum(int maxNum)          { TC_LockT<TC_ThreadMutex> lock(*this); return _maxNum = maxNum;}

		/**
		 * @brief ��ȡдʾ��.
		 *
		 * @return T&
		 */
		WriteT &getWriteT()                 { return _t;}

		/**
		 * ��������
		 *
		 * @param string
		 */
		void roll(const deque<pair<int, string> > &buffer)
		{
			TC_LockT<TC_ThreadMutex> lock(*this);

			if (_path.empty())
			{
				_t(cout, buffer);
				return;
			}

			time_t t = TNOW;
			time_t tt= t - _lt;
			//ÿ��5, ���´�һ���ļ�, �����ļ���ɾ���������ͷ�
			if (tt > 5 || tt < 0)
			{
				_lt = t;
				_of.close();
			}

			//�����־�ļ��Ƿ��
			if (!_of.is_open())
			{
				string sLogFileName = _path + ".log";
				_of.open(sLogFileName.c_str(), ios::app);

				string sLogFilePath = TC_File::extractFilePath(_path);

				if (!TC_File::isFileExist(sLogFilePath,S_IFDIR))
				{
					TC_File::makeDirRecursive(sLogFilePath);
				}

				if (!_of)
				{
					//���쳣ǰ��������_t �Ա��Զ����־
					_t(_of, buffer);

					throw TC_Logger_Exception("[TC_RollBySize::roll]:fopen fail: " + sLogFileName, errno);
				}

			}

			_t(_of, buffer);

			if (tt <= 5)
			{
				return;
			}

			//�ļ���СС������, ֱ�ӷ���
			if (_of.tellp() < _maxSize)
			{
				return;
			}

			//�ļ���С��������,ɾ�����һ���ļ�
			string sLogFileName = _path + TC_Common::tostr(_maxNum - 1) + ".log";
			if (access(sLogFileName.c_str(), F_OK) == 0)
			{
				if (remove(sLogFileName.c_str()) < 0 )
				{
					return;
				}
			}

			//��log�ļ�����shift, xxx1.log=>xxx2.log,��һ�ļ�����Ϊxxx.log
			for (int i = _maxNum-2; i >= 0; i--)
			{
				if (i == 0)
				{
					sLogFileName = _path + ".log";
				}
				else
				{
					sLogFileName = _path + TC_Common::tostr(i) + ".log";
				}

				if (access(sLogFileName.c_str(), F_OK) == 0)
				{
					string sNewLogFileName = _path + TC_Common::tostr(i + 1) + ".log";
					rename(sLogFileName.c_str(), sNewLogFileName.c_str());
				}
			}

			_of.close();
			_of.open(sLogFileName.c_str(), ios::app);
			if (!_of)
			{
				throw TC_Logger_Exception("[TC_RollBySize::roll]:fopen fail: " + sLogFileName, errno);
			}
		}

	protected:

		/**
		 * �ļ�·��
		 */
		string      _path;

		/**
		* ��־�ļ�������С
		*/
		int         _maxSize;

		/**
		* log�ļ������log�ļ�����Ϊ:xxx.log xxx1.log ...
		* xxx[_maxNum-1].log,_maxNum<=1 ?xxx.log
		*/
		int         _maxNum;

		/**
		 * ��־�ļ�
		 */
		ofstream    _of;

		/**
		 * ����д����
		 */
		WriteT      _t;

		/**
		 * �೤ʱ����һ���ļ���С
		 */
		short       _iUpdateCount;

		/**
		 * �೤ʱ����һ���ļ���С
		 */
		time_t      _lt;
	};

	typedef TC_Logger<TC_DefaultWriteT, TC_RollBySize> TC_RollLogger;

   /**
    * @brief ����ʱ�������־�ָ�����
    */
	class TafLogType:public TC_HandleBase
    {
	public:
		TafLogType():_next_time_t(TNOW),_format("%Y%m%d"),_next_cut_time(TC_Common::tm2str(TNOW, "%Y%m%d")),_frequency(1),_des("day"){}
		virtual ~TafLogType(){}
		//Ƶ��ֵ
		virtual size_t frequence()=0;

		virtual std::string get_init_time()
		{
			time_t tmp = _next_time_t - frequence();
			return TC_Common::tm2str(tmp, _format);
		}

		//���ؿձ�ʾû�е���һ����¼��
        virtual std::string get_new_time(time_t t)
		{
			if(t >= _next_time_t && TC_Common::tm2str(t,_format) >= _next_cut_time)
			{
			    //ʱ�����
				time_t new_nt = _next_time_t + ((t - _next_time_t) / frequence()) * frequence();
				_next_time_t = new_nt + frequence();
				_next_cut_time = TC_Common::tm2str(_next_time_t,_format);
	            return  TC_Common::tm2str(new_nt,_format);
			}
			else
			{
				return "";
			}
		}

		//ת������Ӧ���ַ�����ʽ:1day,1hour,2minute
        inline std::string& toString()
        {
			return _des;
		}
	protected:
		//������һ��ʱ���
        void init(const string &format = "%Y%m%d",size_t frequency=1)
		{
			_frequency = (frequency == 0?1:frequency);
			_format = format;

			_next_time_t = TNOW / frequence() + 1;
			_next_time_t *= frequence();

			_next_cut_time = TC_Common::tm2str(_next_time_t, _format);
		}
	protected:
		time_t _next_time_t;
		string _format;
        string _next_cut_time;
        size_t _frequency;
        string _des;
    };
    typedef TC_AutoPtr<TafLogType> TafLogTypePtr;

	class TafLogByDay:public TafLogType
	{
	public:
		static const string FORMAT;
	public:
		explicit TafLogByDay(const string &format = "%Y%m%d",size_t frequency=1)
        {
            init(format,frequency);
            _des = TC_Common::tostr(_frequency) + "day";
        }
	 	size_t frequence()
	 	{
			const size_t DAY_IN_SECOND = 60 * 60 * 24;
			return _frequency * DAY_IN_SECOND;
	 	}
	};

	class TafLogByHour:public TafLogType
	{
	public:
		static const string FORMAT;
	public:
		explicit TafLogByHour(const string &format = "%Y%m%d%H",size_t frequency=1)
        {
            init(format,frequency);
            _des = TC_Common::tostr(_frequency) + "hour";
        }

	 	size_t frequence()
	 	{
			const size_t HOUR_IN_SECOND = 60 * 60;
			return _frequency * HOUR_IN_SECOND;
	 	}
	};


	class TafLogByMinute:public TafLogType
	{
	public:
		static const string FORMAT;
 	public:
		explicit TafLogByMinute(const string &format = "%Y%m%d%H%M",size_t frequency=1)
        {
            init(format,frequency);
            _des = TC_Common::tostr(_frequency) + "minute";
        }
	 	size_t frequence()
	 	{
			const size_t MINUTE_IN_SECOND = 60;
			return _frequency * MINUTE_IN_SECOND;
	 	}
	};


   /**
    * @brief ����ʱ�������־
    */
	template<typename WriteT>
	class TC_RollByTime : public TC_LoggerRoll, public TC_ThreadMutex
	{
	public:

		typedef WriteT T;

		/**
		 * @brief ��װ��(�ӿ���)
		 */
		class RollWrapperI : public RollWrapperBase<TC_RollByTime<WriteT> >
		{
		public:

			/**
			 * @brief ��ʼ��.
			 *
			 * @param path,��־·��
			 * @param format����־�ļ���¼��ʽ�����죬Сʱ������
			 * @param bHasSufix,��־�ļ��Ƿ����".log"��׺
			 * @param sConcatstr,��־·����ʱ���ִ�֮������ӷ�,���磺app_log/test_20121210.log
			 * @param logTypePtr,��־��¼���ͣ����TafLogType
			 * @param bIsRemote,�Ƿ���Զ����־ʵ��
			 */
			void init(const string &path, const string &format = "%Y%m%d",bool bHasSufix=true,const string&sConcatstr="_",const TafLogTypePtr& logTypePtr=NULL,bool bIsRemote=false)
			{
				this->_roll->init(path, format,bHasSufix,sConcatstr,logTypePtr,bIsRemote);
			}

			/**
			 * @brief ��ȡ��־·��.
			 *
			 * @return string
			 */
			string getPath()                        { return this->_roll->getPath();}

			/**
			 * @brief �����ļ�·��
			 */
			void setPath(const string &path)        { this->_roll->setPath(path);}

			/**
			 * @brief ��ȡ��ʽ.
			 *
			 * @return string
			 */
			string getFormat()                      { return this->_roll->getFormat();}

			/**
			 * @brief ���ø�ʽ.
			 *
		 	 * @param format,֧�ְ��죬��Сʱ�������Ӹ�ʽ
			 */
			void setFormat(const string &format)    { this->_roll->setFormat(format);}
		};

		/**
		 * @brief ����
		 */
		TC_RollByTime() : _lt(TNOW),_logTypePtr(NULL),_bRemoteType(false)
		{
			_bHasSufix = true;
			_sConcatStr = "_";
		}

		/**
		 * @brief ����
		 */
		~TC_RollByTime()
		{
			if (_of.is_open())
			{
				_of.close();
			}
		}

		/**
		 * @brief ��ʼ��.
		 *
		 * @param path, �ļ�·��
		 * @param format, ��ʽ
		 * @param bHasSufix
		 * @param sConcatstr
		 * @param logTypePtr
		 * @param bIsRemote
		 */

		void init(const string &path, const string &format = "%Y%m%d",bool bHasSufix=true,const string &sConcatstr="_",const TafLogTypePtr& logTypePtr=NULL,bool bIsRemote=false)
		{
			TC_LockT<TC_ThreadMutex> lock(*this);

			_logTypePtr = logTypePtr;
			_path       = path;
			_format     = format;
			_bHasSufix = bHasSufix;
			_sConcatStr = sConcatstr;
			_bRemoteType = bIsRemote;

			if(_logTypePtr)
			{
				_currentTime = logTypePtr->get_init_time();
			}
			else
			{
				_currentTime = TC_Common::tm2str(TNOW, _format);
			}
		}

		/**
		 * @brief ��ȡ��־·��.
		 *
		 * @return string
		 */
		string getPath()                    { TC_LockT<TC_ThreadMutex> lock(*this); return _path;}

		/**
		 * @brief �����ļ�·��
		 */
		void setPath(const string &path)    { TC_LockT<TC_ThreadMutex> lock(*this); _path = path;}

		/**
		 * @brief ��ȡ��ʽ.
		 *
		 * @return string
		 */
		string getFormat()                  { TC_LockT<TC_ThreadMutex> lock(*this); return _format;}

		/**
		 * @brief ���ø�ʽ.
		 *
		 * @param format,֧�ְ��죬��Сʱ�������Ӹ�ʽ
		 */
		void setFormat(const string &format){ TC_LockT<TC_ThreadMutex> lock(*this); _format = format;}
		/**
		 * @brief ��ȡдʾ��.
		 *
		 * @return WriteT&
		 */
		WriteT &getWriteT()                 { return _t;}

		/**
		 * @brief ��������.
		 *
		 * @param path
		 * @param of
		 * @return string
		 */
		void roll(const deque<pair<int, string> > &buffer)
		{
			TC_LockT<TC_ThreadMutex> lock(*this);

			if (_path.empty())
			{
				_t(cout, buffer);
				return;
			}
			//Զ����־�ڱ��ز��ô��ļ�
			if(_bRemoteType)
			{
				_t(_of, buffer);
				return;
			}

			time_t t = TNOW;

			string nowTime = "";
			//��������˼�¼����,��ʹ�ü�¼����������ʱ��
			if(_logTypePtr)
			{
				nowTime = _logTypePtr->get_new_time(t);
				//ʱ���Խ��
				if(nowTime != "")
				{
					_currentTime = nowTime;
					_of.close();
				}
				else
				{
					nowTime = _currentTime;
				}
			}
			else
			{
				nowTime = TC_Common::tm2str(t, _format);
				//���ʱ���Ƿ��ʱ����, ��ʱ���ر��ļ�
				if (_currentTime != nowTime)
				{
					_currentTime = nowTime;
					_of.close();
				}
			}

			//ÿ��10s, ���´�һ���ļ�, �����ļ���ɾ���������ͷ�
			if (t - _lt > 10 || t - _lt < 0)
			{
				_lt = t;
				_of.close();
			}

			if (!_of.is_open())
			{
				string sLogFileName = _path + _sConcatStr + nowTime + (_bHasSufix?".log":"");
				string sLogFilePath = TC_File::extractFilePath(_path);
				if (!TC_File::isFileExist(sLogFilePath,S_IFDIR))
				{
					TC_File::makeDirRecursive(sLogFilePath);
				}

				_of.open(sLogFileName.c_str(), ios::app);

				if (!_of)
				{
					//���쳣ǰ��������_t �Ա��Զ����־
					_t(_of, buffer);
					throw TC_Logger_Exception("[TC_RollByTime::roll]:fopen fail: " + sLogFileName, errno);
				}
			}

			//д
			_t(_of, buffer);
		}

	protected:
		/**
		 * �ļ�·��
		 */
		string      _path;

		/**
		 * ʱ���ʽ
		 */
		string      _format;

		/**
		 * �ϴ�rollʱ��ʱ��
		 */
		string      _currentTime;

		/**
		 * ��־�ļ�
		 */
		ofstream    _of;

		/**
		 * ����д����
		 */
		WriteT      _t;

		/**
		 * �೤ʱ����һ���ļ���С
		 */
		time_t      _lt;
		/**
		 * ��־�ļ����Ƿ��.log��׺
		 */
		bool					_bHasSufix;
		/**
		 * ��־�ļ������û��Զ����ַ��������ַ�������ӷ���Ĭ����"_"
		 */
		string					_sConcatStr;


		/**
		 * ����/Сʱ/���������־ʱ�ļ�¼����
		 */

		TafLogTypePtr     _logTypePtr;
		/**
		 * �Ƿ���Զ����־ʵ��
		 */
        bool              _bRemoteType;
	};

	typedef TC_Logger<TC_DefaultWriteT, TC_RollByTime> TC_DayLogger;

}

#endif

