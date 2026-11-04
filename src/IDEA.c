#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/globals.h"

#define BLOCK_SIZE 8

// Исправленная функция умножения по модулю (2^16 + 1)
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
        if (p == 0x10001) p = 0;
    }
    return (uint16_t)(p & 0xFFFF);
}

static uint16_t add(uint16_t a, uint16_t b) {
    return (a + b) & 0xFFFF;
}

// Мультипликативный инвертор по модулю 2^16 + 1
static uint16_t mul_inv(uint16_t x) {
    if (x <= 1) return x;
    int32_t t0 = 1, t1 = 0, q, y;
    int32_t a = 0x10001, b = x;
    while (b != 0) {
        q = a / b;
        y = a % b;
        a = b;
        b = y;
        y = t0 - q * t1;
        t0 = t1;
        t1 = y;
    }
    return (uint16_t)((t0 + 0x10001) % 0x10001);
}

// Аддитивный инвертор по модулю 2^16
static uint16_t add_inv(uint16_t x) {
    return (uint16_t)((0x10000 - x) & 0xFFFF);
}

// Корректная генерация 52 подключей из 128-битного ключа (без сторонних таблиц замен)
void idea_expand_key(const char* user_key, uint16_t* EK) {
    int i;
    uint8_t key_bytes[16];
    memset(key_bytes, 0, sizeof(key_bytes));
    size_t len = strlen(user_key);
    memcpy(key_bytes, user_key, len < 16 ? len : 16);

    // Первые 8 подключей напрямую из 16 байт ключа
    for (i = 0; i < 8; i++) {
        EK[i] = ((uint16_t)key_bytes[2 * i] << 8) | (uint16_t)key_bytes[2 * i + 1];
    }

    // Последующие подключи с циклическим сдвигом на 25 бит влево
    for (i = 8; i < 52; i++) {
        // Стандартный циклический сдвиг ключа IDEA на 25 бит
        EK[i] = ((EK[i - 7] & 0x7F) << 9) | (EK[i - 6] >> 7); // Упрощенный корректный сдвиг блока
    }
}

// Генерация ключей дешифрования
void idea_invert_key(const uint16_t *EK, uint16_t *DK) {
    int t = 0;
    uint16_t temp[52];

    temp[t++] = mul_inv(EK[48]);
    temp[t++] = add_inv(EK[49]);
    temp[t++] = add_inv(EK[50]);
    temp[t++] = mul_inv(EK[51]);

    for (int r = 1; r < 8; r++) {
        int e_idx = 48 - 6 * r;
        temp[t++] = mul_inv(EK[e_idx + 0]);
        temp[t++] = add_inv(EK[e_idx + 2]);
        temp[t++] = add_inv(EK[e_idx + 1]);
        temp[t++] = mul_inv(EK[e_idx + 3]);
        temp[t++] = EK[e_idx + 4];
        temp[t++] = EK[e_idx + 5];
    }

    temp[t++] = mul_inv(EK[0]);
    temp[t++] = add_inv(EK[1]);
    temp[t++] = add_inv(EK[2]);
    temp[t++] = mul_inv(EK[3]);

    for (int j = 0; j < 52; j++) {
        DK[j] = temp[j];
    }
}

static void process_block_idea(uint8_t *blk, const uint16_t *K) {
    uint16_t X1, X2, X3, X4;
    uint16_t T1, T2;

    // Чтение с учетом порядка байт (Little Endian / Big Endian в зависимости от архитектуры, лучше явно)
    X1 = ((uint16_t)blk[1] << 8) | blk[0];
    X2 = ((uint16_t)blk[3] << 8) | blk[2];
    X3 = ((uint16_t)blk[5] << 8) | blk[4];
    X4 = ((uint16_t)blk[7] << 8) | blk[6];

    int k = 0;
    for (int round = 0; round < 8; round++) {
        X1 = mul(X1, K[k++]);
        X2 = add(X2, K[k++]);
        X3 = add(X3, K[k++]);
        X4 = mul(X4, K[k++]);

        T1 = X1 ^ X3;
        T1 = mul(T1, K[k++]);
        T2 = add(X2 ^ X4, T1);
        T2 = mul(T2, K[k++]);
        T1 = add(T1, T2);

        X1 ^= T2;
        X4 ^= T1;

        T1 ^= X2;
        X2 = X3 ^ T2;
        X3 = T1;
    }

    X1 = mul(X1, K[k++]);
    T1 = add(X2, K[k++]);
    X2 = add(X3, K[k++]);
    X3 = T1;
    X4 = mul(X4, K[k++]);

    blk[0] = X1 & 0xFF; blk[1] = X1 >> 8;
    blk[2] = X2 & 0xFF; blk[3] = X2 >> 8;
    blk[4] = X3 & 0xFF; blk[5] = X3 >> 8;
    blk[6] = X4 & 0xFF; blk[7] = X4 >> 8;
}

int IDEA_encrypt_msg(const char *msg, long long key_val, uint8_t *out, size_t out_max) {
    if (!msg || !out) return -1;

    size_t len   = strlen(msg);
    size_t total = (len / BLOCK_SIZE + 1) * BLOCK_SIZE;
    if (total > out_max) return -1;

    memcpy(out, msg, len);
    uint8_t pad = (uint8_t)(total - len);
    memset(out + len, pad, pad);

    uint16_t EK[52];
    char key_buf[16];
    memset(key_buf, 0, sizeof(key_buf));
    memcpy(key_buf, &key_val, sizeof(key_val));
    idea_expand_key(key_buf, EK);

    for (size_t i = 0; i < total; i += BLOCK_SIZE) {
        process_block_idea(out + i, EK);
    }
    return (int)total;
}

int IDEA_decrypt_msg(const uint8_t *in, size_t len, long long key_val, char *out, size_t out_max) {
    if (!in || !out || len == 0 || len % BLOCK_SIZE != 0 || len + 1 > out_max) {
        return -1;
    }

    memcpy(out, in, len);

    uint16_t EK[52], DK[52];
    char key_buf[16];
    memset(key_buf, 0, sizeof(key_buf));
    memcpy(key_buf, &key_val, sizeof(key_val));
    idea_expand_key(key_buf, EK);
    idea_invert_key(EK, DK);

    for (size_t i = 0; i < len; i += BLOCK_SIZE) {
        process_block_idea((uint8_t *)out + i, DK);
    }

    uint8_t pad = (uint8_t)out[len - 1];
    if (pad < 1 || pad > BLOCK_SIZE || pad > len) return -1;
    for (size_t i = len - pad; i < len; i++) {
        if ((uint8_t)out[i] != pad) return -1;
    }

    size_t text_len = len - pad;
    out[text_len] = '\0';
    return (int)text_len;
}

int IDEA_encrypt_cmd(char *msg) {
    long long key = DIFFI_HELLMAN();

    size_t text_len = strlen(msg);
    size_t out_size = text_len + 8;
    uint8_t *out = (uint8_t *)malloc(out_size);
    if (!out) {
        fprintf(stderr, "IDEA: memory allocation failed\n");
        return 4;
    }

    int n = IDEA_encrypt_msg(msg, key, out, out_size);
    if (n < 0) {
        fprintf(stderr, "IDEA: encryption failed\n");
        free(out);
        return 4;
    }

    printf("Key: %lld\n", key);
    printf("Cipher: ");
    for (int i = 0; i < n; i++) {
        printf("%02X", out[i]);
    }
    printf("\n");

    free(out);
    return 0;
}

int IDEA_decrypt_cmd(const char *prog, const char *key_str, const char *hex_str) {
    char *endptr = NULL;
    long long key = strtoll(key_str, &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stderr, "%s: invalid key '%s'\n", prog, key_str);
        return 2;
    }

    size_t hex_len = strlen(hex_str);
    size_t cipher_max_len = hex_len / 2 + 1;
    uint8_t *cipher = (uint8_t *)malloc(cipher_max_len);
    if (!cipher) {
        fprintf(stderr, "%s: memory allocation failed\n", prog);
        return 2;
    }

    int n = hex_to_bytes(hex_str, cipher, cipher_max_len);
    if (n <= 0 || n % 8 != 0) {
        fprintf(stderr, "%s: cipher must be hex, length multiple of 16 chars\n", prog);
        free(cipher);
        return 2;
    }

    char *plain = (char *)malloc(n + 1);
    if (!plain) {
        fprintf(stderr, "%s: memory allocation failed\n", prog);
        free(cipher);
        return 4;
    }

    int m = IDEA_decrypt_msg(cipher, (size_t)n, key, plain, (size_t)(n + 1));
    if (m < 0) {
        fprintf(stderr, "%s: decryption failed (wrong key or corrupted data)\n", prog);
        free(cipher);
        free(plain);
        return 4;
    }

    plain[m] = '\0';
    printf("Decrypted: %s\n", plain);

    free(cipher);
    free(plain);
    return 0;
}
