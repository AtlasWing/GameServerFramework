#include "base/nSocketPortData.h"

namespace Tidy
{
    nListenData::nListenData(const DWORD taskConnType, const BYTE listenNetType, const std::string &ip, const WORD port):
        connType(taskConnType),
        netType(listenNetType),
        bindIP(ip),
        bindPort(port)
    {
    }

    nListenConfig::nListenConfig()
    {
    }

    bool nListenConfig::parseConfig(const std::string &data)
    {
#if 0
        assert_debug(_listen_data_list.empty(), "");
        std::vector<std::string> listenDataStringList;
        nStringTool::splitValue(data, ';', listenDataStringList);
        nParamMap listenData;
        DWORD connType  =   0;
        BYTE  netType  =   0;
        const std::string *ip = NULL;
        WORD port = 0;
        DWORD connType  =   0;
        for(DWORD i = 0; i < listenDataStringList.size(); ++ i){
            listenData.clear();
            listenData.parseCmdLine(listenDataStringList[i]);
            connType = ;
        }
#endif
        return true;
    }

    const std::vector<nListenData>& nListenConfig::getListenDataList() const
    {
        return _listen_data_list;
    }
}
