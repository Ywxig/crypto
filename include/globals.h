#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef __cplusplus

extern "C" {
#endif

// Общий глобальный массив
extern char LETTERS[];

// функция для вывода с цыктом из c_halp.c
extern char* cout();

// Прототипы функций из CSR.c
void encrypt_caesar_shift(char *msg);
void decrypt_caesar_shift(char *msg);

// Прототипы функций из CSK.c
void encrypt_caesar_keyword(char *msg);
void decrypt_caesar_keyword(char *msg);

// Прототипы функций из VGN.c
void VGN_encript(char *msg);
void VGN_decript(char *msg);

// Прототипы функций из DFH.c
void DIFFI_HELLMAN();


#ifdef __cplusplus
}
#endif

#endif
