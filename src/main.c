#include <getopt.h>
#include "conv.h"

/* 
options[7:0] 每一位的定义
[7]: 开启位宽限制                        0：关闭 1：开启 默认关闭
[6]：十六进制数是否大写                   0：小写 1：大写 默认开启
[5]：十六、二进制是否显示 "0x/0b"          0：关闭 1：开启 默认开启
[4]: 开启任意进制转换                     0：关闭 1：开启 默认关闭

[3]: 十六、二进制是否四位分割(不足补0)      0：关闭 1：开启 默认关闭
[2]: 转换成十进制                        0：关闭 1：开启 默认开启
[1]: 转换成十六进制                      0：关闭 1：开启 默认开启
[0]: 转换成二进制                        0：关闭 1：开启 默认开启

默认 0b 0110 0111 = 0x67
*/
static const struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'v'},
    {"hex", no_argument, 0, 'x'},
    {"bin", no_argument, 0, 'b'},
    {"upper", no_argument, 0, 'u'},
    {"width", required_argument, 0, 'w'},
    {"opt", required_argument, 0, 'o'},
    {0, 0, 0, 0} // 哨兵
};

static uint8_t options = 0x67;
static char hex_buffer[BUFFER_SIZE]; // 十六进制缓冲区
static char bin_buffer[BUFFER_SIZE]; // 二进制缓冲区
static char any_buffer[BUFFER_SIZE]; // 任意进制缓冲区

/**
 * @brief 命令解析
 * 
 * @param argc 参数的数目
 * @param argv 存储命令的参数
 * @param val  输入的值的指针
 * @param width 位宽限制
 * @return CmdType_t 
 */
CmdType_t command_parse(int argc, char *argv[], int64_t *val, uint8_t *width)
{
    // 只输入了一个参数 返回帮助
    if (argc == 1) return CMD_HELPER;
}

/**
 * @brief 输出版本号
 * 
 * @return int 
 */
int print_version(void)
{
    printf("version:%s\n", BCONV_VERSION);
    return 0;
}

/**
 * @brief 输出帮助
 *
 * @return int
 */
int print_help(void)
{
#if defined DEBUG
    printf("help\n");
#else
    printf("bconv (Base Converter) - 轻量级进制转换工具\n\n");
    printf("用法: bconv [选项] <数值>\n\n");
    printf("默认行为:\n");
    printf("  如果不加任何输出格式选项，默认打印该数值的十进制(Dec)、十六进制(Hex)和二进制(Bin)三种形式\n\n");

    printf("示例:\n");
    printf("  $ bconv 255\n");
    printf("  Dec: 255\n");
    printf("  Hex: 0xFF\n");
    printf("  Bin: 0b11111111\n\n");

    printf("输入控制:\n");
    printf("  -无           系统会自己推断输入的进制\n");

    printf("输出控制 (互斥):\n");
    printf("  -x, --hex     仅输出十六进制\n");
    printf("  -d, --dec     仅输出十进制\n");
    printf("  -b, --bin     仅输出二进制\n");
    printf("  -o <base>     输出为任意指定进制 (2-36)\n\n");

    printf("格式化与修饰:\n");
    printf("  -g, --group   启用分组显示\n");
    printf("                二进制每4位加空格，十六进制每4位(16bit)加空格\n");
    printf("  -w <bits>     位宽限制 (8, 16, 32, 64)\n");
    printf("                模拟数据溢出，例如输入 0x1FF 且限制 -w 8，结果应为 0xFF\n");
    printf("  -u, --upper   强制大写 (主要用于 Hex, 如 0xFF vs 0xff)\n\n");

    printf("其他:\n");
    printf("  -h, --help    显示此帮助信息\n");
    printf("  -v, --version 显示版本信息\n\n");

    printf("使用场景:\n");
    printf("  查看寄存器值:   bconv 0x3A\n");
    printf("  二进制分组:     bconv -b -g 48879\n");
    printf("  模拟溢出:       bconv -d -w 8 300\n");
    printf("  管道输出:       bconv -x 255\n");
#endif

    return 0;
}

int printf_err(void)
{
    // TODO:标准错误输出流
    fprintf(stderr, "\n");
    print_help();
    return 1;
}

int main(int argc, char *argv[])
{
    int64_t input_val = 0; // 输入的值
    int64_t bits_limit = 0; // 位宽限制
    int64_t any_base = 0; // 指定进制
    CmdType_t type = command_parse(argc, argv, &input_val, &bits_limit);

#if defined DEBUG
    printf("type:%d\n", type);
#endif

    if (type == CMD_HELPER)
    {
        return print_help();
    }
    else if (type == CMD_VERSION)
    {
        return print_version();
    }
    else if (type == CMD_CONVERT)
    {
#if defined DEBUG
        printf("input_val = %ld, type = %d\n", input_val, type);
#endif
        convert_to_radix(options, input_val, bits_limit, hex_buffer, bin_buffer);
        if (BIT_TEST(options), OPT_ANY_BASE)
        {
            conver_to_any_radix(input_val, any_base, any_buffer);
            printf("%d Radix:%s\n", any_base, any_buffer);
        }
    }
    else if (type == CMD_INVALID_NUM)
    {
        // TODO：错误输出
    }

    return 0;
}
