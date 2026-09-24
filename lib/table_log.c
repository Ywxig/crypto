#include <stdio.h>
#include <stdint.h>

#include "../include/math_x.h"

#define FIELD_SIZE 256
#define MOD 0x11B // x^8 + x^4 + x^3 + x + 1 (для AES)

// Предрасчет таблиц логарифмов и антилогарифмов
/*
function precompute_tables(g, field_size):
    for i in 0..field_size-2:
        log_table[gf_pow(g, i)] = i
        antilog_table[i] = gf_pow(g, i)
    return log_table, antilog_table
 */

// Глобальные таблицы
uint8_t log_table[FIELD_SIZE];
uint8_t antilog_table[FIELD_SIZE];

void precompute_tables(uint16_t g) {
    for (int i = 0; i < FIELD_SIZE - 1; ++i) {
        // Прямое возведение генератора в степень i
        uint8_t val = gf_pow((uint8_t)g, i);

        antilog_table[i] = val;
        log_table[val]   = (uint8_t)i;
    }

    // Специальное/неопределенное значение для log(0)
    log_table[0] = 0;

    // Закольцовываем antilog для защиты от переполнения
    antilog_table[255] = antilog_table[0];
}

// Быстрое умножение через таблицы логарифмов
uint8_t gf_mult_table(uint8_t a, uint8_t b) {
    if (a == 0 || b == 0) {
        return 0;
    }

    // Сумма логарифмов по модулю (FIELD_SIZE - 1), т.е. % 255
    uint8_t log_sum = (log_table[a] + log_table[b]) % (FIELD_SIZE - 1);

    printf("%u", antilog_table[log_sum]);
    return antilog_table[log_sum];
}
