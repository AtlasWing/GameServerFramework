#!/bin/sh
#获取每日在线数据
YESTERDAY_TIME="`date +%Y-%m-%d -d -24hour` 00:00:00"
TODAY_TIME="`date +%Y-%m-%d` 00:00:00"
YESTERDAY_TIME_STAMP="`date -d "$YESTERDAY_TIME" +%s `"
TODAY_TIME_STAMP="`date -d "$TODAY_TIME" +%s `"
GUIDE_SCENE=`mysql -ufkys -pfkyspass -e "use zoneInfo_601;select count(*) as "通过新手关数量" from LOG_SCENE_DATA_DETAIL where time_stamp > $YESTERDAY_TIME_STAMP and time_stamp < $TODAY_TIME_STAMP and mode_id=5 and pass_scene_times >= 1;"`
TEAM_PK_SCENE=`mysql -ufkys -pfkyspass -e "use zoneInfo_601;select count(*) as "进入第一场团队乱斗的>数量" from LOG_SCENE_DATA_DETAIL where time_stamp > $YESTERDAY_TIME_STAMP and time_stamp < $TODAY_TIME_STAMP and mode_id=10 and into_scene_times = 1;select count(*) as "通过第一场团队乱斗的数量" from LOG_SCENE_DATA_DETAIL where time_stamp > $YESTERDAY_TIME_STAMP and time_stamp < $TODAY_TIME_STAMP and mode_id=10 and pass_scene_times = 1;"`
COMPLATE_GUIDE=`mysql -ufkys -pfkyspass -e "use zoneInfo_601;select count(*) as "完全完成外部新手引导
数量" from LOG_ROLE where create_time > '$YESTERDAY_TIME' and create_time < '$TODAY_TIME' and complete_out_guide > 0;"`
TOTAL_TIME=`mysql -ufkys -pfkyspass -e "use zoneInfo_601;select sum(online_time_ones) as "当日在线总>时间" from LOG_LOGOUT_DATA_DETAIL where time_stamp > $YESTERDAY_TIME_STAMP and time_stamp < $TODAY_TIME_STAMP;"`
TOTAL_ACCOUNT=`mysql -ufkys -pfkyspass -e "use zoneInfo_601;select count(*) as "当日总人数" from ( select count(account_name) from LOG_LOGOUT_DATA_DETAIL where time_stamp > $YESTERDAY_TIME_STAMP and time_stamp < $TODAY_TIME_STAMP group by account_name) as a;"`
TIME=`echo $TOTAL_TIME | awk -F" " '{print $2}'`
ACCOUNT=`echo $TOTAL_ACCOUNT | awk -F" " '{print $2}'`
AVERAGE=`expr "scale=1;$TIME/$ACCOUNT/60"|bc`

echo $YESTERDAY_TIME >> $YESTERDAY_TIME"Data".txt
echo $GUIDE_SCENE >> $YESTERDAY_TIME"Data".txt
echo $TEAM_PK_SCENE >> $YESTERDAY_TIME"Data".txt
echo $COMPLATE_GUIDE >> $YESTERDAY_TIME"Data".txt
echo $TOTAL_TIME >> $YESTERDAY_TIME"Data".txt
echo $TOTAL_ACCOUNT >> $YESTERDAY_TIME"Data".txt
echo "平均游戏时间" $AVERAGE >>$YESTERDAY_TIME"Data".txt
