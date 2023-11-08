#include "./rand.h"
#include <stdlib.h>
#include <stdio.h>
#include <intrin.h>
#include <time.h>
#include <math.h>

/* FOR TESTING ONLY (USE rand.c) */

#pragma intrinsic(__rdtsc)

_UINT128 *_state = NULL;

void _shiftr(_UINT128 *_s)
{
    _s->x0 = (_s->x0 >> 0x1) | (_s->x1 << 63);
    _s->x1 = _s->x1 >> 0x1;
}

void randctx(void)
{
    _state = (_UINT128*) malloc(sizeof(_UINT128));
    if (!_state){
        exit(E_INIT);
    }

    _state->x0 = (uint64_t) __rdtsc();
    _state->x1 = (uint64_t) time(NULL);

    for (int i = 0; i < 128; ++i)
    {
        _shiftr(_state);
        _state->x1 |=
        ((uint64_t)(((_state->x0) ^ (_state->x0) >> 1) ^ ((_state->x0) >> 2) ^ ((_state->x0) >> 7) & 0x1) << 63);
    }
}

uint64_t nbits(uint8_t n, uint8_t enforce)
{
    if (n > 64 || n < 1)
    {
        exit(E_ARGS);
    }

    uint64_t xb;
    uint64_t out = 0x0;

    for (int i = 0; i < n-1; ++i)
    {
        xb = ((_state->x0) ^ (_state->x0) >> 1) ^ ((_state->x0) >> 2) ^ ((_state->x0) >> 7) & 0x1;
        _shiftr(_state);
        _state->x1 |= (xb << 63);
        out |= _state->x0 & 0x1;
        out <<= 1;
    }

    out |= _state->x0 & 0x1;
    
    if (enforce)
    {
        out |= (uint64_t) 0b1 << 63;
    }

    return out;
}

uint64_t ranged(uint64_t a, uint64_t b)
{
    if (!(a <= b && a >= 0))
    {
        exit(E_ARGS);
    }

    uint64_t out;
    int nb = (int) log2(b - a) + 1;

    out = nbits(nb, 0);
    return a + out % ((b - a) + 1);
}

int main() {
    randctx();

    // printf("%llu %llu\n\n", state->x0, state->x1);

    uint64_t res;

    printf("[");

    for (int i = 0; i < 20000; ++i) {
        res = ranged(0, 1000);
        printf("%llu,", res);
    }

    printf("]");

}