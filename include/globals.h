#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

// Общий глобальный массив
extern const char LETTERS[];
extern const char LETTERS2[];
extern const char LETTERS3[];


// функция для вывода с цыклом из c_halp.c
extern char* cout();

// Прототипы функций из CSR.c
void CSR_encrypt(char *msg);
void CSR_decrypt(char *msg);
void CSR_dec_math(char *msg);
void CSR_enc_math(char *msg);

// Прототипы функций из CSK.c
void encrypt_caesar_keyword(char *msg);
void decrypt_caesar_keyword(char *msg);

// Прототипы функций из VGN.c
void VGN_encript(char *msg);
void VGN_decript(char *msg);
void VGN_enc_seed(char *msg);

// Прототипы функций из DFH.c
long long DIFFI_HELLMAN();
void DIFFI_HELLMAN_xor();

// Прототип функции из DFHC.c
long long DH_ELP_CURV();

// RC2 (RC2.c)
// Строка hex -> байты. Возвращает число байт или -1 при ошибке.
int hex_to_bytes(const char *hex_str, uint8_t *out_buf, size_t out_max_len);
// Шифрование строки (ECB, PKCS#7). Возвращает длину шифртекста (кратна 8) или -1.
int RC2_encrypt(const char *msg, long long key, uint8_t *out, size_t out_max);
// Расшифровка. Возвращает длину текста или -1 (неверный ключ / битые данные).
int RC2_decrypt(const uint8_t *in, size_t len, long long key, char *out, size_t out_max);

int rc2_encrypt_cmd(const char *text);
int rc2_decrypt_cmd(const char *prog, const char *key_str, const char *hex_str);

/* ------------------------------------------------------------------------- */
/* Общие таблицы и константы лабораторной работы 03                          */
/* ------------------------------------------------------------------------- */

// Таблица замен ГОСТ 28147-89 (id-tc26-gost-28147-param-Z), определяется в gost.c
extern const uint8_t gost_sbox[8][16];
extern const uint32_t GOST_C1;
extern const uint32_t GOST_C2;

// Ключи по умолчанию (тестовые векторы)
extern const uint8_t idea_default_key[16];
extern const uint8_t gost_default_key[32];
extern const uint8_t kuznyechik_default_key[32];

// S-блок «Кузнечика», определяется в kuznyechik.c
extern const uint8_t kuznyechik_sbox[256];

void print_hex(const uint8_t *buf, size_t len);
void print_hex_lower(const uint8_t *buf, size_t len);

/* ------------------------------------------------------------------------- */
/* Общие вспомогательные функции (gost.c)                                    */
/* ------------------------------------------------------------------------- */

// PKCS#7: дополнить буфер до размера, кратного block. Возвращает новую длину.
size_t pkcs7_pad(uint8_t *buf, size_t len, size_t block, size_t max_len);
// Имя из методички для функции дополнения блока.
size_t padding(uint8_t *buf, size_t len, size_t block, size_t max_len);
// Снять PKCS#7. Возвращает длину текста или -1 при ошибке.
int pkcs7_unpad(uint8_t *buf, size_t len, size_t block);

/* ------------------------------------------------------------------------- */
/* IDEA (idea.c)                                                             */
/* ------------------------------------------------------------------------- */

uint16_t mul65537(uint16_t a, uint16_t b);
uint16_t mulinv(uint16_t a);
uint16_t addinv(uint16_t a);
void keyrot25(uint16_t w[8]);
void load16be(const uint8_t *src, uint16_t *dst, int count);
void store16be(uint8_t *dst, const uint16_t *src, int count);

void IDEA_keyexp(const uint8_t key[16], uint16_t Z[52]);
void IDEA_invkeys(const uint16_t EK[52], uint16_t DK[52]);
void IDEA_encrypt(const uint16_t Z[52], uint8_t data[8]);
void IDEA_decrypt(const uint16_t DK[52], uint8_t data[8]);

int idea_encrypt_msg(const uint8_t key[16], const char *msg,
                     uint8_t *out, size_t out_max);
int idea_decrypt_msg(const uint8_t key[16], const uint8_t *in, size_t len,
                     char *out, size_t out_max);
void idea_ctr_crypt(const uint8_t key[16], const uint8_t iv[8],
                    const uint8_t *in, uint8_t *out, size_t len);

int IDEA_encrypt_cmd(const char *msg, const uint8_t key[16]);
int IDEA_decrypt_cmd(const char *prog, const uint8_t key[16], const char *hex);
int IDEA_ctr_encrypt_cmd(const char *msg, const uint8_t key[16]);
int IDEA_ctr_decrypt_cmd(const char *prog, const uint8_t key[16],
                         const char *iv_hex, const char *hex);

/* ------------------------------------------------------------------------- */
/* ГОСТ 28147-89 / «Магма» (gost.c)                                          */
/* ------------------------------------------------------------------------- */

uint32_t ROL32(uint32_t x, int n);
uint32_t T(uint32_t a);
void load32be(const uint8_t *src, uint32_t *dst, int count);
void store32be(uint8_t *dst, const uint32_t *src, int count);

void GOST_keyexp(const uint8_t key[32], uint32_t K[8]);
void GOST_encrypt(const uint32_t K[8], uint8_t data[8]);
void GOST_decrypt(const uint32_t K[8], uint8_t data[8]);

int gost_encrypt_msg(const uint8_t key[32], const char *msg,
                     uint8_t *out, size_t out_max);
int gost_decrypt_msg(const uint8_t key[32], const uint8_t *in, size_t len,
                     char *out, size_t out_max);
void gost_ctr_crypt(const uint8_t key[32], const uint8_t iv[8],
                    const uint8_t *in, uint8_t *out, size_t len);
uint32_t gost_mac(const uint8_t key[32], const uint8_t *data, size_t len);

int GOST_encrypt_cmd(const char *msg, const uint8_t key[32]);
int GOST_decrypt_cmd(const char *prog, const uint8_t key[32], const char *hex);
int GOST_ctr_encrypt_cmd(const char *msg, const uint8_t key[32]);
int GOST_ctr_decrypt_cmd(const char *prog, const uint8_t key[32],
                         const char *iv_hex, const char *hex);
int GOST_mac_cmd(const char *msg, const uint8_t key[32]);

/* ------------------------------------------------------------------------- */
/* «Кузнечик» ГОСТ Р 34.12-2015 (kuznyechik.c)                               */
/* ------------------------------------------------------------------------- */

void kuznyechik_keyexp(const uint8_t key[32], uint8_t rk[10][16]);
void kuznyechik_encrypt(const uint8_t rk[10][16], uint8_t block[16]);
void kuznyechik_decrypt(const uint8_t rk[10][16], uint8_t block[16]);

int kuznyechik_encrypt_msg(const uint8_t key[32], const char *msg,
                           uint8_t *out, size_t out_max);
int kuznyechik_decrypt_msg(const uint8_t key[32], const uint8_t *in, size_t len,
                           char *out, size_t out_max);
int kuznyechik_encrypt_cmd(const char *msg, const uint8_t key[32]);
int kuznyechik_decrypt_cmd(const char *prog, const uint8_t key[32],
                           const char *hex);

/* ------------------------------------------------------------------------- */
/* Лавинный эффект (avalanche.c)                                             */
/* ------------------------------------------------------------------------- */

void avalanche_test(void);

#ifdef __cplusplus
}
#endif

#endif
