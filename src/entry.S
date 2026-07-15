.section .text.entry
.global _start
_start:
	# 设置每个CPU核心的栈空间
	la sp, boot_stack_top
	call kernel_main

.section .bss.stack
.global boot_stack
boot_stack:
	.space 4096*4
.global boot_stack_top
boot_stack_top:


