#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>
#include "kem.h"
#include "params.h"
#include "tvla_collect.h"

static inline uint64_t rdtsc() {
    unsigned int aux;
    return __rdtscp(&aux);
}

void run_tvla_collection(const char *filename, int samples_per_group) {
    uint8_t pk[256], sk_fixed[256], sk_rand[256], ct[256], ss[32];
    uint64_t start, end, cycles;
    FILE *fp;

    printf("=== TVLA Fixed vs Random Sampling ===\n");
    printf("Samples per group: %d\n", samples_per_group);

    // 1. 生成固定私钥 + 密文
    crypto_kem_keypair(pk, sk_fixed);
    crypto_kem_enc(ct, ss, pk);

    fp = fopen(filename, "w");
    if (!fp) { perror("Cannot open file"); return; }
    fprintf(fp, "group,cycles\n");

    // 预热
    for (int i = 0; i < 1000; i++) crypto_kem_dec(ss, ct, sk_fixed);

    // 2. 采集 Fixed 组：固定 sk
    printf("[*] Collecting Fixed group...\n");
    for (int i = 0; i < samples_per_group; i++) {
        _mm_mfence();
        start = rdtsc();
        crypto_kem_dec(ss, ct, sk_fixed);
        _mm_mfence();
        end = rdtsc();
        fprintf(fp, "0,%lu\n", end - start);
    }

    // 3. 采集 Random 组：每次换新 sk
    printf("[*] Collecting Random group...\n");
    for (int i = 0; i < samples_per_group; i++) {
        crypto_kem_keypair(pk, sk_rand);
        _mm_mfence();
        start = rdtsc();
        crypto_kem_dec(ss, ct, sk_rand);
        _mm_mfence();
        end = rdtsc();
        fprintf(fp, "1,%lu\n", end - start);
    }

    fclose(fp);
    printf("[*] TVLA data saved to %s\n", filename);
}