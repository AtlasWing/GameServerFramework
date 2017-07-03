MESSAGE(STATUS "builder srcdir="${PROJECT_SOURCE_DIR}" binarydir="${PROJECT_BINARY_DIR}" cmake binarydir="${CMAKE_CURRENT_SOURCE_DIR})

MACRO (ADD_PCH_RULE  _header_filename _src_list)
    SET(_gch_filename "${_header_filename}.gch")
    LIST(APPEND ${_src_list} ${_gch_filename})
    SET (_args ${CMAKE_CXX_FLAGS})
    LIST(APPEND _args ${DEFINITIONS} -c ${_header_filename} -o ${_gch_filename})
    GET_DIRECTORY_PROPERTY(DIRINC INCLUDE_DIRECTORIES)
    foreach (_inc ${DIRINC})
        LIST(APPEND _args "-I" ${_inc})
    endforeach(_inc ${DIRINC})
    SEPARATE_ARGUMENTS(_args)
    add_custom_command(OUTPUT ${_gch_filename}
        COMMAND rm -f ${_gch_filename}
        COMMAND ${CMAKE_CXX_COMPILER}
        ${CMAKE_CXX_COMPILER_ARG1} ${_args}
        DEPENDS ${_header_filename}
        )
ENDMACRO(ADD_PCH_RULE _header_filename _src_list)

SET(CMAKE_BUILD_TYPE Debug)
SET(ThirdParty "${PROJECT_SOURCE_DIR}/third_party")

#SET(PROTOBUFF_INCLUDE "${ThirdParty}/protobuf-2.5.0/gtest/include")
SET(PROTOBUFF_INCLUDE "${ThirdParty}/protobuf-2.5.0/src")
SET(MYSQL_INCLUDE "${ThirdParty}/mysql-connector-c-6.0.0/include")
SET(MYSQL++_INCLUDE "${ThirdParty}/mysql++-3.1.0/include")
SET(TINYXML_INCLUDE "${ThirdParty}/tinyxml-2.6.1")
SET(LUA_INCLUDE "${ThirdParty}/LuaJIT-2.0.0-beta10/include")
SET(TOLUA++_INCLUDE "${ThirdParty}/tolua++-1.0.93/include")
SET(LIBSIGC++_INCLUDE "${ThirdParty}/libsigc++-2.2.8")
SET(CURL_INCLUDE "${ThirdParty}/curl-4.2.0/include")
SET(ZLIB_INCLUDE "${ThirdParty}/zlib-1.2.4")
SET(OPENSSL_INCLUDE "${ThirdParty}/openssl-1.0.1/include")
SET(RAPIDJSON_INCLUDE "${ThirdParty}/rapidjson/include")
SET(ARES_INCLUDE "${ThirdParty}/c-ares-1.10.0/include")
SET(SNAPPY_INCLUDE "${ThirdParty}/snappy-1.1.1/include")

SET(PROTOBUFF_BIN "${ThirdParty}/protobuf-2.5.0/src/protoc")
SET(LUA_LIB "${ThirdParty}/LuaJIT-2.0.0-beta10/lib/libluajit-5.1.a")
SET(TOLUA++_BIN "${ThirdParty}/tolua++-1.0.93/bin/tolua++")
SET(PROTOBUFF_LIB "${ThirdParty}/protobuf-2.5.0/src/.libs/libprotobuf.a")
SET(TOLUA++_LIB "${ThirdParty}/tolua++-1.0.93/lib/libtolua++.a")
SET(LIBSIGC++_LIB "${ThirdParty}/libsigc++-2.2.8/lib/libsigc-2.0.a")
SET(CURL_LIB "${ThirdParty}/curl-4.2.0/lib/libcurl.a")
SET(ARES_LIB "${ThirdParty}/c-ares-1.10.0/lib/libcares.a")
SET(SNAPPY_LIB "${ThirdParty}/snappy-1.1.1/lib/libsnappy.a")

SET(GOOGLE_PERFTOOLS_LIB_PATH "${ThirdParty}/gperftools-2.1/lib")
SET(GOOGLE_PERFTOOLS_BIN "${ThirdParty}/gperftools-2.1/bin/pprof")
SET(GOOGLE_PERFTOOLS_INCLUDE "${ThirdParty}/gperftools-2.1/include")
SET(OPENSSL_LIB_PATH "${ThirdParty}/openssl-1.0.1/lib")
SET(CURL_LIB_PATH "${ThirdParty}/curl-4.2.0/lib")

SET(TOMCRYPT_LIB "${PROJECT_SOURCE_DIR}/platform/base_util/crypto/libtomcrypt_116/libtomcrypt64.a")
SET(PTHREAD_64 "/usr/lib64/libpthread.so")
SET(DL_64 "/usr/lib64/libdl.so")
SET(RT_64 "/usr/lib64/librt.so")

SET(GAME_COMMON_TOLUA_PKG "${PROJECT_SOURCE_DIR}/game/LuaPkg/nGameScript.pkg")
FILE(STRINGS ${GAME_COMMON_TOLUA_PKG} GAME_COMMON_LUA_PKG_DEPEND_LIST REGEX ".*h\"")
STRING(REGEX REPLACE "[^;]*(\"([^;]*h)\")[^;]*" "\\2" GAME_COMMON_LUA_PKG_DEPEND "${GAME_COMMON_LUA_PKG_DEPEND_LIST}")

SET(THIRD_PARTY_INCLUDE_PATH "${ZLIB_INCLUDE}
${TINYXML_INCLUDE}
${PROTOBUFF_INCLUDE}
${LIBSIGC++_INCLUDE}
${MYSQL_INCLUDE}
${MYSQL++_INCLUDE}
${LUA_INCLUDE}
${TOLUA++_INCLUDE}
${GOOGLE_PERFTOOLS_INCLUDE}
${CURL_INCLUDE}
${OPENSSL_INCLUDE}
${RAPIDJSON_INCLUDE}
${SNAPPY_INCLUDE}")

SET(THIRD_PARTY_INCLUDE_PATH_A "${ZLIB_INCLUDE}
${TINYXML_INCLUDE}")
SET(THIRD_PARTY_INCLUDE_PATH_B "${PROTOBUFF_INCLUDE}
${LIBSIGC++_INCLUDE}")
SET(THIRD_PARTY_INCLUDE_PATH_C "${MYSQL_INCLUDE}
${MYSQL++_INCLUDE}")
SET(THIRD_PARTY_INCLUDE_PATH_D "${LUA_INCLUDE}
${TOLUA++_INCLUDE}")
SET(THIRD_PARTY_INCLUDE_PATH_E "${GOOGLE_PERFTOOLS_INCLUDE}
${CURL_INCLUDE}")
SET(THIRD_PARTY_INCLUDE_PATH_F "${OPENSSL_INCLUDE}
${RAPIDJSON_INCLUDE}")
SET(THIRD_PARTY_INCLUDE_PATH_G "${SNAPPY_INCLUDE}")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Werror -Winvalid-pch -Wno-sign-compare -DTICK_OPTIMIZE -DRUN_TYPE_SERVER -DSTAR_SERVER -DVISIBILITY_HIDDEN -std=c++0x -D_HASH_MAP_ -DSVC_ONE_SO -g -O0 -fPIC -pipe -fvisibility=hidden -L/usr/lib64 -L/lib64")
SET(CMAKE_CFLAGS "${CMAKE_CFLAGS} -m32 ")

MESSAGE(STATUS "CMAKE_CXX_FLAGS= ${CMAKE_CXX_FLAGS}")
get_directory_property(DIR_DEFINIATIONS COMPILE_DEFINITIONS)
MESSAGE(STATUS "COMPILE_DEFINITIONS= ${DIR_DEFINIATIONS}")

include(Custom.txt)

ADD_SUBDIRECTORY(protocol protocol)
ADD_SUBDIRECTORY(game game)
ADD_SUBDIRECTORY(gameImp gameImp)
ADD_SUBDIRECTORY(zoneServer zoneServer)
ADD_SUBDIRECTORY(dataServer dataServer)
ADD_SUBDIRECTORY(gatewayServer gatewayServer)
