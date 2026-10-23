#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef __cplusplus

extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

// Общий глобальный массив
extern const char LETTERS[];
extern const char LETTERS2[];
extern const char LETTERS3[];


// функция для вывода с цыктом из c_halp.c
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

// IDEA
void idea_expand_key(const char* user_key, uint16_t* EK);

void IDEA_encrypt(char* msg, char* key);

#ifdef __cplusplus
}
#endif

#endif
