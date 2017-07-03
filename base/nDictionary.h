#ifndef BASE_SHAREDICTIONARY_H_
#define BASE_SHAREDICTIONARY_H_

#include "base/nShareMemory.h"

namespace Tidy {
    class nDictionary
    {
        struct DirectoryPerRecordData
        {
            DWORD dataSeek;
            DWORD dataSize; 
        };
        public:
            nDictionary(char* buffer, const DWORD bufferSize, const DWORD maxRecordNum);
            nDictionary(const char* buffer, const DWORD bufferSize, const DWORD maxRecordNum);
            ~nDictionary();

            void clearDirectory();

            const bool addRecord(const DWORD index, const char* data, const DWORD size);
            const char* getRecord(const DWORD index) const;
            const DWORD getRecordSize(const DWORD index) const;

            static const DWORD getDictionarySize(const DWORD maxRecordNum, const DWORD maxDataSize);
        private:
            const DWORD getDirectorySize() const;
            static const DWORD getDirectorySize(const DWORD maxRecordNum);
        private:
            const char* _const_buffer;
            char* _buffer;
            DWORD _buffer_size;
            DWORD _max_record_num;
            DWORD _used_size;
    };
};

#endif
