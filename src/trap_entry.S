.section .text
.global __trap_entry
.global __trap_return
.align 2

__trap_entry:
    # 1. 直接在当前栈上开辟 34 个寄存器空间
    addi sp, sp, -272

    # 2. 保护当前函数的上下文
    sd x1, 1*8(sp)    # 极其重要：保护 ra 寄存器
    sd x2, 2*8(sp)    # 保护旧 sp
    sd x10, 10*8(sp)  # 保护 a0 等临时寄存器
    sd x11, 11*8(sp)
    
    # 3. 构造传递给 C 的假上下文结构，让 cx->sepc 对应 33*8(sp)
    csrr t0, sepc
    sd t0, 33*8(sp)

    # 4. 调用 C 语言查询函数
    mv a0, sp
    call trap_handler

    # 5. 从 C 返回后，将经由异常表修正后的全新 sepc 写回 CPU 硬件
    ld t0, 33*8(sp)
    csrw sepc, t0

    # 6. 原路恢复所有现场寄存器
    ld x1, 1*8(sp)
    ld x10, 10*8(sp)
    ld x11, 11*8(sp)
    addi sp, sp, 272
    
    # 7. 顺畅返回，由于 sepc 已经被换成了 fixup 目标，程序会瞬间降落在安全区
    sret

