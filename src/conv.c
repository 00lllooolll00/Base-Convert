#include "conv.h"

static void dec_to_bin(int64_t dec_val, uint8_t options, char *buffer);
static void dec_to_hex(int64_t dec_val, uint8_t options, char *buffer);

/**
 * @brief 判断输入值的进制
 * 
 * @param input 输入的值
 * @param dec_val 转换成功后的值(十进制)
 * @return RadixType_t 
 */
RadixType_t detect_input_radix_type(const char *input, int64_t *dec_val)
{
    // 输入为空
    if (input == NULL || *input == '\0')
    {
        return INPUT_INVALID;
    }

    // 判断是否全是空格的
    while (isspace((unsigned char)*input)) input++;
    if (*input == '\0') return INPUT_INVALID;

    uint8_t plus_val = 2; // 步进值 正的时候为2 负的时候为3
    uint8_t start_val = 0; // 起始索引
    int8_t multi_val = 1; // 最后结果乘的值

    if (input[0] == '-')
    {
        plus_val = 3;
        start_val = 1;
        multi_val = -1;
    }

    // 是不是二进制
    if (input[start_val] == '0' && (input[start_val + 1] == 'b' || input[start_val + 1] == 'B'))
    {
        char *endptr;
        int64_t temp = strtoll(input + plus_val, &endptr, 2);
        // 如果转换后的结束指针是 '\0' 且 输入不仅仅是 "0b"，则是合法
        if (*endptr == '\0' && input[start_val + 2] != '\0')
        {
            *dec_val = temp * multi_val;
            return INPUT_BIN;
        }
        return INPUT_INVALID;
    }
    // 十六进制
    else if (input[start_val] == '0' && (input[start_val + 1] == 'x' || input[start_val + 1] == 'X'))
    {
        char *endptr;
        int64_t temp = strtoll(input + plus_val, &endptr, 16);
        if (*endptr == '\0' && input[start_val + 2] != '\0')
        {
            *dec_val = temp * multi_val;
            return INPUT_HEX;
        }
        return INPUT_INVALID;
    }
    // 十进制
    else
    {
        char *endptr;
        int64_t temp = strtoll(input, &endptr, 10);
        if (*endptr == '\0')
        {
            *dec_val = temp * multi_val;
            return INPUT_DEC;
        }
        return INPUT_INVALID;
    }
}

void convert_to_radix(uint8_t options, int64_t val, char *hex, char *bin)
{
    // 十进制
    if (BIT_TEST(options, OPT_RADIX_DEC))
    {
        printf("Dec:%ld\n", val);
    }

    // 转换二进制
    if (BIT_TEST(options, OPT_RADIX_BIN))
    {
        dec_to_bin(val, options, bin);
        printf("Bin:%s\n", bin);
    }

    // 十六进制
    if (BIT_TEST(options, OPT_RADIX_HEX))
    {
        dec_to_hex(val, options, hex);
        printf("Hex:%s\n", hex);
    }
}

static void dec_to_bin(int64_t dec_val, uint8_t options, char *buffer)
{
    uint8_t index = 0; // 起始位置索引
    bool split = BIT_TEST(options, OPT_BIN_SPLIT); // 是否需要分隔

    // 开启前缀
    if (BIT_TEST(options, OPT_BIN_PERFIX))
    {
        buffer[0] = '0';
        buffer[1] = 'b';
        index = 2;
    }

    // 值为0的特殊处理
    if (dec_val == 0)
    {
        if (split)
        {
            // 补零到4位：0000
            buffer[index++] = '0';
            buffer[index++] = '0';
            buffer[index++] = '0';
            buffer[index++] = '0';
        }
        else
        {
            buffer[index++] = '0';
        }
        buffer[index] = '\0';
        return;
    }

    uint64_t u_val = (uint64_t)dec_val;

    // 找到最高有效位的位置
    uint8_t msb_pos = 63; // 最高位位置
    while (msb_pos > 0 && ((u_val >> msb_pos) & 0x01) == 0)
    {
        msb_pos--;
    }

    // 计算需要补多少个零到4的倍数
    uint8_t bits_to_print = msb_pos + 1; // 需要打印的总位数
    uint8_t padding_zeros = (4 - (bits_to_print % 4)) % 4; // 补零数量

    // 先打印补零
    for (uint8_t i = 0; i < padding_zeros; i++)
    {
        buffer[index++] = '0';
    }

    // 然后打印实际的二进制位
    for (int8_t i = msb_pos; i >= 0; i--)
    {
        uint8_t bit = (u_val >> i) & 0x01;
        buffer[index++] = bit ? '1' : '0';

        // 在适当位置插入空格（考虑前缀偏移）
        if (split && i > 0 && ((i + padding_zeros) % 4 == 0))
        {
            buffer[index++] = ' ';
        }
    }

    buffer[index] = '\0';
}

static void dec_to_hex(int64_t dec_val, uint8_t options, char *buffer)
{
    // 开启前缀 0x
    if (BIT_TEST(options, OPT_HEX_PERFIX))
    {
        // 开启大写
        if (BIT_TEST(options, OPT_HEX_UPPER))
        {
            snprintf(buffer, BUFFER_SIZE, "0x%lX", dec_val);
        }
        else
        {
            snprintf(buffer, BUFFER_SIZE, "0x%lx", dec_val);
        }
    }
    // 关闭前缀 0x
    else
    {
        if (BIT_TEST(options, OPT_HEX_UPPER))
        {
            snprintf(buffer, BUFFER_SIZE, "%lX", dec_val);
        }
        else
        {
            snprintf(buffer, BUFFER_SIZE, "%lx", dec_val);
        }
    }
}
