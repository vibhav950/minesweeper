#include "./rand.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#ifdef _WIN32
    #include <intrin.h>
    #pragma intrinsic(__rdtsc)
#endif

#ifdef __linux__
    #include <fcntl.h>
    #include <unistd.h>
#endif

_UINT128 *_state = NULL;

/* One shift-update move */
#define _rotate do {\
    _state->x0 = (_state->x0 >> 1) | (_state->x1 << 63);\
    _state->x1 >>= 1;\
    _state->x1 |=\
    ((uint64_t)(((_state->x0) ^ (_state->x0) >> 1) ^ ((_state->x0) >> 2) ^ ((_state->x0) >> 7) & 0x1) << 63);\
    } while(0)

/* Has to be called to initiate LFSR, otherwise error */
void randctx(void)
{
    _state = (_UINT128*) malloc(sizeof(_UINT128));
    if (!_state)
    {
        exit(E_INIT);
    }

    /* On Windows machines, seed the generator with a
       high-precision CPU counter and the current timestamp */
    #ifdef _WIN32
        _state->x0 = (uint64_t) __rdtsc();
        _state->x1 = (uint64_t) time(NULL);
    #endif

    /* On Linux machines, seed the generator entirely with
       random bytes from /dev/urandom */
    #ifdef __linux__
        int res;
        if ((res = open("/dev/urandom", O_RDONLY)) < 0)
        {
            exit(E_INIT);
        }
        if (read(res, (void*) _state, sizeof(_UINT128)) < 0)
        {
            exit(E_INIT);
        }
    #endif

    for (int i = 0; i < 128; ++i)
    {
        /* Prevent state leaks */
        _rotate;
    }
}

/* Generate n bits */
uint64_t nbits(uint8_t n, uint8_t enforce)
{
    if (n > 64 || n < 1)
    {
        exit(E_ARGS);
    }

    uint64_t out = 0x0;

    /* Perform shifting for n iterations */
    for (int i = 0; i < n-1; ++i)
    {
        _rotate;
        out |= _state->x0 & 0x1;
        out <<= 1;
    }

    out |= _state->x0 & 0x1;
    
    if (enforce)
    {
        out |= (uint64_t) 0x1 << 63;
    }

    return out;
}

/* Get unsigned int n; a <= n <= b */
uint64_t ranged(uint64_t a, uint64_t b)
{
    if (!(a <= b && a >= 0))
    {
        exit(E_ARGS);
    }

    uint64_t out;

    out = nbits(64, 0);
    /* Reduce n-bit number to the requested range */
    return a + out % ((b - a) + 1);
}