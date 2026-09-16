#ifndef GLOBALS_H
#define GLOBALS_H

// Общий глобальный массив
extern char LETTERS[];

// Прототипы функций из CSR.c
void encrypt_caesar_shift(char *msg);
void decrypt_caesar_shift(char *msg);

// Прототипы функций из CSK.c
void encrypt_caesar_keyword(char *msg);
void decrypt_caesar_keyword(char *msg);

// Прототипы функций из VGN.c
void VGN_encript(char *msg);
void VGN_decript(char *msg);

#endif
