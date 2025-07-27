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

link_directories(${PROJECT_SOURCE_DIR}/lib)

# 查找所有源文件并创建库
file(GLOB SRC_FILES "src/*.cpp" "src/multi-core/*.cpp")
add_library(AdaptSimLib ${SRC_FILES})

# 为库目标指定头文件搜索路径
# PUBLIC 属性会使链接到此库的目标也继承这些路径
target_include_directories(AdaptSimLib PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/model_include/v_model
        ${PROJECT_SOURCE_DIR}/include/v_include
)

# 创建主可执行文件并链接库
add_executable(AdaptSim main.cpp)
target_link_libraries(AdaptSim PRIVATE AdaptSimLib)

# --- 测试目标 ---
enable_testing()

# 创建测试可执行文件并链接库
add_executable(test_example tests/test_example.cpp)
target_link_libraries(test_example PRIVATE AdaptSimLib)
add_test(NAME ExampleTest COMMAND test_example)

# --- 自定义目标 ---
add_custom_target(
        git_commit
        COMMAND ${CMAKE_MAKE_PROGRAM} commit msg="Commit from CMake build"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Calling Makefile's commit target to create a git commit"
        VERBATIM
)