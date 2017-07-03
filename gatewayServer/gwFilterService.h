#ifndef GATEWAYSERVER_GWFILTERSERVICE_H
#define GATEWAYSERVER_GWFILTERSERVICE_H

#include "base/nSTL.h"
#include "base/nRWLock.h"

class gwFilterService
{
    public:
        gwFilterService();
        ~gwFilterService();

    public:
        bool initService();
        void finalService();
        void dealHupSig();
        bool checkFilter(const char *str);

    private:
        bool loadFilterData(const bool isReload);

    private:
        Tidy::nRWLock _regex_lock;
        Tidy::vector<regex_t> _regex_vec;
};
#endif
