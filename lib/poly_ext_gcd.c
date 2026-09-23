#include <stdio.h>
#include <stdint.h>

#include "../include/math_x.h"

// Расширенный алгоритм Евклида для многочленов над GF(2)
void poly_ext_gcd(uint16_t a, uint16_t mod) {
    printf("poly_ext_gcd: a = 0x%x\n", a);
    if (a == 0) {
        printf("GCD:     0x0\nInverse: 0x0\n");
        return;
    }

    uint16_t old_r = mod, r = a;
    uint16_t old_s = 0,   s = 1;

    while (r != 0) {
        // Побитовое деление многочленов над GF(2)
        uint16_t q = 0;
        uint16_t rem = old_r;
        int deg_r = degree(r);

        while (1) {
            int deg_rem = degree(rem);
            if (deg_rem < deg_r) break; // Остаток меньше делителя

            int shift = deg_rem - deg_r;
            q ^= (1 << shift);       // Формируем частное q
            rem ^= (r << shift);     // Вычитаем (XOR) из остатка
        }

        // Пересчитываем коэффициенты s по алгоритму Евклида
        uint16_t next_s = old_s ^ poly_mult(q, s);

        old_r = r;
        r = rem;
        old_s = s;
        s = next_s;
    }

    // Вывод с отсечением до 8 бит (& 0xFF)
    printf("GCD:     0x%x\n", old_r);
    printf("Inverse: 0x%x\n", old_s & 0xFF);
}
