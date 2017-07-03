#ifndef BASE_N_OBJ_LOGGER_H_
#define BASE_N_OBJ_LOGGER_H_

#include "base/nLogger.h"

namespace Tidy
{
	//模板类需要提供getLogger, getEntityLogHead
	template<typename ObjT>
		class nObjLogger
		{
			protected:
				nObjLogger()
				{
				}
				~nObjLogger()
				{
				}

			public:
				void debug(const char * pattern, ...) const __attribute__((format(printf,2,3)));
				void trace(const char * pattern, ...) const __attribute__((format(printf,2,3)));
				void info(const char * pattern, ...) const __attribute__((format(printf,2,3)));
				void warn(const char * pattern, ...) const __attribute__((format(printf,2,3)));
				void error(const char * pattern, ...) const __attribute__((format(printf,2,3)));
				void fatal(const char * pattern, ...) const __attribute__((format(printf,2,3)));
				void longDebug(const char * pattern, ...) const __attribute__((format(printf,2,3)));
		};

	/**------------------------------------------------------------------------------**/
	template<typename ObjT>
		void nObjLogger<ObjT>::debug(const char *pattern, ...) const
		{
			nLogger *logger = static_cast<const ObjT*>(this)->getLogger();
			if(logger->checkLoggerLevel(eLoggerLevel_Debug) == false)
			{
				return;
			}
			char buffer[MSGBUF_MAX];
			DWORD len = 0;
			len += logger->getContentHeader(&buffer[len], sizeof(buffer) - len, eLoggerLevel_Debug);
			len += (static_cast<ObjT*>(const_cast<nObjLogger<ObjT>*>(this)))->getEntityLogHead(&buffer[len], sizeof(buffer) - len);
			getMessage(&buffer[len], sizeof(buffer) - len, pattern);
			len += msgLen;
			logger->writeLog(buffer, len);
		}

	template<typename ObjT>
		void nObjLogger<ObjT>::warn(const char *pattern, ...) const
		{
			nLogger *logger = static_cast<const ObjT*>(this)->getLogger();
			if(logger->checkLoggerLevel(eLoggerLevel_Warn) == false)
			{
				return;
			}
			char buffer[MSGBUF_MAX];
			DWORD len = 0;
			len += logger->getContentHeader(&buffer[len], sizeof(buffer) - len, eLoggerLevel_Warn);
			len += (static_cast<ObjT*>(const_cast<nObjLogger<ObjT>*>(this)))->getEntityLogHead(&buffer[len], sizeof(buffer) - len);
			getMessage(&buffer[len], sizeof(buffer) - len, pattern);
			len += msgLen;
			logger->writeLog(buffer, len);
		}

	template<typename ObjT>
		void nObjLogger<ObjT>::info(const char *pattern, ...) const
		{
			nLogger *logger = static_cast<const ObjT*>(this)->getLogger();
			if(logger->checkLoggerLevel(eLoggerLevel_Info) == false)
			{
				return;
			}
			char buffer[MSGBUF_MAX];
			DWORD len = 0;
			len += logger->getContentHeader(&buffer[len], sizeof(buffer) - len, eLoggerLevel_Info);
			len += (static_cast<ObjT*>(const_cast<nObjLogger<ObjT>*>(this)))->getEntityLogHead(&buffer[len], sizeof(buffer) - len);
			getMessage(&buffer[len], sizeof(buffer) - len, pattern);
			len += msgLen;
			logger->writeLog(buffer, len);
		}

	template<typename ObjT>
		void nObjLogger<ObjT>::fatal(const char *pattern, ...) const
		{
			nLogger *logger = static_cast<const ObjT*>(this)->getLogger();
			if(logger->checkLoggerLevel(eLoggerLevel_Fatal) == false)
			{
				return;
			}
			char buffer[MSGBUF_MAX];
			DWORD len = 0;
			len += logger->getContentHeader(&buffer[len], sizeof(buffer) - len, eLoggerLevel_Fatal);
			len += (static_cast<ObjT*>(const_cast<nObjLogger<ObjT>*>(this)))->getEntityLogHead(&buffer[len], sizeof(buffer) - len);
			getMessage(&buffer[len], sizeof(buffer) - len, pattern);
			len += msgLen;
			logger->writeLog(buffer, len);
		}

	template<typename ObjT>
		void nObjLogger<ObjT>::error(const char *pattern, ...) const
		{
			nLogger *logger = static_cast<const ObjT*>(this)->getLogger();
			if(logger->checkLoggerLevel(eLoggerLevel_Error) == false)
			{
				return;
			}
			char buffer[MSGBUF_MAX];
			DWORD len = 0;
			len += logger->getContentHeader(&buffer[len], sizeof(buffer) - len, eLoggerLevel_Error);
			len += (static_cast<ObjT*>(const_cast<nObjLogger<ObjT>*>(this)))->getEntityLogHead(&buffer[len], sizeof(buffer) - len);
			getMessage(&buffer[len], sizeof(buffer) - len, pattern);
			len += msgLen;
			logger->writeLog(buffer, len);
		}

	template<typename ObjT>
		void nObjLogger<ObjT>::trace(const char *pattern, ...) const
		{
			nLogger *logger = static_cast<const ObjT*>(this)->getLogger();
			if(logger->checkLoggerLevel(eLoggerLevel_Trace) == false)
			{
				return;
			}
			char buffer[MSGBUF_MAX];
			DWORD len = 0;
			len += logger->getContentHeader(&buffer[len], sizeof(buffer) - len, eLoggerLevel_Trace);
			len += (static_cast<ObjT*>(const_cast<nObjLogger<ObjT>*>(this)))->getEntityLogHead(&buffer[len], sizeof(buffer) - len);
			getMessage(&buffer[len], sizeof(buffer) - len, pattern);
			len += msgLen;
			logger->writeLog(buffer, len);
		}

	template<typename ObjT>
		void nObjLogger<ObjT>::longDebug(const char *pattern, ...) const
		{
			nLogger *logger = static_cast<const ObjT*>(this)->getLogger();
			if(logger->checkLoggerLevel(eLoggerLevel_Debug) == false)
			{
				return;
			}
			char buffer[MSGBUF_MAX * 100];
			DWORD len = 0;
			len += logger->getContentHeader(&buffer[len], sizeof(buffer) - len, eLoggerLevel_Debug);
			len += (static_cast<ObjT*>(const_cast<nObjLogger<ObjT>*>(this)))->getEntityLogHead(&buffer[len], sizeof(buffer) - len);
			getMessage(&buffer[len], sizeof(buffer) - len, pattern);
			len += msgLen;
			logger->writeLog(buffer, len);
		}
};
#endif
