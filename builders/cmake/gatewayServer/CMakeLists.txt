FILE(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/gatewayServer/*.cpp)
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

LINK_DIRECTORIES(${GOOGLE_PERFTOOLS_LIB_PATH} ${PROJECT_SOURCE_DIR}/base ${OPENSSL_LIB_PATH})

ADD_EXECUTABLE(gatewayServer ${SOURCES})

TARGET_LINK_LIBRARIES(gatewayServer unwind profiler crypto)
TARGET_LINK_LIBRARIES(gatewayServer tcmalloc)
TARGET_LINK_LIBRARIES(gatewayServer game ${PROJECT_SOURCE_DIR}/game/libgame.a)
TARGET_LINK_LIBRARIES(gatewayServer ${PROJECT_SOURCE_DIR}/protocol/libprotocol.a)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/gatewayServer)
