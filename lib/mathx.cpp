#include <cstdint>
#include "../include/algoritms.hpp"

// Умножение многочленов без модульной редукции (GF(2))
uint16_t poly_mult(uint16_t a, uint16_t b) {
    uint16_t res = 0;
    while (b > 0) {
        if (b & 1) res ^= a;
        a <<= 1;
        b >>= 1;
    }
    return res;
}

// Вспомогательная функция для определения степени многочлена (индекс старшего бита)
int degree(uint16_t poly) {
    int deg = -1;
    while (poly > 0) {
        deg++;
        poly >>= 1;
    }
    return deg;
}

// Вычисление g^power в GF(2^8)
uint8_t gf_pow(uint8_t g, int power) {
    uint8_t res = 1;
    for (int i = 0; i < power; ++i) {
        res = gf_mult(res, g);
    }
    return res;
}
