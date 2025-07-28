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
file(GLOB_RECURSE SRC_FILES_ADAPT_SIM "src/AdaptSim/*.cpp")
file(GLOB_RECURSE SRC_FILES_UTILS "src/AdaptSim/uitils/*.cpp")
file(GLOB_RECURSE SRC_FILES_VLTSTD "include/v_include/v_include/vltstd/*.cpp")
file(GLOB_RECURSE SRC_FILES_GTKWAVE "include/v_include/v_include/gtkwave/*.cpp")
file(GLOB_RECURSE SRC_FILES_V_INCLUDE "include/v_include/v_include/*.cpp")
file(GLOB_RECURSE SRC_FILES_V_MODEL "include/v_include/*.cpp")
file(GLOB_RECURSE SRC_FILES_V_MODEL_VCORE "include/model_include/v_model/Vcore__ALL.cpp")
add_library(AdaptSimLib ${SRC_FILES} ${SRC_FILES_ADAPT_SIM} ${SRC_FILES_UTILS} ${SRC_FILES_VLTSTD} ${SRC_FILES_GTKWAVE} ${SRC_FILES_V_INCLUDE} ${SRC_FILES_V_MODEL} ${SRC_FILES_V_MODEL_VCORE} ${SRC_FILES_V_MODEL_VCORE})
# 设置库的编译选项


# 为库目标指定头文件搜索路径
# PUBLIC 属性会使链接到此库的目标也继承这些路径
target_include_directories(AdaptSimLib PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/model_include/v_model
        ${PROJECT_SOURCE_DIR}/include/v_include
        ${PROJECT_SOURCE_DIR}/include/v_include/vltstd
        ${PROJECT_SOURCE_DIR}/include/v_include/gtkwave
        ${PROJECT_SOURCE_DIR}/include/v_include/v_include

        ${PROJECT_SOURCE_DIR}/include/AdaptSim/
        ${PROJECT_SOURCE_DIR}/include/AdaptSim/multicore
        ${PROJECT_SOURCE_DIR}/include/AdaptSim/uitils
)

# 创建主可执行文件并链接库
add_executable(AdaptSim main.cpp)
target_link_libraries(AdaptSim PRIVATE AdaptSimLib)

# --- 测试目标 ---
enable_testing()

# 创建测试可执行文件并链接库

# --- 自定义目标 ---
add_custom_target(
        git_commit
        COMMAND ${CMAKE_MAKE_PROGRAM} commit msg="Commit from CMake build"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Calling Makefile's commit target to create a git commit"
        VERBATIM
)