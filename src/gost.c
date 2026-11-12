#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/globals.h"

#define GOST_BLOCK 8

/* Таблица замен ГОСТ 28147-89, вариант id-tc26-gost-28147-param-Z */
const uint8_t gost_sbox[8][16] = {
    {12, 4, 6, 2, 10, 5, 11, 9, 14, 8, 13, 7, 0, 3, 15, 1},
    {6, 8, 2, 3, 9, 10, 5, 12, 1, 14, 4, 7, 11, 13, 0, 15},
    {11, 3, 5, 8, 2, 15, 10, 13, 14, 1, 7, 4, 12, 9, 6, 0},
    {12, 8, 2, 1, 13, 4, 15, 6, 7, 0, 10, 5, 3, 14, 9, 11},
    {7, 15, 5, 10, 8, 1, 6, 13, 0, 9, 3, 14, 11, 4, 2, 12},
    {5, 13, 15, 6, 9, 2, 12, 10, 11, 7, 8, 1, 4, 3, 14, 0},
    {8, 14, 2, 5, 6, 9, 1, 12, 15, 4, 11, 0, 13, 10, 3, 7},
    {1, 7, 14, 13, 0, 5, 8, 3, 4, 15, 10, 6, 9, 12, 11, 2}
};

/* Константы ГОСТ (для имитовставки и дополнительных режимов) */
const uint32_t GOST_C1 = 0x01010104;
const uint32_t GOST_C2 = 0x01010101;

/* Тестовый ключ ГОСТ из методички */
const uint8_t gost_default_key[32] = {
    0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88,
    0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

void print_hex(const uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02X", buf[i]);
    }
}

void print_hex_lower(const uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", buf[i]);
    }
}

/*
 * ГОСТ 28147-89 / «Магма».
 * Классическая сеть Фейстеля: 32 раунда, блок 64 бита, ключ 256 бит.
 * Функция раунда: F(R, K) = ROL11(T(R + K)).
 */

/* Циклический сдвиг 32-битного слова влево. */
uint32_t ROL32(uint32_t x, int n) {
    if (n == 0) return x;
    return (x << n) | (x >> (32 - n));
}

/* Табличная замена по полубайтам (8 S-блоков по 16 значений). */
uint32_t T(uint32_t a) {
    uint32_t r = 0;

    for (int i = 0; i < 8; i++) {
        r |= (uint32_t)gost_sbox[i][(a >> (4 * i)) & 0xF] << (4 * i);
    }

    return r;
}

/* Чтение/запись 32-битных слов в big-endian. */
void load32be(const uint8_t *src, uint32_t *dst, int count) {
    for (int i = 0; i < count; i++) {
        dst[i] = ((uint32_t)src[4 * i] << 24) |
                 ((uint32_t)src[4 * i + 1] << 16) |
                 ((uint32_t)src[4 * i + 2] << 8) |
                 (uint32_t)src[4 * i + 3];
    }
}

void store32be(uint8_t *dst, const uint32_t *src, int count) {
    for (int i = 0; i < count; i++) {
        dst[4 * i] = (uint8_t)(src[i] >> 24);
        dst[4 * i + 1] = (uint8_t)(src[i] >> 16);
        dst[4 * i + 2] = (uint8_t)(src[i] >> 8);
        dst[4 * i + 3] = (uint8_t)(src[i] & 0xFF);
    }
}

/* Расширение 256-битного ключа в 8 подключей. */
void GOST_keyexp(const uint8_t key[32], uint32_t K[8]) {
    load32be(key, K, 8);
}

static uint32_t gost_f(uint32_t r, uint32_t k) {
    return ROL32(T(r + k), 11);
}

/*
 * Один блок. Внутри используется обычная сеть Фейстеля:
 *   (L, R) -> (R, L ^ F(R, K))
 * После 32 раундов половины меняются местами.
 */
void GOST_encrypt(const uint32_t K[8], uint8_t data[8]) {
    uint32_t l, r, t;

    load32be(data, &l, 1);
    load32be(data + 4, &r, 1);

    for (int i = 0; i < 32; i++) {
        uint32_t k = (i < 24) ? K[i % 8] : K[7 - (i - 24)];

        t = l ^ gost_f(r, k);
        l = r;
        r = t;
    }

    /* финальный обмен половин */
    store32be(data, &r, 1);
    store32be(data + 4, &l, 1);
}

void GOST_decrypt(const uint32_t K[8], uint8_t data[8]) {
    uint32_t l, r, t;

    load32be(data, &l, 1);
    load32be(data + 4, &r, 1);

    for (int i = 0; i < 32; i++) {
        uint32_t k = (i < 8) ? K[i] : K[7 - ((i - 8) % 8)];

        t = l ^ gost_f(r, k);
        l = r;
        r = t;
    }

    store32be(data, &r, 1);
    store32be(data + 4, &l, 1);
}

/* ------------------------------------------------------------------------- */
/* PKCS#7 (общие функции для ГОСТ и IDEA)                                    */
/* ------------------------------------------------------------------------- */

size_t pkcs7_pad(uint8_t *buf, size_t len, size_t block, size_t max_len) {
    size_t pad = block - (len % block);

    if (len + pad > max_len) return 0;

    memset(buf + len, (int)pad, pad);
    return len + pad;
}

/* Имя из методички для функции дополнения блока. */
size_t padding(uint8_t *buf, size_t len, size_t block, size_t max_len) {
    return pkcs7_pad(buf, len, block, max_len);
}

int pkcs7_unpad(uint8_t *buf, size_t len, size_t block) {
    uint8_t pad;

    if (len == 0 || (len % block) != 0) return -1;

    pad = buf[len - 1];
    if (pad < 1 || pad > block) return -1;

    for (size_t i = len - pad; i < len; i++) {
        if (buf[i] != pad) return -1;
    }

    return (int)(len - pad);
}

/* ------------------------------------------------------------------------- */
/* Режимы работы                                                             */
/* ------------------------------------------------------------------------- */

int gost_encrypt_msg(const uint8_t key[32], const char *msg,
                     uint8_t *out, size_t out_max) {
    size_t len;
    size_t total;
    uint32_t K[8];

    if (!key || !msg || !out) return -1;

    len = strlen(msg);
    total = (len / GOST_BLOCK + 1) * GOST_BLOCK;
    if (total > out_max) return -1;

    memcpy(out, msg, len);
    if (pkcs7_pad(out, len, GOST_BLOCK, out_max) == 0) return -1;

    GOST_keyexp(key, K);

    for (size_t i = 0; i < total; i += GOST_BLOCK) {
        GOST_encrypt(K, out + i);
    }

    return (int)total;
}

int gost_decrypt_msg(const uint8_t key[32], const uint8_t *in, size_t len,
                     char *out, size_t out_max) {
    uint32_t K[8];
    int text_len;

    if (!key || !in || !out) return -1;
    if (len == 0 || (len % GOST_BLOCK) != 0) return -1;
    if (len + 1 > out_max) return -1;

    memcpy(out, in, len);
    GOST_keyexp(key, K);

    for (size_t i = 0; i < len; i += GOST_BLOCK) {
        GOST_decrypt(K, (uint8_t *)out + i);
    }

    text_len = pkcs7_unpad((uint8_t *)out, len, GOST_BLOCK);
    if (text_len < 0) return -1;

    out[text_len] = '\0';
    return text_len;
}

void gost_ctr_crypt(const uint8_t key[32], const uint8_t iv[8],
                    const uint8_t *in, uint8_t *out, size_t len) {
    uint32_t K[8];
    uint8_t counter[8];
    uint8_t stream[8];

    GOST_keyexp(key, K);
    memcpy(counter, iv, 8);

    for (size_t off = 0; off < len; off += GOST_BLOCK) {
        size_t n = len - off;

        if (n > GOST_BLOCK) n = GOST_BLOCK;

        memcpy(stream, counter, 8);
        GOST_encrypt(K, stream);

        for (size_t i = 0; i < n; i++) {
            out[off + i] = (uint8_t)(in[off + i] ^ stream[i]);
        }

        for (int i = 7; i >= 0; i--) {
            if (++counter[i] != 0) break;
        }
    }
}

/*
 * Имитовставка ГОСТ 28147-89.
 * Каждый блок складывается по XOR с текущим состоянием, после чего
 * выполняется 16 раундов с последовательностью ключей K0..K7, K0..K7.
 * Результат — 32-битное значение.
 */
uint32_t gost_mac(const uint8_t key[32], const uint8_t *data, size_t len) {
    uint32_t K[8];
    uint32_t n1 = 0, n2 = 0;
    size_t blocks = (len + GOST_BLOCK - 1) / GOST_BLOCK;

    if (blocks == 0) blocks = 1;

    GOST_keyexp(key, K);

    for (size_t b = 0; b < blocks; b++) {
        uint8_t block[GOST_BLOCK] = {0};
        size_t off = b * GOST_BLOCK;
        size_t n = len - off;

        if (n > GOST_BLOCK) n = GOST_BLOCK;
        if (off < len) memcpy(block, data + off, n);

        uint32_t b1, b2;
        load32be(block, &b1, 1);
        load32be(block + 4, &b2, 1);

        n1 ^= b1;
        n2 ^= b2;

        for (int i = 0; i < 16; i++) {
            uint32_t t = n1 ^ gost_f(n2, K[i % 8]);
            n1 = n2;
            n2 = t;
        }
    }

    return n1;
}

/* ------------------------------------------------------------------------- */
/* Команды для main.c                                                        */
/* ------------------------------------------------------------------------- */

int GOST_encrypt_cmd(const char *msg, const uint8_t key[32]) {
    size_t len = strlen(msg);
    size_t out_max = len + GOST_BLOCK;
    uint8_t *out = (uint8_t *)malloc(out_max);
    int n;

    if (!out) {
        fprintf(stderr, "GOST: memory allocation failed\n");
        return 4;
    }

    n = gost_encrypt_msg(key, msg, out, out_max);
    if (n < 0) {
        fprintf(stderr, "GOST: encryption failed\n");
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

int GOST_decrypt_cmd(const char *prog, const uint8_t key[32], const char *hex) {
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
    if (n <= 0 || (n % GOST_BLOCK) != 0) {
        fprintf(stderr, "%s: cipher must be hex, length multiple of 16 chars\n", prog);
        free(cipher);
        return 2;
    }

    plain = (char *)malloc((size_t)n + 1);
    if (!plain) {
        fprintf(stderr, "%s: memory allocation failed\n", prog);
        free(cipher);
        return 4;
    }

    m = gost_decrypt_msg(key, cipher, (size_t)n, plain, (size_t)n + 1);
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

int GOST_ctr_encrypt_cmd(const char *msg, const uint8_t key[32]) {
    uint8_t iv[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    size_t len = strlen(msg);
    uint8_t *out = (uint8_t *)malloc(len ? len : 1);

    if (!out) {
        fprintf(stderr, "GOST-CTR: memory allocation failed\n");
        return 4;
    }

    gost_ctr_crypt(key, iv, (const uint8_t *)msg, out, len);

    printf("key: ");
    print_hex_lower(key, 32);
    printf("\niv: ");
    print_hex(iv, 8);
    printf("\nenc: ");
    print_hex(out, len);
    printf("\n");

    free(out);
    return 0;
}

int GOST_ctr_decrypt_cmd(const char *prog, const uint8_t key[32],
                         const char *iv_hex, const char *hex) {
    uint8_t iv[8];
    size_t hex_len = strlen(hex);
    size_t cipher_max = hex_len / 2 + 1;
    uint8_t *cipher = (uint8_t *)malloc(cipher_max);
    char *plain;
    int n;

    if (!cipher) {
        fprintf(stderr, "%s: memory allocation failed\n", prog);
        return 4;
    }

    if (hex_to_bytes(iv_hex, iv, sizeof(iv)) != (int)sizeof(iv)) {
        fprintf(stderr, "%s: IV must be 16 hex chars\n", prog);
        free(cipher);
        return 2;
    }

    n = hex_to_bytes(hex, cipher, cipher_max);
    if (n <= 0) {
        fprintf(stderr, "%s: cipher must be hex\n", prog);
        free(cipher);
        return 2;
    }

    plain = (char *)malloc((size_t)n + 1);
    if (!plain) {
        fprintf(stderr, "%s: memory allocation failed\n", prog);
        free(cipher);
        return 4;
    }

    gost_ctr_crypt(key, iv, cipher, (uint8_t *)plain, (size_t)n);
    plain[n] = '\0';

    printf("key: ");
    print_hex_lower(key, 32);
    printf("\niv: ");
    print_hex(iv, 8);
    printf("\ndec: %s\n", plain);

    free(cipher);
    free(plain);
    return 0;
}

int GOST_mac_cmd(const char *msg, const uint8_t key[32]) {
    uint32_t mac = gost_mac(key, (const uint8_t *)msg, strlen(msg));

    printf("key: ");
    print_hex_lower(key, 32);
    printf("\nmac: %08X\n", mac);

    return 0;
}
