#include "conv.h"

#define CMD_EQUAL(input_cmd, const_str) (strcmp((input_cmd), (const_str)) == 0)

/* 
options[7:0] 每一位的定义
[7]: 保留                                            默认为0
[6]：十六进制数是否大写                   0：小写 1：大写 默认开启
[5]：十六进制是否显示 "0x"                0：关闭 1：开启 默认开启
[4]: 二进制是否显示 "0b"                 0：关闭 1：开启 默认开启
[3]: 二进制是否四位分割(不足补0)           0：关闭 1：开启 默认关闭
[2]: 转换成十进制                        0：关闭 1：开启 默认开启
[1]: 转换成十六进制                      0：关闭 1：开启 默认开启
[0]: 转换成二进制                        0：关闭 1：开启 默认开启

默认 0b 0111 0111 = 0x77
*/
static uint8_t options = 0x77;
static char hex_buffer[BUFFER_SIZE]; // 十六进制缓冲区
static char bin_buffer[BUFFER_SIZE]; // 二进制缓冲区

/**
 * @brief 命令解析
 * 
 * @param argc 参数的数目
 * @param argv 存储命令的参数
 * @param val  输入的值的指针
 * @return CmdType_t 
 */
CmdType_t command_parse(int argc, char *argv[], int64_t *val)
{
    // 只输入了一个参数 返回帮助
    if (argc == 1) return CMD_HELPER;

    // 输入两个参数
    else if (argc == 2)
    {
        // 帮助信息
        if (CMD_EQUAL(argv[1], "-h") || CMD_EQUAL(argv[1], "--help"))
        {
            return CMD_HELPER;
        }
        // 版本号
        else if (CMD_EQUAL(argv[1], "-v") || CMD_EQUAL(argv[1], "--version"))
        {
            return CMD_VERSION;
        }
        // 看是否是合法数字
        else
        {
            RadixType_t res;

            res = detect_input_radix_type(argv[1], val);
#if defined DEBUG
            printf("InputType:%d\n", res);
#endif
            if (res == INPUT_INVALID) return CMD_UNKOWN;
            else return CMD_DEFAULT;
        }
    }
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
    printf("  Bin: 1111 1111\n\n");

    printf("输入控制:\n");
    printf("  -i <base>     强制指定输入进制 (2-36)\n");
    printf("                如果省略，程序将尝试自动推断 (0x=Hex, 0b=Bin, 其他=Dec)\n\n");

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
    fprintf(stderr, "err\n");
}

int main(int argc, char *argv[])
{
    int64_t input_val = 0;
    CmdType_t type = command_parse(argc, argv, &input_val);

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
    else if (type == CMD_DEFAULT)
    {
#if defined DEBUG
        printf("input_val = %ld, type = %d\n", input_val, type);
        convert_to_radix(options, input_val, bin_buffer, hex_buffer);
#else

#endif
    }

    return 0;
}
