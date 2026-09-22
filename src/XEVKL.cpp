#include <iostream>
#include <cstdint>

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

// Расширенный алгоритм Евклида для многочленов
void poly_ext_gcd(uint16_t a, uint16_t mod = 0x11B) {
    std::cout << "poly_ext_gcd: a = 0x" << std::hex << a << std::endl;
    if (a == 0) {
        std::cout << "GCD:     0x0\nInverse: 0x0" << std::endl;
        return;
    }

    // ИСПРАВЛЕНО: модуль делится на a, стартовые коэффициенты s = 0 и 1
    uint16_t old_r = mod, r = a;
    uint16_t old_s = 0,   s = 1;

    while (r != 0) {
        // Побитовое деление многочленов над GF(2)
        uint16_t q = 0;
        uint16_t rem = old_r;
        int deg_r = degree(r);

        while (true) {
            int deg_rem = degree(rem);
            if (deg_rem < deg_r) break; // Остаток меньше делителя

            int shift = deg_rem - deg_r;
            q ^= (1 << shift);       // Формируем частное q
            rem ^= (r << shift);     // Вычитаем (XOR) из остатка
        }

        // Пересчитываем коэффициенты s по алгоритму Евклида
        uint16_t next_s = old_s ^ poly_mult(q, s);

        old_r = r;
        r = rem;
        old_s = s;
        s = next_s;
    }

    // Вывод с отсечением до 8 бит (& 0xFF)
    std::cout << "GCD:     0x" << std::hex << old_r << std::endl;
    std::cout << "Inverse: 0x" << std::hex << (old_s & 0xFF) << std::endl;
}
