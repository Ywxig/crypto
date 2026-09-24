#include <stdio.h>
#include <stdint.h>
#include <math.h>

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

// Простой прототип для проверки дискретного логарифма перебором
int brute_force_dlog(int g, int h, int p) {
    for (int x = 0; x < p - 1; x++) {
        if (mod_pow_ltr(g, x, p) == h) {
            return x;
        }
    }
    return -1;
}

/*
function BSGS(g, h, p):
    n = ceil(sqrt(p - 1))
    table = {}
    for j in 0..n-1:
        table[mod_pow(g, j, p)] = j
    g_inv_n = mod_inverse(mod_pow(g, n, p), p)
    gamma = h
    for i in 0..n-1:
        if gamma in table:
            return i*n + table[gamma]
        gamma = (gamma * g_inv_n) mod p
    return "there is no solution"
 */

int bsgs_dlog(int g, int h, int p) {
    // Нормализация оснований по модулю p (для случаев g >= p, как 31 mod 29)
    g = (g % p + p) % p;
    h = (h % p + p) % p;

    int n = (int)ceil(sqrt(p - 1));

    // Таблица шагов младенца: table[j] = g^j mod p
    int table[n];
    for (int j = 0; j < n; j++) {
        table[j] = mod_pow_ltr(g, j, p);
    }

    // 2. Фактор великана: (g^n)^(-1) mod p
    int g_n = mod_pow_ltr(g, n, p);
    int g_inv_n = mod_inverse(g_n, p);

    int gamma = h;

    // Шаги великана: ищем gamma среди ВСЕХ элементов таблицы baby steps
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j < n; j++) {
            if (gamma == table[j]) {
                return i * n + j; // Правильная формула: i * n + j
            }
        }
        gamma = (int)(((long long)gamma * g_inv_n) % p);
    }

    return -1; // Логарифм не найден
}
