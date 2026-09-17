#include <stdio.h>
#include <string.h>
#include "../include/globals.h"
#include "../include/colorise.h"

// Обычная расшифровка Цезаря (числовой сдвиг)
void decrypt_caesar_shift(char *msg) {
    int key;
    c_print_format("<key>");
    if (scanf("%d", &key) != 1) return;

    int alphabet_size = strlen(LETTERS);
    key = (key % alphabet_size + alphabet_size) % alphabet_size;

    size_t len = strlen(msg);
    for (size_t i = 0; i < len; i++) {
        for (int j = 0; j < alphabet_size; j++) {
            if (msg[i] == LETTERS[j]) {
                int new_pos = (j - key + alphabet_size) % alphabet_size;
                msg[i] = LETTERS[new_pos];
                break;
            }
        }
    }
    c_print_format("<dec> %s", msg);
}

// Обычное шифрование Цезаря (числовой сдвиг)
void encrypt_caesar_shift(char *msg) {
    int key;
    c_print_format("<key>");
    if (scanf("%d", &key) != 1) return;

    int alphabet_size = strlen(LETTERS);
    key = (key % alphabet_size + alphabet_size) % alphabet_size;

    size_t len = strlen(msg);
    for (size_t i = 0; i < len; i++) {
        for (int j = 0; j < alphabet_size; j++) {
            if (msg[i] == LETTERS[j]) {
                int new_pos = (j + key) % alphabet_size;
                msg[i] = LETTERS[new_pos];
                break;
            }
        }
    }
    c_print_format("<enc> %s", msg);
}
