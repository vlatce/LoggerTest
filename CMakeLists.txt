cmake_minimum_required(VERSION 3.8)

project (testcmake)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTHITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

#message(STATUS "CMAKE_SYSTEM_NAME          = ${CMAKE_SYSTEM_NAME}"         )
#message(STATUS "CMAKE_CXX_COMPILER_ID      = ${CMAKE_CXX_COMPILER_ID}"     )
#message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")
#message(STATUS "CMAKE_COMPILER_IS_GNUCXX   = ${CMAKE_COMPILER_IS_GNUCXX}"  )

if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++17" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++17")
    endif()
endif()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG(-std=c++17 HAVE_FLAG_STD_CXX17)
if(HAVE_FLAG_STD_CXX17)
    # Have -std=c++17, use it
	add_compile_options("-std=c++17")
#else()
#    CHECK_CXX_COMPILER_FLAG(-std=c++1z HAVE_FLAG_STD_CXX1Z)
#    if(HAVE_FLAG_STD_CXX1Z)
#        # Have -std=c++1z, use it
#		add_compile_options("-std=c++1z")
#    else()
#        # And so on and on...
#		CHECK_CXX_COMPILER_FLAG(-std=c++14 HAVE_FLAG_STD_CXX14)
#		if(HAVE_FLAG_STD_CXX14)
#			add_compile_options("-std=c++14")
#		endif()
#    endif()
endif()

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system program_options date_time filesystem thread)

if(Boost_FOUND)
	add_definitions( -DBOOST_ALL_NO_LIB )
#add_definitions( -DBOOST_ALL_DYN_LINK )
#add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
    include_directories(${Boost_INCLUDE_DIRS}) 
	#link_directories(${Boost_LIBRARIES})
    add_executable (TestLogger main.cpp fmt/format.cpp)
    target_link_libraries(TestLogger ${Boost_LIBRARIES})
	
	if (WIN32)
		get_filename_component(UTF_PATH "${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG}" PATH)
		
		if (CMAKE_BUILD_TYPE STREQUAL "Debug")
			get_filename_component(UTF_PROGRAM_OPTIONS_NAME "${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG}" NAME_WE)
			get_filename_component(UTF_SYSTEM_NAME "${Boost_SYSTEM_LIBRARY_DEBUG}" NAME_WE)
			get_filename_component(UTF_DATE_TIME_NAME "${Boost_DATE_TIME_LIBRARY_DEBUG}" NAME_WE)
			get_filename_component(UTF_FILE_SYSTEM_NAME "${Boost_FILESYSTEM_LIBRARY_DEBUG}" NAME_WE)

			#message(STATUS "${UTF_PATH}/${UTF_BASE_NAME_DEBUG}.dll")
			#message(STATUS "${Boost_FILESYSTEM_LIBRARY_DEBUG}")
			#install(FILES ${UTF_PATH}/${UTF_BASE_NAME_DEBUG}.dll DESTINATION ${CMAKE_BINARY_DIR} CONFIGURATIONS Debug)
	
			file(COPY ${UTF_PATH}/${UTF_PROGRAM_OPTIONS_NAME}.dll DESTINATION ${CMAKE_BINARY_DIR} NO_SOURCE_PERMISSIONS)
			file(COPY ${UTF_PATH}/${UTF_SYSTEM_NAME}.dll DESTINATION ${CMAKE_BINARY_DIR} NO_SOURCE_PERMISSIONS)
			file(COPY ${UTF_PATH}/${UTF_DATE_TIME_NAME}.dll DESTINATION ${CMAKE_BINARY_DIR} NO_SOURCE_PERMISSIONS)
			file(COPY ${UTF_PATH}/${UTF_FILE_SYSTEM_NAME}.dll DESTINATION ${CMAKE_BINARY_DIR} NO_SOURCE_PERMISSIONS)
		elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")

			get_filename_component(UTF_PROGRAM_OPTIONS_NAME "${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE}" NAME_WE)
			get_filename_component(UTF_SYSTEM_NAME "${Boost_SYSTEM_LIBRARY_RELEASE}" NAME_WE)
			get_filename_component(UTF_DATE_TIME_NAME "${Boost_DATE_TIME_LIBRARY_RELEASE}" NAME_WE)
			get_filename_component(UTF_FILE_SYSTEM_NAME "${Boost_FILESYSTEM_LIBRARY_RELEASE}" NAME_WE)

			#message(STATUS "${UTF_PATH}/${UTF_PROGRAM_OPTIONS_NAME}.dll")
			#message(STATUS "${CMAKE_BINARY_DIR}")
			#install(FILES ${UTF_PATH}/${UTF_BASE_NAME_DEBUG}.dll DESTINATION ${CMAKE_BINARY_DIR} CONFIGURATIONS Debug)
	
			file(COPY ${UTF_PATH}/${UTF_PROGRAM_OPTIONS_NAME}.dll DESTINATION ${CMAKE_BINARY_DIR} NO_SOURCE_PERMISSIONS)
			file(COPY ${UTF_PATH}/${UTF_SYSTEM_NAME}.dll DESTINATION ${CMAKE_BINARY_DIR} NO_SOURCE_PERMISSIONS)
			file(COPY ${UTF_PATH}/${UTF_DATE_TIME_NAME}.dll DESTINATION ${CMAKE_BINARY_DIR} NO_SOURCE_PERMISSIONS)
			file(COPY ${UTF_PATH}/${UTF_FILE_SYSTEM_NAME}.dll DESTINATION ${CMAKE_BINARY_DIR} NO_SOURCE_PERMISSIONS)
		elseif (CMAKE_BUILD_TYPE STREQUAL "Release")

			get_filename_component(UTF_PROGRAM_OPTIONS_NAME "${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE}" NAME_WE)
			get_filename_component(UTF_SYSTEM_NAME "${Boost_SYSTEM_LIBRARY_RELEASE}" NAME_WE)
			get_filename_component(UTF_DATE_TIME_NAME "${Boost_DATE_TIME_LIBRARY_RELEASE}" NAME_WE)
			get_filename_component(UTF_FILE_SYSTEM_NAME "${Boost_FILESYSTEM_LIBRARY_RELEASE}" NAME_WE)

			#message(STATUS "${UTF_PATH}/${UTF_PROGRAM_OPTIONS_NAME}.dll")
			#message(STATUS "${CMAKE_BINARY_DIR}")
			#install(FILES ${UTF_PATH}/${UTF_BASE_NAME_DEBUG}.dll DESTINATION ${CMAKE_BINARY_DIR} CONFIGURATIONS Debug)
	
			file(COPY ${UTF_PATH}/${UTF_PROGRAM_OPTIONS_NAME}.dll DESTINATION ${CMAKE_BINARY_DIR} NO_SOURCE_PERMISSIONS)
			file(COPY ${UTF_PATH}/${UTF_SYSTEM_NAME}.dll DESTINATION ${CMAKE_BINARY_DIR} NO_SOURCE_PERMISSIONS)
			file(COPY ${UTF_PATH}/${UTF_DATE_TIME_NAME}.dll DESTINATION ${CMAKE_BINARY_DIR} NO_SOURCE_PERMISSIONS)
			file(COPY ${UTF_PATH}/${UTF_FILE_SYSTEM_NAME}.dll DESTINATION ${CMAKE_BINARY_DIR} NO_SOURCE_PERMISSIONS)
		endif()
	endif()
endif()