cmake_minimum_required(VERSION 3.7)
project(cursed_taskmanager_installer)
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES Main.cpp)
add_executable(cursed_taskmanager_installer ${SOURCE_FILES})

if(WIN32)
    if(MSVC)
        set_target_properties(cursed_taskmanager_installer PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:windows")
        set_target_properties(cursed_taskmanager_installer PROPERTIES COMPILE_DEFINITIONS_DEBUG "_WINDOWS")
        set_target_properties(cursed_taskmanager_installer PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:windows")
        set_target_properties(cursed_taskmanager_installer PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_WINDOWS")
        set_target_properties(cursed_taskmanager_installer PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows")
        set_target_properties(cursed_taskmanager_installer PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:windows")
		if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
			string(REGEX REPLACE "/W[0-4]" ${WARN_LEVEL} CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
		else()
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARN_LEVEL}")
		endif()
    else()
        message(SEND_ERROR "You are using an unsupported Windows compiler! (Not MSVC)")
    endif()
else()
    message(SEND_ERROR "You are on an unsupported platform! (Not Win32)")
endif()

set_target_properties(
        cursed_taskmanager_installer
        PROPERTIES
        OUTPUT_NAME "cursed_taskmgr"
        SUFFIX ".exe"
)