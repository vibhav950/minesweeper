#pragma once

#define E_INIT 0x2
#define E_ARGS 0x3

#include <inttypes.h>

typedef struct _UINT128 {
    uint64_t x0;
    uint64_t x1;
} UINT128;

void randctx(void);
uint64_t ranged(uint64_t a, uint64_t b);
uint64_t nbits(uint8_t n, uint8_t enforce);