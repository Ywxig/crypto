#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "../include/math_x.h"
#include "../include/test_math.h"



void test_galois_field(void) {
    printf("[1/5] Тестирование операций в Поле Галуа GF(2^8)...\n");

    // Пример из FIPS-197 / Раздел 3.1.4: 0x57 * 0x83 = 0xC1
    uint8_t gf_prod = gf_mult(0x57, 0x83);
    printf("  gf_mult(0x57, 0x83) = 0x%02X (ожидается: 0xC1)\n", gf_prod);
    assert(gf_prod == 0xC1);

    // Тестирование таблиц логарифмов/антилогарифмов (Раздел 3.1.3)
    precompute_tables(0x03);
    printf("  gf_mult_table(0x57, 0x83) = ");
    uint8_t gf_table_prod = gf_mult_table(0x57, 0x83);
    printf(" (ожидается: 0xC1)\n");
    assert(gf_table_prod == 0xC1);

    // Расширенный алгоритм Евклида для многочленов (Раздел 3.1.2)
    uint8_t* poly_res = poly_ext_gcd(0x57, 0x11B);
    printf("  poly_ext_gcd(0x57, mod=0x11B): GCD = 0x%02X, Inverse = 0x%02X\n", poly_res[0], poly_res[1]);

    printf("  -> Поле Галуа: УСПЕШНО\n\n");
}

void test_modular_exponentiation(void) {
    printf("[2/5] Тестирование модулярного возведения в степень...\n");

    // Пример 1 из раздела 3.2.5: 4^13 mod 497 = 445
    int res1_ltr = mod_pow_ltr(4, 13, 497);
    int res1_mont = montgomery_ladder(4, 13, 497);
    int res1_win = sliding_window_pow(4, 13, 497, 2);
    printf("  4^13 mod 497: LTR=%d, Montgomery=%d, Window=%d (ожидается: 445)\n", res1_ltr, res1_mont, res1_win);
    assert(res1_ltr == 445);
    assert(res1_mont == 445);

    // Пример из раздела 3.5.1 (Пример 1): 15^4947 mod 31 = 23
    int res2 = mod_pow_ltr(15, 4947, 31);
    printf("  15^4947 mod 31 = %d (ожидается: 23)\n", res2);
    assert(res2 == 23);

    // Пример из раздела 3.5.1 (Пример 2): 7181^2953 mod 9286 = 9247
    int res3 = mod_pow_ltr(7181, 2953, 9286);
    printf("  7181^2953 mod 9286 = %d (ожидается: 9247)\n", res3);
    assert(res3 == 9247);

    // Пример из раздела 3.5.1 (Пример 3): 5617^1234 mod 9627 = 3979
    int res4 = mod_pow_ltr(5617, 1234, 9627);
    printf("  5617^1234 mod 9627 = %d (ожидается: 3979)\n", res4);
    assert(res4 == 3979);

    printf("  -> Модулярное возведение в степень: УСПЕШНО\n\n");
}

void test_modular_inverse(void) {
    printf("[3/5] Тестирование поиска обратного элемента по модулю...\n");

    // Пример из раздела 3.3.4 & 3.5.2 (Пример 1): 7^-1 mod 11 = 8
    uint16_t inv1 = mod_inverse(7, 11);
    printf("  7^-1 mod 11 = %d (ожидается: 8)\n", inv1);
    assert(inv1 == 8);

    // Пример из раздела 3.5.2 (Пример 2): 1182^-1 mod 578 (обратного не существует)
    uint16_t inv2 = mod_inverse(1182, 578);
    printf("  1182^-1 mod 578 = %d (ожидается: 0 — не существует)\n", inv2);
    assert(inv2 == 0);

    // Пример из раздела 3.5.2 (Пример 3): 1777^-1 mod 2699 = 1853
    uint16_t inv3 = mod_inverse(1777, 2699);
    printf("  1777^-1 mod 2699 = %d (ожидается: 1853)\n", inv3);
    assert(inv3 == 1853);

    printf("  -> Обратный элемент по модулю: УСПЕШНО\n\n");
}

void test_discrete_logarithm(void) {
    printf("[4/5] Тестирование дискретного логарифмирования...\n");

    // Пример из раздела 3.4.6: 2^x === 22 (mod 29) => x = 26
    int x1 = brute_force_dlog(2, 22, 29);
    printf("  2^x === 22 (mod 29) -> x = %d (ожидается: 26)\n", x1);
    assert(x1 == 26);

    // Пример из раздела 3.5.3: 31^x === 3 (mod 29) => x = 5
    int x2 = brute_force_dlog(31, 3, 29);
    printf("  31^x === 3 (mod 29) -> x = %d (ожидается: 5)\n", x2);
    assert(x2 == 5);

    printf("  -> Дискретный логарифм: УСПЕШНО\n\n");
}

void test_bsgs_logarithm(void) {
    printf("[5/5] Тестирование дискретного логарифмирования (BSGS)...\n");

    // Пример из раздела 3.4.6: 2^x === 22 (mod 29) => x = 26
    int x1 = bsgs_dlog(2, 22, 29);
    printf("  2^x === 22 (mod 29) -> x = %d (ожидается: 26)\n", x1);
    assert(x1 == 26);

    // Пример из раздела 3.5.3: 31^x === 3 (mod 29) -> x = 5
    int x2 = bsgs_dlog(31, 3, 29);
    printf("  31^x === 3 (mod 29) -> x = %d (ожидается: 5)\n", x2);
    assert(x2 == 5);

    printf("  -> Дискретный логарифм (BSGS): УСПЕШНО\n\n");
}

int START_TEST(void) {
    printf("=========================================\n");
    printf("   Запуск тестов криптографических тем   \n");
    printf("=========================================\n\n");

    test_galois_field();
    test_modular_exponentiation();
    test_modular_inverse();
    test_discrete_logarithm();
    test_bsgs_logarithm();

    printf("=========================================\n");
    printf("   ВСЕ ТЕСТЫ УСПЕШНО ПРОЙДЕНЫ!           \n");
    printf("=========================================\n");

    return 0;
}
