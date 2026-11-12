#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/globals.h"

#define KUZ_BLOCK 16

/*
 * «Кузнечик» (ГОСТ Р 34.12-2015).
 * Блок 128 бит, ключ 256 бит, 10 раундов SP-сети.
 * Для 128-битных значений используется расширение GCC unsigned __int128.
 */

typedef unsigned __int128 u128;

const uint8_t kuznyechik_sbox[256] = {
    252, 238, 221, 17, 207, 110, 49, 22, 251, 196, 250, 218, 35, 197, 4, 77,
    233, 119, 240, 219, 147, 46, 153, 186, 23, 54, 241, 187, 20, 205, 95, 193,
    249, 24, 101, 90, 226, 92, 239, 33, 129, 28, 60, 66, 139, 1, 142, 79,
    5, 132, 2, 174, 227, 106, 143, 160, 6, 11, 237, 152, 127, 212, 211, 31,
    235, 52, 44, 81, 234, 200, 72, 171, 242, 42, 104, 162, 253, 58, 206, 204,
    181, 112, 14, 86, 8, 12, 118, 18, 191, 114, 19, 71, 156, 183, 93, 135,
    21, 161, 150, 41, 16, 123, 154, 199, 243, 145, 120, 111, 157, 158, 178, 177,
    50, 117, 25, 61, 255, 53, 138, 126, 109, 84, 198, 128, 195, 189, 13, 87,
    223, 245, 36, 169, 62, 168, 67, 201, 215, 121, 214, 246, 124, 34, 185, 3,
    224, 15, 236, 222, 122, 148, 176, 188, 220, 232, 40, 80, 78, 51, 10, 74,
    167, 151, 96, 115, 30, 0, 98, 68, 26, 184, 56, 130, 100, 159, 38, 65,
    173, 69, 70, 146, 39, 94, 85, 47, 140, 163, 165, 125, 105, 213, 149, 59,
    7, 88, 179, 64, 134, 172, 29, 247, 48, 55, 107, 228, 136, 217, 231, 137,
    225, 27, 131, 73, 76, 63, 248, 254, 141, 83, 170, 144, 202, 216, 133, 97,
    32, 113, 103, 164, 45, 43, 9, 91, 203, 155, 37, 208, 190, 229, 108, 82,
    89, 166, 116, 210, 230, 244, 180, 192, 209, 102, 175, 194, 57, 75, 99, 182
};

/* Тестовый ключ «Кузнечика» из ГОСТ Р 34.12-2015 */
const uint8_t kuznyechik_default_key[32] = {
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
};

/* Коэффициенты линейного преобразования (GF(2^8), полином 0xC3). */
static const uint8_t kuz_coef[16] = {
    1, 148, 32, 133, 16, 194, 192, 1,
    251, 1, 192, 194, 16, 133, 32, 148
};

static uint8_t gf_mul(uint8_t a, uint8_t b) {
    uint8_t r = 0;

    for (int i = 0; i < 8; i++) {
        if (b & 1) r ^= a;
        uint8_t hi = a & 0x80;
        a = (uint8_t)(a << 1);
        if (hi) a ^= 0xC3;
        b >>= 1;
    }

    return r;
}

static uint8_t kuz_l(u128 x) {
    uint8_t r = 0;

    for (int i = 0; i < 16; i++) {
        r ^= gf_mul(kuz_coef[i], (uint8_t)((x >> (8 * i)) & 0xFF));
    }

    return r;
}

static u128 kuz_R(u128 x) {
    return (x >> 8) | ((u128)kuz_l(x) << 120);
}

static u128 kuz_L(u128 x) {
    for (int i = 0; i < 16; i++) {
        x = kuz_R(x);
    }
    return x;
}

static u128 kuz_Rinv(u128 y) {
    const u128 low120 = (((u128)1) << 120) - 1;
    u128 hi = y & low120;
    uint8_t top = (uint8_t)(y >> 120);
    uint8_t a0 = (uint8_t)(top ^ kuz_l(hi << 8));

    return (hi << 8) | a0;
}

static u128 kuz_Linv(u128 x) {
    for (int i = 0; i < 16; i++) {
        x = kuz_Rinv(x);
    }
    return x;
}

static u128 kuz_S(u128 x) {
    u128 r = 0;

    for (int i = 0; i < 16; i++) {
        uint8_t b = (uint8_t)((x >> (8 * i)) & 0xFF);
        r |= (u128)kuznyechik_sbox[b] << (8 * i);
    }

    return r;
}

static u128 kuz_Sinv(u128 x) {
    u128 r = 0;

    for (int i = 0; i < 16; i++) {
        uint8_t b = (uint8_t)((x >> (8 * i)) & 0xFF);
        uint8_t v = 0;

        for (int j = 0; j < 256; j++) {
            if (kuznyechik_sbox[j] == b) {
                v = (uint8_t)j;
                break;
            }
        }

        r |= (u128)v << (8 * i);
    }

    return r;
}

static u128 load128be(const uint8_t *p) {
    u128 x = 0;

    for (int i = 0; i < 16; i++) {
        x = (x << 8) | p[i];
    }

    return x;
}

static void store128be(uint8_t *p, u128 x) {
    for (int i = 15; i >= 0; i--) {
        p[i] = (uint8_t)(x & 0xFF);
        x >>= 8;
    }
}

/* Расширение ключа: 10 раундовых ключей по 128 бит. */
void kuznyechik_keyexp(const uint8_t key[32], uint8_t rk[10][16]) {
    u128 K[11];

    K[1] = load128be(key);
    K[2] = load128be(key + 16);

    for (int i = 1; i <= 4; i++) {
        u128 a1 = K[2 * i - 1];
        u128 a0 = K[2 * i];

        for (int j = 1; j <= 8; j++) {
            u128 c = kuz_L((u128)(8 * (i - 1) + j));
            u128 t = kuz_L(kuz_S(a1 ^ c)) ^ a0;

            a0 = a1;
            a1 = t;
        }

        K[2 * i + 1] = a1;
        K[2 * i + 2] = a0;
    }

    for (int i = 1; i <= 10; i++) {
        store128be(rk[i - 1], K[i]);
    }
}

void kuznyechik_encrypt(const uint8_t rk[10][16], uint8_t block[16]) {
    u128 x = load128be(block);

    for (int i = 0; i < 9; i++) {
        x = kuz_L(kuz_S(x ^ load128be(rk[i])));
    }

    x ^= load128be(rk[9]);
    store128be(block, x);
}

void kuznyechik_decrypt(const uint8_t rk[10][16], uint8_t block[16]) {
    u128 x = load128be(block);

    x ^= load128be(rk[9]);

    for (int i = 8; i >= 0; i--) {
        x = kuz_Sinv(kuz_Linv(x)) ^ load128be(rk[i]);
    }

    store128be(block, x);
}

int kuznyechik_encrypt_msg(const uint8_t key[32], const char *msg,
                           uint8_t *out, size_t out_max) {
    size_t len;
    size_t total;
    uint8_t rk[10][16];

    if (!key || !msg || !out) return -1;

    len = strlen(msg);
    total = (len / KUZ_BLOCK + 1) * KUZ_BLOCK;
    if (total > out_max) return -1;

    memcpy(out, msg, len);
    if (pkcs7_pad(out, len, KUZ_BLOCK, out_max) == 0) return -1;

    kuznyechik_keyexp(key, rk);

    for (size_t i = 0; i < total; i += KUZ_BLOCK) {
        kuznyechik_encrypt(rk, out + i);
    }

    return (int)total;
}

int kuznyechik_decrypt_msg(const uint8_t key[32], const uint8_t *in, size_t len,
                           char *out, size_t out_max) {
    uint8_t rk[10][16];
    int text_len;

    if (!key || !in || !out) return -1;
    if (len == 0 || (len % KUZ_BLOCK) != 0) return -1;
    if (len + 1 > out_max) return -1;

    memcpy(out, in, len);
    kuznyechik_keyexp(key, rk);

    for (size_t i = 0; i < len; i += KUZ_BLOCK) {
        kuznyechik_decrypt(rk, (uint8_t *)out + i);
    }

    text_len = pkcs7_unpad((uint8_t *)out, len, KUZ_BLOCK);
    if (text_len < 0) return -1;

    out[text_len] = '\0';
    return text_len;
}

int kuznyechik_encrypt_cmd(const char *msg, const uint8_t key[32]) {
    size_t len = strlen(msg);
    size_t out_max = len + KUZ_BLOCK;
    uint8_t *out = (uint8_t *)malloc(out_max);
    int n;

    if (!out) {
        fprintf(stderr, "Kuznyechik: memory allocation failed\n");
        return 4;
    }

    n = kuznyechik_encrypt_msg(key, msg, out, out_max);
    if (n < 0) {
        fprintf(stderr, "Kuznyechik: encryption failed\n");
        free(out);
        return 4;
    }

    printf("key: ");
    print_hex_lower(key, 32);
    printf("\nenc: ");
    print_hex(out, (size_t)n);
    printf("\n");

    free(out);
    return 0;
}

int kuznyechik_decrypt_cmd(const char *prog, const uint8_t key[32], const char *hex) {
    size_t hex_len = strlen(hex);
    size_t cipher_max = hex_len / 2 + 1;
    uint8_t *cipher = (uint8_t *)malloc(cipher_max);
    char *plain;
    int n, m;

    if (!cipher) {
        fprintf(stderr, "%s: memory allocation failed\n", prog);
        return 4;
    }

    n = hex_to_bytes(hex, cipher, cipher_max);
    if (n <= 0 || (n % KUZ_BLOCK) != 0) {
        fprintf(stderr, "%s: cipher must be hex, length multiple of 32 chars\n", prog);
        free(cipher);
        return 2;
    }

    plain = (char *)malloc((size_t)n + 1);
    if (!plain) {
        fprintf(stderr, "%s: memory allocation failed\n", prog);
        free(cipher);
        return 4;
    }

    m = kuznyechik_decrypt_msg(key, cipher, (size_t)n, plain, (size_t)n + 1);
    if (m < 0) {
        fprintf(stderr, "%s: decryption failed (wrong key or corrupted data)\n", prog);
        free(cipher);
        free(plain);
        return 4;
    }

    printf("key: ");
    print_hex_lower(key, 32);
    printf("\ndec: %s\n", plain);

    free(cipher);
    free(plain);
    return 0;
}
