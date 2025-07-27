//
// Created by sealessland on 2025/7/27.
//

#ifndef CFG_H
#define CFG_H
#include <string>
namespace multiple {

class cfg {
    struct cfg
    {
        static constexpr bool diff_enaled = true;  // 差分测试启用
        static constexpr bool trace_enabled = true; // 波形追踪启用
        static constexpr bool inst_trace_enabled = true; // 指令追踪启用
        static constexpr bool mem_trace_enabled = true; // 内存追踪启用
        std::string diff_ref_path = ""; // 差分测试参考路径
        std::string wave_file = "wave.vcd"; // 波形文件名
        static constexpr std::string img_path = ""; // 默认内存镜像路径
        static constexpr bool extern_img = false; // 是否使用外部内存镜像
    };
};

} // multiple

#endif //CFG_H
