#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Вспомогательные функции для IDEA
static uint16_t mul(uint16_t a, uint16_t b) {
    uint32_t p;
    if (a == 0) {
        p = 0x10001 - b;
    } else if (b == 0) {
        p = 0x10001 - a;
    } else {
        p = (uint32_t)a * b;
        b = p & 0xFFFF;
        a = p >> 16;
        p = b - a + (b < a ? 1 : 0);
        p = (p == 0) ? 0x10001 : p;
    }
    return (uint16_t)(p & 0xFFFF);
}

static uint16_t add(uint16_t a, uint16_t b) {
    return (a + b) & 0xFFFF;
}

__attribute__((unused))
static uint16_t inv(uint16_t x) {
    uint16_t t0, t1;
    uint32_t q, y;

    if (x <= 1) return x;
    t1 = 0x10001L / x;
    y = 0x10001L % x;
    if (y == 1) return (1 - t1) & 0xFFFF;

    t0 = 1;
    do {
        q = x / y;
        x = x % y;
        t0 = t0 + q * t1;
        if (x == 1) return t0;
        q = y / x;
        y = y % x;
        t1 = t1 + q * t0;
    } while (y != 1);

    return (1 - t1) & 0xFFFF;
}

// Генерация ключей для шифрования (128-битный ключ -> 52 подключей по 16 бит)
static void idea_expand_key(const char* user_key, uint16_t* EK) {
    int i;
    for (i = 0; i < 8; i++) {
        EK[i] = ((uint8_t)user_key[2 * i] << 8) | (uint8_t)user_key[2 * i + 1];
    }
    for (i = 8; i < 52; i++) {
        if ((i % 8) == 6) {
            EK[i] = ((EK[i - 7] & 0x7F) << 9) | (EK[i - 14] >> 7);
        } else if ((i % 8) == 7) {
            EK[i] = ((EK[i - 15] & 0x7F) << 9) | (EK[i - 14] >> 7);
        } else {
            EK[i] = ((EK[i - 7] & 0x7F) << 9) | (EK[i - 6] >> 7);
        }
    }
}

void IDEA_encrypt(char* msg, char* key) {
    uint16_t X1, X2, X3, X4;
    uint16_t T1, T2;
    uint16_t EK[52];
    int round;

    memcpy(&X1, msg + 0, 2);
    memcpy(&X2, msg + 2, 2);
    memcpy(&X3, msg + 4, 2);
    memcpy(&X4, msg + 6, 2);

    idea_expand_key(key, EK);

    int k = 0;
    for (round = 0; round < 8; round++) {
        X1 = mul(X1, EK[k++]);
        X2 = add(X2, EK[k++]);
        X3 = add(X3, EK[k++]);
        X4 = mul(X4, EK[k++]);

        T1 = X1 ^ X3;
        T1 = mul(T1, EK[k++]);
        T2 = add(X2 ^ X4, T1);
        T2 = mul(T2, EK[k++]);
        T1 = add(T1, T2);

        X1 ^= T2;
        X4 ^= T1;

        T1 ^= X2;
        X2 = X3 ^ T2;
        X3 = T1;
    }

    X1 = mul(X1, EK[k++]);
    T1 = add(X2, EK[k++]);
    X2 = add(X3, EK[k++]);
    X3 = T1;
    X4 = mul(X4, EK[k++]);

    memcpy(msg + 0, &X1, 2);
    memcpy(msg + 2, &X2, 2);
    memcpy(msg + 4, &X3, 2);
    memcpy(msg + 6, &X4, 2);

    printf("Encrypted result: %04X %04X %04X %04X\n", X1, X2, X3, X4);
}
