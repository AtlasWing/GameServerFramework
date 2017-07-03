#ifndef _PROTOCOL_EQUIP_DATA_H 
#define _PROTOCOL_EQUIP_DATA_H
#include "protocol/commonType.h"
#include "base/nStringTool.h"
#include <string>

namespace protocol{
    // 获取装备属性数据
    inline void getEquipAttrData(const std::string str, DWORD &proportion, DWORD &minValue, DWORD &maxValue)
    {
        std::vector< std::vector<DWORD> > equipDataList;
        Tidy::nStringTool::splitValue(str, ';', '-', equipDataList);
        if(equipDataList.empty() == false){
            return;
        }
        if(equipDataList.size() != 2 || equipDataList[0].size() != 1 || equipDataList[1].size() != 2){
            // 系数是(权重;最小值-最大值) 第一层解析不等于2则少参数
            return;
        }
        proportion = equipDataList[0][0];
        minValue = equipDataList[1][0];
        maxValue = equipDataList[1][1];
    }
    // 获取随机品质列表
    inline std::vector< std::vector<DWORD> > splitData(const std::string str)
    {
        std::vector< std::vector<DWORD> > qualityDataList;
        Tidy::nStringTool::splitValue(str, ';', '-', qualityDataList);
        return qualityDataList;
    }
}
namespace protocol{ 
    namespace equip{
        enum eEquipAttr
        {
            eEquipAttr_None,
#define EQUIP_BASE_ATTRIBUTE(type, name, protoname, defaultValue, desc) \
        eEquipAttr_##name,
#include "protocol/equipAttribute.h"
#undef EQUIP_BASE_ATTRIBUTE
        };
    };      
};          
#endif      
            
            
            
            
