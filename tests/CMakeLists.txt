if(BUILD_TESTS)

set (TEST_SOURCES
    tests/main.cpp
    tests/CMakeLists.txt
    ${MAUDIO_ROOT}/src/maudio.test.cpp
    ${TEST_SOURCES}
)

enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_HAS_TR1_TUPLE=0")

add_executable (unittests ${TEST_SOURCES} )

target_link_libraries (unittests 
    PRIVATE
        MAudio::MAudio
        ${PLATFORM_LINKLIBS}
        ${CMAKE_THREAD_LIBS_INIT})

add_test(unittests unittests)

target_include_directories(unittests PRIVATE
    ${M3RDPARTY_DIR}/nodegraph/include
    ${CMAKE_BINARY_DIR}
)

source_group(Tests FILES ${TEST_SOURCES})

endif()
