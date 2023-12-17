# Try to find the GMP library
# See http://www.gmplib.org/
#
# This module supports requiring a minimum version, e.g. you can do
#   find_package(GMP 2.3.0)
# to require version 2.3.0 to newer of GMP.
#
# Once done this will define
#
#  GMP_FOUND - system has GMP lib with correct version
#  GMP_INCLUDES - the GMP include directory
#  GMP_LIBRARIES - the GMP library
#  GMP_VERSION - GMP version

# Copyright (c) 2006, 2007 Montel Laurent, <montel@kde.org>
# Copyright (c) 2008, 2009 Gael Guennebaud, <g.gael@free.fr>
# Copyright (c) 2010 Jitse Niesen, <jitse@maths.leeds.ac.uk>
# Redistribution and use is allowed according to the terms of the BSD license.

# Set GMP_INCLUDES

find_path(GMP_INCLUDES
  NAMES
  gmp.h
  PATHS
  $ENV{GMPDIR}
  ${INCLUDE_INSTALL_DIR}
)

# Set GMP_FIND_VERSION to 1.0.0 if no minimum version is specified

if(NOT GMP_FIND_VERSION)
  if(NOT GMP_FIND_VERSION_MAJOR)
    set(GMP_FIND_VERSION_MAJOR 1)
  endif(NOT GMP_FIND_VERSION_MAJOR)
  if(NOT GMP_FIND_VERSION_MINOR)
    set(GMP_FIND_VERSION_MINOR 0)
  endif(NOT GMP_FIND_VERSION_MINOR)
  if(NOT GMP_FIND_VERSION_PATCH)
    set(GMP_FIND_VERSION_PATCH 0)
  endif(NOT GMP_FIND_VERSION_PATCH)

  set(GMP_FIND_VERSION "${GMP_FIND_VERSION_MAJOR}.${GMP_FIND_VERSION_MINOR}.${GMP_FIND_VERSION_PATCH}")
endif(NOT GMP_FIND_VERSION)


if(GMP_INCLUDES)

  # Set GMP_VERSION
  
  file(READ "${GMP_INCLUDES}/gmp.h" _GMP_version_header)
  
  string(REGEX MATCH "define[ \t]+__GNU_MP_VERSION[ \t]+([0-9]+)" _GMP_major_version_match "${_GMP_version_header}")
  set(GMP_MAJOR_VERSION "${CMAKE_MATCH_1}")
  string(REGEX MATCH "define[ \t]+__GNU_MP_VERSION_MINOR[ \t]+([0-9]+)" _GMP_minor_version_match "${_GMP_version_header}")
  set(GMP_MINOR_VERSION "${CMAKE_MATCH_1}")
  string(REGEX MATCH "define[ \t]+__GNU_MP_VERSION_PATCHLEVEL[ \t]+([0-9]+)" _GMP_patchlevel_version_match "${_GMP_version_header}")
  set(GMP_PATCHLEVEL_VERSION "${CMAKE_MATCH_1}")
  
  set(GMP_VERSION ${GMP_MAJOR_VERSION}.${GMP_MINOR_VERSION}.${GMP_PATCHLEVEL_VERSION})
  
  # Check whether found version exceeds minimum version
  
  if(${GMP_VERSION} VERSION_LESS ${GMP_FIND_VERSION})
    set(GMP_VERSION_OK FALSE)
    message(STATUS "GMP version ${GMP_VERSION} found in ${GMP_INCLUDES}, "
                   "but at least version ${GMP_FIND_VERSION} is required")
  else(${GMP_VERSION} VERSION_LESS ${GMP_FIND_VERSION})
    set(GMP_VERSION_OK TRUE)
  endif(${GMP_VERSION} VERSION_LESS ${GMP_FIND_VERSION})

endif(GMP_INCLUDES)

# Set GMP_LIBRARIES

find_library(GMP_LIBRARIES GMP PATHS $ENV{GMPDIR} ${LIB_INSTALL_DIR})

# Epilogue

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP DEFAULT_MSG
                                  GMP_INCLUDES GMP_LIBRARIES GMP_VERSION_OK)
mark_as_advanced(GMP_INCLUDES GMP_LIBRARIES)