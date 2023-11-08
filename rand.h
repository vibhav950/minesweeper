#pragma once

#define E_INIT 0x2
#define E_ARGS 0x3

#include <inttypes.h>

typedef struct {
    uint64_t x0:64;
    uint64_t x1;
} _UINT128;

void randctx(void);
uint64_t ranged(uint64_t a, uint64_t b);
uint64_t nbits(uint8_t n, uint8_t enforce);