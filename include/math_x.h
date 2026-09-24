#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Модулярное возведение в степень
int mod_pow_ltr(int b, int e, int m);
int mod_pow_rtl(int b, int e, int m);
int montgomery_ladder(int b, int e, int m);
int sliding_window_pow(int b, int e, int m, int k);

// Операции с многочленами над GF(2) и элементами GF(2^8)
int degree(uint16_t poly);
uint16_t poly_mult(uint16_t a, uint16_t b);
uint8_t gf_mult(uint8_t a, uint8_t b);
uint8_t gf_pow(uint8_t g, int power);

// Табличные вычисления (из table_log.c)
uint8_t gf_mult_table(uint8_t a, uint8_t b);
void precompute_tables(uint16_t g);



// Операции с многочленами над GF(2) делением и остатком
uint16_t poly_xor(uint16_t a, uint16_t b);
uint16_t poly_div(uint16_t a, uint16_t b);
uint16_t poly_mod(uint16_t a, uint16_t b);

// Расширенный алгоритм Евклида для многочленов
#ifdef __cplusplus
uint8_t* poly_ext_gcd(uint16_t a, uint16_t mod = 0x11B);
#else
uint8_t* poly_ext_gcd(uint16_t a, uint16_t mod);
#endif

#ifdef __cplusplus
}
#endif
