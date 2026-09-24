
#include <stdint.h>
#include <sys/types.h>

#include "../include/math_x.h"

// Расширенный алгоритм Евклида для многочленов над GF(2)
// Если тип возвращаемого значения uint8_t:
/*
function poly_ext_gcd(a, b):
    old_r, r = a, b
    old_s, s = 1, 0
    while r != 0:
        // polynomial division (XOR-based)
        q = poly_div(old_r, r)
        old_r, r = r, poly_xor(old_r, poly_mult(q, r))
        old_s, s = s, poly_xor(old_s, poly_mult(q, s))
    // old_r = gcd (polynomial); old_s = inverse, if old_r = 1
    return old_r, old_s
 */
uint8_t* poly_ext_gcd(uint16_t a, uint16_t mod) {
    // Статический массив: память сохраняется между вызовами
    static uint8_t res[2];

    if (a == 0) { // Предотвращение зацикливания и математическая оптимизация
        res[0] = 0;
        res[1] = 0;
        return res;
    }

    uint16_t old_r = mod, r = a;
    uint16_t old_s = 0,   s = 1;

    while (r != 0) {
        uint16_t q = poly_div(old_r, r);

        // Исправлено: сначала вычисляем новый остаток с использованием старого old_r
        uint16_t temp_r = poly_xor(old_r, poly_mult(q, r));
        old_r = r;
        r = temp_r;

        // Вычисление нового коэффициента s
        uint16_t temp_s = poly_xor(old_s, poly_mult(q, s));
        old_s = s;
        s = temp_s;
    }

    res[0] = (uint8_t)old_r;
    res[1] = (uint8_t)old_s;

    return res;
}

/*
    function ext_gcd(a, b):
        old_r, r = a, b
        old_s, s = 1, 0
        while r != 0:
            q = old_r div r
            old_r, r = r, old_r - q*r
            old_s, s = s, old_s - q*s
        return old_r, old_s

    function mod_inverse(a, m):
        g, x = ext_gcd(a, m)
        if g != 1:
            error("the inverse does not exist")
        return x mod m
 */

uint16_t* ext_gcd(uint16_t a, uint16_t mod) {
    uint16_t old_r = a, r = mod;
    uint16_t old_s = 1, s = 0;

    static uint16_t res[2];

    if (a==0) { // предотвращения зацикливания и математичская оптимизация
        res[0] = 0;
        res[1] = 0;
        return res;
    }

    while (r != 0) {
        uint16_t q = poly_div(old_r, r);

        uint16_t temp_r = old_r; // Временная переменная так как без неё алгоритм превратится в r = r - q * r
        old_r = r;
        r = temp_r - q * r;

        uint16_t temp_s = old_s; // Временная переменная так как без неё алгоритм превратится в s = s - q * s
        old_s = s;
        s = temp_s - q * s;
    }

    res[0] = old_r;
    res[1] = old_s;
    return res;
}

uint16_t mod_inverse(uint16_t a, uint16_t mod) {
    uint16_t g = ext_gcd(a, mod)[0];
    if (g != 1) {
        return 0;
    }
    uint16_t x = ext_gcd(a, mod)[1];
    return x % mod;
}
