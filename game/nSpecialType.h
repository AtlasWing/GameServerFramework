#ifndef _N_SPECIALTYPE_H_
#define _N_SPECIALTYPE_H_
#include "game/nTableBase.h"

namespace Game {
    class SpecialTypeClass
    {
        protected:
            DECLARE_CLASS(SpecialTypeClass)
        public:
                static void * createInstance()
                {
                
                }
                SpecialTypeClass()
                {
                }
                virtual ~SpecialTypeClass()
                {
                }
    };
};

#endif
