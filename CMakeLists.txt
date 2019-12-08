cmake_minimum_required(VERSION 3.2)

message(STATUS " CMakeLists: MAudio")

# Global Options
option (BUILD_APP "Make the test app" ON)
option (BUILD_TESTS "Make the tests" ON)

# Global Settings
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (CMAKE_BUILD_TYPE MATCHES Coverage)
    set (CMAKE_BUILD_TYPE "Debug")
    set (PROJECT_COVERAGE ON)
endif()

set(CMAKE_DEBUG_POSTFIX "-debug")
set(CMAKE_RELWITHDEBINFO_POSTFIX "-reldbg")

project (MAudio 
    LANGUAGES CXX C
    VERSION 0.5.0)

# Used Packages
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(m3rdparty/cmake/copy_files.cmake)

# config_app.h checks
# This makes a config_shared.h file which can be included for system settings
#  Must be after setting up the platforms
set (MAUDIO_ROOT ${CMAKE_CURRENT_LIST_DIR})
set (M3RDPARTY_DIR ${CMAKE_CURRENT_LIST_DIR}/m3rdparty)

configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/config_app.h.cmake ${CMAKE_BINARY_DIR}/config_app.h)

# Set all compiler flags 
include(cmake/all.cmake)

# The main library
include(src/CMakeLists.txt)

include(tests/CMakeLists.txt)
include(app/CMakeLists.txt)

# Make the CMake bits that ensure find_package does the right thing
install(EXPORT maudio-targets
    FILE maudio-targets.cmake
    NAMESPACE MAudio::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/maudio
)

configure_package_config_file(
    ${MAUDIO_ROOT}/cmake/maudio-config.cmake.in
    ${CMAKE_BINARY_DIR}/cmake/maudio-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/maudio
)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/cmake/maudio-config-version.cmake
    VERSION 1.0
    COMPATIBILITY AnyNewerVersion
)

install(
    FILES
        ${CMAKE_BINARY_DIR}/cmake/maudio-config.cmake
        ${CMAKE_BINARY_DIR}/cmake/maudio-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/maudio
)

