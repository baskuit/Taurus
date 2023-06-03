cmake_minimum_required(VERSION 3.0)
project(Taurus)
set (CMAKE_CXX_STANDARD 20)

include_directories(
    src
)

# set(CMAKE_CXX_FLAGS "-no-pie")


add_subdirectory(wrapsire)
add_subdirectory(surskit)

# Testing
file(GLOB TEST_FILES tests/*.cc)

foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} engine-wrapper)
    target_link_libraries(${TEST_NAME} surskit)

endforeach()
enable_testing()