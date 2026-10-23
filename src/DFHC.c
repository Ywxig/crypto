#include "../include/globals.h"
#include "../include/colorise.h"

// Структура для представления точки на эллиптической кривой
typedef struct {
    long long x;
    long long y;
    int is_infinity; // Флаг бесконечно удаленной точки (нейтральный элемент)
} Point;

// Функция для вычисления мультипликативного обратного по модулю p (исправлена работа с отрицательными числами)
static long long modInverse(long long n, long long m) {
    n = n % m;
    if (n < 0) n += m;

    long long t = 0, newt = 1;
    long long r = m, newr = n;
    while (newr != 0) {
        long long quotient = r / newr;
        long long temp = t - quotient * newt;
        t = newt;
        newt = temp;
        temp = r - quotient * newr;
        r = newr;
        newr = temp;
    }
    if (r > 1) return -1; // Обратного элемента не существует
    if (t < 0) t = t + m;
    return t;
}

// Функция сложения двух точек P и Q на эллиптической кривой
static Point point_add(Point P, Point Q, long long a_param, long long p) {
    if (P.is_infinity) return Q;
    if (Q.is_infinity) return P;

    // Если x-координаты равны, а y противоположны, сумма дает бесконечно удаленную точку
    if (P.x == Q.x && P.y != Q.y) {
        Point inf = {0, 0, 1};
        return inf;
    }

    long long lam;
    if (P.x == Q.x && P.y == Q.y) {
        if (P.y == 0) {
            Point inf = {0, 0, 1};
            return inf;
        }
        long long num = (3 * P.x * P.x + a_param) % p;
        if (num < 0) num += p;
        long long den = modInverse(2 * P.y, p);
        lam = (num * den) % p;
    } else {
        long long num = (Q.y - P.y) % p;
        if (num < 0) num += p;
        long long den = modInverse(Q.x - P.x, p);
        lam = (num * den) % p;
    }

    Point R;
    R.is_infinity = 0;

    // Безопасное вычисление координат с учетом возможных отрицательных остатков
    R.x = ((lam * lam - P.x - Q.x) % p + p) % p;
    R.y = ((lam * (P.x - R.x) - P.y) % p + p) % p;

    return R;
}

// Функция скалярного умножения точки k * P (алгоритм двой и сложи)
static Point point_mul(long long k, Point P, long long a_param, long long p) {
    Point result = {0, 0, 1}; // Нейтральный элемент (бесконечность)
    Point addend = P;
    while (k > 0) {
        if (k & 1) {
            result = point_add(result, addend, a_param, p);
        }
        addend = point_add(addend, addend, a_param, p);
        k >>= 1;
    }
    return result;
}

// Основная функция обмена ключами на эллиптической кривой (ECDH)
long long DH_ELP_CURV() {
    // 1. Публичные параметры эллиптической кривой из примера
    long long p = 17;       // Простое число модульной арифметики
    long long a_param = 2;  // Коэффициент 'a' кривой
    long long b_param = 2;  // Коэффициент 'b' кривой
    (void)b_param;

    // Фиксированная базовая точка-генератор G
    Point G = {5, 1, 0};

    // 2. Секретные числа пользователей A и B
    long long a = 6;  // Секрет пользователя A
    long long b = 15; // Секрет пользователя B

    // 3. Каждый пользователь вычисляет свой открытый ключ (точки на кривой)
    Point A = point_mul(a, G, a_param, p); // Открытый ключ Алисы
    Point B = point_mul(b, G, a_param, p); // Открытый ключ Боба

    // 4. Пользователи обмениваются открытыми ключами и вычисляют общий секретный ключ
    Point K_A = point_mul(a, B, a_param, p); // Общий секрет у Алисы
    Point K_B = point_mul(b, A, a_param, p); // Общий секрет у Боба

    // Проверка совпадения полученных точек общего секретного ключа
    if (K_A.x == K_B.x && K_A.y == K_B.y && !K_A.is_infinity) {
        c_print_format("<green> Успех! Общий секретный ключ (точка) совпадает: <reset> (%lld, %lld)", K_A.x, K_A.y);
        return K_A.x;
    } else {
        c_print_format("<red> Ошибка! Ключи не совпадают. <reset>");
    }
    return -1;
}
