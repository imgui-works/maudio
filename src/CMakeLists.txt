find_package(MUtils REQUIRED)
find_package(NodeGraph REQUIRED)
set_target_properties(MUtils::MUtils PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)
set_target_properties(NodeGraph::NodeGraph PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)

set(MAUDIO_SOURCE
${MAUDIO_ROOT}/src/maudio.cpp
${MAUDIO_ROOT}/src/mosc.cpp
${MAUDIO_ROOT}/src/wavetable.cpp
${MAUDIO_ROOT}/src/maudio_utils.cpp
${MAUDIO_ROOT}/src/msamples.cpp

${MAUDIO_ROOT}/src/node_audiobase.cpp
${MAUDIO_ROOT}/src/node_adsr.cpp
${MAUDIO_ROOT}/src/node_oscillator.cpp
${MAUDIO_ROOT}/src/node_file_output.cpp
${MAUDIO_ROOT}/src/node_bitcrush.cpp
${MAUDIO_ROOT}/src/node_chorus.cpp
${MAUDIO_ROOT}/src/node_noise.cpp
${MAUDIO_ROOT}/src/node_mixer.cpp

${MAUDIO_ROOT}/include/maudio/maudio.h
${MAUDIO_ROOT}/include/maudio/mosc.h
${MAUDIO_ROOT}/include/maudio/wavetable.h
${MAUDIO_ROOT}/include/maudio/msamples.h
${MAUDIO_ROOT}/include/maudio/maudio_utils.h

${MAUDIO_ROOT}/include/maudio/node_audiobase.h
${MAUDIO_ROOT}/include/maudio/node_adsr.h
${MAUDIO_ROOT}/include/maudio/node_oscillator.h
${MAUDIO_ROOT}/include/maudio/node_file_output.h
${MAUDIO_ROOT}/include/maudio/node_bitcrush.h
${MAUDIO_ROOT}/include/maudio/node_chorus.h
${MAUDIO_ROOT}/include/maudio/node_noise.h
${MAUDIO_ROOT}/include/maudio/node_mixer.h

${MAUDIO_ROOT}/src/CMakeLists.txt)

set(SCHEME_SOURCE
${MAUDIO_ROOT}/src/scheme/scheme.cpp
${MAUDIO_ROOT}/include/maudio/scheme/scheme.h
)

set(OSCPACK_SOURCE
${M3RDPARTY_DIR}/oscpack/src/osc/OscOutboundPacketStream.cpp
${M3RDPARTY_DIR}/oscpack/src/osc/OscPrintReceivedElements.cpp
${M3RDPARTY_DIR}/oscpack/src/osc/OscReceivedElements.cpp
${M3RDPARTY_DIR}/oscpack/src/osc/OscTypes.cpp
${M3RDPARTY_DIR}/oscpack/src/ip/IpEndpointName.cpp)

if (WIN32)
list(APPEND OSCPACK_SOURCE 
    ${M3RDPARTY_DIR}/oscpack/src/ip/win32/NetworkingUtils.cpp
    ${M3RDPARTY_DIR}/oscpack/src/ip/win32/UdpSocket.cpp)
endif()


# Create the library - No depenencies on anything else in this file
add_library(MAudio STATIC 
    ${MAUDIO_SOURCE}
    ${OSCPACK_SOURCE}
    ${SCHEME_SOURCE}
    )
add_library(MAudio::MAudio ALIAS MAudio)

target_link_libraries(MAudio 
PUBLIC 
    NodeGraph::NodeGraph
    MUtils::MUtils)

target_include_directories(MAudio 
PRIVATE
    ${CMAKE_BINARY_DIR}
    ${MAUDIO_ROOT}/include
    ${M3RDPARTY_DIR}
    ${M3RDPARTY_DIR}/oscpack/include
PUBLIC
    $<BUILD_INTERFACE:${MAUDIO_ROOT}/include>
    $<INSTALL_INTERFACE:include>)

# Set locations for components
set_target_properties(MAudio PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Install the binary
install(TARGETS MAudio
    EXPORT maudio-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${LIBLEGACY_INCLUDE_DIRS}
)

# Install the include files
install(DIRECTORY ${MAUDIO_ROOT}/include/maudio
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

SOURCE_GROUP ("MAudio" FILES ${MAUDIO_SOURCE})
SOURCE_GROUP ("NodeGraph" FILES ${NODEGRAPH_MODEL} ${NODEGRAPH_VIEW})
SOURCE_GROUP ("OscPack" FILES ${OSCPACK_SOURCE})
SOURCE_GROUP ("Scheme" FILES ${SCHEME_SOURCE})

