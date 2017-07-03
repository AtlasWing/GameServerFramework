#include "base/nByteBuffer.h"

namespace Tidy {

	template <>
		nByteBuffer<unsigned char*>::nByteBuffer() : _max_size(0), _offPtr(0), _currPtr(0), _buffer_data(NULL) 
		{
		}

	template <>
		nByteBuffer<unsigned char*>::~nByteBuffer()
		{
			if(_buffer_data != NULL){
				free(_buffer_data);
			}
		}

	template <>
		bool nByteBuffer<unsigned char*>::wr_reserve(const unsigned int size)
		{
			if (wr_size() < (size + 8)) {
				if(_offPtr > 0){
					if(_currPtr > _offPtr){
						unsigned int tmp = _currPtr - _offPtr;
						//memmove可以处理重叠的情况
						memmove(&_buffer_data[0], &_buffer_data[_offPtr], tmp);
						_offPtr = 0;
						_currPtr = tmp;
					}
					else{
						_offPtr = 0;
						_currPtr = 0;
					}
				}

				if (wr_size() < (size + 8)) {
					_max_size += (trunkSize * (((size + 8) + trunkSize - 1) / trunkSize));
					_buffer_data = reinterpret_cast<unsigned char*>(realloc(_buffer_data, _max_size));
				}
			}
			return true;
		}

	template <>
		StackCmdQueue::nByteBuffer() : _max_size(PACKET_ZIP_BUFFER), _offPtr(0), _currPtr(0) 
		{
		}

	template <>
		StackCmdQueue::~nByteBuffer()
		{
		}

	template <>
		bool StackCmdQueue::wr_reserve(const unsigned int size)
		{
			//如果需求的静态缓冲大小小于需求的缓冲大小，肯定会导致栈溢出
			//这里直接退出运行是最安全的做法
			//assert(wr_size() >= size);
			if(wr_size() < size) {
				assert("缓存区不够");
				return false;
			}
			return true;
		}
};
