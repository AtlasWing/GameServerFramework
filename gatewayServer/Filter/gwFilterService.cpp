#include "base/nStringTool.h"

#include "game/nTable.h"
#include "game/nTableManager.h"

#include "gatewayServer/gwFilterService.h"
#include "gatewayServer/gwServer.h"

gwFilterService::gwFilterService()
{
}

gwFilterService::~gwFilterService()
{
}

bool gwFilterService::initService()
{
    return loadFilterData(false);
}

void gwFilterService::finalService()
{
}

void gwFilterService::dealHupSig()
{
    loadFilterData(true);
}

bool gwFilterService::checkFilter(const char *str)
{
    Tidy::nScopeRDLock rdLock(_regex_lock);
    int result = 0;
    for(DWORD i = 0; i < _regex_vec.size(); ++ i){
        result = regexec(&_regex_vec[i], str, 0, NULL, 0);
        if(result != REG_NOMATCH){
            Tidy::baseServer->debug("match id=[%u]", i);
            return true;
        }
    }

    return false;
}

namespace{
    struct LoadFilterExec{
        std::string keyStr;
        DWORD reservedStrSize;
        std::string regexStr;
        regex_t regex;
        Tidy::vector<regex_t> regexVec;
        LoadFilterExec(){
            reservedStrSize = (Tidy::global["GROUP_KEY_STR_SIZE_IN_KB"].getDword() + 1) * 1024;
            regexStr.reserve(reservedStrSize);
        }
        bool exec(const Game::FilterBaseTable *data){
            keyStr.assign(data->Name);
            escapeKeyStr(keyStr);
            if(keyStr.empty() == true){
                return true;
            }
            if(regexStr.empty() == false){
                regexStr.append("|(");
            }
            else{
                regexStr.append("(");
            }
            regexStr.append(keyStr);
            regexStr.append(")");
            //调试关键字可以打开
            if(regexStr.size() > 0){
                if(createRegex() == false){
                    assert_fail("创建正则失败");
                    return false;
                }
            }
            return true;
        }
        bool createRegex(){
            if(regexStr.empty() == true){
                return true;
            }
            int result = regcomp(&regex, regexStr.c_str(), REG_EXTENDED|REG_NEWLINE);
            if(result != REG_NOERROR){
                char errorInfo[1024];
                regerror(result, &regex, errorInfo, sizeof(errorInfo));
                assert_fail("表达式错误[%u, %s]", result, errorInfo);
                return false;
            }
            regexVec.push_back(regex);
#if 0
            _debug_regex_str_map[regexVec.size() - 1] = regexStr;
#endif
            regexStr.clear();
            return true;
        }
        bool escapeKeyStr(std::string &str) {
            if(str.empty() == true){
                return true;
            }
            char escapeCharList[] = {'+', '?', '.', '*', ')', '(', '[', ']', '{', '}', '|', '\\', '^', '$'};
            char *destBuffer = reinterpret_cast<char*>(alloca(str.length() * 2 + 1));
            DWORD size = 0;
            const char *orgData = str.c_str();
            for(DWORD i = 0; i < str.length(); ++ i){
                if(orgData[i] == ' ' || orgData[i] == '\r' || orgData[i] == '\n'){
                    continue;
                }
                for(DWORD j = 0; j < sizeof(escapeCharList); ++ j){
                    if(escapeCharList[j] == orgData[i]){
                        destBuffer[size] = '\\';
                        size += 1;
                        break;
                    }
                }
                destBuffer[size] = orgData[i];
                ++ size;
            }
            assert_debug(size <= (str.length() * 2 + 1), "缓存区长度不足");

            str.assign(destBuffer, size);
            return true;
        }
    };
};
bool gwFilterService::loadFilterData(const bool isReload)
{
    LoadFilterExec loadExec;
    std::vector<const Game::FilterBaseTable*> *table = Game::TableManager::GetTable<const Game::FilterBaseTable>();
    for(std::vector<const Game::FilterBaseTable*>::iterator it = table->begin(); it != table->end(); ++it){
        loadExec.exec((*it));
    }
    // 剩余的部分
    loadExec.createRegex();

    _regex_lock.wrlock();
    _regex_vec.swap(loadExec.regexVec);
    _regex_lock.unlock();

    for(DWORD i = 0; i < loadExec.regexVec.size(); ++ i){
        regfree(&loadExec.regexVec[i]);
    }
    return true;
}

