// difftest.h

#ifndef DIFFTEST_H
#define DIFFTEST_H

#include <cstdint>
#include <cstddef> // for size_t

// 类型定义，保持与你的风格一致
using word_t = uint32_t;
using paddr_t = uint32_t;

// Difftest约定的传输方向，增加可读性
#define DIFFTEST_TO_DUT 0
#define DIFFTEST_TO_REF 1

namespace utils {

    // 寄存器上下文结构体
    // 命名和字段顺序必须与NEMU中 `diff_context_t` 的内存布局严格一致
    struct diff_context_t {
        word_t gpr[32]; // 通用寄存器。注意：标准的Difftest通常把GPR放在前面
        word_t pc;      // 程序计数器
        // word_t inst; // 'inst'通常不是通过regcpy传递的，而是DUT执行后自身的产物，可以移出
    };

    class Difftest {
    public:
        /**
         * @brief 构造函数，加载动态链接库并查找函数符号
         * @param ref_so_file 指向NEMU等参考模型编译出的.so文件的路径
         * @param img_size    程序镜像的大小，用于初始化时内存拷贝
         */
        Difftest(const char* ref_so_file, long img_size);
        ~Difftest();

        // --- 标准Difftest接口函数封装 ---

        /**
         * @brief 在DUT和REF之间拷贝内存
         * @param addr 目标地址 (在REF的客户机地址空间中)
         * @param buf  源/目标缓冲区 (在DUT的仿真环境中)
         * @param n    拷贝的字节数
         * @param direction 拷贝方向 (DIFFTEST_TO_DUT or DIFFTEST_TO_REF)
         */
        void memcpy(paddr_t addr, void* buf, size_t n, bool direction);

        /**
         * @brief 在DUT和REF之间拷贝寄存器状态
         * @param dut_context 指向DUT寄存器上下文的指针
         * @param direction   拷贝方向 (DIFFTEST_TO_DUT or DIFFTEST_TO_REF)
         */
        void regcpy(void* dut_context, bool direction);

        /**
         * @brief 让REF执行指定数量的指令
         * @param n 要执行的指令数
         */
        void exec(uint64_t n);

        /**
         * @brief 在REF中触发一个中断/异常 (通常在高级测试中使用)
         * @param NO 中断号
         */
        void raise_intr(uint64_t NO);

        /**
         * @brief 初始化REF，包括内存和ISA状态
         */
        void init();

        // --- 自定义辅助函数 ---

        /**
         * @brief 检查Difftest环境是否设置成功
         */
        bool is_good() const { return good; }

        /**
         * @brief 一步完整的difftest对比流程
         * @param dut_pc DUT当前执行指令的PC
         * @return 如果对比一致则返回true，否则返回false
         */
        bool step(paddr_t dut_pc);

        // 用于处理写内存时的对比
        void store_commit(paddr_t addr, word_t data, int len);

        // 用于跳过一条指令的执行（例如，当遇到CSR指令时）
        void skip_dut_once();


    private:
        void* handle; // 动态库的句柄 (void* is the correct type for dlopen handle)

        // 函数指针，用于存储从 .so 文件中查找到的函数地址
        void (*func_memcpy)(paddr_t, void*, size_t, bool);
        void (*func_regcpy)(void*, bool);
        void (*func_exec)(uint64_t);
        void (*func_raise_intr)(uint64_t);
        void (*func_init)();

        bool good = false;    // 标志位，表示动态库是否加载和符号查找成功
        bool is_skip = false; // 标志位，用于跳过一次对比
    };

} // namespace utils

#endif //DIFFTEST_H