#include "base/nDictionary.h"
#include "base/nAssert.h"

namespace Tidy {
    nDictionary::nDictionary(char* buffer, const DWORD bufferSize, const DWORD maxRecordNum) : 
        _const_buffer(buffer),
        _buffer(buffer),
        _buffer_size(bufferSize),
        _max_record_num(maxRecordNum),
        _used_size(0)
    {
    }

    nDictionary::nDictionary(const char* buffer, const DWORD bufferSize, const DWORD maxRecordNum) : 
        _const_buffer(buffer),
        _buffer(NULL),
        _buffer_size(bufferSize),
        _max_record_num(maxRecordNum),
        _used_size(0)
    {
    }

    nDictionary::~nDictionary()
    {
    }

    void nDictionary::clearDirectory()
    {
        memset(_buffer, 0, sizeof(DirectoryPerRecordData) * _max_record_num);
    }

    const bool nDictionary::addRecord(const DWORD index, const char* data, const DWORD size)
    {
        if(_buffer == NULL) {
            assert_fail("没有写字典权限");
            return false;
        }
        if(index >= _max_record_num) {
            assert_fail("目录索引越界");
            return false;
        }
        if(_used_size + getDirectorySize() + size > _buffer_size) {
            assert_fail("溢出");
            return false;
        }
        if(data == NULL || size == 0) {
            assert_fail("没有数据");
            return false;
        }
        /// 设置目录
        DirectoryPerRecordData* directory = reinterpret_cast<DirectoryPerRecordData*>(_buffer); 
        directory[index].dataSeek = _used_size;
        directory[index].dataSize = size;
        memcpy(_buffer + getDirectorySize() + _used_size, data, size);
        _used_size += size;
        return true;
    }

    const char* nDictionary::getRecord(const DWORD index) const
    {
        if(index >= _max_record_num) {
            assert_fail("目录索引越界");
            return false;
        }
        const DirectoryPerRecordData* directory = reinterpret_cast<const DirectoryPerRecordData*>(_const_buffer);
        return _const_buffer + getDirectorySize() + directory[index].dataSeek;
    }

    const DWORD nDictionary::getRecordSize(const DWORD index) const
    {
        if(index >= _max_record_num) {
            assert_fail("目录索引越界");
            return false;
        }
        const DirectoryPerRecordData* directory = reinterpret_cast<const DirectoryPerRecordData*>(_const_buffer);
        return directory[index].dataSize;
    }

    const DWORD nDictionary::getDictionarySize(const DWORD maxRecordNum, const DWORD maxDataSize)
    {
        return getDirectorySize(maxRecordNum) + maxDataSize;
    }

    const DWORD nDictionary::getDirectorySize() const
    {
        return getDirectorySize(_max_record_num);
    }
    
    const DWORD nDictionary::getDirectorySize(const DWORD maxRecordNum)
    {
        return maxRecordNum * sizeof(DirectoryPerRecordData);
    }
}
