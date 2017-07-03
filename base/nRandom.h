#ifndef _N_RANDOM_H_
#define _N_RANDOM_H_

#include "base/nSys.h"
#include "base/nAssert.h"

/// @brief 数值对
struct odds_t
{
    /// @brief 最大数
    DWORD upNum;
    /// @brief 最小数
    DWORD downNum;
    odds_t():upNum(0),downNum(0){}
};

namespace Tidy
{
    /// @brief 随机数种子
    // Thread-Local storage
    extern __thread unsigned int seedp;

    /// @brief 随机产生min~max之间的数字，包裹min和max
    /// @param min	最小值
    /// @param max	最大值
    /// @return 随机到的数值
    inline int randBetween(int min,int max)
    {
        if(min==max)
            return min;
        else if (min > max)
            return max + (int) (((double) min - (double)max + 1.0) * rand_r(&seedp) / (RAND_MAX + 1.0));
        else
            return min + (int) (((double) max - (double)min + 1.0) * rand_r(&seedp) / (RAND_MAX + 1.0));
    }

    /// @brief 随机产生min~max之间的数字，包裹min和max
    /// @param min	最小值
    /// @param max	最大值
    /// @return 随机到的数值
    inline DWORD randBetween(DWORD min, DWORD max)
    {
        if(min==max)
            return min;
        else if (min > max)
            return max + (unsigned int) (((double) min - (double)max + 1.0) * rand_r(&seedp) / (RAND_MAX + 1.0));
        else
            return min + (unsigned int) (((double) max - (double)min + 1.0) * rand_r(&seedp) / (RAND_MAX + 1.0));
    }
    /// @brief 随机产生min~max之间的数字，包裹min和max
    /// @param min	最小值
    /// @param max	最大值
    /// @return 随机到的数值
    inline float randBetween(const float min, const float max)
    {
        if(min==max)
            return min;
        else if (min > max)
            return static_cast<float>(max + (((double)min - (double)max) * rand_r(&seedp) / (RAND_MAX + 1.0)));
        else
            return static_cast<float>(min + (((double)max - (double)min) * rand_r(&seedp) / (RAND_MAX + 1.0)));
    }

    /// @brief 获取几分之几的几率
    /// @param upNum		随机范围的大值
    /// @param downNum	随机范围的小值
    /// @return 随机返回
    inline bool selectByOdds(const unsigned int upNum, const unsigned int downNum)
    {
        if (upNum > downNum - 1) return true;
        if (upNum < 1) return false;
        if (downNum < 1) return false;
        unsigned int m_rand;
        m_rand = 1 + (unsigned int) ((double)downNum * rand_r(&seedp) / (RAND_MAX + 1.0));
        return (m_rand <= upNum);
    }

    /// @brief 获取几分之几的几率
    /// @param odds	随机范围
    /// @return 随机返回
    inline bool selectByt_Odds(const odds_t &odds)
    {
        return selectByOdds(odds.upNum, odds.downNum);
    }

    /// @brief 获取百分之的几率
    /// @param percent	概率
    /// @return 随机返回
    inline bool selectByPercent(const unsigned int percent)
    {
        return selectByOdds(percent, 100);
    }

    /// @brief 获取万分之的几率
    /// @param tenth	概率
    /// @return 随机返回
    inline bool selectByTenTh(const unsigned int tenth)
    {
        return selectByOdds(tenth, 10000);
    }

    /// @brief 获取十万分之的几率
    /// @param lakh	概率
    /// @return 随机返回
    inline bool selectByLakh(const unsigned int lakh)
    {
        return selectByOdds(lakh, 100000);
    }

    /// @brief	获取百万分之一的几率
    /// @param lakh	概率
    /// @return 随机返回
    inline bool selectByPpm(const unsigned int lakh)
    {
        return selectByOdds(lakh, 1000000);
    }

    /// @brief	获取百万分之一的几率
    /// @param lakh	概率
    /// @return 随机返回
    inline bool selectByPpm(const double lakh)
    {
      assert_debug(lakh <= 1.0 && lakh >= 0.0, "");
      return selectByOdds(static_cast<DWORD>(lakh*1000000), 1000000);
    }

    /// @brief 获取亿分之一的几率
    /// @param lakh	概率
    /// @return 随机返回
    inline bool selectByOneHM(const unsigned int lakh)
    {
        return selectByOdds(lakh, 100000000);
    }
    /// @brief	获得随机方向
    inline BYTE randDirect()
    {
        return (BYTE)randBetween(0, 7);
    }

	template<typename EntityType>
	struct ProbList
	{
		ProbList() : sumProb(0)
		{
		}
		void clear()
		{
			entityList.clear();
			sumProb = 0;
		}
		void addEntity(EntityType &e)
		{
			entityList.push_back(e);
			sumProb += e.prob;
		}
		const EntityType * getRandomEntity() const
		{
			DWORD randomValue = randBetween(static_cast<DWORD>(1), sumProb);
			DWORD probNum = 0;
			for(DWORD i = 0; i < entityList.size(); ++ i){
				probNum += entityList[i].prob;
				if(probNum >= randomValue){
					return &entityList[i];
				}
			}
			return NULL;
		}
		std::vector<EntityType> entityList;
		DWORD sumProb;
	};
	
	/// @brief 相对于伪随机更随机,从/dev/urandom读取随机数.代价也更大一些,确切需要时调用.随机产生min~max之间的数字，包裹min和max
    /// @param min	最小值
    /// @param max	最大值
    /// @return 随机到的数值
    inline DWORD urandomBetween(DWORD min, DWORD max)
    {
        FILE *urandom = fopen("/dev/urandom", "r");
		if(urandom == NULL){
			assert_fail("打开文件/dev/urandom失败");
			return randBetween(min, max);
		}
		unsigned int rand = 0;
		fread(&rand, sizeof(rand), 1, urandom);
        fclose(urandom);
        if(min==max)
            return min;
        else if (min > max)
            return max + (int) (((double) min - (double)max + 1.0) * rand / ((DWORD)-1 + 1.0));
        else
            return min + (int) (((double) max - (double)min + 1.0) * rand / ((DWORD)-1 + 1.0));
    }
}
//end namespace Tidy
#endif
