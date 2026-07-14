#include "../include/sbi.h"

// 名字完全由你决定，只要和汇编中的 call 保持一致即可
__attribute__((noreturn)) void kernel_main() 
{
    int version = 1;
    char *author = "zx";
    void *stack_ptr;

    // 获取当前 sp 寄存器的值（用于测试指针打印）
    __asm__ __volatile__("mv %0, sp" : "=r"(stack_ptr));

    // 测试格式化输出
    printf("\n--- Welcome to My C OS (v%d) ---\n", version);
    printf("Author: %s, Test Char: %c\n", author, 'A');
    printf("Current Stack Pointer: %p\n", stack_ptr);
    printf("Hex test: %x\n", 0x2026);

    while (1) {
        // 卡死在这里
    }
}

