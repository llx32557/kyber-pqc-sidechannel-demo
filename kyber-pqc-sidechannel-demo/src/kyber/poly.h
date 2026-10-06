#ifndef POLY_H
#define POLY_H
#include <stdint.h>
#include "params.h"

typedef struct {
    int16_t coeffs[KYBER_N];
} poly;

void poly_compress(uint8_t r[32], const poly *a);
void poly_tomsg(uint8_t msg[KYBER_INDCPA_MSGBYTES], const poly *a);
void poly_frommsg(poly *r, const uint8_t msg[KYBER_INDCPA_MSGBYTES]);

#endif