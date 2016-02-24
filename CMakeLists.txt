cmake_minimum_required(VERSION 2.8)
project(demo-variant-clang-format)

if(UNIX)
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		add_definitions("-std=c++1y")
	else()
		execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
		if(GCC_VERSION VERSION_GREATER 4.7)
			add_definitions(-std=c++1y)
			#relevant for GCC 5.X: use C++03 stdlib ABI because Conan builds Boost in C++03 mode apparently.
			add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
		else()
			add_definitions(-std=c++0x)
		endif()
	endif()
	add_definitions("-Wall -Wextra -Wvla")
	add_definitions("-Wno-missing-field-initializers")
	add_definitions("-fvisibility=hidden -fvisibility-inlines-hidden")

	#currently not possible due to websocketpp and Boost 1.60
	#add_definitions("-Wconversion")
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
	message(FATAL_ERROR "you must use Conan to install the dependencies")
endif()

if(WIN32)
	#Boost.Asio wants this for no reason
	add_definitions("-D_WIN32_WINDOWS")

	add_definitions("-DBOOST_ASIO_HAS_IOCP")
endif()

add_subdirectory("tests")

if(WIN32)
	set(CLANG_FORMAT "C:/Program Files/LLVM/bin/clang-format.exe" CACHE TYPE PATH)
else()
	set(CLANG_FORMAT "clang-format-3.7" CACHE TYPE PATH)
endif()
add_custom_target(clang-format COMMAND ${CLANG_FORMAT} -i ${formatted} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
