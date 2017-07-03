#include "gatewayServer/gwConfigService.h"

gwConfigService::gwConfigService()
{
}

gwConfigService::~gwConfigService()
{
}

bool gwConfigService::initService()
{
    return loadAllTable(true);
}

bool gwConfigService::loadAllTable(const bool isFirstLoad)
{
    /*
	//!!!这里的表格使用记住加锁
    if(Game::GameMapTableManager::getInstance().load("GameMap.tbl64", isFirstLoad, true) == false){
        return false;
    }
	if(Game::VipTypeTableManager::getInstance().load("VipType.tbl64", isFirstLoad, true) == false){
		return false;
	}
	if(Game::VipOpenTableManager::getInstance().load("VipOpen.tbl64", isFirstLoad, true) == false){
		return false;
	}
	if(Game::PresentBroadcastTableManager::getInstance().load("PresentBroadcast.tbl64", isFirstLoad, true) == false){
		return false;
	}
	if(Game::FunctionLimitTableManager::getInstance().load("FunctionLimit.tbl64", isFirstLoad, true) == false){
		return false;
	}
    */
    return true;
}

bool gwConfigService::finalService()
{
    return true;
}

void gwConfigService::dealHupSig()
{
	if(loadAllTable(false) == false){
		//assert_fail("加载表格失败");
        // TODO 断言
	}
}
