#ifndef _N_ZLIB_H_
#define _N_ZLIB_H_

#include "base/nSys.h"

namespace Tidy 
{
	// 压缩的等级
	extern int ZLIB_COMPRESS_LEVEL;
	// 当前线程的压缩等级
	extern __thread int CURR_ZLIB_COMPRESS_LEVEL;
    /// @brief	带内存池的压缩接口
    /// @param	dest 输出
    /// @param	destLen	输出缓冲区长度
    /// @param	source 输入
    /// @param	srcLen	输入缓冲区长度
    /// @return	返回码
    int zlib_compress(Bytef *dest,uLongf *destLen,const Bytef *source,unsigned long srclen);
    /// @brief	带内存池的压缩接口
    /// @param	dest 输出
    /// @param	destLen	输出缓冲区长度
    /// @param	source 输入
    /// @param	srcLen	输入缓冲区长度
    /// @param	level	压缩级别
    /// @return	返回码
    int zlib_compress2(Bytef *dest,uLongf *destLen,const Bytef *source,unsigned long srclen,int level);
    /// @brief	带内存池的解压接口
    /// @param	dest 输出
    /// @param	destLen	输出缓冲区长度
    /// @param	source 输入
    /// @param	srcLen	输入缓冲区长度
    /// @return	返回码
    int zlib_uncompress(Bytef *dest,uLongf *destLen,const Bytef *source,uLong sourceLen);
}//end namespace Tidy
#endif
