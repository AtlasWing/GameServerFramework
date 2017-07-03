#include "base/nAssert.h"
#include "base/nSys.h"

#include "base/nBinaryQueue.h"

namespace Tidy
{
	nBinaryQueue::nBinaryQueue():
		_reserved_space(NULL),
		_reserved_size(0),
		_data_start_pos(NULL),
		_data_end_pos(NULL)
	{
	}

	bool nBinaryQueue::parseFromBinary(char *dataSpace, const DWORD dataSize)
	{
		_reserved_space = dataSpace;
		_reserved_size = dataSize;

		_data_start_pos = reinterpret_cast<DWORD*>(_reserved_space);
		_reserved_space += sizeof(DWORD);
		assert_debug(_reserved_size > sizeof(DWORD), "大小不足");
		_reserved_size -= sizeof(DWORD);

		_data_end_pos = reinterpret_cast<DWORD*>(_reserved_space);
		_reserved_space += sizeof(DWORD);
		assert_debug(_reserved_size > sizeof(DWORD), "大小不足");
		_reserved_size -= sizeof(DWORD);
		if(_reserved_size < (*_data_end_pos) || (*_data_start_pos) > (*_data_end_pos)){
			return false;
		}

		return true;
	}

	bool nBinaryQueue::setSpace(char *reservedSpace, const DWORD reservedSize)
	{
		_reserved_space = reservedSpace;
		_reserved_size = reservedSize;
		_data_start_pos = reinterpret_cast<DWORD*>(_reserved_space);
		*_data_start_pos = 0;
		_reserved_space += sizeof(DWORD);
		assert_debug(_reserved_size > sizeof(DWORD), "大小不足");
		_reserved_size -= sizeof(DWORD);

		_data_end_pos = reinterpret_cast<DWORD*>(_reserved_space);
		*_data_end_pos = 0;
		_reserved_space += sizeof(DWORD);
		assert_debug(_reserved_size > sizeof(DWORD), "大小不足");
		_reserved_size -= sizeof(DWORD);

		return true;
	}

	bool nBinaryQueue::pushBack(const char *data, const DWORD len)
	{
		if(((*_data_end_pos) + sizeof(DWORD) + len) > _reserved_size){
			assert_fail("大小不够[%u,%u]", ((*_data_end_pos) + sizeof(DWORD) + len), _reserved_size);
			return false;
		}
		*(reinterpret_cast<DWORD*>((*_data_end_pos) + _reserved_space)) = len;
		(*_data_end_pos) += sizeof(DWORD);
		memcpy(_reserved_space + (*_data_end_pos), data, len);
		(*_data_end_pos) += len;
		return true;
	}

	char* nBinaryQueue::popFront(DWORD *len)
	{
		char *data = getFront(len);
		if(data == NULL){
			return NULL;
		}
		(*_data_start_pos) += (sizeof(DWORD) + (*len));
		if((*_data_start_pos) == (*_data_end_pos)){
			(*_data_start_pos) = 0;
			(*_data_end_pos) = 0;
		}
		assert_debug((*_data_end_pos) <= _reserved_size, "内存错误");
		return data;
	}

	char* nBinaryQueue::getFront(DWORD *len) const
	{
		if((*_data_start_pos) == (*_data_end_pos)){
			return NULL;
		}
		*len = *(reinterpret_cast<DWORD*>((*_data_start_pos) + _reserved_space));
		return _reserved_space + (*_data_start_pos) + sizeof(DWORD);
	}

	void nBinaryQueue::popFront()
	{
		DWORD len = 0;
		char *data = getFront(&len);
		if(data == NULL){
			return;
		}
		(*_data_start_pos) += (sizeof(DWORD) + len);
		if((*_data_start_pos) == (*_data_end_pos)){
			(*_data_start_pos) = 0;
			(*_data_end_pos) = 0;
		}
		assert_debug((*_data_end_pos) <= _reserved_size, "内存错误");
	}

	bool nBinaryQueue::empty() const
	{
		return (*_data_start_pos) == (*_data_end_pos);
	}

	void nBinaryQueue::rollBackPopFront(const DWORD len)
	{
		assert_debug((*_data_start_pos) > 0 && (*_data_start_pos) >= (sizeof(DWORD) + len), "没有数据或者数据已经结束,无法回退");
		(*_data_start_pos) -= (sizeof(DWORD) + len);
	}
};
