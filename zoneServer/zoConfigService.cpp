#include "zoneServer/zoServer.h"
#include "zoneServer/zoConfigService.h"

zoConfigService::zoConfigService() :
    _max_get_friend_vitality_times(Tidy::global["MAX_GET_FRIEND_VITALITY_TIMES"].getDword()),
    _init_common_elo_value(Tidy::global["INIT_COMMON_ELO_VALUE"].getDword()),
    _expect_average_wait_time_in_sec(Tidy::global["MATCH_QUEUE_ADD_USER_SPEED_STATICTICS_TIME_IN_SEC"].getDword()),
    _max_club_log_num(Tidy::global["MAX_CLUB_LOG_NUM"].getWord())
{
}

zoConfigService::~zoConfigService()
{
}

bool zoConfigService::initService()
{
    _init_common_elo_value = Tidy::global["INIT_COMMON_ELO_VALUE"].getDword();
    std::string eloPartParam = Tidy::global["ELO_SEQUENCE_PART_PARAM"].getString();
    std::vector< std::vector<DWORD> > eloVec;
    Tidy::nStringTool::splitValue(eloPartParam, ';', '-', eloVec);
    for(DWORD i = 0; i < eloVec.size(); ++ i){
        if(eloVec[i].size() != 2){
            server->debug("天梯段位数据错误");
            return false;
        }
        _elo_seq_group_part_param[eloVec[i][0]] = eloVec[i][1];
    }
    return true;
}

bool zoConfigService::finalService()
{
    return true;
}

DWORD zoConfigService::getMaxTimesForVitality() const
{
    return _max_get_friend_vitality_times;
}

WORD zoConfigService::getInitCommonEloValue() const
{
    return _init_common_elo_value;
}

DWORD zoConfigService::resetCommonEloValue(const DWORD eloValue) const
{
    if(eloValue <= _init_common_elo_value){
        return _init_common_elo_value;
    }
    return _init_common_elo_value + ((eloValue - _init_common_elo_value) * Tidy::global["RESET_ELO_PARAM"].getFloat());
}

WORD zoConfigService::getExpectWaitTimeInSec() const
{
    return _expect_average_wait_time_in_sec;
}

double zoConfigService::getEloSeqPartParamByGroupID(const WORD groupID) const
{
    std::map<WORD, DWORD>::const_iterator iter = _elo_seq_group_part_param.find(groupID);
    if(iter == _elo_seq_group_part_param.end()){
        server->debug("找不到的ELO分组[%u]", groupID);
        return 0.0f;
    }
    return (iter->second / 10000.0f);
}

WORD zoConfigService::getMaxClubLogNum() const
{
    return _max_club_log_num;
}
