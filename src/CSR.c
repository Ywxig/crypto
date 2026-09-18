#include <stdio.h>
#include <string.h>
#include "../include/globals.h"
#include "../include/colorise.h"

// Обычная расшифровка Цезаря (числовой сдвиг) без формул в математике
void CSR_decrypt(char *msg) {
    int key;
    c_print_format("<key>");
    if (scanf("%d", &key) != 1) return;

    int alphabet_size = strlen(LETTERS2);

    // Сдвиг "без математики" (через циклический инкремент/декремент по шагам)
    key = key % alphabet_size;
    if (key < 0) key += alphabet_size;

    size_t len = strlen(msg);
    for (size_t i = 0; i < len; i++) {
        for (int j = 0; j < alphabet_size; j++) {
            if (msg[i] == LETTERS2[j]) {
                int current_pos = j;
                // Идем назад по алфавиту на key шагов поштучно
                for (int k = 0; k < key; k++) {
                    current_pos--;
                    if (current_pos < 0) {
                        current_pos = alphabet_size - 1; // Возвращаемся в конец алфавита
                    }
                }
                msg[i] = LETTERS2[current_pos];
                break;
            }
        }
    }
    c_print_format("<dec> %s", msg);
}

// Обычное шифрование Цезаря (числовой сдвиг) без формул в математике
void CSR_encrypt(char *msg) {
    int key;
    c_print_format("<key>");
    if (scanf("%d", &key) != 1) return;

    int alphabet_size = strlen(LETTERS2);
    key = key % alphabet_size;
    if (key < 0) key += alphabet_size;

    size_t len = strlen(msg);
    for (size_t i = 0; i < len; i++) {
        for (int j = 0; j < alphabet_size; j++) {
            if (msg[i] == LETTERS2[j]) {
                int current_pos = j;
                // Идем вперед по алфавиту на key шагов поштучно
                for (int k = 0; k < key; k++) {
                    current_pos++;
                    if (current_pos >= alphabet_size) {
                        current_pos = 0; // Возвращаемся в начало алфавита
                    }
                }
                msg[i] = LETTERS2[current_pos];
                break;
            }
        }
    }
    c_print_format("<enc> %s", msg);
}

// Обычная расшифровка Цезаря (числовой сдвиг) тоже самое но через формулы в математике
// encrypt (mi)=(mi +k )mod 26
void CSR_dec_math(char *msg) {
    int key;
    c_print_format("<key>");
    if (scanf("%d", &key) != 1) return;

    int alphabet_size = strlen(LETTERS2);
    key = (key % alphabet_size + alphabet_size) % alphabet_size;

    size_t len = strlen(msg);
    for (size_t i = 0; i < len; i++) {
        for (int j = 0; j < alphabet_size; j++) {
            if (msg[i] == LETTERS2[j]) {
                int new_pos = (j - key + alphabet_size) % alphabet_size;
                msg[i] = LETTERS2[new_pos];
                break;
            }
        }
    }

    c_print_format("<dec> %s", msg);
}

// Обычное шифрование Цезаря (числовой сдвиг)
void CSR_enc_math(char *msg) {
    int key;
    c_print_format("<key>");
    if (scanf("%d", &key) != 1) return;

    int alphabet_size = strlen(LETTERS2);
    key = (key % alphabet_size + alphabet_size) % alphabet_size;

    size_t len = strlen(msg);
    for (size_t i = 0; i < len; i++) {
        for (int j = 0; j < alphabet_size; j++) {
            if (msg[i] == LETTERS2[j]) {
                int new_pos = (j + key) % alphabet_size;
                msg[i] = LETTERS2[new_pos];
                break;
            }
        }
    }
    c_print_format("<enc> %s", msg);
}
