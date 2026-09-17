#include <stdio.h>
#include <string.h>
#include "../include/globals.h"
#include "../include/colorise.h"

// Генерация алфавита по ключевому слову
char *caesar_with_key() {
    static char key[100];
    c_print_format("<key>");
    if (scanf("%99s", key) != 1) return NULL;

    strcat(key, LETTERS);
    int n = strlen(key);

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (key[i] == key[j]) {
                for (int k = j; k < n - 1; k++) {
                    key[k] = key[k + 1];
                }
                n--;
                j--;
            }
        }
    }

    key[n] = '\0';
    return key;
}

// Шифрование Цезаря с ключевым словом
void encrypt_caesar_keyword(char *msg) {
    char *alphabet = caesar_with_key();
    if (!alphabet) return;

    int n = strlen(LETTERS);
    size_t len = strlen(msg);
    for (size_t i = 0; i < len; i++) {
        for (int j = 0; j < n; j++) {
            if (msg[i] == LETTERS[j]) {
                msg[i] = alphabet[j];
                break;
            }
        }
    }
    c_print_format("<enc> %s", msg);
}

// Расшифровка Цезаря с ключевым словом
void decrypt_caesar_keyword(char *msg) {
    char *alphabet = caesar_with_key();
    if (!alphabet) return;

    size_t len = strlen(msg);
    size_t alphabet_len = strlen(alphabet);
    for (size_t i = 0; i < len; i++) {
        for (size_t j = 0; j < alphabet_len; j++) {
            if (msg[i] == alphabet[j]) {
                msg[i] = LETTERS[j];
                break;
            }
        }
    }
    c_print_format("<dec> %s", msg);
}
