#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef __cplusplus

extern "C" {
#endif

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

//RC2
void RC2_encript(char *msg, long long *key);

#ifdef __cplusplus
}
#endif

#endif
