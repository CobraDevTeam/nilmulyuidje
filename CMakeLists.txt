CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

Project(Nilulyuidje)

#======================================================================
# Version
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "1")

SET(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

#======================================================================
# Recurse to subdirectories
#add_subdirectory(bench)
#add_subdirectory(examples)
#add_subdirectory(include)

add_executable(Examples examples/main.cpp)
target_include_directories(Examples
                           PUBLIC
                           "include"
                           )
