//
// Created by sealessland on 2025/7/27.
//

#ifndef CORE_H
#define CORE_H

#include <cstdint>
#include <memory> // For std::unique_ptr
#include "utils/difftest.h"

// 前向声明 Verilator 生成的类，以避免在头文件中包含大型 Verilator 头文件
class Vcore;
class VerilatedVcdC;

namespace multiple {

    // 调试信息结构体，用于从仿真核心获取状态
    struct CoreDebugInfo {
        uint32_t pc;
        uint32_t inst;
        uint32_t in1;
        uint32_t in2;
        uint32_t out;
        uint32_t mem_addr;
        uint32_t mem_data;
        uint32_t gpr[32];
        uint64_t inst_cnt;
        uint64_t cycle_cnt;
    };

    // Verilator 仿真核心的封装类
    class Sim_core {
    private:
        std::unique_ptr<Vcore> Top;
        std::unique_ptr<VerilatedVcdC> tfp;

        void toggle_clock();

    public:
        Sim_core();
        ~Sim_core();

        // 禁止拷贝和赋值，因为该类管理着独特的资源
        Sim_core(const Sim_core&) = delete;
        Sim_core& operator=(const Sim_core&) = delete;

        void sim_init();
        CoreDebugInfo get_debug_info() const;
        void run_inst_once();
        int run_inst(int num_inst);
        int run_cycle(int num_cycle);
        utils::diff_context_t get_diff_info();

    };

} // namespace multiple

#endif //CORE_H
