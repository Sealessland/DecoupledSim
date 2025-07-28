// src/vmemory.cpp
//
// Created by sealessland on 2025/7/27.
//

#include "AdaptSim/vmemory.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip> // For std::hex, std::dec

namespace memory
{
    // Define the single global instance of memory
    static VMem g_memory;

    // Implement the global access function
    VMem& get_memory() {
        return g_memory;
    }

    VMem::VMem() {
        std::cout << "Virtual memory initialized (sparse model, 4KB blocks)." << std::endl;
    }

    // Get or create a memory block on demand
    std::vector<uint8_t>& VMem::get_or_create_block(uint32_t addr) {
        uint32_t block_index = addr / BLOCK_SIZE;
        auto it = memory_blocks.find(block_index);
        if (it == memory_blocks.end()) {
            // Block doesn't exist, create and initialize with zeros
            auto result = memory_blocks.emplace(block_index, std::vector<uint8_t>(BLOCK_SIZE, 0));
            return result.first->second;
        }
        return it->second;
    }

    // Read data from memory (little-endian)
    uint32_t VMem::read(uint32_t addr, uint32_t len) {
        if (len == 0 || len > 4) {
            // Error handling could be added here
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

    // Write data to memory (little-endian)
    void VMem::write(uint32_t addr, uint32_t len, uint32_t data) {
        if (len == 0 || len > 4) {
            // Error handling could be added here
            return;
        }
        for (uint32_t i = 0; i < len; ++i) {
            uint32_t current_addr = addr + i;
            uint32_t block_offset = current_addr % BLOCK_SIZE;
            std::vector<uint8_t>& block = get_or_create_block(current_addr);
            block[block_offset] = (data >> (i * 8)) & 0xFF;
        }
    }

    // Load binary content from a file
    bool VMem::load_from_file(const std::string& filename, uint32_t offset) {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open memory image file '" << filename << "'" << std::endl;
            return false;
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) {
            std::cerr << "Error: Cannot read from file '" << filename << "'" << std::endl;
            return false;
        }

        for (std::streamsize i = 0; i < size; ++i) {
            write(offset + i, 1, static_cast<uint8_t>(buffer[i]));
        }

        std::cout << "Loaded memory image: " << filename << " (" << size << " bytes) to address 0x"
                  << std::hex << offset << std::dec << std::endl;
        return true;
    }

    // Load a hardcoded default program into memory
    bool VMem::load_default_img(uint32_t offset)
    {
        const uint32_t default_img[] = {
            0x87fff137,  // lui sp, 0x87fff
            0xff010113,  // addi sp, sp, 0xff0
            0xa0000537,  // lui a0, 0xa0000
            0x3f800593,  // li a1, 0x3f8
            0x00b50633,  // add a2, a0, a1
            0x04800793,  // li a5, 0x48
            0x00f507b3,  // add a5, a0, a5
            0x05200693,  // li a3, 'R'
            0x00d62023,  // sw a3, 0(a2)
            0x05400693,  // li a3, 'T'
            0x00d62023,  // sw a3, 0(a2)
            0x04300693,  // li a3, 'C'
            0x00d62023,  // sw a3, 0(a2)
            0x02000693,  // li a3, ' '
            0x00d62023,  // sw a3, 0(a2)
            0x04c00693,  // li a3, 'L'
            0x00d62023,  // sw a3, 0(a2)
            0x06f00693,  // li a3, 'o'
            0x00d62023,  // sw a3, 0(a2)
            0x03a00693,  // li a3, ':'
            0x00d62023,  // sw a3, 0(a2)
            0x02000693,  // li a3, ' '
            0x00d62023,  // sw a3, 0(a2)
            0x0007a683,  // lw a3, 0(a5)
            0x00800713,  // li a4, 8
            0x01c00393,  // li t2, 28
            0x0006f29b,  // srl t0, a3, t2
            0x00f2f293,  // andi t0, t0, 0xf
            0x0092f313,  // addi t1, t0, -9
            0x00031063,  // bgtz t1, is_letter
            0x0302e313,  // addi t1, t0, '0'
            0x0050006f,  // j print_char
            0x0372e313,  // addi t1, t0, 'A'-10
            0x00662023,  // sw t1, 0(a2)
            0xfec3d393,  // addi t2, t2, -4
            0xfff71ee3,  // bnez a4, print_hex_loop_start
            0x00a00693,  // li a3, 10
            0x00d62023,  // sw a3, 0(a2)
            0x0000006f,  // j .
        };

        size_t instr_count = std::size(default_img);
        for (size_t i = 0; i < instr_count; ++i) {
            // Reuse the existing write method for consistency and simplicity
            write(offset + i * 4, 4, default_img[i]);
        }

        std::cout << "Loaded default memory image (" << instr_count * 4 << " bytes) to address 0x"
                  << std::hex << offset << std::dec << std::endl;

        return true;
    }

} // namespace memory

// C-style interface for Verilator DPI-C
extern "C" uint32_t mem_read(int addr) {
    std::cout << "mem_read: addr=0x" << std::hex << addr << std::endl;
    // Always read a full 4-byte word; byte selection is handled by the Verilog side
    return memory::get_memory().read(static_cast<uint32_t>(addr), 4);
}

extern "C" void mem_write(int addr, int data) {
    std::cout << "mem_write: addr=0x" << std::hex << addr << ", data=0x" << data << std::endl;
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