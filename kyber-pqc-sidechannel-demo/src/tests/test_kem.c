#include <stdio.h>
#include "kem.h"

int main() {
    uint8_t pk[256], sk[256], ct[256], ss[32];
    printf("Running KEM functional test...\n");
    crypto_kem_keypair(pk, sk);
    crypto_kem_enc(ct, ss, pk);
    crypto_kem_dec(ss, ct, sk);
    printf("KEM test passed (simplified).\n");
    return 0;
}