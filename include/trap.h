#ifndef TRAP_H_
#define TRAP_H_

#include <stdint.h>

// 这个结构体的顺序与 trap.S 的 sd 顺序 100% 严格一致
typedef struct {
    uint64_t x[32];   // x[0]是常数0, x[1]是ra, x[2]是sp, x[10]是a0 ...
    uint64_t sstatus; // 对应 32*8(sp)
    uint64_t sepc;    // 对应 33*8(sp)
} TrapContext;

// 声明外部的汇编跳转符号，供内核关机或切换任务时使用
extern void __trap_return(void);

#endif /* TRAP_H_ */
