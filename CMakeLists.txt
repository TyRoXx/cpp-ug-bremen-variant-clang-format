cmake_minimum_required(VERSION 2.8)
project(demo-variant-clang-format)

if(UNIX)
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		add_definitions("-std=c++1y")
	else()
		execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
		if(GCC_VERSION VERSION_GREATER 4.7)
			add_definitions(-std=c++1y)
		else()
			add_definitions(-std=c++0x)
		endif()
	endif()
	add_definitions("-Wall -Wextra -Wvla")
	add_definitions("-Wno-missing-field-initializers")
	add_definitions("-fvisibility=hidden -fvisibility-inlines-hidden")

	#currently not possible due to websocketpp and Boost 1.60
	#add_definitions("-Wconversion")

	option(EINKAEUFER_PEDANTIC "pass -pedantic to the compiler (may generate useless warnings)" OFF)
	if(EINKAEUFER_PEDANTIC)
		add_definitions("-pedantic")
	endif()
endif()

if(MSVC)
	#make Visual C++ 2013 build in parallel
	add_definitions("/MP")

	#we want more warnings
	add_definitions("/W4")

	#disable "'std::_Copy_n': Function call with parameters that may be unsafe"
	add_definitions("/wd4996")

	#workaround for Boost 1.55 Context error LNK2026
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup()
else()
	if(MSVC)
		set(Boost_USE_STATIC_LIBS ON) 
	endif()
	find_package(Boost REQUIRED COMPONENTS system filesystem date_time regex coroutine context thread chrono program_options unit_test_framework)
	if(UNIX)
		add_definitions("-DBOOST_TEST_DYN_LINK")
	endif()
	include_directories(SYSTEM ${Boost_INCLUDE_DIR})
	link_directories(${Boost_LIBRARY_DIR})

	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

	find_package(SQLite3 REQUIRED)
	include_directories(SYSTEM ${SQLITE3_INCLUDE_DIR})

	set(SILICIUM_INCLUDE_DIR "/usr/local/include" CACHE PATH "")
	include_directories(SYSTEM ${SILICIUM_INCLUDE_DIR})

	set(WEBSOCKETPP_INCLUDE_DIR "/usr/local/include" CACHE PATH "")
	include_directories(SYSTEM ${WEBSOCKETPP_INCLUDE_DIR})
endif()

if(WIN32)
	#Boost.Asio wants this for no reason
	add_definitions("-D_WIN32_WINDOWS")

	add_definitions("-DBOOST_ASIO_HAS_IOCP")
endif()

option(EINKAEUFER_NO_EXCEPTIONS "disable try/catch and throw; when something would normally throw, the process will std::terminate() instead" OFF)
if(EINKAEUFER_NO_EXCEPTIONS)
	if(MSVC)
		add_definitions("-D_HAS_EXCEPTIONS=0")
		add_definitions("-DBOOST_NO_CXX11_NOEXCEPT=")
		add_definitions("-DBOOST_ASIO_ERROR_CATEGORY_NOEXCEPT=")
		set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W3 /GR")
	else()
		add_definitions("-fno-exceptions")
	endif()
endif()

include_directories(.)

add_subdirectory("tests")

if(WIN32)
	set(CLANG_FORMAT "C:/Program Files/LLVM/bin/clang-format.exe" CACHE TYPE PATH)
else()
	set(CLANG_FORMAT "clang-format-3.7" CACHE TYPE PATH)
endif()
add_custom_target(clang-format COMMAND ${CLANG_FORMAT} -i ${formatted} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
