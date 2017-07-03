#ifndef _ZONE_ID_VERIFY_H_
#define _ZONE_ID_VERIFY_H_

#include "base/nSTL.h"

namespace protocol
{
    namespace admin
    {
        struct Range;
    };
};

struct IDRange
{
    IDRange(const DWORD startID, const DWORD endID) : start(startID), end(endID)
    {
    }
    DWORD start;
    DWORD end;
};

class zoIDPool
{
    public:
        zoIDPool(const DWORD idType, const DWORD initStartID, const DWORD maxPoolNum);
        ~zoIDPool();

    public:
        const DWORD getID() const;
        const bool requestID(DWORD* id);
        bool addIDPool(const WORD num, const protocol::admin::Range* data);
        bool reuseID(const DWORD *idList, const DWORD num);

    public:
        void checkRequestPool();
        void dealAdminConnClose();

    private:
        void final();

    private:
        DWORD _id_type;
        DWORD _init_start_id;
        DWORD _max_pool_num;
        DWORD _cur_pool_num;
        bool _is_wait_return;

    private:
        Tidy::list<IDRange> _id_range_list;
        std::set<DWORD> _reuse_id_set;
};

#endif
