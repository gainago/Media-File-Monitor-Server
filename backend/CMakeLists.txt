cmake_minimum_required(VERSION 3.16)
project(server_monitor_media_files)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O0 -g")

include_directories(
    ${CMAKE_SOURCE_DIR}/Parser
    ${CMAKE_SOURCE_DIR}/Thread
    ${CMAKE_SOURCE_DIR}/nlohmann
    ${CMAKE_SOURCE_DIR}/httplib
    ${CMAKE_SOURCE_DIR}/Server/include
    ${CMAKE_SOURCE_DIR}/
    )
set(PATH_TO_PARSER "${CMAKE_SOURCE_DIR}/Parser")


add_executable(${PROJECT_NAME}
    ${PATH_TO_PARSER}/Parser.cpp
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/Server/src/Server.cpp
)

target_link_libraries(${PROJECT_NAME} pthread)