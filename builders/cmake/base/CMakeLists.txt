INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
    ${THIRD_PARTY_INCLUDE_PATH_A}
    ${THIRD_PARTY_INCLUDE_PATH_B}
    ${THIRD_PARTY_INCLUDE_PATH_C}
    ${THIRD_PARTY_INCLUDE_PATH_D}
    ${THIRD_PARTY_INCLUDE_PATH_E}
    ${THIRD_PARTY_INCLUDE_PATH_F}
    ${THIRD_PARTY_INCLUDE_PATH_G}
)

#ADD_DEFINITIONS(-DTIXML_USE_STL)

ADD_PCH_RULE(${PROJECT_SOURCE_DIR}/base/nSys.h SOURCES)
ADD_LIBRARY(base STATIC "${PROJECT_SOURCE_DIR}/base/nBase.cpp")
TARGET_LINK_LIBRARIES(base pthread rt dl baseImp
${PROJECT_SOURCE_DIR}/base/libbaseImp.a
${ThirdParty}/zlib-1.2.4/lib/libz.a
${ThirdParty}/tinyxml-2.6.1/lib/libtinyxml.a
${ThirdParty}/mysql++-3.1.0/lib/libmysqlpp.a
${ThirdParty}/mysql-connector-c-6.0.0/lib/libmysqlclient.a
${LUA_LIB}
${TOLUA++_LIB}
${LIBSIGC++_LIB}
${SNAPPY_LIB}
)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/base)
