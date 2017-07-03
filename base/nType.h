#ifndef _BASE_N_TYPE_H_
#define _BASE_N_TYPE_H_

typedef unsigned char		BYTE;
typedef unsigned short		WORD;
typedef signed short		SWORD;
#ifndef _MSC_VER
typedef unsigned int		DWORD;
#endif
typedef signed int			SDWORD;
typedef unsigned long long	QWORD;
typedef signed long long	SQWORD;

#ifndef MAX_NAMESIZE
#define MAX_NAMESIZE 32
typedef char NameStr[MAX_NAMESIZE];
#endif

#ifndef MAX_IP_LENGTH
#define MAX_IP_LENGTH 16
#endif

/// @brief 安全删除一个指针
#ifndef SAFE_DELETE
#define SAFE_DELETE(x)		{ if (x) { delete (x); (x) = NULL; } }
#endif

#ifndef SAFE_DELETE_VEC
/// @brief 安全删除一个指针数组
#define SAFE_DELETE_VEC(x)	{ if (x) { delete [] (x); (x) = NULL; } }
#endif

/// @brief	事件参数
struct EventParam
{
	union
	{
		BYTE bParam[8];
		WORD wParam[4];
		SWORD swParam[4];
		DWORD dwParam[2];
		SDWORD sdwParam[2];
		QWORD qwParam;
		SQWORD sqwParam;
        const void* dataPtr;
	};
    void *extendData[4];
};

template<typename ValueT>
struct Array{
    Array(ValueT *valueList, const DWORD num) : data(valueList), size(num){
    }
    ValueT *data;
    DWORD  size;
};
inline DWORD copyStr(char* dest, const char* src, DWORD num)
{
	// 返回长度已经包括'\0'结尾符.保证dest字符串以'\0'结尾
	DWORD i = 0;
	while(i < num && src[i] != '\0') {
		dest[i] = src[i];
		++ i;
	}
	if(i == num) {
		dest[num - 1] = '\0';
		return num;
	}
	else {
		dest[i] = '\0';
	}

	return i + 1;
}

#endif
