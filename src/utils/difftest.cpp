//
// Created by sealessland on 2025/7/27.
//

#include "AdaptSim/utils/difftest.h"
// difftest.cpp


#include <dlfcn.h>
#include <iostream>
#include <vector>

// 宏，用于简化 dlsym 的调用和错误处理
#define MAP_SYMBOL(handle, func_ptr, type, symbol_name_str) \
    do { \
        func_ptr = (type)dlsym(handle, symbol_name_str); \
        if (!func_ptr) { \
            std::cerr << "[Difftest] Failed to find symbol '" << symbol_name_str << "' in shared library." << std::endl; \
            std::cerr << "  dlerror: " << dlerror() << std::endl; \
            this->good = false; \
            return; \
        } \
    } while (0)

namespace utils {

Difftest::Difftest(const char* ref_so_file, long img_size) {
    if (!ref_so_file) {
        std::cout << "[Difftest] Reference SO file not provided. Difftest is disabled." << std::endl;
        return;
    }

    // 1. 加载 (Load)
    handle = dlopen(ref_so_file, RTLD_LAZY);
    if (!handle) {
        std::cerr << "[Difftest] Failed to load shared library '" << ref_so_file << "'." << std::endl;
        std::cerr << "  dlerror: " << dlerror() << std::endl;
        return;
    }

    // 2. 符号查找 (Symbol Lookup)
    MAP_SYMBOL(handle, func_memcpy,     void (*)(paddr_t, void*, size_t, bool), "difftest_memcpy");
    MAP_SYMBOL(handle, func_regcpy,     void (*)(void*, bool),                  "difftest_regcpy");
    MAP_SYMBOL(handle, func_exec,       void (*)(uint64_t),                     "difftest_exec");
    MAP_SYMBOL(handle, func_raise_intr, void (*)(uint64_t),                     "difftest_raise_intr");
    MAP_SYMBOL(handle, func_init,       void (*)(),                             "difftest_init");

    this->good = true; // 所有符号都找到了

    // 初始化REF
    this->init();

    // 将REF的初始内存镜像拷贝到DUT的内存中
    // 假设DUT的内存起始地址是0x80000000
    paddr_t guest_mem_start = 0x80000000;
    std::vector<uint8_t> guest_mem(img_size);
    this->memcpy(guest_mem_start, guest_mem.data(), img_size, DIFFTEST_TO_DUT);

    // 在这里，你需要将 guest_mem.data() 中的内容写入到你的DUT仿真模型中
    // 这通常通过AXI master或类似的后门访问接口完成
    // (This part is specific to your DUT's testbench)

    std::cout << "[Difftest] Successfully loaded REF model '" << ref_so_file << "' and initialized memory." << std::endl;
}

Difftest::~Difftest() {
    if (handle) {
        dlclose(handle);
    }
}

// --- 标准接口实现 ---
void Difftest::init() {
    if (good) func_init();
}

void Difftest::memcpy(paddr_t addr, void* buf, size_t n, bool direction) {
    if (good) func_memcpy(addr, buf, n, direction);
}

void Difftest::regcpy(void* dut_context, bool direction) {
    if (good) func_regcpy(dut_context, direction);
}

void Difftest::exec(uint64_t n) {
    if (good) func_exec(n);
}

void Difftest::raise_intr(uint64_t NO) {
    if (good) func_raise_intr(NO);
}

// --- 辅助函数实现 ---
void Difftest::skip_dut_once() {
    if (good) this->is_skip = true;
}

void Difftest::store_commit(paddr_t addr, word_t data, int len) {
    // 商业级的Difftest通常会在这里缓存store操作，
    // 在下一次step对比寄存器之前，先对比这些缓存的store操作是否一致。
    // 这里简化处理，可以暂时为空。
}


bool Difftest::step(diff_context_t *dut) {
    if (!good) return true; // Difftest未启用，默认通过
    if (is_skip) {
        is_skip = false;
        return true;
    }

    // 1. 获取REF执行前的寄存器状态 (用于对比)
    diff_context_t ref_context_before;
    regcpy(&ref_context_before, DIFFTEST_TO_DUT);

    // 2. 检查PC是否一致
    if (ref_context_before.pc != dut_pc) {
        std::cerr << "[Difftest] PC mismatch before execution!" << std::endl;
        std::cerr << "  REF PC = 0x" << std::hex << ref_context_before.pc << std::endl;
        std::cerr << "  DUT PC = 0x" << std::hex << dut_pc << std::endl;
        return false;
    }

    // 3. 让REF执行一步
    exec(1);

    // 4. 获取REF执行后的寄存器状态
    diff_context_t ref_context_after;
    regcpy(&ref_context_after, DIFFTEST_TO_DUT);

    // 5. 获取DUT执行后的寄存器状态
    diff_context_t dut_context_after;

    bool regs_match = true;
    if (ref_context_after.pc != dut_context_after.pc) {
        regs_match = false;
        std::cerr << "[Difftest] PC mismatch after execution!" << std::endl;
        std::cerr << "  REF next_PC = 0x" << std::hex << ref_context_after.pc << std::endl;
        std::cerr << "  DUT next_PC = 0x" << std::hex << dut_context_after.pc << std::endl;
    }
    for (int i = 0; i < 32; ++i) {
        if (ref_context_after.gpr[i] != dut_context_after.gpr[i]) {
            regs_match = false;
            std::cerr << "[Difftest] GPR x" << i << " mismatch!" << std::endl;
            std::cerr << "  REF value = 0x" << std::hex << ref_context_after.gpr[i] << std::endl;
            std::cerr << "  DUT value = 0x" << std::hex << dut_context_after.gpr[i] << std::endl;
        }
    }

    return regs_match;
}

} // namespace utils