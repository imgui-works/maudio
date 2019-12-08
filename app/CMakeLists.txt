find_package(SDL2 REQUIRED)
find_package(Zep REQUIRED)
set_target_properties(SDL2::SDL2 PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)
set_target_properties(Zep::Zep PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)

set(MAUDIO_APP_ROOT ${CMAKE_CURRENT_LIST_DIR}/../)

set(MAUDIO_APP_SOURCE
    ${MAUDIO_ROOT}/app/main.cpp
    ${MAUDIO_ROOT}/app/audio.cpp
    ${MAUDIO_ROOT}/app/zepeditor.h
    ${MAUDIO_ROOT}/app/CMakeLists.txt
    )

if(WIN32)
    # Let windows know that the app is DPI Aware
    SET(MANIFEST_SOURCE dpiawarescaling.manifest)
endif()

add_executable (MAudio_App WIN32
    ${MAUDIO_APP_SOURCE}
    ${GRAPH_SOURCE}
    ${MANIFEST_SOURCE}
    ) # Win32 ignored on non-windows

target_include_directories(MAudio_App
    PRIVATE
    ${MAUDIO_ROOT}/app
    ${MAUDIO_ROOT}/m3rdparty
    ${CMAKE_BINARY_DIR}
    )

target_link_libraries (MAudio_App
    PRIVATE
    MAudio 
    Zep::Zep
    SDL2::SDL2-static
    SDL2::SDL2main
    ws2_32
    ${PLATFORM_LINKLIBS}
    )

target_compile_definitions(MAudio_App PUBLIC -DZEP_USE_SDL)

add_dependencies(MAudio_App MAudio unittests)

copy_existing_files(MAudio_App ${MUTILS_INCLUDE_DIR}/chibi/init-7.scm ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration) )

source_group ("m3rdparty\\gl" REGULAR_EXPRESSION "(gl3w)+.*")
source_group ("Source" FILES ${MAUDIO_APP_SOURCE})

