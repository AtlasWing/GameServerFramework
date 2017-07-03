#ifndef PROTOCOL_PLATFORM_COMMON_H
#define PROTOCOL_PLATFORM_COMMON_H

#include <string>
#include "commonType.h"

namespace protocol{
	namespace platform{
        // 与客户端版本号
        const DWORD editionVersion = 1;

		bool inline getAccountName(const char *srcAccountName, const DWORD srcPlatformID, std::string *accountName){
			char accountNameBuffer[MAX_ACCOUNT_NAME_SIZE];
			if(static_cast<DWORD>(snprintf(accountNameBuffer, sizeof(accountNameBuffer), "%03u%s", srcPlatformID, srcAccountName)) >= sizeof(accountNameBuffer)){
				return false;
			}
			accountName->assign(accountNameBuffer);
			return true;
		}
		bool inline getSrcAccountName(const std::string &accountName, const DWORD srcPlatformID, std::string *srcAccountName){
			if(accountName.length() < 3){
				return false;
			}
			srcAccountName->assign(&accountName[3], accountName.length() - 3);
			return true;
		}

		enum RechargeMoneyResult{
			RechargeMoneyResult_None                =   0,
			RechargeMoneyResult_OK                  =   1,
			RechargeMoneyResult_InvalidServerID     =   -10,
			RechargeMoneyResult_InvalidAccountID    =   -11,
			RechargeMoneyResult_OrderIDRepeated     =   -12,
			RechargeMoneyResult_InvalidOrderTime    =   -14,
			RechargeMoneyResult_InvalidMoney        =   -15,
			RechargeMoneyResult_InvalidSign         =   -17,
			RechargeMoneyResult_Other               =   -18,
		};
	};
};
#endif
