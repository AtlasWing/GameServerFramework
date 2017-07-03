#include "base/nShareMemory.h"
#include "base/nBaseServer.h"

namespace Tidy {
    nShareMemory::nShareMemory() : 
        _is_creater(false),
        _memory(NULL),
        _info(NULL)
    {
    }

    nShareMemory::~nShareMemory()
    {
        if(_info != NULL) {
            munmap((char*)_info, _info->size); 
            _info = NULL;
            _memory = NULL;
        }
    }

    const bool nShareMemory::create(const char* name, const DWORD size)
    {
        if(size == 0) {
            assert_fail("size不能为0");
            return false;
        }
        /// 创建共享内存文件
        int fd = open(name, O_CREAT|O_RDWR|O_TRUNC, 00666);
        if(fd == -1) {
            baseServer->fatal("共享内存创建失败,打开文件失败[%s]", name);
            return false;
        }
        int memsize = getSize(size);
        lseek(fd, memsize - 1, SEEK_SET);
        write(fd, "", 1);
        /// 映射
        void* buffer = static_cast<char*>(mmap(NULL, memsize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)); 
        close(fd);

        if(buffer == NULL) {
            baseServer->debug("共享内存创建失败，映射失败");
            return false;
        }
        baseServer->debug("共享内存[%s, size:%u(请求size:%u)]创建成功！地址:[%p]", name, memsize, size, buffer);
        return init(buffer, memsize);
    }

    const bool nShareMemory::hookup(const char* name)
    {
        int fd = open(name, O_RDONLY, 0);
        if(fd == -1) {
            baseServer->debug("挂接到共享内存[%s]打开文件失败", name);
            return false;
        }
        int memsize = lseek(fd, 0, SEEK_END);
        char* buffer = static_cast<char*>(mmap(NULL, memsize, PROT_READ, MAP_SHARED, fd, 0));
        close(fd);
        if(buffer == NULL) {
            baseServer->debug("挂接到共享内存[%s]映射失败", name);
            return false;
        }
        /// 验证共享内存HeadInfo中的size是否和给出的size相符
        _info = reinterpret_cast<ShareMemoryInfo*>(buffer);
        if(_info->size != memsize) {
            if(munmap(buffer , memsize) == -1) {
                assert_fail("消除共享内存映射失败");
            }
            _info = NULL;
            baseServer->debug("挂接到共享内存失败，size不符，该共享内存的size为:[%u],其文件实际size为:[%u]", _info->size, memsize);
            return false;
        }
        _memory = buffer + sizeof(ShareMemoryInfo);
        baseServer->debug("挂接到共享内存[%s]成功，size:[%u]", name, _info->size);
        return true;
    }

    const bool nShareMemory::isCreater() const
    {
        return _is_creater;
    }

    const DWORD nShareMemory::size() const
    {
        if(_info == NULL) {
            return 0;
        }
        return _info->size;
    }

    char* nShareMemory::getShareMemory() const
    {
        if(_is_creater == false) {
            assert_fail("不是创建者，不允许修改共享内存");
            return NULL;
        }
        return _memory;
    }

    const char* nShareMemory::getConstShareMemory() const
    {
        return _memory; 
    }

    const bool nShareMemory::init(void* buffer, const DWORD size)
    {
        _info = reinterpret_cast<ShareMemoryInfo*>(buffer);
        _info->size = size;
        _memory = (char*)buffer + sizeof(ShareMemoryInfo);
        _is_creater = true;
        baseServer->debug("共享内存初始化成功, size : [%u]", _info->size);
        return true;
    }

    const int nShareMemory::getSize(const DWORD size) const
    {
        return (size + sizeof(ShareMemoryInfo) + getpagesize()) / getpagesize() * getpagesize(); 
    }
}
