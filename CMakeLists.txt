cmake_minimum_required(VERSION 3.31)
project(AdaptSim)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "找到 ccache，将用于加速编译。")
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
else()
    message(STATUS "未找到 ccache。")
endif()

set(CMAKE_CXX_STANDARD 26)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/v_include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

file(GLOB SRC_FILES src/*.cpp)
add_library(AdaptSimLib ${SRC_FILES})
add_executable(AdaptSim main.cpp)
target_link_libraries(AdaptSim PRIVATE AdaptSimLib)

# 测试目标
enable_testing()
add_executable(test_example tests/test_example.cpp)
target_link_libraries(test_example PRIVATE AdaptSimLib)
target_include_directories(test_example PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/v_include)
add_test(NAME ExampleTest COMMAND test_example)

# 添加综合测试目标


# 添加一个自定义目标来调用 Makefile 中的 commit 功能
# 使用方法: cmake --build <build_dir> --target git_commit
add_custom_target(
    git_commit
    COMMAND ${CMAKE_MAKE_PROGRAM} commit msg="Commit from CMake build"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Calling Makefile's commit target to create a git commit"
    VERBATIM
)
