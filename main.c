#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char LETTERS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
#define ALPHABET_SIZE 26

// Генерация подстановочного алфавита на основе ключевого слова (Caesar Keyword)
void CSK_keygen(const char *user_key, char *out_alphabet) {
    int used[26] = {0};
    int idx = 0;

    // Добавляем уникальные буквы из ключа
    for (int i = 0; user_key[i] != '\0'; i++) {
        char c = toupper((unsigned char)user_key[i]);
        if (c >= 'A' && c <= 'Z') {
            int pos = c - 'A';
            if (!used[pos]) {
                used[pos] = 1;
                out_alphabet[idx++] = c;
            }
        }
    }

    // Дописываем остальные буквы алфавита
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (!used[i]) {
            out_alphabet[idx++] = LETTERS[i];
        }
    }
    out_alphabet[ALPHABET_SIZE] = '\0';
}

void CSK_ENCRYPT(char *msg) {
    char user_key[100];
    char custom_alphabet[27];

    printf("key: ");
    if (scanf("%99s", user_key) != 1) return;

    CSK_keygen(user_key, custom_alphabet);

    size_t len = strlen(msg);
    char *enc = malloc(len + 1);
    if (!enc) return;

    for (size_t i = 0; i < len; i++) {
        char c = msg[i];
        if (isupper((unsigned char)c)) {
            enc[i] = custom_alphabet[c - 'A'];
        } else if (islower((unsigned char)c)) {
            enc[i] = tolower(custom_alphabet[toupper((unsigned char)c) - 'A']);
        } else {
            enc[i] = c;
        }
    }
    enc[len] = '\0';

    printf("%s\n", enc);
    free(enc);
}

void CSK_DECRYPT(char *msg) {
    char user_key[100];
    char custom_alphabet[27];

    printf("key: ");
    if (scanf("%99s", user_key) != 1) return;

    CSK_keygen(user_key, custom_alphabet);

    size_t len = strlen(msg);
    char *dec = malloc(len + 1);
    if (!dec) return;

    for (size_t i = 0; i < len; i++) {
        char c = msg[i];
        char upper_c = toupper((unsigned char)c);
        if (isalpha((unsigned char)c)) {
            for (int j = 0; j < ALPHABET_SIZE; j++) {
                if (custom_alphabet[j] == upper_c) {
                    char orig = LETTERS[j];
                    dec[i] = islower((unsigned char)c) ? tolower(orig) : orig;
                    break;
                }
            }
        } else {
            dec[i] = c;
        }
    }
    dec[len] = '\0';

    printf("%s\n", dec);
    free(dec);
}

void CSR_ENCRYPT(char *msg) {
    int key;
    printf("key: ");
    if (scanf("%d", &key) != 1) return;

    key = (key % ALPHABET_SIZE + ALPHABET_SIZE) % ALPHABET_SIZE;

    size_t len = strlen(msg);
    char *enc = malloc(len + 1);
    if (!enc) return;

    for (size_t i = 0; i < len; i++) {
        char c = msg[i];
        if (isupper((unsigned char)c)) {
            enc[i] = LETTERS[(c - 'A' + key) % ALPHABET_SIZE];
        } else if (islower((unsigned char)c)) {
            enc[i] = tolower(LETTERS[(toupper((unsigned char)c) - 'A' + key) % ALPHABET_SIZE]);
        } else {
            enc[i] = c;
        }
    }
    enc[len] = '\0';

    printf("%s\n", enc);
    free(enc);
}

void CSR_DECRYPT(char *msg) {
    int key;
    printf("key: ");
    if (scanf("%d", &key) != 1) return;

    key = (key % ALPHABET_SIZE + ALPHABET_SIZE) % ALPHABET_SIZE;
    int shift = (ALPHABET_SIZE - key) % ALPHABET_SIZE;

    size_t len = strlen(msg);
    char *dec = malloc(len + 1);
    if (!dec) return;

    for (size_t i = 0; i < len; i++) {
        char c = msg[i];
        if (isupper((unsigned char)c)) {
            dec[i] = LETTERS[(c - 'A' + shift) % ALPHABET_SIZE];
        } else if (islower((unsigned char)c)) {
            dec[i] = tolower(LETTERS[(toupper((unsigned char)c) - 'A' + shift) % ALPHABET_SIZE]);
        } else {
            dec[i] = c;
        }
    }
    dec[len] = '\0';

    printf("%s\n", dec);
    free(dec);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <option> <message>\n", argv[0]);
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -Ecsr  Encrypt message (Caesar cipher)\n");
        fprintf(stderr, "  -Dcsr  Decrypt message (Caesar cipher)\n");
        fprintf(stderr, "  -Ecsk  Encrypt message (Caesar Keyword cipher)\n");
        fprintf(stderr, "  -Dcsk  Decrypt message (Caesar Keyword cipher)\n");
        exit(1);
    }

    if (argc < 3) {
        fprintf(stderr, "%s: missing message argument\n", argv[0]);
        exit(2);
    }

    char *opt = argv[1];
    char *msg = argv[2];

    if (strcmp(opt, "-Ecsr") == 0) {
        CSR_ENCRYPT(msg);
    } else if (strcmp(opt, "-Dcsr") == 0) {
        CSR_DECRYPT(msg);
    } else if (strcmp(opt, "-Ecsk") == 0) {
        CSK_ENCRYPT(msg);
    } else if (strcmp(opt, "-Dcsk") == 0) {
        CSK_DECRYPT(msg);
    } else {
        fprintf(stderr, "%s: unknown option '%s'\n", argv[0], opt);
        exit(3);
    }

    return 0;
}