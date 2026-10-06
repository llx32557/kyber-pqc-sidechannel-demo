#include "kem.h"
#include "poly.h"
#include "params.h"
#include <stdlib.h>
#include <string.h>

// 简化 KEM 实现，重点在于调起 decap 流程
int crypto_kem_keypair(uint8_t *pk, uint8_t *sk) {
    for (int i = 0; i < KYBER_N; i++) {
        sk[i] = rand() % 256; // 生成随机私钥
    }
    memset(pk, 0, 32);
    return 0;
}

int crypto_kem_enc(uint8_t *ct, uint8_t *ss, const uint8_t *pk) {
    memset(ct, 0, 32);
    memset(ss, 0, 32);
    return 0;
}

// ★ decap 是侧信道检测的核心目标
int crypto_kem_dec(uint8_t *ss, const uint8_t *ct, const uint8_t *sk) {
    poly secret_poly;
    // 从 sk 恢复秘密多项式
    for (int i = 0; i < KYBER_N; i++) {
        secret_poly.coeffs[i] = sk[i] % KYBER_Q; 
    }
    
#ifdef USE_CONSTANT_TIME
    extern void poly_tomsg_constant_time(uint8_t msg[KYBER_INDCPA_MSGBYTES], const poly *a);
    poly_tomsg_constant_time(ss, &secret_poly);
#else
    poly_tomsg(ss, &secret_poly);
#endif
    return 0;
}