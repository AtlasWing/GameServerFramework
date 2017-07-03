#include "base/nZlib.h"
#include "base/nAssert.h"

namespace Tidy 
{
	int ZLIB_COMPRESS_LEVEL		=	4;
    voidpf zcalloc(voidpf opaque, unsigned items, unsigned size)
    {
        return (voidpf)::malloc(items * size);
    } 

    void  zcfree(voidpf opaque, voidpf ptr)
    {
        ::free(ptr);
    }

    int zlib_compress(Bytef *dest,uLongf *destLen,const Bytef *source,unsigned long sourceLen)
    {
        return zlib_compress2(dest, destLen, source, sourceLen, Z_DEFAULT_COMPRESSION);
    }

	__thread z_stream *deflateStream = NULL;
	__thread int CURR_ZLIB_COMPRESS_LEVEL = 0;
	bool initDeflateStream()
	{
		int err = Z_OK;
		if(deflateStream != NULL){
			if(CURR_ZLIB_COMPRESS_LEVEL == ZLIB_COMPRESS_LEVEL){
				return true;
			}
			else{
				// 压缩等级改变
				int err = deflateEnd(deflateStream);
				assert_debug(err == Z_OK, "压缩流结束失败[%d]", err);
				SAFE_DELETE(deflateStream);
			}
		}
		CURR_ZLIB_COMPRESS_LEVEL = ZLIB_COMPRESS_LEVEL;
		deflateStream = new z_stream();
		deflateStream->zalloc = (alloc_func)zcalloc;
		deflateStream->zfree = (free_func)zcfree;
		deflateStream->opaque = (voidpf)0;
		deflateStream->data_type = Z_BINARY;

		// 4比默认的压缩等级快,压缩比稍有损失
		err = deflateInit2(deflateStream, CURR_ZLIB_COMPRESS_LEVEL, Z_DEFLATED, MAX_WBITS, MAX_MEM_LEVEL, Z_FILTERED);
		return err == Z_OK;
	}

    int zlib_compress2(Bytef *dest,uLongf *destLen,const Bytef *source,unsigned long sourceLen, int level)
    {
		if(initDeflateStream() == false){
			assert_fail("初始化失败");
			return Z_ERRNO;
		}
        int err;

        deflateStream->next_in = (Bytef*)source;
        deflateStream->avail_in = (uInt)sourceLen;
#ifdef MAXSEG_64K
        /* Check for source > 64K on 16-bit machine: */
        if ((uLong)deflateStream->avail_in != sourceLen) return Z_BUF_ERROR;
#endif
        deflateStream->next_out = dest;
        deflateStream->avail_out = (uInt)*destLen;
        if ((uLong)deflateStream->avail_out != *destLen) return Z_BUF_ERROR;

        err = deflate(deflateStream, Z_FINISH);
        if (err != Z_STREAM_END) {
            return err == Z_OK ? Z_BUF_ERROR : err;
        }
        *destLen = deflateStream->total_out;
		err = deflateReset(deflateStream);
		assert_debug(err == Z_OK, "[%d]", err);

        return err;
    }

	__thread z_stream *inflateStream = NULL;
	bool initInflateStream()
	{
		if(inflateStream != NULL){
			return true;
		}
		int err;
		inflateStream = new z_stream();
		inflateStream->zalloc = (alloc_func)zcalloc;
		inflateStream->zfree = (free_func)zcfree;
		inflateStream->opaque = (voidpf)0;
		inflateStream->data_type = Z_BINARY;

		err = inflateInit(inflateStream);
		return err == Z_OK;
	}
    int zlib_uncompress(Bytef *dest,uLongf *destLen,const Bytef *source,uLong sourceLen)
    {
		if(initInflateStream() == false){
			assert_fail("初始化失败");
			return Z_ERRNO;
		}
        int err;

        inflateStream->next_in = (Bytef*)source;
        inflateStream->avail_in = (uInt)sourceLen;
        /* Check for source > 64K on 16-bit machine: */
        if ((uLong)inflateStream->avail_in != sourceLen) return Z_BUF_ERROR;

        inflateStream->next_out = dest;
        inflateStream->avail_out = (uInt)*destLen;
        if ((uLong)inflateStream->avail_out != *destLen) return Z_BUF_ERROR;

        err = inflate(inflateStream, Z_FINISH);
        if (err != Z_STREAM_END) {
            return err == Z_OK ? Z_BUF_ERROR : err;
        }
        *destLen = inflateStream->total_out;
		err = inflateReset(inflateStream);
		assert_debug(err == Z_OK, "[%d]", err);
        return err;
    }
};//end namespace Tidy
