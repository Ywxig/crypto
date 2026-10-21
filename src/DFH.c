#include <pthread.h>
#include <stdio.h>
#include "../include/globals.h"
#include "../include/colorise.h"
#include "../include/random.h"
#include "../include/math_x.h"

// Быстрое модульное возведение в степень: (base^exp) % mod
// long long mod_exp(long long base, long long exp, long long mod) {
//     long long result = 1;
//     base = base % mod;

//     while (exp > 0) {
//         if (exp % 2 == 1) {
//             result = (result * base) % mod;
//         }
//         exp = exp >> 1;
//         base = (base * base) % mod;
//     }
//     return result;
// }

// Алгоритм обмена ключами Диффи-Хеллмана (по примеру из задания)
long long DIFFI_HELLMAN() {
    // 1. Публичные параметры: p - простое число, g - его примитивный элемент по модулю p
    long long p = 2147483647; //randInt();
    long long g = 16807; //randInt();

    // 2. Секретные числа пользователей A и B (в задании — заданы конкретно, не случайны)
    long long a = randInt();   // секрет пользователя A
    long long b = randInt();  // секрет пользователя B


    c_print_format("<yellow> Параметры Diffie-Hellman <reset>");
    c_print_format("<cyan> Простое число (p): <reset> %lld", p);
    c_print_format("<cyan> Примитивный элемент (g): <reset> %lld", g);

    c_print_format("<magenta> Секрет пользователя A (a): <reset> %lld", a);
    c_print_format("<magenta> Секрет пользователя B (b): <reset> %lld", b);

    // 3. Каждый пользователь вычисляет свой открытый (публичный) ключ
    long long A = pow_mod(g, a, p); // A = g^a mod p
    long long B = pow_mod(g, b, p); // B = g^b mod p

    // 4. Пользователи обмениваются открытыми ключами и вычисляют общий секретный ключ
    long long K_A = pow_mod(B, a, p); // K_A = B^a mod p
    long long K_B = pow_mod(A, b, p); // K_B = A^b mod p

    if (K_A == K_B) {
        c_print_format("<green> Успех! Общий секретный ключ совпадает: <reset> %lld", K_A);
        return K_A;
    } else {
        c_print_format("<red> Ошибка! Ключи не совпадают. <reset>");
    }
    return -1;
}
