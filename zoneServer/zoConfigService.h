#ifndef _ZONE_CONFIG_SERVICE_H
#define _ZONE_CONFIG_SERVICE_H

class zoConfigService
{
    public:
        zoConfigService();
        ~zoConfigService();

    public:
        bool initService();
        bool finalService();

    public:
        DWORD getMaxTimesForVitality() const;
        WORD getInitCommonEloValue() const;
        DWORD resetCommonEloValue(const DWORD eloValue) const;
        WORD getExpectWaitTimeInSec() const;
        double getEloSeqPartParamByGroupID(const WORD groupID) const;
        WORD getMaxClubLogNum() const;

    private:
        DWORD _max_get_friend_vitality_times;
        WORD _init_common_elo_value;
        WORD _expect_average_wait_time_in_sec;
        WORD _max_club_log_num;
        std::map<WORD, DWORD> _elo_seq_group_part_param;
};

#endif
