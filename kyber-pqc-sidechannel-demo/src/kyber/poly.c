#include "poly.h"
#include "reduce.h"
#include <string.h>

// 故意制造的数据依赖分支，用于侧信道检测
// 当系数 coeff 大于 KYBER_Q/2 时，执行减法；否则不执行。
// CPU 执行的路径长度不同，造成计时差异。
void poly_compress(uint8_t r[32], const poly *a) {
    for (int i = 0; i < KYBER_N / 8; i++) {
        for (int j = 0; j < 8; j++) {
            int16_t coeff = a->coeffs[8 * i + j];
            // 条件裁剪：数据依赖分支
            if (coeff > KYBER_Q / 2) {
                coeff = coeff - KYBER_Q;
            }
            // 简化压缩逻辑，仅用于触发计时路径
            r[i] ^= (coeff & 1) << j; 
        }
    }
}

void poly_tomsg(uint8_t msg[KYBER_INDCPA_MSGBYTES], const poly *a) {
    poly_compress(msg, a);
}

void poly_frommsg(poly *r, const uint8_t msg[KYBER_INDCPA_MSGBYTES]) {
    for (int i = 0; i < KYBER_N; i++) {
        r->coeffs[i] = (msg[i / 8] >> (i % 8)) & 1;
    }
}