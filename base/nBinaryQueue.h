#ifndef BASE_NBINARYQUEUE_H
#define BASE_NBINARYQUEUE_H

#include "base/nType.h"

namespace Tidy
{
	class nBinaryQueue
	{
		public:
			nBinaryQueue();

		public:
			bool parseFromBinary(char *dataSpace, const DWORD dataSize);
			bool setSpace(char *reservedSpace, const DWORD reservedSize);
			bool pushBack(const char *data, const DWORD len);
			char* popFront(DWORD *len);
			void popFront();
			char* getFront(DWORD *len) const;
			void rollBackPopFront(const DWORD len);
			bool empty() const;

		private:
			char *_reserved_space;
			DWORD _reserved_size;
			DWORD *_data_start_pos;
			DWORD *_data_end_pos;
	};
};
#endif
