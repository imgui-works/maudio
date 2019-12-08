# Compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)
  include(${PROJECT_SOURCE_DIR}/cmake/g++.cmake)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  include(${PROJECT_SOURCE_DIR}/cmake/clang++.cmake)
elseif(MSVC)
  include(${PROJECT_SOURCE_DIR}/cmake/msvc.cmake)
else()
  message(WARNING "Unknown compiler, not setting flags")
endif()


if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCH_64 TRUE)
  set(PROCESSOR_ARCH "x64")
else()
  set(ARCH_64 FALSE)
  set(PROCESSOR_ARCH "x86")
endif()

# System flags
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    message(STATUS "TARGET_PC")
    set(TARGET_PC 1)

    include(${PROJECT_SOURCE_DIR}/cmake/pc.cmake)
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    message(STATUS "TARGET_MAC")
    SET(TARGET_MAC 1)
    INCLUDE(${PROJECT_SOURCE_DIR}/cmake/mac.cmake)
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    message(STATUS "TARGET_LINUX")
    SET(TARGET_LINUX 1)
    INCLUDE(${PROJECT_SOURCE_DIR}/cmake/linux.cmake)
endif()
	
message(STATUS "System: ${CMAKE_SYSTEM}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Debug Flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Release Flags: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "Arch: ${PROCESSOR_ARCH}")


# Function:                 EXCLUDE_FILES_FROM_DIR_IN_LIST
# Description:              Exclude all files from a list under a specific directory.
# Param _InFileList:        Input and returned List 
# Param _excludeDirName:    Name of the directory, which shall be ignored.
# Param _verbose:           Print the names of the files handled
function (exclude_files_from_dir_in_list _InFileList _excludeDirName _verbose)
  foreach (ITR ${_InFileList})
    if ("${_verbose}")
      message(STATUS "ITR=${ITR}")
    endif ("${_verbose}")

    if ("${ITR}" MATCHES "(.*)${_excludeDirName}(.*)")                   # Check if the item matches the directory name in _excludeDirName
      message(STATUS "Remove Item from List:${ITR}")
      list (REMOVE_ITEM _InFileList ${ITR})                              # Remove the item from the list
    endif ("${ITR}" MATCHES "(.*)${_excludeDirName}(.*)")

  endforeach(ITR)
  set(SOURCE_FILES ${_InFileList} PARENT_SCOPE)                          # Return the SOURCE_FILES variable to the calling parent

endfunction (exclude_files_from_dir_in_list)

