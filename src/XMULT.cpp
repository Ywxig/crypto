#include <iostream>
#include <cstdint>


uint8_t gf_mult(uint8_t a, uint8_t b) {
    uint8_t p = 0;

    for (int i = 0; i < 8; ++i) {
        if (b & 1) {
            p ^= a;
        }

        bool hi_bit = (a & 0x80) != 0;
        a = static_cast<unsigned char>(a << 1); // Поведение unsigned char в C++ автоматически выполняет AND 0xFF

        if (hi_bit) {
            a ^= 0x1B;
        }

        b >>= 1;
    }

    return p;
}

void print_gf_mult(uint8_t a, uint8_t b) {
    uint8_t p = gf_mult(a, b);
    std::cout << "gf_mult(" << static_cast<int>(a) << ", " << static_cast<int>(b) << ") = " << static_cast<int>(p) << std::endl;
}
