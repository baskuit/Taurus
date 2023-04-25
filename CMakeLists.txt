cmake_minimum_required(VERSION 3.0)
project(Taurus)
set (CMAKE_CXX_STANDARD 20)

include_directories(
    src
    surskit
    surskit/src
    # surskit/gambit/src
    # wrapsire
)

# add_library(wrapsire INTERFACE)
# target_include_directories(wrapsire INTERFACE wrapsire/include)

add_subdirectory(surskit/gambit)
add_subdirectory(wrapsire)

# Testing
file(GLOB TEST_FILES tests/*.cc)

foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} gambit)
    target_link_libraries(${TEST_NAME} engine-wrapper)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    set_target_properties(${TEST_NAME} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
endforeach()
enable_testing()