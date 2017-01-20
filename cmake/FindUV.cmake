#.rst:
# FindUV
# --------
#
# Find uv 
#
# Find the native UV includes and library.  Once done this will define
#
# ::
#
#   UV_INCLUDE_DIRS   - where to find uv.h, etc.
#   UV_LIBRARIES      - List of libraries when using uv.
#   UV_FOUND          - True if uv found.
#
#
#

#=============================================================================
# Copyright 2001-2011 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

find_path(UV_INCLUDE_DIR NAMES uv.h)
find_library(UV_LIBRARY  NAMES uv)

mark_as_advanced(UV_LIBRARY UV_INCLUDE_DIR)

# handle the QUIETLY and REQUIRED arguments and set UV_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(UV REQUIRED_VARS UV_LIBRARY UV_INCLUDE_DIR
                                       VERSION_VAR UV_VERSION_STRING)

if(UV_FOUND)
    set(UV_INCLUDE_DIRS ${UV_INCLUDE_DIR})
    set(UV_LIBRARIES ${UV_LIBRARY})
    message(STATUS "UV_INCLUDE_DIRS ${UV_INCLUDE_DIRS}")
    message(STATUS "UV_LIBRARIES ${UV_LIBRARIES}")
endif()

