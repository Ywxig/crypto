#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char letters[] = "abcdefghijklmnopqrstuvwxyz";

// Генерация алфавита по ключевому слову
char *caesar_with_key() {
    static char key[100];
    printf("key: ");
    if (scanf("%99s", key) != 1) return NULL;

    strcat(key, letters);
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

    int n = strlen(letters);
    for (int i = 0; i < strlen(msg); i++) {
        for (int j = 0; j < n; j++) {
            if (msg[i] == letters[j]) {
                msg[i] = alphabet[j];
                break;
            }
        }
    }
    printf("%s\n", msg);
}

// Расшифровка Цезаря с ключевым словом
void decrypt_caesar_keyword(char *msg) {
    char *alphabet = caesar_with_key();
    if (!alphabet) return;

    int n = strlen(letters);
    for (int i = 0; i < strlen(msg); i++) {
        for (int j = 0; j < n; j++) {
            if (msg[i] == alphabet[j]) {
                msg[i] = letters[j];
                break;
            }
        }
    }
    printf("%s\n", msg);
}

// Обычное шифрование Цезаря (числовой сдвиг)
void encrypt_caesar_shift(char *msg) {
    int key;
    printf("key: ");
    if (scanf("%d", &key) != 1) return;

    key = (key % 26 + 26) % 26;

    for (int i = 0; i < strlen(msg); i++) {
        for (int j = 0; j < 26; j++) {
            if (msg[i] == letters[j]) {
                int new_pos = (j + key) % 26;
                msg[i] = letters[new_pos];
                break;
            }
        }
    }
    printf("%s\n", msg);
}

// Обычная расшифровка Цезаря (числовой сдвиг)
void decrypt_caesar_shift(char *msg) {
    int key;
    printf("key: ");
    if (scanf("%d", &key) != 1) return;

    key = (key % 26 + 26) % 26;

    for (int i = 0; i < strlen(msg); i++) {
        for (int j = 0; j < 26; j++) {
            if (msg[i] == letters[j]) {
                int new_pos = (j - key + 26) % 26;
                msg[i] = letters[new_pos];
                break;
            }
        }
    }
    printf("%s\n", msg);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <option> <message>\n", argv[0]);
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -Ecsr  Encrypt (Caesar Shift)\n");
        fprintf(stderr, "  -Dcsr  Decrypt (Caesar Shift)\n");
        fprintf(stderr, "  -Ecsk  Encrypt (Caesar Keyword)\n");
        fprintf(stderr, "  -Dcsk  Decrypt (Caesar Keyword)\n");
        exit(1);
    }

    if (argc < 3) {
        fprintf(stderr, "%s: missing message argument\n", argv[0]);
        exit(2);
    }

    char *opt = argv[1];
    char *msg = argv[2];

    if (strcmp(opt, "-Ecsr") == 0) {
        encrypt_caesar_shift(msg);
    } else if (strcmp(opt, "-Dcsr") == 0) {
        decrypt_caesar_shift(msg);
    } else if (strcmp(opt, "-Ecsk") == 0) {
        encrypt_caesar_keyword(msg);
    } else if (strcmp(opt, "-Dcsk") == 0) {
        decrypt_caesar_keyword(msg);
    } else {
        fprintf(stderr, "%s: unknown option '%s'\n", argv[0], opt);
        exit(3);
    }

    return 0;
}
