
#pragma once
#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include <cstdint>

// Операции в GF(2^8) принимают и возвращают беззнаковые 8-битные байты
uint8_t gf_mult(uint8_t a, uint8_t b);
void print_gf_mult(uint8_t a, uint8_t b);

// Вспомогательное умножение для Расширенного алгоритма Евклида (до 16 бит)
uint16_t poly_mult(uint16_t a, uint16_t b);

// Определение степени многочлена (старшего бита)
int degree(uint16_t poly);

// Расширенный алгоритм Евклида
void poly_ext_gcd(uint16_t a, uint16_t mod = 0x11B);

// таблица логорифмов


#endif // ALGORITHMS_HPP
