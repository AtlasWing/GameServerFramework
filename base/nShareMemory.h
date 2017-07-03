#ifndef BASE_NSHAREMEMORY_H_
#define BASE_NSHAREMEMORY_H_

#include "nType.h"
#include "nSys.h"

namespace Tidy {
    class nShareMemory
    {
        enum eShareMemoryStatus
        {
            eShareMemoryStatus_None = 0,
            eShareMemoryStatus_Prepareing,
            eShareMemoryStatus_Ready,
            eShareMemoryStatus_Max
        };
        struct ShareMemoryInfo
        {
            DWORD size;
        };
        public:
            nShareMemory();
            ~nShareMemory();
            
            /// @ brief 创建共享内存, size需要是pageSize的整数倍，不是pageSize倍数的大小将被放大至最接近pageSize倍数的尺寸
            /// 参数说明:
            /// name 共享内存名称
            /// size 共享内存大小
            const bool create(const char* name, const DWORD size);
            /// @ brief 挂接到共享内存
            const bool hookup(const char* name);

            const bool isCreater() const;
            const DWORD size() const;
            char* getShareMemory() const;
            const char* getConstShareMemory() const;
        private:
            const bool init(void* buffer, const DWORD size);
            const int getSize(const DWORD size) const;
        private:
            bool _is_creater;
            char* _memory;
            ShareMemoryInfo* _info;
    };
};

#endif
