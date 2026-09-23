#include <stdio.h>
#include <stdint.h>

#include "../include/math_x.h"

// Расширенный алгоритм Евклида для многочленов над GF(2)
// Если тип возвращаемого значения uint8_t:
uint8_t poly_ext_gcd(uint16_t a, uint16_t mod) {
    if (a == 0) {
        printf("GCD:     0x0\nInverse: 0x0\n");
        return 0;
    }

    uint16_t old_r = mod, r = a;
    uint16_t old_s = 0,   s = 1;

    while (r != 0) {
        uint16_t q = 0;
        uint16_t rem = old_r;
        int deg_r = degree(r);

        while (1) {
            int deg_rem = degree(rem);
            if (deg_rem < deg_r) break;

            int shift = deg_rem - deg_r;
            q ^= (1 << shift);
            rem ^= (r << shift);
        }

        uint16_t next_s = old_s ^ poly_mult(q, s);

        old_r = r;
        r = rem;
        old_s = s;
        s = next_s;
    }

    printf("GCD:     0x%x\n", old_r);
    printf("Inverse: 0x%x\n", old_s & 0xFF);

    // Возврат значения с отсечением до 8 бит
    return (uint8_t)(old_s & 0xFF);
}
