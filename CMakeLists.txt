
cmake_minimum_required(VERSION 2.8)

project( UMN CXX )

set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fpermissive -pthread " )
SET( CMAKE_EXE_LINKER_FLAGS "-pthread" )

option(USE_NANOMSG "Build with nanomsg components" ON)

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

include_directories( ./include )

if( USE_NANOMSG )
    add_subdirectory( ${PROJECT_SOURCE_DIR}/extdeps/nanomsg )
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/extdeps/nanomsg/src/ )
    file(GLOB_RECURSE NNHEADERS ${PROJECT_SOURCE_DIR}/extdeps/nanomsg/src/*.h)

    file(GLOB_RECURSE NN_TRANSPORT_HEADERS ${PROJECT_SOURCE_DIR}/nanomsg_transport/*.h)
    file(GLOB_RECURSE NN_TRANSPORT_SRC ${PROJECT_SOURCE_DIR}/nanomsg_transport/*.cc)

    set( DEPS "${DEPS};nanomsg" )
endif()

file(GLOB_RECURSE PROJSRC ${PROJECT_SOURCE_DIR}/src/*.c*)
file(GLOB_RECURSE PROJHEADERS ${PROJECT_SOURCE_DIR}/include/*.h)

add_executable( umn ${PROJSRC} ${PROJHEADERS} ${NN_TRANSPORT_HEADERS} ${NN_TRANSPORT_SRC} )

if( USE_NANOMSG )
add_dependencies( umn nanomsg )
endif()

target_link_libraries( umn ${DEPS} )

# Install target
install(TARGETS umn DESTINATION bin)
