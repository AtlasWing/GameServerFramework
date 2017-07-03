#ifndef _N_SIG_H_
#define _N_SIG_H_

#include "base/nSys.h"

namespace Tidy
{
    using namespace sigc;

    class nTrackBase : public trackable
    {
        protected:
            nTrackBase()
            {
            }
            ~nTrackBase()
            {
            }

        public:
            /// @brief 当对象内存释放和逻辑上销毁对象分开时,需要显式调用
            void clearAllConnect()
            {
                notify_callbacks();
            }
    };
};
#endif
