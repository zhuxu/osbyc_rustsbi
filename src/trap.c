#include <stdint.h>
#include "../include/trap.h"
#include "../include/sbi.h"
#include "../include/api.h"

// 映射链接脚本中的异常表边界
extern uint64_t __start___ex_table[];
extern uint64_t __stop___ex_table[];

void trap_handler(TrapContext *cx) 
{
    uint64_t scause;
    __asm__ volatile("csrr %0, scause" : "=r"(scause));

    uint64_t exception_code = scause & 0xfff;

    // 只拦截你的 store_fault (Code 7)
    if (exception_code == 7) {
        printf("\n[Trap Engine] Detected Store Fault! Looking up exception table...\n");

        // 遍历异常表，寻找匹配的崩溃 PC
        uint64_t *table_ptr = __start___ex_table;
        uint64_t *table_end = __stop___ex_table;

        while (table_ptr < table_end) {
            uint64_t fault_addr  = table_ptr[0]; // 对应汇编中的 1b
            uint64_t fixup_addr  = table_ptr[1]; // 对应汇编中的 2b

            if (cx->sepc == fault_addr) {
                printf("[Trap Engine] Match found! Diverting execution flow safely.\n");

                // 关键核心：直接将硬件返回目标修改为修复地址，完美跃过崩溃指令和后面任何潜在的恶性汇编
                cx->sepc = fixup_addr;
                return;
            }
            table_ptr += 2; // 每个表项占 2 个 uint64_t (16 字节)
        }

        printf("[Trap Engine] Fault address not in exception table. Halted.\n");
        while(1);
    } else {
        printf("\n[Trap Engine] Other unhandled exception. Halted.\n");
        while(1);
    }
}
