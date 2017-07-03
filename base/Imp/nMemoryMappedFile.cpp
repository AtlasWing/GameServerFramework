#include "base/nAssert.h"
#include "base/nSys.h"

#include "base/nMemoryMappedFile.h"

namespace Tidy
{
	nMemoryMappedFile::nMemoryMappedFile():
		_file_handle(0),
		_addr(NULL),
		_len(0)
	{
	}

	nMemoryMappedFile::~nMemoryMappedFile()
	{
		final();
		assert_debug(_file_handle == 0 && _addr == NULL, "没有关闭文件");
	}

	bool nMemoryMappedFile::exists(const char *file)
	{
		struct stat buf;
		return stat(file, &buf) > -1;
	}

	bool nMemoryMappedFile::initFile(const char *file, DWORD len)
	{
		_file_handle = open(file, O_RDWR | O_CREAT, 0644);
		if(_file_handle == -1){
			return false;
		}
		int fd = _file_handle;
		bool ok = true;
		do{
			struct stat statData;
			DWORD fileLen = statData.st_size;
			if(fstat(fd, &statData) < 0){
				fileLen = 0;
			}
			else{
				fileLen = statData.st_size;
			}
			if(fileLen < len){
				if(ftruncate(fd, len) == -1){
					ok = false;
					break;
				}
			}
			else{
				len = fileLen;
			}
			_addr = reinterpret_cast<char*>(mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0));
			if(_addr == MAP_FAILED){
				ok = false;
				break;
			}
		}while(0);
		if(ok == false){
			close(_file_handle);
			return false;
		}

		_len = len;
		return true;
	}

	void nMemoryMappedFile::final()
	{
		if(_file_handle != 0){
			sync();
			munmap(_addr, _len);
			close(_file_handle);
			_file_handle = 0;
			_addr = NULL;
		}
	}

	char* nMemoryMappedFile::getData() const
	{
		return _addr;
	}

	DWORD nMemoryMappedFile::getLength() const
	{
		return _len;
	}

	void nMemoryMappedFile::sync()
	{
		assert_debug(_addr != NULL, "没有文件映射");
		msync(_addr, _len, MS_SYNC);
	}
};
