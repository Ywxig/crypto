#pragma once

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

int mod_pow_ltr(int b, int e, int m);
int mod_pow_rtl(int b, int e, int m);
int montgomery_ladder(int b, int e, int m);
int sliding_window_pow(int b, int e, int m, int k);

int degree(uint16_t poly);
uint16_t poly_mult(uint16_t a, uint16_t b);
uint8_t gf_mult(uint8_t a, uint8_t b);
uint8_t gf_pow(uint8_t g, int power);


#ifdef __cplusplus

}
#endif
