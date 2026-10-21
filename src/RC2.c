#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../include/globals.h"

// PITABLE — таблица подстановки (S-Box)
static const uint8_t PITABLE[256] = {
    0xD9, 0x78, 0xF9, 0xC4, 0x19, 0xDD, 0xB5, 0xED, 0x28, 0xE9, 0xFD, 0x79, 0x4A, 0xA0, 0xD8, 0x9D,
    0xC6, 0x7E, 0x37, 0x83, 0x2B, 0x76, 0x53, 0x8E, 0x62, 0x4C, 0x64, 0x88, 0x44, 0x8B, 0xFB, 0xA2,
    0x17, 0x9A, 0x59, 0xF5, 0x87, 0xB3, 0x4F, 0xCE, 0xE1, 0x29, 0x57, 0x0C, 0x40, 0x7B, 0x0D, 0x48,
    0x89, 0x2A, 0x1E, 0xE7, 0x38, 0xC5, 0xD6, 0xB7, 0xA5, 0xE5, 0x82, 0x69, 0x70, 0x9E, 0xEB, 0x56,
    0x8C, 0x1B, 0xDB, 0x4B, 0xBE, 0x46, 0xD3, 0x90, 0x7C, 0x21, 0xD0, 0x7D, 0x8A, 0xCE, 0x14, 0x81,
    0x6C, 0x16, 0x0E, 0x61, 0x12, 0x1C, 0x05, 0x47, 0xC0, 0x42, 0x7A, 0x41, 0x61, 0xC1, 0x31, 0xC8,
    0x7F, 0x2E, 0xF0, 0x51, 0xA7, 0x71, 0xCD, 0xEF, 0xCB, 0x2C, 0x83, 0xC9, 0x4E, 0x6F, 0x2F, 0x84,
    0x2A, 0xDF, 0xB8, 0x01, 0x5B, 0x36, 0x58, 0xB2, 0x3D, 0xD5, 0x22, 0x6C, 0xDE, 0x06, 0x30, 0x6E,
    0x23, 0x50, 0x60, 0xCE, 0x69, 0x67, 0x32, 0xA8, 0x07, 0x1D, 0x25, 0xF1, 0x98, 0x11, 0x60, 0x35,
    0xAE, 0x8E, 0x2D, 0x03, 0x00, 0x34, 0xB6, 0x85, 0x20, 0xBF, 0x92, 0x77, 0x0F, 0x81, 0x10, 0x00,
    0x80, 0xA1, 0x4D, 0x24, 0x95, 0xB4, 0x96, 0x0A, 0x15, 0x0B, 0xC3, 0x2D, 0x18, 0x39, 0x23, 0x9B,
    0x26, 0x8F, 0x3A, 0x9F, 0x63, 0xF7, 0xC2, 0xD2, 0x5E, 0xC7, 0x27, 0x09, 0xF6, 0xB1, 0x91, 0x2B,
    0x1F, 0x52, 0x09, 0xAD, 0x73, 0x67, 0x13, 0x3F, 0x5C, 0x49, 0x43, 0x3E, 0x80, 0xB0, 0x33, 0x6A,
    0x97, 0x3B, 0xD1, 0x5F, 0x3C, 0x4D, 0x54, 0x8F, 0xE2, 0x29, 0x8B, 0x3C, 0x99, 0xA6, 0x74, 0x31,
    0xE4, 0x5D, 0x1A, 0xE8, 0x08, 0x72, 0x00, 0xA3, 0x6D, 0x94, 0xAC, 0x93, 0x75, 0x14, 0xE0, 0x68,
    0x65, 0xBC, 0xB9, 0x4C, 0xC8, 0x48, 0x86, 0x66, 0x3E, 0x10, 0x7B, 0x20, 0xB7, 0x8D, 0xAA, 0xD4
};

#define BLOCK_SIZE 8

//
// Вспомогательные функции
//

// Строка hex -> байты
int hex_to_bytes(const char *hex_str, uint8_t *out_buf, size_t out_max_len) {
    size_t len = strlen(hex_str);
    if (len % 2 != 0 || len / 2 > out_max_len) {
        return -1;
    }

    for (size_t i = 0; i < len; i += 2) {
        char byte_str[3] = { hex_str[i], hex_str[i + 1], '\0' };
        char *endptr = NULL;
        unsigned long val = strtoul(byte_str, &endptr, 16);
        if (*endptr != '\0') {
            return -1;
        }
        out_buf[i / 2] = (uint8_t)val;
    }
    return (int)(len / 2);
}

// Циклический сдвиг 16-битного числа влево / вправо (shift от 1 до 15)
static inline uint16_t rol16(uint16_t v, int s) {
    return (uint16_t)((v << s) | (v >> (16 - s)));
}

static inline uint16_t ror16(uint16_t v, int s) {
    return (uint16_t)((v >> s) | (v << (16 - s)));
}

//
// Генерация 64 субключей
//
static void rc2_key_gen(const uint8_t *key, size_t key_len, uint16_t *K) {
    uint8_t L[128];
    memset(L, 0, sizeof(L));
    memcpy(L, key, key_len < 128 ? key_len : 128);

    for (size_t i = key_len; i < 128; i++) {
        L[i] = PITABLE[(L[i - 1] + L[i - key_len]) & 0xFF];
    }

    for (int i = 0; i < 64; i++) {
        K[i] = (uint16_t)L[2 * i] | ((uint16_t)L[2 * i + 1] << 8);
    }
}

//
// Раунды: прямой и обратный
//

// Один раунд Mixing (4 шага), использует K[j .. j+3]
static void mix_round(uint16_t *A, uint16_t *B, uint16_t *C, uint16_t *D,
                      const uint16_t *K, int j) {
    *A = rol16((uint16_t)(*A + (*B & ~*D) + (*C & *D) + K[j + 0]), 1);
    *B = rol16((uint16_t)(*B + (*C & ~*A) + (*D & *A) + K[j + 1]), 2);
    *C = rol16((uint16_t)(*C + (*D & ~*B) + (*A & *B) + K[j + 2]), 3);
    *D = rol16((uint16_t)(*D + (*A & ~*C) + (*B & *C) + K[j + 3]), 5);
}

// Обратный раунд Mixing: шаги в обратном порядке (D, C, B, A)
static void unmix_round(uint16_t *A, uint16_t *B, uint16_t *C, uint16_t *D,
                        const uint16_t *K, int j) {
    *D = (uint16_t)(ror16(*D, 5) - (*A & ~*C) - (*B & *C) - K[j + 3]);
    *C = (uint16_t)(ror16(*C, 3) - (*D & ~*B) - (*A & *B) - K[j + 2]);
    *B = (uint16_t)(ror16(*B, 2) - (*C & ~*A) - (*D & *A) - K[j + 1]);
    *A = (uint16_t)(ror16(*A, 1) - (*B & ~*D) - (*C & *D) - K[j + 0]);
}

// Mashing
static void mash(uint16_t *A, uint16_t *B, uint16_t *C, uint16_t *D,
                 const uint16_t *K) {
    *A = rol16((uint16_t)(*A + K[*D & 63]), 1);   // D ещё не изменён
    *B = rol16(*B, 1);
    *C = rol16(*C, 1);
    *D = rol16(*D, 1);
}

// Обратный Mashing
static void unmash(uint16_t *A, uint16_t *B, uint16_t *C, uint16_t *D,
                   const uint16_t *K) {
    *B = ror16(*B, 1);
    *C = ror16(*C, 1);
    *D = ror16(*D, 1);                             // D восстановлен первым
    *A = (uint16_t)(ror16(*A, 1) - K[*D & 63]);
}

// Один 8-байтный блок
static void load_block(const uint8_t *b, uint16_t *A, uint16_t *B,
                       uint16_t *C, uint16_t *D) {
    *A = (uint16_t)(b[0] | (b[1] << 8));
    *B = (uint16_t)(b[2] | (b[3] << 8));
    *C = (uint16_t)(b[4] | (b[5] << 8));
    *D = (uint16_t)(b[6] | (b[7] << 8));
}

static void store_block(uint8_t *b, uint16_t A, uint16_t B,
                        uint16_t C, uint16_t D) {
    b[0] = A & 0xFF; b[1] = A >> 8;
    b[2] = B & 0xFF; b[3] = B >> 8;
    b[4] = C & 0xFF; b[5] = C >> 8;
    b[6] = D & 0xFF; b[7] = D >> 8;
}

static void encrypt_block(uint8_t *blk, const uint16_t *K) {
    uint16_t A, B, C, D;
    load_block(blk, &A, &B, &C, &D);

    int j = 0;
    for (int i = 0; i < 5; i++, j += 4) mix_round(&A, &B, &C, &D, K, j);
    mash(&A, &B, &C, &D, K);
    for (int i = 0; i < 6; i++, j += 4) mix_round(&A, &B, &C, &D, K, j);
    mash(&A, &B, &C, &D, K);
    for (int i = 0; i < 5; i++, j += 4) mix_round(&A, &B, &C, &D, K, j);

    store_block(blk, A, B, C, D);
}

static void decrypt_block(uint8_t *blk, const uint16_t *K) {
    uint16_t A, B, C, D;
    load_block(blk, &A, &B, &C, &D);

    int j = 60;                                    // 64 - 4
    for (int i = 0; i < 5; i++, j -= 4) unmix_round(&A, &B, &C, &D, K, j);
    unmash(&A, &B, &C, &D, K);
    for (int i = 0; i < 6; i++, j -= 4) unmix_round(&A, &B, &C, &D, K, j);
    unmash(&A, &B, &C, &D, K);
    for (int i = 0; i < 5; i++, j -= 4) unmix_round(&A, &B, &C, &D, K, j);

    store_block(blk, A, B, C, D);
}

//
// Публичный интерфейс
//
int RC2_encrypt(const char *msg, long long key, uint8_t *out, size_t out_max) {
    if (!msg || !out) return -1;

    size_t len   = strlen(msg);
    size_t total = (len / BLOCK_SIZE + 1) * BLOCK_SIZE;   // PKCS#7: всегда 1..8 байт
    if (total > out_max) return -1;

    memcpy(out, msg, len);
    memset(out + len, (int)(total - len), total - len);

    uint16_t K[64];
    rc2_key_gen((const uint8_t *)&key, sizeof(key), K);

    for (size_t i = 0; i < total; i += BLOCK_SIZE) {
        encrypt_block(out + i, K);
    }
    return (int)total;
}

int RC2_decrypt(const uint8_t *in, size_t len, long long key,
                char *out, size_t out_max) {
    if (!in || !out || len == 0 || len % BLOCK_SIZE != 0 || len + 1 > out_max) {
        return -1;
    }

    memcpy(out, in, len);

    uint16_t K[64];
    rc2_key_gen((const uint8_t *)&key, sizeof(key), K);

    for (size_t i = 0; i < len; i += BLOCK_SIZE) {
        decrypt_block((uint8_t *)out + i, K);
    }

    // Проверка и снятие PKCS#7
    uint8_t pad = (uint8_t)out[len - 1];
    if (pad < 1 || pad > BLOCK_SIZE || pad > len) return -1;
    for (size_t i = len - pad; i < len; i++) {
        if ((uint8_t)out[i] != pad) return -1;
    }

    size_t text_len = len - pad;
    out[text_len] = '\0';
    return (int)text_len;
}
