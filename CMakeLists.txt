cmake_minimum_required(VERSION 3.10)

file(READ "version.txt" ver)
string(REGEX MATCH "VERSION_MAJOR ([0-9]*)" _ ${ver})
set(ver_major ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_MINOR ([0-9]*)" _ ${ver})
set(ver_minor ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_PATCH ([0-9]*)" _ ${ver})
set(ver_patch ${CMAKE_MATCH_1})

project(turtle VERSION ${ver_major}.${ver_minor}.${ver_patch})
message("version: ${PROJECT_VERSION}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/version.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.hpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Boost 1.71 COMPONENTS system REQUIRED)

find_path(ZeroMQ_INCLUDE_DIR zmq.h PATHS ${ZeroMQ_DIR})
find_library(ZeroMQ_LIBRARY NAMES zmq PATHS ${ZeroMQ_DIR})

add_subdirectory(proto)

set(SVC_HDR_FILES
        src/iot_service/microsvc_controller.hpp
        src/iot_service/phoenix_connector.hpp
        src/iot_service/lion_connector.hpp
        src/iot_service/task_manager.hpp
        src/utils/config.hpp
        src/utils/config_st.hpp
        src/utils/interrupt_handler.hpp
        src/utils/logger.hpp
        src/utils/logger.hpp
        )

set(SVC_SRC_FILES
        src/iot_service/microsvc_controller.cpp
        src/iot_service/phoenix_connector.cpp
        src/iot_service/lion_connector.cpp
        src/iot_service/task_manager.cpp
        src/utils/config.cpp
        src/utils/logger.cpp
        )

set(SVC_SRC_MAIN_FILES src/main.cpp)

include_directories(src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME} ${SVC_SRC_MAIN_FILES} ${SVC_SRC_FILES} ${SVC_HDR_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/args)
target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/cppzmq)
target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/json/include)
target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/spdlog/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${ZeroMQ_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${ZeroMQ_LIBRARY} lion_proto phoenix_proto)

install (TARGETS ${PROJECT_NAME} DESTINATION bin)
