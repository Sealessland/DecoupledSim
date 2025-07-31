// src/multi-core/core.cpp
//
// Created by sealessland on 2025/7/27.
//

#include "AdaptSim/multicore/core.h"
#include "AdaptSim/multicore/cfg.h"
#include <iostream>
#include <memory>

#include "Vcore.h"
#include "Vcore___024root.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

namespace multiple {

    // 使用在 cfg.h 中声明并在 cfg.cpp 中定义的全局配置实例
    extern cfg cfg_inst;

    Sim_core::Sim_core() {
        Verilated::traceEverOn(true);
        Top = std::make_unique<Vcore>();



    }

    Sim_core::~Sim_core() {
        // unique_ptr 会自动管理内存，但我们需要确保波形文件被正确关闭
        if (tfp) {
            tfp->close();
        }
    }

    void Sim_core::sim_init()
    {
        // 根据全局配置初始化仿真环境，例如开启波形追踪
        if (cfg_inst.trace_enabled) {
            tfp = std::make_unique<VerilatedVcdC>();
            Top->trace(tfp.get(), 99);
            tfp->open(cfg_inst.wave_file.c_str());
            std::cout << "Wave trace enabled, output file: " << cfg_inst.wave_file << std::endl;
        }
        // 执行复位序列
        Top->clock = 0;
        Top->reset = 1;
        for (int i = 0; i < 20; i++) {
            toggle_clock();
        }
        Top->reset = 0;

    }

    CoreDebugInfo Sim_core::get_debug_info() const {
        CoreDebugInfo info{}; // 使用聚合初始化
        info.pc = Top->io_debugPC;
        info.inst = Top->io_debugInst;
        info.in1 = Top->io_debugin1;
        info.in2 = Top->io_debugin2;
        // info.out = Top->io_debugout1;
        // info.mem_addr = Top->io_debugmemaddr;
        // info.mem_data = Top->io_debugmemdata;
        // 如果 Verilog 模块提供了 GPR 的调试接口，可以取消下面的注释
        // for (int i = 0; i < 32; ++i) {
        //     info.gpr[i] = Top->io_debugGPR[i];
        // }
        return info;
    }

    void Sim_core::toggle_clock() {
        Top->clock = !Top->clock;
        Top->eval();
        if (tfp) {
            // 使用 Verilated 的全局时间戳来记录波形
            tfp->dump(Verilated::time());
        }
        Verilated::timeInc(1); // 增加仿真时间
    }

    void Sim_core::run_inst_once() {
        // 持续翻转时钟直到指令完成信号 `io_inst_done` 为高
        do {
            toggle_clock();
        } while (!Top->io_inst_done);
    }

    int Sim_core::run_inst(int num_inst) {
        int i = 0;
        for (; i < num_inst; i++) {
            run_inst_once();
        }
        return i; // 返回实际执行的指令数
    }
    int Sim_core::run_cycle(int num_cycle) {
        int i = 0;
        for (; i < num_cycle; i++) {
            toggle_clock();
        }
        return i; // 返回实际执行的周期数
    }

} // namespace multiple
