cmake_minimum_required(VERSION 3.16)
project(server_monitor_media_files)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O0 -g")

include_directories(
    ${CMAKE_SOURCE_DIR}/backend/Parser
    ${CMAKE_SOURCE_DIR}/backend/Thread
    ${CMAKE_SOURCE_DIR}/backend/nlohmann
    ${CMAKE_SOURCE_DIR}/backend/httplib
    ${CMAKE_SOURCE_DIR}/backend/Server/include
    ${CMAKE_SOURCE_DIR}/backend/
    )
set(PATH_TO_PARSER "${CMAKE_SOURCE_DIR}/backend/Parser")


add_executable(${PROJECT_NAME}
    ${PATH_TO_PARSER}/Parser.cpp
    ${CMAKE_SOURCE_DIR}/backend/main.cpp
    ${CMAKE_SOURCE_DIR}/backend/Server/src/Server.cpp
)

target_link_libraries(${PROJECT_NAME} pthread)