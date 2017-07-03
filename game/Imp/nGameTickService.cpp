#include "game/nGameTickService.h"
#include "game/nGameServer.h"

namespace Game
{
    nGameTickService::nGameTickService()
    {
    }

    nGameTickService::~nGameTickService() 
    {
    }

    void nGameTickService::run()
    {
        long t = 0L;
        struct timespec _tv_1;
        clock_gettime(CLOCK_REALTIME, &_tv_1);

        DWORD tickTimeInMsec = 0;
        QWORD startTimeOutTimeInMsec = 0;
        while(!isFinal()) {
            setRunning();
            tickTimeInMsec = gameServer->getTickTimeInMsec();

            struct timespec _tv_2;
            clock_gettime(CLOCK_REALTIME, &_tv_2);
            QWORD end=_tv_2.tv_sec*1000L + _tv_2.tv_nsec/1000000L;
            QWORD begin= _tv_1.tv_sec*1000L + _tv_1.tv_nsec/1000000L;
            _tv_1 = _tv_2; 

            if(tickTimeInMsec >= (end - begin)){
                t = tickTimeInMsec - (end - begin);
                if(t > 0){
                    Tidy::nThread::msleep(t);
                }
                startTimeOutTimeInMsec = 0;
            }
            else{
                t = (end - begin) - tickTimeInMsec;
                if(startTimeOutTimeInMsec == 0){
                    startTimeOutTimeInMsec = begin;
                }
                else if(end > (startTimeOutTimeInMsec + Tidy::global["MAX_TICK_TIME_OUT_TIME_IN_MSEC"].getDword())){
                    gameServer->setNeedAnalyze();
                }
            }

            gameServer->now();
            gameServer->doServerTimetick();

            if(gameServer->checkNeedAnalyze() == true){
                gameServer->clearNeedAnalyze();
                if(startTimeOutTimeInMsec > 0){
                    gameServer->fatal("心跳帧超时时间[%llu]", (end - startTimeOutTimeInMsec));
                    startTimeOutTimeInMsec = 0;
                }
            }
        }

        gameServer->doServerTimetick();
    }

};
