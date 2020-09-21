# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindLibuv
-------

Finds the Libuv library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``Libuv::Libuv``
  The Libuv library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``Libuv_FOUND``
  True if the system has the Libuv library.
# ``Libuv_VERSION``
#   The version of the Libuv library which was found.
``Libuv_INCLUDE_DIRS``
  Include directories needed to use Libuv.
``Libuv_LIBRARIES``
  Libraries needed to link to Libuv.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``Libuv_INCLUDE_DIR``
  The directory containing ``libgo.h``.
``Libuv_LIBRARY``
  The path to the Libuv library.

HINTS
^^^^^
Libuv_ROOT_DIR
Where to find the base directory of Libuv.

#]=======================================================================]

find_package(PkgConfig)
list(APPEND CMAKE_MODULE_PATH ${Libuv_ROOT_DIR}/lib/pkgconfig)
set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH True)
pkg_check_modules(PC_Libuv REQUIRED libuv)

find_path(Libuv_INCLUDE_DIR
  NAMES uv.h
  PATHS ${PC_Libuv_INCLUDE_DIRS}

)
find_library(Libuv_LIBRARY
  NAMES libuv.so
  PATHS ${PC_Libuv_LIBRARY_DIRS}
)

find_library(Libuv_LIBRARY_STATIC
    libuv_a
    PATHS ${PC_Libuv_LIBRARY_DIRS}
)

set(Libuv_VERSION ${PC_Libuv_VERSION})

# find_path(Libuv_INCLUDE_DIR
#     uv.h
#     # PATHS /usr/include /usr/local/include ${Libuv_ROOT_DIR}/include
#     HINTS ${Libuv_ROOT_DIR}/include
#     # PATH_SUFFIXES libgo
# )

# find_library(Libuv_LIBRARY
#     libuv
#     PATHS /usr/lib /usr/local/lib ${Libuv_ROOT_DIR}/lib
# )

# find_library(Libuv_LIBRARY_STATIC
#     libuv_a
#     PATHS /usr/lib /usr/local/lib ${Libuv_ROOT_DIR}/lib
# )

include( FindPackageHandleStandardArgs )

# find_package_handle_standard_args(Libuv  
#     DEBAULT_MSG
#     Libuv_INCLUDE_DIR Libuv_LIBRARY

# ) 

find_package_handle_standard_args(Libuv  
    FOUND_VAR Libuv_FOUND
    REQUIRED_VARS Libuv_INCLUDE_DIR Libuv_LIBRARY
    VERSION_VAR Libuv_VERSION
#   [HANDLE_COMPONENTS]
#   [CONFIG_MODE]
    FAIL_MESSAGE "Can't Find Libuv !"
) 

if(Libuv-FOUND)
    message(STATUS "Libuv:${Libuv_INCLUDE_DIR}")
    set(Libuv_LIBRARIES ${Libuv_LIBRARY})
    set(Libuv_INCLUDE_DIRS ${Libuv_INCLUDE_DIR})
    # set(Libuv_DEFINITIONS ${PC_Libuv_CFLAGS_OTHER})
    mark_as_advanced(
    Libuv_INCLUDE_DIR
    Libuv_LIBRARY
    )
endif()

if(Libuv_FOUND AND NOT TARGET Libuv::Libuv)
  add_library(Libuv::Libuv UNKNOWN IMPORTED)
  set_target_properties(Libuv::Libuv PROPERTIES
    IMPORTED_LOCATION "${Libuv_LIBRARY}"
    # INTERFACE_COMPILE_OPTIONS "${PC_Libuv_CFLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${Libuv_INCLUDE_DIR}"
  )
endif()

# if(Libuv_FOUND)
#   if (NOT TARGET Libuv::Libuv)
#     add_library(Libuv::Libuv UNKNOWN IMPORTED)
#   endif()
#   if (Libuv_LIBRARY_RELEASE)
#     set_property(TARGET Libuv::Libuv APPEND PROPERTY
#       IMPORTED_CONFIGURATIONS RELEASE
#     )
#     set_target_properties(Libuv::Libuv PROPERTIES
#       IMPORTED_LOCATION_RELEASE "${Libuv_LIBRARY_RELEASE}"
#     )
#   endif()
#   if (Libuv_LIBRARY_DEBUG)
#     set_property(TARGET Libuv::Libuv APPEND PROPERTY
#       IMPORTED_CONFIGURATIONS DEBUG
#     )
#     set_target_properties(Libuv::Libuv PROPERTIES
#       IMPORTED_LOCATION_DEBUG "${Libuv_LIBRARY_DEBUG}"
#     )
#   endif()
#   set_target_properties(Libuv::Libuv PROPERTIES
#     # INTERFACE_COMPILE_OPTIONS "${PC_Libuv_CFLAGS_OTHER}"
#     INTERFACE_INCLUDE_DIRECTORIES "${Libuv_INCLUDE_DIR}"
#   )
# endif()
