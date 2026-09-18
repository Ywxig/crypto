#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/globals.h"
#include "../include/colorise.h"
#include "../include/random.h"

#define MAX_MSG_LEN 128

// Быстрое модульное возведение в степень: (base^exp) % mod
long long mod_exp(long long base, long long exp, long long mod) {
    long long result = 1;
    base = base % mod;

    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

// Простой XOR-шифр: каждый байт сообщения складывается по XOR с байтом,
// зависящим от общего секрета и позиции символа.
// Симметрично: та же функция и шифрует, и расшифровывает.
void xor_cipher(const char *input, char *output, size_t len, long long secret) {
    for (size_t i = 0; i < len; i++) {
        unsigned char key_byte = (unsigned char)((secret + i * 31) % 256);
        output[i] = input[i] ^ key_byte;
    }
}

void DIFFI_HELLMAN() {
    long long p = 23;
    long long g = 5;

    long long a = randInt() % p;
    long long b = randInt() % p;

    if (a <= 0) a = 4;
    if (b <= 0) b = 3;

    long long A = mod_exp(g, a, p);
    long long B = mod_exp(g, b, p);

    c_print_format("<yellow> Параметры Diffie-Hellman <reset>");
    c_print_format("<cyan> Публичное простое число (p): <reset> %lld", p);
    c_print_format("<cyan> Публичное основание (g): <reset> %lld", g);

    c_print_format("<magenta> Приватный ключ Алисы (a): <reset> %lld", a);
    c_print_format("<magenta> Приватный ключ Боба (b): <reset> %lld", b);

    c_print_format("<green> Публичный ключ Боба (B): <reset> %lld", B);
    c_print_format("<green> Публичный ключ Алисы (A): <reset> %lld", A);

    long long Secret_A = mod_exp(B, a, p);
    long long Secret_B = mod_exp(A, b, p);

    c_print_format("<yellow> Результат обмена <reset>");
    c_print_format("<blue> Общий секрет, вычисленный Алисой: <reset> %lld", Secret_A);
    c_print_format("<blue> Общий секрет, вычисленный Бобом: <reset>  %lld", Secret_B);

    if (Secret_A != Secret_B) {
        c_print_format("<red> Ошибка! Секретные ключи не совпадают. <reset>");
        return;
    }

    c_print_format("<green> Успех! Общий секретный ключ установлен и совпадает: <reset> %lld", Secret_A);

    const char *message = "Hello, this is a secret message!";
    size_t len = strlen(message);

    char encrypted[MAX_MSG_LEN];
    char decrypted[MAX_MSG_LEN];

    xor_cipher(message, encrypted, len, Secret_A);

    c_print_format("<yellow> Шифрование сообщения <reset>");
    c_print_format("<cyan> Исходное сообщение: <reset> %s", message);

    c_print_format("<magenta> Зашифровано (hex): <reset> ");
    for (size_t i = 0; i < len; i++) {
        printf("%02x ", (unsigned char)encrypted[i]);
    }

    xor_cipher(encrypted, decrypted, len, Secret_B);
    decrypted[len] = '\0';

    c_print_format("<green> Расшифровано Бобом: <reset> %s", decrypted);
}
