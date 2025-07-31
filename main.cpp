// main.cpp

#include <iostream>
#include <iomanip> // 用于 std::hex 和 std::setw
#include "AdaptSim/multicore/core.h" // 包含 Sim_core 类的定义
#include "AdaptSim/multicore/cfg.h"  // 包含 cfg 结构体和命名空间的定义
#include "AdaptSim/vmemory.h"      // 新增：包含虚拟内存头文件，以便直接访问内存

// 假设 print_debug_info 函数定义在别处或在此文件中
void print_debug_info(const multiple::CoreDebugInfo& info) {
    std::cout << "--- Core Debug Info ---" << std::endl;
    std::cout << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0') << info.pc
              << " | INST: 0x" << std::setw(8) << std::setfill('0') << info.inst << std::dec << std::endl;
    // 可以按需打印更多信息
}

int main(int argc, char* argv[]) {
    // 1. 配置仿真核心的其他设置 (如波形追踪)
    multiple::cfg_inst.trace_enabled = true;
    multiple::cfg_inst.wave_file = "sim_wave.vcd";

    // *** 在 main 函数中显式初始化内存 ***
    // 这种方式绕过了 core 的配置，直接操作全局内存实例。
    if (argc > 1) {
        // 如果提供了命令行参数，则从文件加载内存
        std::string img_path = argv[1];
        std::cout << "从命令行加载内存映像: " << img_path << std::endl;
        // 直接调用内存加载函数，加载到 RISC-V 默认起始地址 0x80000000
        if (!memory::get_memory().load_from_file(img_path, 0x80000000)) {
            std::cerr << "错误: 内存加载失败，程序终止。" << std::endl;
            return 1; // 加载失败则退出
        }
    } else {
        // 如果没有提供命令行参数，则加载 vmemory.cpp 中内置的默认程序
        std::cout << "未提供内存映像文件作为参数，将加载内置默认程序。" << std::endl;
        memory::get_memory().load_default_img(0x80000000);
    }
    std::cout << "内存初始化完成。" << std::endl;


    // 2. 创建并初始化 Sim_core 实例
    // 构造函数会自动处理复位序列
    multiple::Sim_core core;
    std::cout << "Sim_core created and reset." << std::endl;

    // 根据配置初始化仿真（现在只处理波形文件等，因为内存已由我们手动加载）
    core.sim_init();

    // 3. 运行单条指令并打印状态
    std::cout << "\nRunning a single instruction..." << std::endl;
    core.run_inst(50);
    std::cout << "Instruction finished." << std::endl;

    // 获取并打印执行第一条指令后的调试信息
    multiple::CoreDebugInfo debug_info = core.get_debug_info();
    print_debug_info(debug_info);

    // 4. 批量运行多条指令
    const int num_instructions_to_run = 0;
    std::cout << "\nRunning " << num_instructions_to_run << " more instructions..." << std::endl;
    int executed_count = core.run_inst(num_instructions_to_run);
    std::cout << "Finished running " << executed_count << " insts." << std::endl;

    // 获取并打印最终的调试信息
    debug_info = core.get_debug_info();
    print_debug_info(debug_info);

    std::cout << "\nSimulation finished successfully." << std::endl;

    // Sim_core 的析构函数会自动被调用，关闭波形文件并释放内存
    return 0;
}