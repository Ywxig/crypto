#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/globals.h"

#define IDEA_BLOCK 8

/* Тестовый ключ IDEA из методички */
const uint8_t idea_default_key[16] = {
    0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04,
    0x00, 0x05, 0x00, 0x06, 0x00, 0x07, 0x00, 0x08
};

/*
 * IDEA (International Data Encryption Algorithm).
 * Реализация по псевдокоду из методички:
 *   - 8 раундов;
 *   - операции XOR, сложение по mod 2^16 и умножение по mod 2^16+1;
 *   - 52 подключа (6 на раунд + 4 на выходное преобразование).
 */

/* Умножение в группе Z(65537). 0x0000 означает 2^16 = 65536. */
uint16_t mul65537(uint16_t a, uint16_t b) {
    uint32_t x = a;
    uint32_t y = b;
    uint64_t p;

    if (x == 0) x = 65536;
    if (y == 0) y = 65536;

    p = ((uint64_t)x * (uint64_t)y) % 65537;
    if (p == 65536) p = 0;

    return (uint16_t)p;
}

/* Обратный элемент по умножению mod 65537 (расширенный Евклид). */
uint16_t mulinv(uint16_t a) {
    int64_t t = 0;
    int64_t newt = 1;
    int64_t r = 65537;
    int64_t newr = a;
    int64_t q, tmp;

    if (a == 0) return 0;

    while (newr != 0) {
        q = r / newr;

        tmp = t - q * newt;
        t = newt;
        newt = tmp;

        tmp = r - q * newr;
        r = newr;
        newr = tmp;
    }

    if (t < 0) t += 65537;
    if (t == 65536) return 0;

    return (uint16_t)t;
}

/* Обратный элемент по сложению mod 2^16. */
uint16_t addinv(uint16_t a) {
    return (uint16_t)((0 - (uint32_t)a) & 0xFFFF);
}

/* Циклический сдвиг 128-битного ключа (8 слов) влево на 25 бит. */
void keyrot25(uint16_t w[8]) {
    uint16_t t[8];

    for (int j = 0; j < 8; j++) {
        t[j] = (uint16_t)((w[(j + 1) % 8] << 9) | (w[(j + 2) % 8] >> 7));
    }

    memcpy(w, t, sizeof(t));
}

/* Чтение/запись 16-битных слов в big-endian. */
void load16be(const uint8_t *src, uint16_t *dst, int count) {
    for (int i = 0; i < count; i++) {
        dst[i] = (uint16_t)(((uint16_t)src[2 * i] << 8) | src[2 * i + 1]);
    }
}

void store16be(uint8_t *dst, const uint16_t *src, int count) {
    for (int i = 0; i < count; i++) {
        dst[2 * i] = (uint8_t)(src[i] >> 8);
        dst[2 * i + 1] = (uint8_t)(src[i] & 0xFF);
    }
}

/* Расширение 128-битного ключа до 52 подключей. */
void IDEA_keyexp(const uint8_t key[16], uint16_t Z[52]) {
    uint16_t w[8];

    load16be(key, w, 8);

    for (int i = 0; i < 52; i++) {
        if (i > 0 && (i % 8) == 0) {
            keyrot25(w);
        }
        Z[i] = w[i % 8];
    }
}

/* Подключи расшифрования: мультипликативные и аддитивные обратные. */
void IDEA_invkeys(const uint16_t EK[52], uint16_t DK[52]) {
    DK[0] = mulinv(EK[48]);
    DK[1] = addinv(EK[49]);
    DK[2] = addinv(EK[50]);
    DK[3] = mulinv(EK[51]);

    DK[4] = EK[46];
    DK[5] = EK[47];

    for (int r = 2; r <= 8; r++) {
        int d = 6 * (r - 1);
        int s = 6 * (9 - r);

        DK[d + 0] = mulinv(EK[s + 0]);
        DK[d + 1] = addinv(EK[s + 2]);
        DK[d + 2] = addinv(EK[s + 1]);
        DK[d + 3] = mulinv(EK[s + 3]);
        DK[d + 4] = EK[s - 2];
        DK[d + 5] = EK[s - 1];
    }

    DK[48] = mulinv(EK[0]);
    DK[49] = addinv(EK[1]);
    DK[50] = addinv(EK[2]);
    DK[51] = mulinv(EK[3]);
}

static uint16_t add16(uint16_t a, uint16_t b) {
    return (uint16_t)((a + b) & 0xFFFF);
}

/* Шифрование одного 64-битного блока. */
void IDEA_encrypt(const uint16_t Z[52], uint8_t data[8]) {
    uint16_t x[4];
    uint16_t y1, y2, y3, y4;
    uint16_t t0, t1, t2, t3, t4, t5;
    uint16_t c[4];
    int j = 0;

    load16be(data, x, 4);

    for (int round = 0; round < 8; round++) {
        y1 = mul65537(x[0], Z[j++]);
        y2 = add16(x[1], Z[j++]);
        y3 = add16(x[2], Z[j++]);
        y4 = mul65537(x[3], Z[j++]);

        t0 = y1 ^ y3;
        t1 = y2 ^ y4;
        t2 = mul65537(t0, Z[j++]);
        t3 = add16(t1, t2);
        t4 = mul65537(t3, Z[j++]);
        t5 = add16(t2, t4);

        x[0] = y1 ^ t4;
        x[1] = y3 ^ t4;
        x[2] = y2 ^ t5;
        x[3] = y4 ^ t5;
    }

    c[0] = mul65537(x[0], Z[48]);
    c[1] = add16(x[2], Z[49]);
    c[2] = add16(x[1], Z[50]);
    c[3] = mul65537(x[3], Z[51]);

    store16be(data, c, 4);
}

/* Расшифрование — та же сеть, но с подключями DK. */
void IDEA_decrypt(const uint16_t DK[52], uint8_t data[8]) {
    IDEA_encrypt(DK, data);
}

/* Шифрование строки в режиме ECB с PKCS#7. */
int idea_encrypt_msg(const uint8_t key[16], const char *msg,
                     uint8_t *out, size_t out_max) {
    size_t len;
    size_t total;
    uint16_t Z[52];

    if (!key || !msg || !out) return -1;

    len = strlen(msg);
    total = (len / IDEA_BLOCK + 1) * IDEA_BLOCK;
    if (total > out_max) return -1;

    memcpy(out, msg, len);
    if (pkcs7_pad(out, len, IDEA_BLOCK, out_max) == 0) return -1;

    IDEA_keyexp(key, Z);

    for (size_t i = 0; i < total; i += IDEA_BLOCK) {
        IDEA_encrypt(Z, out + i);
    }

    return (int)total;
}

/* Расшифрование строки и снятие PKCS#7. */
int idea_decrypt_msg(const uint8_t key[16], const uint8_t *in, size_t len,
                     char *out, size_t out_max) {
    uint16_t EK[52];
    uint16_t DK[52];
    int text_len;

    if (!key || !in || !out) return -1;
    if (len == 0 || (len % IDEA_BLOCK) != 0) return -1;
    if (len + 1 > out_max) return -1;

    memcpy(out, in, len);

    IDEA_keyexp(key, EK);
    IDEA_invkeys(EK, DK);

    for (size_t i = 0; i < len; i += IDEA_BLOCK) {
        IDEA_decrypt(DK, (uint8_t *)out + i);
    }

    text_len = pkcs7_unpad((uint8_t *)out, len, IDEA_BLOCK);
    if (text_len < 0) return -1;

    out[text_len] = '\0';
    return text_len;
}

/* Режим гаммирования CTR: гамма = IDEA(счётчик). */
void idea_ctr_crypt(const uint8_t key[16], const uint8_t iv[8],
                    const uint8_t *in, uint8_t *out, size_t len) {
    uint16_t Z[52];
    uint8_t counter[8];
    uint8_t stream[8];

    IDEA_keyexp(key, Z);
    memcpy(counter, iv, 8);

    for (size_t off = 0; off < len; off += IDEA_BLOCK) {
        size_t n = len - off;

        if (n > IDEA_BLOCK) n = IDEA_BLOCK;

        memcpy(stream, counter, 8);
        IDEA_encrypt(Z, stream);

        for (size_t i = 0; i < n; i++) {
            out[off + i] = (uint8_t)(in[off + i] ^ stream[i]);
        }

        /* инкремент 64-битного счётчика (big-endian) */
        for (int i = 7; i >= 0; i--) {
            if (++counter[i] != 0) break;
        }
    }
}

/* ------------------------------------------------------------------------- */
/* Команды для main.c                                                        */
/* ------------------------------------------------------------------------- */

int IDEA_encrypt_cmd(const char *msg, const uint8_t key[16]) {
    size_t len = strlen(msg);
    size_t out_max = len + IDEA_BLOCK;
    uint8_t *out = (uint8_t *)malloc(out_max);
    int n;

    if (!out) {
        fprintf(stderr, "IDEA: memory allocation failed\n");
        return 4;
    }

    n = idea_encrypt_msg(key, msg, out, out_max);
    if (n < 0) {
        fprintf(stderr, "IDEA: encryption failed\n");
        free(out);
        return 4;
    }

    printf("key: ");
    print_hex_lower(key, 16);
    printf("\nenc: ");
    print_hex(out, (size_t)n);
    printf("\n");

    free(out);
    return 0;
}

int IDEA_decrypt_cmd(const char *prog, const uint8_t key[16], const char *hex) {
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
    if (n <= 0 || (n % IDEA_BLOCK) != 0) {
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

    m = idea_decrypt_msg(key, cipher, (size_t)n, plain, (size_t)n + 1);
    if (m < 0) {
        fprintf(stderr, "%s: decryption failed (wrong key or corrupted data)\n", prog);
        free(cipher);
        free(plain);
        return 4;
    }

    printf("key: ");
    print_hex_lower(key, 16);
    printf("\ndec: %s\n", plain);

    free(cipher);
    free(plain);
    return 0;
}

int IDEA_ctr_encrypt_cmd(const char *msg, const uint8_t key[16]) {
    uint8_t iv[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    size_t len = strlen(msg);
    uint8_t *out = (uint8_t *)malloc(len ? len : 1);

    if (!out) {
        fprintf(stderr, "IDEA-CTR: memory allocation failed\n");
        return 4;
    }

    idea_ctr_crypt(key, iv, (const uint8_t *)msg, out, len);

    printf("key: ");
    print_hex_lower(key, 16);
    printf("\niv: ");
    print_hex(iv, 8);
    printf("\nenc: ");
    print_hex(out, len);
    printf("\n");

    free(out);
    return 0;
}

int IDEA_ctr_decrypt_cmd(const char *prog, const uint8_t key[16],
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

    idea_ctr_crypt(key, iv, cipher, (uint8_t *)plain, (size_t)n);
    plain[n] = '\0';

    printf("key: ");
    print_hex_lower(key, 16);
    printf("\niv: ");
    print_hex(iv, 8);
    printf("\ndec: %s\n", plain);

    free(cipher);
    free(plain);
    return 0;
}
