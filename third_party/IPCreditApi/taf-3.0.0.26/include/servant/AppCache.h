#ifndef __APP_CACHE_H_
#define __APP_CACHE_H_

#include <string>
#include <memory>
#include <map>
#include <vector>
#include "util/tc_singleton.h"
#include "util/tc_config.h"
#include "util/tc_timeprovider.h"
#include "util/tc_file.h"
#include "util/tc_thread_mutex.h"
#include "log/taf_logger.h"

#define APPCACHE_ROOT_PATH "/cache"

using namespace std;

namespace taf
{
//////////////////////////////////////////////////////////////////////
// ����
class AppCache : public TC_Singleton<AppCache>, public TC_ThreadMutex
{    
public:
    AppCache() : _tLastSynTime(0), _iSynInterval(1000)
    {

    }

    ~AppCache()
    {

    }
     /**
     * ���ñ�����Ϣ
     * @param sFilePath, cache�ļ�·��
     * @param iSynInterval(ms), ͬ�����ʱ��
     */
    void setCacheInfo(const string &sFile,int32_t iSynInterval = 1000);

    /**
     * ��cache�ļ���ȡ�����б�
     * @param sObjName 
     * @param sDomain 
     * @return string
     */
    string get(const string & sName,const string sDomain = ""/*=APPCACHE_ROOT_PATH*/);

	/**
	 * ��ȡ������Ĳ���ֵ�� 
	 * �����򷵻ؿ�map 
     * @param path: ������, ���ʶ, ����: /Main/Domain
     * 
     * @return map<string,string>
     */
    map<string, string> getDomainMap(const string &path);
    
     /**
     * �������ݵ�cache�ļ�
     * @param sName 
     * @param sValue
     * @param sDomain
     * @return int
     */
    int set(const string &sName,const string &sValue,const string sDomain = ""/*=APPCACHE_ROOT_PATH*/);

private:
    string      _sFile;
    TC_Config   _tFileCache;      
    time_t      _tLastSynTime;   
    int32_t     _iSynInterval;
};
//////////////////////////////////////////////////////////////////////
}
#endif
