#include <stdio.h>
#include <stdint.h>

#include "../include/math_x.h"

#define FIELD_SIZE 256
#define MOD 0x11B // x^8 + x^4 + x^3 + x + 1 (для AES)

// Таблицы поиска (Lookup Tables)
uint8_t log_table[FIELD_SIZE];
uint8_t antilog_table[FIELD_SIZE];

// Предрасчет таблиц логарифмов и антилогарифмов
void precompute_tables(uint8_t g) {
    // Таблица антилогарифмов (экспонент) и логарифмов
    // Цикл по всем степеням от 0 до 254 (field_size - 2)
    for (int i = 0; i < FIELD_SIZE - 1; ++i) {
        uint8_t val = gf_pow(g, i);
        antilog_table[i] = val;
        log_table[val] = (uint8_t)i;
    }

    // Значение для 255 дублирует 0 из-за цикличности (g^255 = 1)
    antilog_table[255] = antilog_table[0];
}

// Быстрое умножение через таблицы логарифмов
uint8_t gf_mult_table(uint8_t a, uint8_t b) {
    if (a == 0 || b == 0) {
        return 0;
    }

    // Сумма логарифмов по модулю (FIELD_SIZE - 1), т.е. % 255
    int log_sum = (log_table[a] + log_table[b]) % (FIELD_SIZE - 1);

    printf("%u", antilog_table[log_sum]);
    return antilog_table[log_sum];
}
