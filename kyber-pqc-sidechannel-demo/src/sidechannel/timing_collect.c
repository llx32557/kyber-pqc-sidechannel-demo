#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <x86intrin.h> // 提供 __rdtscp
#include "kem.h"
#include "timing_collect.h"

static inline uint64_t rdtsc() {
    unsigned int aux;
    return __rdtscp(&aux);
}

void run_timing_collection(const char *filename, int iterations) {
    uint8_t pk[256], sk[256], ct[256], ss[32];
    uint64_t start, end, cycles;
    
    // 生成密钥对
    crypto_kem_keypair(pk, sk);
    
    FILE *fp = fopen(filename, "w");
    if (!fp) { perror("Error opening file"); return; }
    fprintf(fp, "iteration,cycles\n");

    // 预热 CPU，让频率稳定
    for (int i = 0; i < 1000; i++) {
        crypto_kem_dec(ss, ct, sk);
    }

    for (int i = 0; i < iterations; i++) {
        _mm_mfence(); // 内存屏障，防止指令重排
        start = rdtsc();
        
        // 核心：执行 decapsulation
        crypto_kem_dec(ss, ct, sk);
        
        _mm_mfence();
        end = rdtsc();
        
        cycles = end - start;
        fprintf(fp, "%d,%lu\n", i, cycles);
    }
    
    fclose(fp);
    printf("Successfully collected %d samples to %s\n", iterations, filename);
}