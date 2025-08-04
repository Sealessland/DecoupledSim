// src/vmemory.cpp
//
// Created by sealessland on 2025/7/27.
//

#include "AdaptSim/vmemory.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip> // For std::hex, std::dec

#include "cfg.h"

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

            // 依次将1~31号寄存器填充为其序号
            0x00100093, // addi x1, x0, 1
            0x00200113, // addi x2, x0, 2
            0x00300193, // addi x3, x0, 3
            0x00400213, // addi x4, x0, 4
            0x00000297, // auipc x5, 0x0   // 第32条：x5 = pc + 0（假设此时pc为当前指令地址）
            0x005283b3, // add x7, x5, x5  // 第33条：x7 = x5 + x5
            0x00500293, // addi x5, x0, 5
            0x00600313, // addi x6, x0, 6
            0x00700393, // addi x7, x0, 7
            0x00800413, // addi x8, x0, 8
            0x00900493, // addi x9, x0, 9
            0x00a00513, // addi x10, x0, 10
            0x00b00593, // addi x11, x0, 11
            0x00c00613, // addi x12, x0, 12
            0x00d00693, // addi x13, x0, 13
            0x00e00713, // addi x14, x0, 14
            0x00f00793, // addi x15, x0, 15
            0x01000813, // addi x16, x0, 16
            0x01100893, // addi x17, x0, 17
            0x01200913, // addi x18, x0, 18
            0x01300993, // addi x19, x0, 19
            0x01400a13, // addi x20, x0, 20
            0x01500a93, // addi x21, x0, 21
            0x01600b13, // addi x22, x0, 22
            0x01700b93, // addi x23, x0, 23
            0x01800c13, // addi x24, x0, 24
            0x01900c93, // addi x25, x0, 25
            0x01a00d13, // addi x26, x0, 26
            0x01b00d93, // addi x27, x0, 27
            0x01c00e13, // addi x28, x0, 28
            0x01d00e93, // addi x29, x0, 29
            0x01e00f13, // addi x30, x0, 30
            0x01f00f93  // addi x31, x0, 31

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
extern "C" void mem_read(int addr, int* data) {
    uint32_t read_val = memory::get_memory().read(static_cast<uint32_t>(addr), 4);
    std::cout << "mem_read: addr=0x" << std::hex << addr << ", data=0x" << read_val << std::dec << std::endl;
    *data = read_val;
}

extern "C" void mem_write(int addr, int data) {
    std::cout << "mem_write: addr=0x" << std::hex << addr << ", data=0x" << data << std::endl;
    memory::get_memory().write(static_cast<uint32_t>(addr), 4, static_cast<uint32_t>(data));
}
