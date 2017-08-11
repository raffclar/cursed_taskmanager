cmake_minimum_required(VERSION 3.7)
project(cursed_taskmanager_installer)
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES main.cpp)
add_executable(cursed_taskmanager_installer ${SOURCE_FILES})
set_target_properties(cursed_taskmanager_installer PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")

set_target_properties(
        cursed_taskmanager_installer
        PROPERTIES
        OUTPUT_NAME "cursed_taskmgr"
        SUFFIX ".exe"
)