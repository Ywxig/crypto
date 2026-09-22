#pragma once
#ifndef MATHX_HPP
#define MATHX_HPP

#include <cstdint>

uint8_t gf_pow(uint8_t g, int power);
int degree(uint16_t g);
uint16_t poly_mult(uint16_t a, uint16_t b);

#endif // MATHX_HPP
