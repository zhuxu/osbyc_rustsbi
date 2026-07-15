#include "../include/sbi.h"

void store_fault(void) 
{
    __asm__ volatile (
        ".option push\n\t"
        ".option norvc\n\t"             /* 强行使用标准 4 字节指令 */
        "1:\n\t"
        "sb zero, 0(zero)\n\t"          /* 1. 这条指令会触发 Exception */
        "j 3f\n\t"                      /* 正常情况下（如果没有崩溃），直接跳到结束 */
        
        ".section .fixup, \"ax\"\n\t"    /* 2. 开辟一个修复代码段 */
        "2:\n\t"
        "j 3f\n\t"                      /* 如果崩溃了，Trap 会强行带程序跳到这里，然后安全退出函数 */
        ".previous\n\t"
        
        ".section __ex_table, \"a\"\n\t" /* 3. 核心：异常表项 */
        ".balign 8\n\t"
        ".quad 1b, 2b\n\t"              /* 记录：当地址 1b 崩溃时，修复目标是地址 2b */
        ".previous\n\t"
        
        "3:\n\t"                        /* 4. 函数出口 */
        ".option pop\n\t"
        : 
        : 
        : "memory"
    );
}
