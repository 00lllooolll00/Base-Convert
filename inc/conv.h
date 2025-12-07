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

#define OPT_BIT_LIMIT       7
#define OPT_HEX_UPPER       6
#define OPT_HEX_BIN_PREFIX  5
#define OPT_CUSTOM_BASE     4
#define OPT_HEX_BIN_GROUP   3
#define OPT_RADIX_DEC       2
#define OPT_RADIX_HEX       1
#define OPT_RADIX_BIN       0

#define BIT(x)              ((0x01) << (x))
#define BIT_SET(arg, bit)   ((arg) |= BIT(bit))
#define BIT_RESET(arg, bit) ((arg) &= ~BIT(bit))
#define BIT_TEST(arg, bit)  (((arg) & BIT(bit)) != 0)

void print_version(void);
void print_help(void);
bool get_input_val(const char *input, int64_t *dec_val);
void convert_to_radix(
    uint8_t options, int64_t val, uint8_t bit_limit, uint8_t custom_base, char *hex, char *bin, char *custom);
void conver_to_any_radix(int64_t dec_val, int64_t base, char *buffer);

#endif /* __CONV_H */
