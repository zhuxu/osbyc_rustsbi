struct sbiret {
    long error;
    long value;
};

// 基础的 SBI 调用封装
struct sbiret sbi_call(long ext, long fid, long arg0, long arg1) 
{
    struct sbiret ret;
    register void *a0 __asm__("a0") = (void*)(arg0);
    register void *a1 __asm__("a1") = (void*)(arg1);
    register void *a6 __asm__("a6") = (void*)(fid);
    register void *a7 __asm__("a7") = (void*)(ext);
    
    __asm__ __volatile__(
        "ecall"
        : "+r"(a0), "+r"(a1)
        : "r"(a6), "r"(a7)
        : "memory"
    );
    
    ret.error = (long)a0;
    ret.value = (long)a1;
    return ret;
}

// 使用 SBI 接口打印字符
void console_putchar(char c) 
{
    // 0x01 是旧版 Legacy 或者是 Console Putchar 的 Extension ID
    // 现代 SBI 推荐使用 DBCN (Debug Console Extension, Ext ID: 0x4442434E)
    sbi_call(0x01, 0, c, 0); 
}


// 辅助函数：打印字符串
void print_string(const char *s) 
{
    while (*s) {
        console_putchar(*s++);
    }
}

// 辅助函数：打印无符号数字（支持10进制和16进制）
void print_num(unsigned long long num, int base) 
{
    char buf[64];
    int i = 0;

    // 如果是 16 进制，先打印 0x 前缀
    if (base == 16 && num == 0) {
        console_putchar('0');
        return;
    }

    // 循环取余，倒序存入缓冲区
    do {
        unsigned long long rem = num % base;
        buf[i++] = (rem < 10) ? (rem + '0') : (rem - 10 + 'a');
        num /= base;
    } while (num > 0);

    // 反向打印缓冲区中的字符
    while (i > 0) {
        console_putchar(buf[--i]);
    }
}

// 辅助函数：打印有符号10进制整数
void print_int(long long num) 
{
    if (num < 0) {
        console_putchar('-');
        num = -num;
    }
    print_num((unsigned long long)num, 10);
}

// 核心实现：自己的伪 printf
void printf(const char *fmt, ...) 
{
    // 使用 GCC 内置宏代替 <stdarg.h>，完全不需要头文件
    __builtin_va_list ap;
    __builtin_va_start(ap, fmt);

    for (const char *p = fmt; *p != '\0'; p++) {
        if (*p != '%') {
            console_putchar(*p);
            continue;
        }

        // 遇到 '%'，解析下一个字符
        p++;
        switch (*p) {
            case 'c': { // 字符
                char c = (char)__builtin_va_arg(ap, int);
                console_putchar(c);
                break;
            }
            case 's': { // 字符串
                char *s = __builtin_va_arg(ap, char *);
                if (s == 0) s = "(null)";
                print_string(s);
                break;
            }
            case 'd': { // 10进制有符号整数
                int d = __builtin_va_arg(ap, int);
                print_int(d);
                break;
            }
            case 'x':   // 16进制
            case 'p': { // 指针地址
                if (*p == 'p') {
                    print_string("0x");
                }
                unsigned long long x = __builtin_va_arg(ap, unsigned long long);
                print_num(x, 16);
                break;
            }
            case '%': { // 打印 % 本身
                console_putchar('%');
                break;
            }
            default: { // 不支持的格式直接原样输出
                console_putchar('%');
                console_putchar(*p);
                break;
            }
        }
    }
    __builtin_va_end(ap);
}
