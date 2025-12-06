#ifndef __CONV_H
#define __CONV_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define BCONV_VERSION       "0.0.1"
#define BUFFER_SIZE         100

#define OPT_bits_limit      7
#define OPT_HEX_UPPER       6
#define OPT_HEX_BIN_PERFIX  5
#define OPT_ANY_BASE        4
#define OPT_HEX_BIN_SPLIT   3
#define OPT_RADIX_DEC       2
#define OPT_RADIX_HEX       1
#define OPT_RADIX_BIN       0

#define BIT(x)              ((0x01) << (x))
#define BIT_SET(arg, bit)   ((arg) |= BIT(bit))
#define BIT_RESET(arg, bit) ((arg) &= ~BIT(bit))
#define BIT_TEST(arg, bit)  (((arg) & BIT(bit)) != 0)

/**
 * @brief 命令类型
 * 
 */
typedef enum
{
    CMD_INVALID_NUM = -1, // 非法数字
    CMD_CONVERT = 0, // 转换
    CMD_HELPER, // 获取帮助
    CMD_VERSION, // 获取版本号

} CmdType_t;

/**
 * @brief 输入的类型
 * 
 */
typedef enum
{
    INPUT_INVALID = -1, // 非法数据
    INPUT_DEC = 0, // 十进制
    INPUT_BIN, // 二进制
    INPUT_HEX, // 十六进制
} RadixType_t;

RadixType_t detect_input_radix_type(const char *input, int64_t *dec_val);
void convert_to_radix(uint8_t options, int64_t val, uint8_t bit_limit, char *hex, char *bin);
void conver_to_any_radix(int64_t dec_val, int64_t base, char *buffer);

#endif /* __CONV_H */
