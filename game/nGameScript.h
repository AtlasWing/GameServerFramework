#ifndef _N_GAME_SCRIPT_H_
#define _N_GAME_SCRIPT_H_

#include "base/Tidy.h"

#define FUNC_IMPORT extern "C" __attribute__ ((visibility ("protected")))

/// @brief	断言失败
void c_assertFail(const char* info);

/// @brief	获得脚本根目录
FUNC_IMPORT const char* c_getRootDir();
FUNC_IMPORT void c_debug(const char *pattern, ...);

/// @brief	获得文件夹下的所有文件名
const char* c_getFileListByFolder(const char* folder);

namespace command{
    namespace messagebox{
    };
};
using namespace command::messagebox;
#endif
