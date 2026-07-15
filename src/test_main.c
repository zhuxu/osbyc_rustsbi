#include <stdint.h>
#include "../include/sbi.h"
#include "../include/api.h"

extern void __trap_entry(void);
extern uint8_t boot_stack_top[];

void trap_init(void)
{
    uint64_t trap_addr = (uint64_t)__trap_entry;
    __asm__ volatile("csrw stvec, %0" :: "r"(trap_addr));

    uint64_t stack_top_addr = (uint64_t)boot_stack_top;
    __asm__ volatile("csrw sscratch, %0" :: "r"(stack_top_addr));

    return;
}

// 名字完全由你决定，只要和汇编中的 call 保持一致即可
__attribute__((noreturn)) void kernel_main() 
{
    trap_init();
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

    store_fault();
    printf("\n--- end store fault...\n");



    while (1) {
        // 卡死在这里
    }
}

