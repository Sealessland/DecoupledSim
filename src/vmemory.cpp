// src/vmemory.cpp
//
// Created by sealessland on 2025/7/27.
//

#include "../include/AdaptSim/vmemory.h"
#include <iostream>
#include <fstream>
#include <vector>
#include<../include/AdaptSim/multicore/cfg.h>

namespace memory
{
    // 定义全局唯一的内存实例
    static VMem g_memory;

    // 实现全局访问函数
    VMem& get_memory() {
        return g_memory;
    }

    VMem::VMem() {
        std::cout << "虚拟内存已初始化 (稀疏存储模型, 4KB 块)。" << std::endl;
    }

    // 获取或按需创建内存块
    std::vector<uint8_t>& VMem::get_or_create_block(uint32_t addr) {
        uint32_t block_index = addr / BLOCK_SIZE;
        auto it = memory_blocks.find(block_index);
        if (it == memory_blocks.end()) {
            // 块不存在，创建并用 0 初始化
            auto result = memory_blocks.emplace(block_index, std::vector<uint8_t>(BLOCK_SIZE, 0));
            return result.first->second;
        }
        return it->second;
    }

    // 从内存读取数据 (小端字节序)
    uint32_t VMem::read(uint32_t addr, uint32_t len) {
        if (len == 0 || len > 4) {
            // 可以添加错误处理
            return 0;
        }
        uint32_t result = 0;
        for (uint32_t i = 0; i < len; ++i) {
            uint32_t current_addr = addr + i;
            uint32_t block_index = current_addr / BLOCK_SIZE;
            uint32_t block_offset = current_addr % BLOCK_SIZE;

            auto it = memory_blocks.find(block_index);
            uint8_t byte_val = 0;
            if (it != memory_blocks.end()) {
                byte_val = it->second[block_offset];
            }
            result |= static_cast<uint32_t>(byte_val) << (i * 8);
        }
        return result;
    }

    // 向内存写入数据 (小端字节序)
    void VMem::write(uint32_t addr, uint32_t len, uint32_t data) {
        if (len == 0 || len > 4) {
            // 可以添加错误处理
            return;
        }
        for (uint32_t i = 0; i < len; ++i) {
            uint32_t current_addr = addr + i;
            uint32_t block_offset = current_addr % BLOCK_SIZE;
            std::vector<uint8_t>& block = get_or_create_block(current_addr);
            block[block_offset] = (data >> (i * 8)) & 0xFF;
        }
    }

    // 从文件加载二进制内容
    bool VMem::load_from_file(const std::string& filename, uint32_t offset) {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cerr << "错误: 无法打开内存映像文件 '" << filename << "'" << std::endl;
            return false;
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) {
            std::cerr << "错误: 无法读取文件 '" << filename << "'" << std::endl;
            return false;
        }

        for (std::streamsize i = 0; i < size; ++i) {
            write(offset + i, 1, static_cast<uint8_t>(buffer[i]));
        }

        std::cout << "已加载内存映像: " << filename << " (" << size << " 字节) 到地址 0x"
                  << std::hex << offset << std::dec << std::endl;
        return true;
    }

} // namespace memory
extern "C" uint32_t mem_read(int addr) {
    // 总是读取一个完整的4字节字，因为Verilog端会处理字节选择
    return static_cast<int>(memory::get_memory().read(static_cast<uint32_t>(addr), 4));
}

extern "C" void mem_write(int addr, int data) {
    memory::get_memory().write(static_cast<uint32_t>(addr), 4, static_cast<uint32_t>(data));
}

// C 接口实现，供 Verilator 的 DPI-C 调用
// extern "C" void inst_mem_read(int addr, int len, int* data) {
//     *data = static_cast<int>(memory::get_memory().read(static_cast<uint32_t>(addr), static_cast<uint32_t>(len)));
//     // 可选的调试输出
//     // std::cout << "指令内存读取: addr=0x" << std::hex << addr << ", len=" << len << ", data=0x" << *data << std::dec << std::endl;
// }
//
// extern "C" void data_mem_read(int addr, int len, int* data) {
//     *data = static_cast<int>(memory::get_memory().read(static_cast<uint32_t>(addr), static_cast<uint32_t>(len)));
//     // 可选的调试输出
//     // std::cout << "数据内存读取: addr=0x" << std::hex << addr << ", len=" << len << ", data=0x" << *data << std::dec << std::endl;
// }
//
// extern "C" void data_mem_write(int addr, int len, int data) {
//     memory::get_memory().write(static_cast<uint32_t>(addr), static_cast<uint32_t>(len), static_cast<uint32_t>(data));
//     // 可选的调试输出
//     // std::cout << "数据内存写入: addr=0x" << std::hex << addr << ", len=" << len << ", data=0x" << data << std::dec << std::endl;
// }