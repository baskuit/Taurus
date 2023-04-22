cmake_minimum_required(VERSION 3.0)
project(taurus)
set (CMAKE_CXX_STANDARD 20)


include_directories(
    surskit
    surskit/src
    surskit/gambit/src
    wrapsire
)

add_library(wrapsire INTERFACE)
target_include_directories(wrapsire INTERFACE include)

add_executable(test test.cc)
target_link_libraries(test INTERFACE wrapsire PUBLIC ${PROJECT_SOURCE_DIR}/wrapsire/dependencies/libpkmn-showdown.a)


# # Testing
# file(GLOB TEST_FILES test/*.cc)

# foreach(TEST_FILE ${TEST_FILES})
#     get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
#     add_executable(${TEST_NAME} ${TEST_FILE})
#     # target_link_libraries(${TEST_NAME} gambit)
#     add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
#     set_target_properties(${TEST_NAME} PROPERTIES
#                       RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
# endforeach()
# enable_testing()