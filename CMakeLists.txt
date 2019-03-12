
cmake_minimum_required(VERSION 2.8)

project( UMN C CXX )

set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fpermissive -pthread " )
SET( CMAKE_EXE_LINKER_FLAGS "-pthread" )

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


find_package( UV REQUIRED )
if( UV_FOUND )
    include_directories( ${UV_INCLUDE_DIRS} )
    set( DEPS "${DEPS};${UV_LIBRARIES}" )
else()
    message("oops :/")
endif()

INCLUDE_DIRECTORIES(./extdeps)

include_directories( ./sandbox/include )
file(GLOB_RECURSE PROJSRC ${PROJECT_SOURCE_DIR}/sandbox/*.c*)
file(GLOB_RECURSE PROJHEADERS {PROJECT_SOURCE_DIR}/sandbox/*.h)

add_executable( umn ${PROJSRC} ${PROJHEADERS} )

target_link_libraries( umn ${DEPS} )

# Install target
install(TARGETS umn DESTINATION bin)
