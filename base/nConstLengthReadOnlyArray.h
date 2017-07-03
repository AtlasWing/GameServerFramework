#ifndef _NCONST_LENGTH_ARRAY_H_
#define _NCONST_LENGTH_ARRAY_H_

#include "base/nType.h"

namespace Tidy{
    template<typename T>
    class nConstLengthReadOnlyArray
    {
        public:
            nConstLengthReadOnlyArray() :
                _element_num(0),
                _pointer(NULL)
            {
            }
            nConstLengthReadOnlyArray(const DWORD elementNum, const T* firstElementPointer) :
                _element_num(elementNum),
                _pointer(firstElementPointer)
            {
            }
            ~nConstLengthReadOnlyArray()
            {
            }

            void setup(const DWORD elementNum, const T* firstElementPointer)
            {
                _element_num = elementNum;
                _pointer = firstElementPointer;
            }

            const T& operator[](const DWORD index) const
            {
                if(index >= _element_num) {
                    assert_fail("越界");
                    static T errorObj;
                    return errorObj;
                }
                return _pointer[index];
            }
            const DWORD size() const
            {
                return _element_num;
            }

            bool empty() const
            {
                return _element_num == 0;
            }
        private:
            DWORD _element_num;
            const T* _pointer;
    };
};

#endif
