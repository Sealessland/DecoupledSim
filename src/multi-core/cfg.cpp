//
// Created by sealessland on 2025/7/27.
//

#include "cfg.h" // 包含结构体定义和外部变量声明

namespace multiple {

    // 为在 cfg.h 中声明的全局变量 cfg_inst 提供定义和初始值。
    // 这里是它在内存中唯一存在的地方。
    // 我们在这里设置具体地默认值，这些值会覆盖在结构体定义中给出的初始值。
    cfg cfg_inst {
        .diff_enaled = true,
        .trace_enabled = true,
        .inst_trace_enabled = true,
        .mem_trace_enabled = true,
        .diff_ref_path = "/home/sealessland/ysyx-workbench/nemu/build/riscv32-nemu-interpreter-so",
        .wave_file = "wave.vcd",
        .img_path = "",
        .extern_img = false
    };

} // namespace multiple