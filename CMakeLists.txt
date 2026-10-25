cmake_minimum_required(VERSION 3.12)
project(ModeratedLMS CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(ModeratedLMS
    main.cpp
    sqlite3.c
)

if(NOT WIN32)
    target_link_libraries(ModeratedLMS PRIVATE pthread dl)
else()
    target_link_libraries(ModeratedLMS PRIVATE sqlite3)
endif()