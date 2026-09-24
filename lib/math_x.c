#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>

#include "../include/math_x.h"

// Функция принимает число и массив, куда запишет строку
void binary_representation(int e, char *buffer) {
    int index = 0;

    // Особый случай для нуля
    if (e == 0) {
        buffer[index++] = '0';
        buffer[index] = '\0';
        return;
    }

    // Находим позицию самого старшего бита, равного 1
    int started = 0;
    for (int i = sizeof(e) * 8 - 1; i >= 0; i--) {
        int bit = (e >> i) & 1;

        if (bit == 1) {
            started = 1; // Нашли первую единицу, начинаем запись
        }

        if (started) {
            buffer[index++] = bit ? '1' : '0';
        }
    }

    buffer[index] = '\0'; // Закрываем строку символом конца строки
}


/*
function mod_pow_rtl(b, e, m):
    result = 1
    b = b mod m
    while e > 0:
        if (e AND 1) == 1:
            result = (result * b) mod m
        e = e >> 1
        b = (b * b) mod m
    return result
*/

// int mod_pow_rtl(int b, int e, int m) {
//     int result = 1;
//     b = b % m;
//     while (e > 0) {
//         if (e & 1) {
//             result = (result * b) % m;
//         }
//         e >>= 1;
//         b = (b * b) % m;
//     }
//     return result;
// }

/*
function mod_pow_ltr(b, e, m):
    result = 1
    // LSB first
    bits = binary_representation(e)
    for bit in bits:
        if bit == 1:
            result = (result * b) % m
        result = (result * result) % m
    return result
*/

int mod_pow_ltr(int b, int e, int m) {
    if (e == 0) return 1;
    int result = 1;

    char bits[33];
    binary_representation(e, bits);

    for (int i = 0; bits[i] != '\0'; i++) {
        result = (result * result) % m;
        if (bits[i] == '1') {
            result = (result * b) % m;
        }
    }
    return result;
}

/* function montgomery_ladder(b, e, m):
R0 = 1
R1 = b mod m
// MSB first
bits = binary_representation(e)
for bit in bits:
    if bit == 0:
        R1 = (R0 * R1) % m
        R0 = (R0 * R0) % m
    else:
        R0 = (R0 * R1) % m
        R1 = (R1 * R1) % m
return R0*/

int montgomery_ladder(int b, int e, int m) {
    int R0 = 1;
    int R1 = b % m;

    char bits[33];
    binary_representation(e, bits);

    for (int i = 0; bits[i] != '\0'; i++) {
        if (bits[i] == '0') {
            R1 = (R0 * R1) % m;
            R0 = (R0 * R0) % m;
        } else {
            R0 = (R0 * R1) % m;
            R1 = (R1 * R1) % m;
        }
    }
    return R0;
}

/*
function sliding_window_pow(b, e, m, k):
    precalculate b^1, b^3, b^5, ..., b^(2^k - 1) mod m
    result = 1
    divide the bits of e (from L to R) into windows of at most k bits
    each window starts and ends with bit = 1
    for each window:
        result = result^(2^window_length) mod m
        result = (result * b^(window_value)) mod m
    return result
*/

int sliding_window_pow(int b, int e, int m, int k) {
    int result = 1;
    int window_value = 0;
    int window_length = 0;

    char bits[33];
    binary_representation(e, bits);

    for (int i = 0; bits[i] != '\0'; i++) {
        if (bits[i] == '1') {
            window_value = (window_value << 1) | 1;
            window_length++;
        } else {
            window_value <<= 1;
        }

        if (window_length == k) {
            result = (result * mod_pow_ltr(b, window_value, m)) % m;
            window_value = 0;
            window_length = 0;
        }
    }
    return result;
}

uint8_t gf_mult(uint8_t a, uint8_t b) {
    uint8_t p = 0;

    for (int i = 0; i < 8; ++i) {
        if (b & 1) {
            p ^= a;
        }

        uint8_t hi_bit = a & 0x80;
        a = (uint8_t)(a << 1);

        if (hi_bit) {
            a ^= 0x1B;
        }

        b >>= 1;
    }

    return p;
}


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

uint16_t poly_xor(uint16_t a, uint16_t b) {
    return a ^ b;
}

// Функция вычисляет только частное (q)
uint16_t poly_div(uint16_t a, uint16_t b) {
    if (b == 0) { exit(1); }
    uint16_t q = 0, rem = a;
    int deg_b = degree(b);

    for (int shift = degree(a) - deg_b; shift >= 0; --shift) {
        if (rem & (1 << (shift + deg_b))) {
            q ^= (1 << shift);
            rem ^= (b << shift);
        }
    }
    return q;
}

// Функция вычисляет только остаток (rem)
uint16_t poly_mod(uint16_t a, uint16_t b) {
    if (b == 0) { exit(1); }
    uint16_t rem = a;
    int deg_b = degree(b);

    for (int shift = degree(a) - deg_b; shift >= 0; --shift) {
        if (rem & (1 << (shift + deg_b))) {
            rem ^= (b << shift);
        }
    }
    return rem;
}
