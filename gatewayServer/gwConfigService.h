#ifndef _GATEWAY_TABLE_MANAGER_
#define _GATEWAY_TABLE_MANAGER_

class gwConfigService
{
    public:
        gwConfigService();
        ~gwConfigService();

    public:
        bool initService();
        bool finalService();

    public:
        void dealHupSig();

    private:
        bool loadAllTable(const bool isFirstLoad);
};

#endif
