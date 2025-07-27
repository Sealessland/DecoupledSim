// src/multi-core/core.cpp
//
// Created by sealessland on 2025/7/27.
//

#include "../../include/AdaptSim/multicore/core.h"
#include "../../include/AdaptSim/multicore/cfg.h" // 包含 cfg.h
#include <iostream>
#include <memory>

#include "../../include/model_include/v_model/Vcore.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
// #include "model_include/v_model/Vcore___024root.h" // 不应包含内部头文件

namespace multiple {

    // 假设 cfg.h 中定义了全局配置实例 `cfg_t cfg;`
    extern cfg_t cfg;

    struct CoreDebugInfo
    {
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

    class Sim_core
    {
    private:
        std::unique_ptr<Vcore> Top;
        std::unique_ptr<VerilatedVcdC> tfp;
        // CoreDebugInfo debug_info; // 此成员变量不再需要

    public:
        Sim_core() { // 无需在初始化列表中将 unique_ptr 初始化为 nullptr
            Verilated::traceEverOn(true);
            Top = std::make_unique<Vcore>();

            Top->clock = 0;
            Top->reset = 1;
            for (int i = 0; i < 20; i++) {
                Top->eval();
                Top->clock = !Top->clock;
                Top->eval();
            }
            Top->reset = 0;
        }

        ~Sim_core() {
            // unique_ptr 会自动释放 Top 和 tfp 的内存
            // 如果需要，可以确保波形文件被关闭
            if (tfp) {
                tfp->close();
            }
        }

        // 禁止拷贝和赋值
        Sim_core(const Sim_core&) = delete;
        Sim_core& operator=(const Sim_core&) = delete;

        void sim_init()
        {
            // 假设 cfg 是一个全局实例
            if (cfg.trace_enabled) {
                tfp = std::make_unique<VerilatedVcdC>();
                Top->trace(tfp.get(), 99);
                tfp->open(cfg.wave_file.c_str());
                std::cout << "Wave trace enabled, output file: " << cfg.wave_file << std::endl;
            }
        }

        // 标记为 const，因为它不修改 Sim_core 的状态
        CoreDebugInfo get_debug_info() const {
            CoreDebugInfo info{}; // 创建局部变量
            info.pc = Top->io_debugPC;
            info.inst = Top->io_debugInst;
            info.in1 = Top->io_debugin1;
            info.in2 = Top->io_debugin2;
            info.out = Top->io_debugout1;
            info.mem_addr = Top->io_debugmemaddr;
            info.mem_data = Top->io_debugmemdata;
            // 假设 io_debugGPR 存在
            // for (int i = 0; i < 32; ++i) {
            //     info.gpr[i] = Top->io_debugGPR[i];
            // }
            return info; // 返回局部变量的副本
        }

        void toggle_clock() {
            Top->clock = !Top->clock;
            Top->eval();
            if (tfp) {
                // 假设时间由 Verilated 管理
                tfp->dump(Verilated::time());
            }
        }

        void run_inst_once() {
            // 假设 io_inst_done 信号存在
            while (!Top->io_inst_done)
            {
                toggle_clock();
            }
        }

        int run_inst(int num_inst) {
            int i = 0;
            for (; i < num_inst; i++) {
                run_inst_once();
            }
            return i;
        }
    };
}
