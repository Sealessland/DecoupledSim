// include/AdaptSim/vmemory.h
#ifndef ADAPTSIM_VMEMORY_H
#define ADAPTSIM_VMEMORY_H

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <map>

namespace memory
{
    class VMem {
    private:
        // 使用 4KB 大小的内存块
        static constexpr uint32_t BLOCK_SIZE = 4096;
        // 使用 map 来稀疏地存储内存块
        std::map<uint32_t, std::vector<uint8_t>> memory_blocks;

        // 内部辅助函数，用于获取或创建内存块
        std::vector<uint8_t>& get_or_create_block(uint32_t addr);

    public:
        VMem();
        ~VMem() = default;

        // 禁止拷贝和赋值
        VMem(const VMem&) = delete;
        VMem& operator=(const VMem&) = delete;

        // 核心读写接口
        uint32_t read(uint32_t addr, uint32_t len);
        void write(uint32_t addr, uint32_t len, uint32_t data);

        // 从文件加载内容到内存
        bool load_from_file(const std::string& filename, uint32_t offset);
        bool load_default_img(uint32_t offset);
    };

    // 提供一个全局的内存访问点
    VMem& get_memory();

} // namespace memory

// 为 Verilator DPI-C 提供的 C 语言风格接口
extern "C" {
    void inst_mem_read(int addr, int len, int* data);
    void data_mem_read(int addr, int len, int* data);
    void data_mem_write(int addr, int len, int data);
}

#endif //ADAPTSIM_VMEMORY_H