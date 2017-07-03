#ifndef BASE_NFILE_H
#define BASE_NFILE_H

#include "base/nSTL.h"

namespace Tidy{
	class nFile{
		public:
			nFile();
			~nFile();

		public:
			static bool exists(const std::string &fileName);
	};
};
#endif
