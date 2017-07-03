#ifndef BASE_NBYTEBUFFER_H_
#define BASE_NBYTEBUFFER_H_

#include "base/nMutex.h"
#include "base/nSTL.h"
#include "base/nAssert.h"

namespace Tidy {
	/// @brief	内存块单位大小
	//const unsigned int trunkSize = 64 * 1024 * 16;
	const unsigned int trunkSize = 64 * 1024;
	/// @brief	压缩需要比原缓冲大的缓冲区
	/// @param	zip_size	需要压缩的数据大小
	/// @return	
#define unzip_size(zip_size) ((zip_size) * 1.2 + 12)
	/// @brief	压缩单位内存块所需要的内存
	const unsigned int PACKET_ZIP_BUFFER = static_cast<DWORD>(unzip_size(trunkSize - 1)) + sizeof(unsigned int) + 8;	///< 压缩需要的缓冲 

	/// @brief	字节缓冲，用于套接口接收和发送数据的缓冲
	template <typename BufferT>
		class nByteBuffer {
			public: 
				nByteBuffer();
				~nByteBuffer();

			public:
				/// @brief	向缓冲填入数据
				/// @param	buf 待填入缓冲的数据
				/// @param	size 待填入缓冲数据的长度
				inline bool put(const unsigned char *buf, const unsigned int size);
				/// @brief	得到当前可写bf的未知
				// 		保证在调用此函数写入数据之前需要调用wr_reserve(size)来预留缓冲区大小
				/// @return	可写入缓冲开始地址
				inline char *wr_buf();
				/// @brief	返回缓冲中有效数据的开始地址
				/// @return	有效数据地址
				inline const unsigned char *rd_buf() const;
				/// @brief	判断缓冲中时候有有效数据
				/// @return	返回缓冲中是否有有效数据
				inline bool rd_ready() const;
				/// @brief	得到缓冲中有效数据的大小
				/// @return	返回缓冲中有效数据大小
				inline unsigned int rd_size() const;
				/// @brief	当缓冲的有效数据被使用以后，需要对缓冲进行整理
				/// @param	size 最后一次使用的有效数据长度
				inline void rd_flip(unsigned int size);
				/// @brief	得到缓冲可写入数据的大小
				/// @return	可写入数据的大小
				inline unsigned int wr_size() const;
				/// @brief	实际向缓冲写入了数据，需要对缓冲进行整理
				/// @param	size 实际写入的数据
				inline void wr_flip(const unsigned int size);
				// 重值缓冲中的数据，清空无用的垃圾数据
				inline void reset();
				/// @brief	返回缓冲最大大小
				/// @return	缓冲最大大小
				inline unsigned int maxSize() const;
				/// @brief	对缓冲的内存进行重新整理，向缓冲写数据，如果缓冲大小不足，重新调整缓冲大小，
				//			大小调整原则按照trunkSize的整数倍进行增加
				/// @param	size 向缓冲写入了多少数据
				bool wr_reserve(const unsigned int size);

			private:
				/// @brief	缓冲区的大小
				unsigned int _max_size;
				/// @brief	当前可读缓冲的有效地址
				unsigned int _offPtr;
				/// @brief	当前可写缓冲的有效地址
				unsigned int _currPtr;
				BufferT _buffer_data;
		};

	// 动态内存的缓冲区，可以动态扩展缓冲区大小
	typedef nByteBuffer<unsigned char *> BufferCmdQueue;
	typedef nByteBuffer<unsigned char *> RawDataBuffer;

	/// @brief	静态大小的缓冲区
	// 		以栈空间数组的方式来分配内存,用于一些临时变量的获取
	typedef nByteBuffer<unsigned char [PACKET_ZIP_BUFFER]> StackCmdQueue;

	template <typename BufferT>
		bool nByteBuffer<BufferT>::put(const unsigned char *buf, const unsigned int size)
		{
			//首先确认缓冲内存是否足够
			if(wr_reserve(size) == false){
				return false;
			}
			bcopy(buf, &_buffer_data[_currPtr], size);
			_currPtr += size;
			return true;
		}

	template <typename BufferT>
		inline char *nByteBuffer<BufferT>::wr_buf()
		{
			return (char*)&_buffer_data[_currPtr];
		}

	template <typename BufferT>
		inline const unsigned char *nByteBuffer<BufferT>::rd_buf() const
		{
			return &_buffer_data[_offPtr];
		}

	template <typename BufferT>
		inline bool nByteBuffer<BufferT>::rd_ready() const
		{
			return _currPtr > _offPtr;
		}

	template <typename BufferT>
		inline unsigned int nByteBuffer<BufferT>::rd_size() const
		{
			return _currPtr - _offPtr;
		}

	template <typename BufferT>
		inline void nByteBuffer<BufferT>::rd_flip(unsigned int size)
		{
			_offPtr += size;
		}

	template <typename BufferT>
		inline unsigned int nByteBuffer<BufferT>::wr_size() const
		{
			return _max_size - _currPtr;
		}

	template <typename BufferT>
		inline void nByteBuffer<BufferT>::wr_flip(const unsigned int size)
		{
			_currPtr += size;
		}

	template <typename BufferT>
		inline void nByteBuffer<BufferT>::reset()
		{
			_offPtr = 0;
			_currPtr = 0;
		}

	template <typename BufferT>
		inline unsigned int nByteBuffer<BufferT>::maxSize() const
		{
			return _max_size;
		}
};

#endif
