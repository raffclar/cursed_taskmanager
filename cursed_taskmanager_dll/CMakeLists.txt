cmake_minimum_required(VERSION 3.7)
project(cursed_taskmanager_dll)
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES Dllmain.cpp Zalgo.h Zalgo.cpp)
add_library(cursed_taskmanager_dll MODULE ${SOURCE_FILES})

if(WIN32)
    if(MSVC)
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
        cursed_taskmanager_dll
        PROPERTIES
        OUTPUT_NAME "cursed_taskmgr"
        SUFFIX ".dll"
)
