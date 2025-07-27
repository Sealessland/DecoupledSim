//
// Created by sealessland on 2025/7/27.
//

#include "cfg.h"
#include <string>
namespace multiple {
    struct cfg
    {   static constexpr bool time_analysisenabled = true; // 调试模式启用
        static constexpr bool diff_enaled = true;  // 差分测试启用
        static constexpr bool trace_enabled = true; // 波形追踪启用
        static constexpr bool inst_trace_enabled = true; // 指令追踪启用
        static constexpr bool mem_trace_enabled = true; // 内存追踪启用
        std::string diff_ref_path = ""; // 差分测试参考路径
        std::string wave_file = "wave.vcd"; // 波形文件名
    };
} // multiple