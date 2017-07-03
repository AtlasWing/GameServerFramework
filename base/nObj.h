#ifndef BASE_NOBJ_H_
#define BASE_NOBJ_H_

#include "base/nObjLogger.h"

namespace Tidy
{
	class nObj : public nObjLogger<nObj> {
		protected:
			DISALLOW_COPY_AND_ASSIGN(nObj);
			nObj();
			nObj(const DWORD id, const char* name);

		protected:
			~nObj();

		public:
			const DWORD getID() const;
			const char* getName() const;
			DWORD getNameLength() const;
			const bool isSame(const nObj* other) const;
			nLogger* getLogger() const;
			const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;

		protected:
			void setID(DWORD newID);
			void setName(const char *name);
			void setName(const std::string &name);

		private:
			DWORD _id;
			std::string _name;
	};
};

#endif
