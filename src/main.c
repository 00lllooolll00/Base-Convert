#include <getopt.h>
#include "conv.h"

// 命令行参数列表
static const struct option long_options[] = {
    {"help",    no_argument,       NULL, 'h'}, // 获取帮助
    {"version", no_argument,       NULL, 'v'}, // 获取版本号
    {"hex",     no_argument,       NULL, 'x'}, // 只显示十六进制
    {"dec",     no_argument,       NULL, 'd'}, // 只显示十进制
    {"bin",     no_argument,       NULL, 'b'}, // 只显示二进制
    {"prefix",  no_argument,       NULL, 'p'}, // 显示二进制、十六进制前缀
    {"upper",   no_argument,       NULL, 'u'}, // 十六进制大写
    {"group",   no_argument,       NULL, 'g'}, // 二进制、十六进制分组显示
    {"width",   required_argument, NULL, 'w'}, // 二进制、十六进制位宽
    {"custom",  required_argument, NULL, 'c'}, // 自定义进制
    {0,         0,                 NULL, 0  }  // 哨兵
};

/* 
options[7:0] 每一位的定义
[7]: 开启位宽限制                        0：关闭 1：开启 默认关闭
[6]：十六进制数是否大写                   0：小写 1：大写 默认关闭
[5]：十六、二进制是否显示 "0x/0b"          0：关闭 1：开启 默认关闭
[4]: 开启任意进制转换                     0：关闭 1：开启 默认关闭

[3]: 十六、二进制是否四位分割(不足补0)      0：关闭 1：开启 默认关闭
[2]: 转换成十进制                        0：关闭 1：开启 默认开启
[1]: 转换成十六进制                      0：关闭 1：开启 默认开启
[0]: 转换成二进制                        0：关闭 1：开启 默认开启

默认 0b 0000 0111 = 0x07
*/

int main(int argc, char *argv[])
{
    static int64_t input_val = 0; // 输入的值
    static int64_t bits_limit = 0; // 位宽限制
    static int64_t custom_base = 0; // 指定进制
    static uint8_t mutex_count = 0; // 用来保证 -b -d -x 只有一个参数

    static char hex_buffer[BUFFER_SIZE]; // 十六进制缓冲区
    static char bin_buffer[BUFFER_SIZE]; // 二进制缓冲区
    static char custom_buffer[BUFFER_SIZE]; // 任意进制缓冲区

    uint8_t options = 0x07;
    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "hvxbdpugw:c:", long_options, &option_index)) != -1)
    {
        char *endptr;
        int64_t temp;

        switch (opt)
        {
            case 'h':
                print_help();
                exit(0);

            case 'v':
                print_version();
                exit(0);

                // 互斥进制显示
            case 'x':
                BIT_SET(options, OPT_RADIX_HEX);
                BIT_RESET(options, OPT_RADIX_DEC);
                BIT_RESET(options, OPT_RADIX_BIN);
                mutex_count++;
                break;

            case 'b':
                BIT_SET(options, OPT_RADIX_BIN);
                BIT_RESET(options, OPT_RADIX_DEC);
                BIT_RESET(options, OPT_RADIX_HEX);
                mutex_count++;
                break;

            case 'd':
                BIT_SET(options, OPT_RADIX_DEC);
                BIT_RESET(options, OPT_RADIX_BIN);
                BIT_RESET(options, OPT_RADIX_HEX);
                mutex_count++;
                break;

            case 'p':
                BIT_SET(options, OPT_HEX_BIN_PREFIX);
                break;

            // 开启十六进制大写
            case 'u':
                BIT_SET(options, OPT_HEX_UPPER);
                break;

                // 开启分组显示
            case 'g':
                BIT_SET(options, OPT_HEX_BIN_GROUP);
                break;

                // 位宽限制
            case 'w':
                BIT_SET(options, OPT_BIT_LIMIT);

                temp = strtoll(optarg, &endptr, 10);

                if (*endptr != '\0' || (temp != 8 && temp != 16 && temp != 32 && temp != 64))
                {
                    printf("%ld\n", temp);
                    fprintf(stderr, "-w/--width 参数只能接受 8、16、32、64\n");
                    exit(1);
                }
                bits_limit = temp;

                break;

            // 自定义任意进制转换
            case 'c':
                BIT_RESET(options, OPT_RADIX_BIN);
                BIT_RESET(options, OPT_RADIX_HEX);
                BIT_RESET(options, OPT_RADIX_DEC);
                BIT_SET(options, OPT_CUSTOM_BASE);

                temp = strtoll(optarg, &endptr, 10);

                if (*endptr != '\0' || (temp < 2 || temp > 36))
                {
                    fprintf(stderr, "-c/--custom 参数只能接受 2~36\n");
                    exit(1);
                }
                custom_base = temp;
                break;

                // 处理未知选项
            case '?':
                if (optopt >= '0' && optopt <= '9')
                {
                    fprintf(stderr, "错误: 负数数值请使用 -- 分隔，例如: bconv -- -%c\n", optopt);
                }
                else
                {
                    fprintf(stderr, "错误: 未知选项 -%c\n", optopt);
                    print_help();
                }
                exit(1);

            default:
                exit(1);
        }
    }

    if (argc == 1)
    {
        print_help();
        exit(0);
    }

    if (mutex_count > 1)
    {
        fprintf(stderr, "-x/--hex -d/--dec -b/--bin 参数为互斥选项，只能输入其中一个\n");
        exit(1);
    }

    // 解析第一个非选项参数
    else if (get_input_val(argv[optind], &input_val) != true)
    {
        fprintf(stderr, "输入的参数非法\n");
        exit(1);
    }

    // 转换
    convert_to_radix(options, input_val, bits_limit, custom_base, hex_buffer, bin_buffer, custom_buffer);

    exit(0);
}
