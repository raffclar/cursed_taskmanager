cmake_minimum_required(VERSION 3.7)
project(cursed_taskmanager_dll)
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES dllmain.cpp Zalgo.h Zalgo.cpp)
add_library(cursed_taskmanager_dll MODULE ${SOURCE_FILES})

set_target_properties(
        cursed_taskmanager_dll
        PROPERTIES
        OUTPUT_NAME "cursed_taskmgr"
        SUFFIX ".dll"
)
