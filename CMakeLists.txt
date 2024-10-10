cmake_minimum_required(VERSION 3.22)
project(SearchEngine)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)

# 查找必需的包
find_package(PkgConfig REQUIRED)
pkg_check_modules(HIREDIS REQUIRED hiredis)
pkg_check_modules(REDIS_PLUS_PLUS REQUIRED redis++)
pkg_check_modules(LOG4CPP REQUIRED log4cpp)
pkg_check_modules(OPENSSL REQUIRED openssl)
find_package(Protobuf REQUIRED)

# 包含和链接工作流库
include_directories(/usr/local/include/workflow)
link_directories(/usr/local/lib)

# 查找 wfrest 库和头文件
find_path(WFREST_INCLUDE_DIR wfrest)
find_library(WFREST_LIB wfrest)

# 查找其他库
find_package(Threads REQUIRED)
find_library(CURL_LIBRARY curl)
find_library(CRYPTO_LIBRARY crypto)
find_library(OSS_SDK_LIBRARY alibabacloud-oss-cpp-sdk)
find_library(SIMPLE_AMQP_CLIENT_LIBRARY SimpleAmqpClient)

find_library(SRPC_LIBRARY srpc)

# 包含头文件目录
include_directories(
        ${HIREDIS_INCLUDE_DIRS}
        ${REDIS_PLUS_PLUS_INCLUDE_DIRS}
        ${LOG4CPP_INCLUDE_DIRS}
        ${OPENSSL_INCLUDE_DIRS}
        ${WFREST_INCLUDE_DIR}
        ${Protobuf_INCLUDE_DIRS}
)

# 链接库目录
link_directories(
        ${HIREDIS_LIBRARY_DIRS}
        ${REDIS_PLUS_PLUS_LIBRARY_DIRS}
        ${LOG4CPP_LIBRARY_DIRS}
        ${OPENSSL_LIBRARY_DIRS}
)

# 添加可执行文件并列出所有源文件
add_executable(SearchEngine
        main.cpp
        # 其他源文件
)

# 链接库到可执行文件
target_link_libraries(SearchEngine
        ${HIREDIS_LIBRARIES}
        ${REDIS_PLUS_PLUS_LIBRARIES}
        ${LOG4CPP_LIBRARIES}
        ${OPENSSL_LIBRARIES}
        ${Protobuf_LIBRARIES}
        workflow
        wfrest
        ${OSS_SDK_LIBRARY}
        ${CURL_LIBRARY}
        ${CRYPTO_LIBRARY}
        Threads::Threads
        ${SIMPLE_AMQP_CLIENT_LIBRARY}
        rt
        lz4
        snappy
        crypt
        ${SRPC_LIBRARY}
)
