#include "conv.h"

// 映射表 (支持到36进制: 0-9 + A-Z)
const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static void dec_to_bin(int64_t dec_val, uint8_t options, uint8_t bit_limit, char *buffer);
static void dec_to_hex(int64_t dec_val, uint8_t options, uint8_t bit_limit, char *buffer);
static void reverse_str(char *str);

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

void convert_to_radix(uint8_t options, int64_t val, uint8_t bit_limit, char *hex, char *bin)
{
    // 十进制
    if (BIT_TEST(options, OPT_RADIX_DEC))
    {
        printf("Dec:%ld\n", val);
    }

    // 十六进制
    if (BIT_TEST(options, OPT_RADIX_HEX))
    {
        dec_to_hex(val, options, bit_limit, hex);
        printf("Hex:%s\n", hex);
    }

    // 转换二进制
    if (BIT_TEST(options, OPT_RADIX_BIN))
    {
        dec_to_bin(val, options, bit_limit, bin);
        printf("Bin:%s\n", bin);
    }
}

void conver_to_any_radix(int64_t dec_val, int64_t base, char *buffer)
{
    // 检查进制范围：2-36进制
    if (base < 2 || base > 36)
    {
        sprintf(buffer, "不支持的进制: %u", base);
        return;
    }

    // 处理 0 的特殊情况
    if (dec_val == 0)
    {
        strcpy(buffer, "0");
        return;
    }

    // 处理负数 (通常任意进制转换倾向于视作无符号，但这里按数学逻辑保留符号)
    // 使用 unsigned 防止 INT64_MIN 溢出

    int is_negative = 0;
    uint64_t num;
    if (dec_val < 0)
    {
        is_negative = 1;
        num = (uint64_t)(-dec_val);
    }
    else
    {
        num = (uint64_t)dec_val;
    }

    int index = 0;

    // 核心循环：取模 -> 查表 -> 除法
    while (num > 0)
    {
        int remainder = num % base; // 拿到余数
        buffer[index++] = digits[remainder];
        num /= base;
    }

    // 如果是负数，补上负号
    if (is_negative)
    {
        buffer[index++] = '-';
    }

    // 封口
    buffer[index] = '\0';

    // 翻转 (因为上面算出来的是倒着的)
    reverse_str(buffer);
}

static void reverse_str(char *str)
{
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++)
    {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}

static void dec_to_bin(int64_t dec_val, uint8_t options, uint8_t bit_limit, char *buffer)
{
    uint8_t index = 0; // 起始位置索引
    bool split = BIT_TEST(options, OPT_HEX_BIN_SPLIT); // 是否需要分隔

    // 开启前缀
    if (BIT_TEST(options, OPT_HEX_BIN_PERFIX))
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

static void dec_to_hex(int64_t dec_val, uint8_t options, uint8_t bit_limit, char *buffer)
{
    uint8_t index = 0;

    // 开启前缀 0x
    if (BIT_TEST(options, OPT_HEX_BIN_PERFIX))
    {
        buffer[index++] = '0';
        buffer[index++] = 'x';
    }

    // 开启了分割
    if (BIT_TEST(options, OPT_HEX_BIN_SPLIT))
    {
        // 计算十六进制字符串的长度
        char temp_buffer[32] = {0}; // 临时存储十六进制字符串
        int hex_len;

        // 大写
        if (BIT_TEST(options, OPT_HEX_UPPER))
        {
            hex_len = snprintf(temp_buffer, sizeof(temp_buffer), "%lX", dec_val);
        }
        else
        {
            hex_len = snprintf(temp_buffer, sizeof(temp_buffer), "%lx", dec_val);
        }

        // 计算需要补齐的位数，使其成为4的倍数
        int padding = (4 - (hex_len % 4)) % 4;
        int total_len = hex_len + padding;
        int space_count = (total_len / 4) - 1; // 空格数量

        // 确保缓冲区足够大
        if (index + total_len + space_count >= BUFFER_SIZE)
        {
            buffer[index] = '\0';
            return;
        }

        // 补前导零（如果需要）
        for (int i = 0; i < padding; i++)
        {
            buffer[index++] = '0';
        }

        // 复制十六进制字符并添加空格
        int temp_idx = 0;
        int group_count = 0;

        while (temp_idx < hex_len)
        {
            // 每4个字符添加一个空格（除了第一组）
            if (group_count > 0 && (temp_idx % 4) == 0)
            {
                buffer[index++] = ' ';
            }

            buffer[index++] = temp_buffer[temp_idx++];
            group_count = (temp_idx + padding) / 4; // 计算当前组数
        }

        buffer[index] = '\0';
    }
    else
    {
        // 大写
        if (BIT_TEST(options, OPT_HEX_UPPER))
        {
            snprintf(buffer + index, BUFFER_SIZE - index, "%lX");
        }
        else
        {
            snprintf(buffer + index, BUFFER_SIZE - index, "%lx");
        }
    }
}
