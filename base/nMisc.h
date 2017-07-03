#ifndef BASE_N_MISC_H_
#define BASE_N_MISC_H_

#include "nConstructInPlace.h"

/// @brief Proto消息最大长度
extern const DWORD MAX_PROTO_CMD_LEN;
/// @brief 大消息Proto最大长度,用于数据量大的消息逻辑
extern const DWORD MAX_BIG_PROTO_CMD_LEN;
/// @brief User消息最大长度
extern const DWORD MAX_USER_CMD_LEN;

/// @brief 声明变长指令
#define BUFFER_DATA(cmd,name,len) char buffer##name[len];cmd *name=(cmd *)buffer##name;Tidy::constructInPlace(name);

/// @brief Proto指令
#define BUFFER_PROTO_CMD(cmd,name) BUFFER_DATA(cmd,name,MAX_PROTO_CMD_LEN)

/// @brief	Proto指令数据空间容量
#ifdef _DEBUG
    #define PROTO_CMD_CAPACITY(TYPE) ({TYPE temp_##type;(MAX_PROTO_CMD_LEN - sizeof(temp_##type) - 1);})
#else
    #define PROTO_CMD_CAPACITY(TYPE) ({(MAX_PROTO_CMD_LEN - sizeof(TYPE) - 1);})
#endif

/// @brief Proto指令
#define BUFFER_BIG_PROTO_CMD(cmd,name) BUFFER_DATA(cmd,name,MAX_BIG_PROTO_CMD_LEN)

/// @brief	大消息Proto数据空间容量
#ifdef _DEBUG
    #define BIG_PROTO_CMD_CAPACITY(TYPE) ({TYPE temp_##type;(MAX_BIG_PROTO_CMD_LEN - sizeof(temp_##type) - 1);})
#else
    #define BIG_PROTO_CMD_CAPACITY(TYPE) ({(MAX_BIG_PROTO_CMD_LEN - sizeof(TYPE) - 1);})
#endif

/// @brief User指令
#define BUFFER_USER_CMD(cmd,name) BUFFER_DATA(cmd,name,MAX_USER_CMD_LEN)

/// @brief	User指令数据空间容量
#ifdef _DEBUG
    #define USER_CMD_CAPACITY(TYPE) ({TYPE temp_##type;(MAX_USER_CMD_LEN - sizeof(temp_##type) - 1);})
#else
    #define USER_CMD_CAPACITY(TYPE) ({(MAX_USER_CMD_LEN - sizeof(TYPE) - 1);})
#endif

#define FMTString(msg,msglen,pat)	\
	do	\
{	\
	va_list ap;	\
	va_start(ap, pat);		\
	vsnprintf(msg, msglen - 1, pat, ap);	\
	va_end(ap);	\
}while(false)

#define FormatString(msg, msglen, pat, resultLen)	\
	do	\
{	\
	va_list ap;	\
	va_start(ap, pat);		\
	int _msg_len__ = vsnprintf(msg, ((msglen) - 1), pat, ap) + 1;\
	if(_msg_len__ >= msglen){ \
		_msg_len__ = msglen; \
		(msg)[((msglen) - 1)] = 0; \
	}\
	resultLen = resultLen + _msg_len__;	\
	va_end(ap);	\
}while(false)

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);               \
void operator=(const TypeName&)

// 在缓冲区空间不足时snprintf的返回值>=bufferSize.这里改成最大为bufferSize,避免在使用+=时的错误
#undef snprintf
#define snprintf(buffer, bufferSize, ...)\
	({\
		DWORD ____local___len = snprintf((buffer), (bufferSize), __VA_ARGS__);\
		if(____local___len > (bufferSize)){\
			____local___len = (bufferSize);\
		}\
		____local___len;\
	})

#endif
