#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char LETTERS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void CSR_ENCRIPT(char *msg) {
    int i, j, temp, key;
    printf("key: ");
    scanf("%i", &key);

    printf("%s\n", msg);

    char enc[26];

    for (i = 0; i < strlen(msg); i++)
        for (j = 0; j < strlen(LETTERS); j++)
            if (msg[i] == LETTERS[j]) {
                if (j + key<strlen(LETTERS)) {
                    temp = j + key;
                    enc[i] = LETTERS[temp];
                } else {
                    temp = j + key - strlen(LETTERS);
                    enc[i] = LETTERS[temp];
                }
            }
    printf("%s\n", enc);
}

void CSR_DECRYPT(char *msg) {
    int i, j, temp, key;
    printf("key: ");
    scanf("%d", &key);

    for (int i = 0; msg[i]; i++) {
        for (int j = 0; j < strlen(LETTERS); j++) {
            if (msg [i] == LETTERS[j]) {
                if (j - key >= 0) {
                    temp = j - key;
                    printf("%c", LETTERS[temp]);
                } else {
                    temp = j - key + strlen(LETTERS);
                    printf("%c", LETTERS[temp]);
                }
                break;
            }
        }
    }
}


int main(int argc, char *argv[]) {

    char *prg = argv[0];
    char *opt = argv[1];
    char *msg = argv[2];

    if (argc==1) {
        fprintf(stderr, "%s: no option and message", prg);
        exit(1);
    }
    if (argc==2) {
        if (opt[0]=='-') {
            fprintf(stderr, "%s: no message", prg);
            exit(2);
        } else {
            fprintf(stderr, "%s: no option", prg);
            exit(2);
        }
    }

    if (argc==3) {
        if (strcmp(opt, "-Ecsr")==0) {
            CSR_ENCRIPT(msg);
        } else if(strcmp(opt, "-Dcsr")==0) {
            CSR_DECRYPT(msg);
        } else {
            fprintf(stderr, "%s: unknown option '%s'", prg, opt);
            exit(3);
        }
    }

    exit(0);
}
