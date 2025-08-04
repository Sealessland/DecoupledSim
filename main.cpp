#include <iostream>
#include <memory>

// 包含项目所需的头文件
#include "AdaptSim/multicore/cfg.h"
#include "AdaptSim/multicore/core.h"
#include "AdaptSim/vmemory.h"
#include "AdaptSim/utils/difftest.h"

// 引用在 cfg.cpp 中定义的全局配置实例
namespace multiple {
    extern cfg cfg_inst;
}

/**
 * @brief 为Difftest提供C链接方式的回调函数。
 * 这些函数由 .so 文件中的参考模型（REF）调用，因此必须是全局可见的。
 */

// ... (其他 include 和全局变量) ...

int main(int argc, char* argv[]) {


    return 0;
}