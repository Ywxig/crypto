#include <stdlib.h>
#include <time.h>

int randInt() {
    // Инициализация генератора случайных чисел текущим временем
    srand(time(NULL));

    // Генерация числа от 0 до RAND_MAX
    int random_num = rand();

    return random_num;
}
