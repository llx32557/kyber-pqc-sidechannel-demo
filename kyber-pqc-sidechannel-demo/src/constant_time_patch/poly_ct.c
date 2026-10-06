#include "../kyber/poly.h"
#include "../kyber/params.h"

// ★ 常数时间修复版：消除数据依赖的 if 分支
// 使用位运算掩码替代条件跳转
void poly_compress_constant_time(uint8_t r[32], const poly *a) {
    for (int i = 0; i < KYBER_N / 8; i++) {
        for (int j = 0; j < 8; j++) {
            int16_t coeff = a->coeffs[8 * i + j];
            
            // 常数时间条件减法
            // 如果 coeff > Q/2，mask 为全 1；否则为 0
            int16_t mask = -(coeff > (KYBER_Q / 2));
            // 使用掩码替代 if (coeff > Q/2) coeff -= Q;
            coeff -= (KYBER_Q & mask);
            
            r[i] ^= (coeff & 1) << j;
        }
    }
}

void poly_tomsg_constant_time(uint8_t msg[KYBER_INDCPA_MSGBYTES], const poly *a) {
    poly_compress_constant_time(msg, a);
}