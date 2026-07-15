# 1. 交叉编译器路径
CROSS_COMPILE ?= /opt/gcc-riscv64-unknown-elf/bin/riscv64-unknown-elf-

CC      := $(CROSS_COMPILE)gcc
LD      := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump

# 2. 编译选项
# -march=rv64gc: 目标架构为 64位 基础整数(g) 与 压缩指令(c)
# -mabi=lp64d: 对应的 ABI 接口
# -ffreestanding: 声明为裸机环境，不假设标准库存在
# -nostdlib: 不链接系统标准库
CFLAGS  := -march=rv64gc -mabi=lp64d -Wall -mcmodel=medany -O0 -ffreestanding -nostdlib -ggdb
LDFLAGS := -T linker.ld

# 3. 定义目标文件和源文件
TARGET := kernel.bin
ELF    := kernel.elf

SRCS_C := $(wildcard *.c)
SRCS_S := $(wildcard *.S)
OBJS   := $(SRCS_C:.c=.o) $(SRCS_S:.S=.o)

# 4. 构建规则
.PHONY: all clean run

all: $(TARGET)

# 生成最终的纯二进制镜像（RustSBI/QEMU 直接加载这个）
$(TARGET): $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(TARGET)

# 链接生成 ELF 文件
$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) -o $(ELF) $(OBJS)

# 编译 C 文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 编译汇编文件
%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

# 5. 清理规则
clean:
	rm -f $(OBJS) $(ELF) $(TARGET)

# 6. 一键运行规则（使用 QEMU 模拟器搭配 RustSBI）
run: all
	qemu-system-riscv64 \
		-machine virt \
		-bios ../bootloader/rustsbi-qemu.bin \
		-device loader,file=$(TARGET),addr=0x80200000

