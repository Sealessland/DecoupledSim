//
// Created by sealessland on 2025/7/27.
//

#ifndef CFG_H
#define CFG_H
#include <string>

namespace multiple {

    struct cfg
    {
        bool diff_enaled = true;  // 差分测试启用
        bool trace_enabled = true; // 波形追踪启用
        bool inst_trace_enabled = true; // 指令追踪启用
        bool mem_trace_enabled = true; // 内存追踪启用
        std::string diff_ref_path = ""; // 差分测试参考路径
        std::string wave_file = "wave.vcd"; // 波形文件名
        std::string img_path = ""; // 默认内存镜像路径
        bool extern_img = false; // 是否使用外部内存镜像
    };

    extern cfg cfg_inst; // 声明一个外部链接的全局配置实例

} // multiple

#endif //CFG_H
