#ifndef BASE_NMEMORYMAPPED_FILE_H
#define BASE_NMEMORYMAPPED_FILE_H

#include "base/nType.h"

namespace Tidy
{
	class nMemoryMappedFile
	{
		public:
			nMemoryMappedFile();
			~nMemoryMappedFile();

		public:
			char* getData() const;
			DWORD getLength() const;

		public:
			bool exists(const char *file);
			bool initFile(const char *file, const DWORD len);
			void final();
			void sync();

		private:
			int _file_handle;
			char *_addr;
			DWORD _len;
	};
};
#endif
