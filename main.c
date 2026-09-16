#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/globals.h"

char LETTERS[] = "abcdefghijklmnopqrstuvwxyz";

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <option> <message>\n", argv[0]);
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -Ecsr  Encrypt (Caesar Shift)\n");
        fprintf(stderr, "  -Dcsr  Decrypt (Caesar Shift)\n");
        fprintf(stderr, "  -Ecsk  Encrypt (Caesar Keyword)\n");
        fprintf(stderr, "  -Dcsk  Decrypt (Caesar Keyword)\n");
        fprintf(stderr, "  -Evge  Encrypt (Vigenere)\n");
        fprintf(stderr, "  -Dvge  Decrypt (Vigenere)\n");
        exit(1);
    }

    char *opt = argv[1];
    char *msg = argv[2];

    if (strcmp(opt, "-v") == 0) {
        printf("v1.0.0\n");
        exit(0);
    }

    if (argc < 3) {
        fprintf(stderr, "%s: missing message argument\n", argv[0]);
        exit(2);
    }

    if (strcmp(opt, "-Ecsr") == 0) {
        encrypt_caesar_shift(msg);
    } else if (strcmp(opt, "-Dcsr") == 0) {
        decrypt_caesar_shift(msg);
    } else if (strcmp(opt, "-Ecsk") == 0) {
        encrypt_caesar_keyword(msg);
    } else if (strcmp(opt, "-Dcsk") == 0) {
        decrypt_caesar_keyword(msg);
    } else if (strcmp(opt, "-Evgn") == 0) {
        VGN_encript(msg);
    } else if (strcmp(opt, "-Dvgn") == 0) {
        VGN_decript(msg);
    } else {
        fprintf(stderr, "%s: unknown option '%s'\n", argv[0], opt);
        exit(3);
    }

    return 0;
}
