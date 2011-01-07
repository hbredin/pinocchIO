cmake_minimum_required (VERSION 2.6)
project (pinocchIO)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-modules")

# The version number.
set (PINOCCHIO_VERSION_MAJOR 0)
set (PINOCCHIO_VERSION_MINOR 2)
set (PINOCCHIO_VERSION_PATCH 2)
set (PINOCCHIO_RELEASE_DATE "2010-12-01")

set (PINOCCHIO_VERSION ${PINOCCHIO_VERSION_MAJOR}.${PINOCCHIO_VERSION_MINOR})
set (PINOCCHIO_API_VERSION ${PINOCCHIO_VERSION}.${PINOCCHIO_VERSION_PATCH})

# Compiler FLAGS
if (NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif (NOT CMAKE_BUILD_TYPE)
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -DBZ_DEBUG -ftemplate-depth-30 -g2" CACHE STRING "CXX DEBUG FLAGS" FORCE)
set(CMAKE_C_FLAGS_DEBUG "-DDEBUG -DBZ_DEBUG -g2" CACHE STRING "C DEBUG FLAGS" FORCE)
add_definitions(-Wno-write-strings)

find_package(HDF5 REQUIRED)
find_package(LIBCONFIG)

# install paths
set(CMAKE_INSTALL_BINDIR bin CACHE PATH "Output directory for programs")
set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Output directory for libraries")
set(CMAKE_INSTALL_INCLUDE include CACHE PATH "Output directory for header files")

# add targets
add_definitions(-DVERSION="${PINOCCHIO_API_VERSION}")
add_definitions(-DRELEASEDATE="${PINOCCHIO_RELEASE_DATE}")
add_subdirectory (library)
add_subdirectory (tools)
