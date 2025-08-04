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
#include "utils/difftest.h"
#include "Vcore___024root.h" // 包含Verilator生成的内部头文件

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
    utils::diff_context_t Sim_core::get_diff_info() { // 建议设为 const 函数
        utils::diff_context_t context{};
        context.pc = Top->io_debugPC;

        // 从Verilator内部信号逐一读取GPRs
        // 注意：x0/rf_0 物理上可能不存在，但为保持接口一致性，通常会赋值为0
        context.gpr[0]  = 0; // RISC-V x0 is always zero
        context.gpr[1]  = Top->rootp->core__DOT__RF__DOT__rf_1;
        context.gpr[2]  = Top->rootp->core__DOT__RF__DOT__rf_2;
        context.gpr[3]  = Top->rootp->core__DOT__RF__DOT__rf_3;
        context.gpr[4]  = Top->rootp->core__DOT__RF__DOT__rf_4;
        context.gpr[5]  = Top->rootp->core__DOT__RF__DOT__rf_5;
        context.gpr[6]  = Top->rootp->core__DOT__RF__DOT__rf_6;
        context.gpr[7]  = Top->rootp->core__DOT__RF__DOT__rf_7;
        context.gpr[8]  = Top->rootp->core__DOT__RF__DOT__rf_8;
        context.gpr[9]  = Top->rootp->core__DOT__RF__DOT__rf_9;
        context.gpr[10] = Top->rootp->core__DOT__RF__DOT__rf_10;
        context.gpr[11] = Top->rootp->core__DOT__RF__DOT__rf_11;
        context.gpr[12] = Top->rootp->core__DOT__RF__DOT__rf_12;
        context.gpr[13] = Top->rootp->core__DOT__RF__DOT__rf_13;
        context.gpr[14] = Top->rootp->core__DOT__RF__DOT__rf_14;
        context.gpr[15] = Top->rootp->core__DOT__RF__DOT__rf_15;
        context.gpr[16] = Top->rootp->core__DOT__RF__DOT__rf_16;
        context.gpr[17] = Top->rootp->core__DOT__RF__DOT__rf_17;
        context.gpr[18] = Top->rootp->core__DOT__RF__DOT__rf_18;
        context.gpr[19] = Top->rootp->core__DOT__RF__DOT__rf_19;
        context.gpr[20] = Top->rootp->core__DOT__RF__DOT__rf_20;
        context.gpr[21] = Top->rootp->core__DOT__RF__DOT__rf_21;
        context.gpr[22] = Top->rootp->core__DOT__RF__DOT__rf_22;
        context.gpr[23] = Top->rootp->core__DOT__RF__DOT__rf_23;
        context.gpr[24] = Top->rootp->core__DOT__RF__DOT__rf_24;
        context.gpr[25] = Top->rootp->core__DOT__RF__DOT__rf_25;
        context.gpr[26] = Top->rootp->core__DOT__RF__DOT__rf_26;
        context.gpr[27] = Top->rootp->core__DOT__RF__DOT__rf_27;
        context.gpr[28] = Top->rootp->core__DOT__RF__DOT__rf_28;
        context.gpr[29] = Top->rootp->core__DOT__RF__DOT__rf_29;
        context.gpr[30] = Top->rootp->core__DOT__RF__DOT__rf_30;
        context.gpr[31] = Top->rootp->core__DOT__RF__DOT__rf_31;

        return context;
    }
} // namespace multiple