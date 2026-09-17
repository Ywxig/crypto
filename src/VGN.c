#include <stdio.h>
#include <string.h>
#include "../include/globals.h"
#include "../include/colorise.h"

void VGN_encript(char *msg) {
    int k, imsg = 0, ikey = 0, tmp;
    char key[26], enc[26];
    c_print_format("<key>");
    if (scanf("%25s", key) != 1) return;

    k = 0;
    size_t msg_len = strlen(msg);
    size_t letters_len = strlen(LETTERS);
    size_t key_len = strlen(key);

    for (size_t i = 0; i < msg_len; i++) {
        for (size_t j = 0; j < letters_len; j++) {
            if (msg[i] == LETTERS[j]) {
                imsg = j;
            }
            if (key[k] == LETTERS[j]) {
                ikey = j;
            }
        }
        if ((size_t)(imsg + ikey) < letters_len) {
            tmp = imsg + ikey;
            enc[i] = LETTERS[tmp];
        } else {
            tmp = imsg + ikey - letters_len;
            enc[i] = LETTERS[tmp];
        }
        k = (k == (int)key_len - 1) ? 0 : k + 1;
    }
    enc[msg_len] = '\0';
    c_print_format("<enc> %s", enc);
}

void VGN_decript(char *msg) {
    int k, imsg = 0, ikey = 0, tmp;
    char key[26], enc[26];
    c_print_format("<key>");
    if (scanf("%25s", key) != 1) return;

    k = 0;
    size_t msg_len = strlen(msg);
    size_t letters_len = strlen(LETTERS);
    size_t key_len = strlen(key);

    for (size_t i = 0; i < msg_len; i++) {
        for (size_t j = 0; j < letters_len; j++) {
            if (msg[i] == LETTERS[j]) {
                imsg = j;
            }
            if (key[k] == LETTERS[j]) {
                ikey = j;
            }
        }

        if (imsg >= ikey) {
            tmp = imsg - ikey;
            enc[i] = LETTERS[tmp];
        } else {
            tmp = imsg - ikey + letters_len;
            enc[i] = LETTERS[tmp];
        }
        k = (k == (int)key_len - 1) ? 0 : k + 1;
    }
    enc[msg_len] = '\0';
    c_print_format("<dec> %s", enc);
}
