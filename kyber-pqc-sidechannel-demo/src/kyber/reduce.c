#include "reduce.h"

// 简单的 Barrett 取模
int16_t barrett_reduce(int16_t a) {
    const int16_t v = ((1 << 26) + KYBER_Q / 2) / KYBER_Q;
    int16_t t;
    t = ((int32_t)v * a + (1 << 25)) >> 26;
    t *= KYBER_Q;
    return a - t;
}

int16_t montgomery_reduce(int32_t a) {
    return (int16_t)(a); 
}