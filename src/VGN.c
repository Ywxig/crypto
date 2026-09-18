#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "../include/globals.h"
#include "../include/colorise.h"

void VGN_encript(char *msg) {
    int k, imsg = 0, ikey = 0, tmp;
    char key[26], enc[26];
    c_print_format("<key>");
    if (scanf("%25s", key) != 1) return;

    k = 0;
    size_t msg_len = strlen(msg);
    size_t LETTERS3_len = strlen(LETTERS3);
    size_t key_len = strlen(key);

    for (size_t i = 0; i < msg_len; i++) {
        for (size_t j = 0; j < LETTERS3_len; j++) {
            if (msg[i] == LETTERS3[j]) {
                imsg = j;
            }
            if (key[k] == LETTERS3[j]) {
                ikey = j;
            }
        }
        if ((size_t)(imsg + ikey) < LETTERS3_len) {
            tmp = imsg + ikey;
            enc[i] = LETTERS3[tmp];
        } else {
            tmp = imsg + ikey - LETTERS3_len;
            enc[i] = LETTERS3[tmp];
        }
        k = (k == (int)key_len - 1) ? 0 : k + 1;
    }
    enc[msg_len] = '\0';
    c_print_format("<enc> %s", enc);
}

void VGN_enc_seed(char *msg) {
    int k, imsg = 0, ikey = 0, tmp;
    char key[26], enc[26];
    c_print_format("<key>");
    if (scanf("%25s", key) != 1) return;

    // Устанавливаем seed времени шифрования (фиксированный или текущий)
    // Для проверки по заданию можно передать 1644485555 или сгенерировать через time(NULL)
    unsigned int seed = 1644485555;
    srand(seed);

    k = 0;
    size_t msg_len = strlen(msg);
    size_t LETTERS3_len = strlen(LETTERS3);
    size_t key_len = strlen(key);

    for (size_t i = 0; i < msg_len; i++) {
        for (size_t j = 0; j < LETTERS3_len; j++) {
            if (msg[i] == LETTERS3[j]) {
                imsg = j;
            }
            if (key[k] == LETTERS3[j]) {
                ikey = j;
            }
        }

        // Добавляем псевдослучайное смещение на основе seed (криптографическая имитация)
        int random_offset = rand() % 3;
        int combined_key = (ikey + random_offset) % LETTERS3_len;

        if ((size_t)(imsg + combined_key) < LETTERS3_len) {
            tmp = imsg + combined_key;
            enc[i] = LETTERS3[tmp];
        } else {
            tmp = imsg + combined_key - LETTERS3_len;
            enc[i] = LETTERS3[tmp];
        }
        k = (k == (int)key_len - 1) ? 0 : k + 1;
    }
    enc[msg_len] = '\0';
    c_print_format("<enc> seed: %u | result: %s", seed, enc);
}

void VGN_decript(char *msg) {
    int k, imsg = 0, ikey = 0, tmp;
    char key[26], enc[26];
    c_print_format("<key>");
    if (scanf("%25s", key) != 1) return;

    k = 0;
    size_t msg_len = strlen(msg);
    size_t LETTERS3_len = strlen(LETTERS3);
    size_t key_len = strlen(key);

    for (size_t i = 0; i < msg_len; i++) {
        for (size_t j = 0; j < LETTERS3_len; j++) {
            if (msg[i] == LETTERS3[j]) {
                imsg = j;
            }
            if (key[k] == LETTERS3[j]) {
                ikey = j;
            }
        }

        if (imsg >= ikey) {
            tmp = imsg - ikey;
            enc[i] = LETTERS3[tmp];
        } else {
            tmp = imsg - ikey + LETTERS3_len;
            enc[i] = LETTERS3[tmp];
        }
        k = (k == (int)key_len - 1) ? 0 : k + 1;
    }
    enc[msg_len] = '\0';
    c_print_format("<dec> %s", enc);
}
