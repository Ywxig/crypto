#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "../include/random.h"

#define POSITIVE_MASK 0x7FFFFFFFFFFFFFFFULL

#ifdef _WIN32

#ifdef _MSC_VER
#pragma comment(lib, "bcrypt.lib")   // для MinGW добавьте флаг -lbcrypt
#endif
#include <windows.h>
#include <bcrypt.h>

long long randIntWin(void) {
    uint64_t x = 0;

    // Системный криптографический генератор Windows
    NTSTATUS st = BCryptGenRandom(NULL, (PUCHAR)&x, (ULONG)sizeof(x),
                                  BCRYPT_USE_SYSTEM_PREFERRED_RNG);

    if (st < 0) {
        // Запасной вариант, если BCrypt недоступен (слабее!)
        LARGE_INTEGER c;
        QueryPerformanceCounter(&c);
        x = (uint64_t)c.QuadPart
          ^ ((uint64_t)GetTickCount64() << 20)
          ^ ((uint64_t)GetCurrentProcessId() << 40);
    }

    return (long long)(x & POSITIVE_MASK);
}

long long randInt(void) {
    return randIntWin();
}

#else

// ЭТО РАБОТАЕТ ТОЛЬКО НА ЛИНУКСЕ

static uint64_t read_cpu_temp(void) {
    FILE *f = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (!f) return 0;
    long t = 0;
    if (fscanf(f, "%ld", &t) != 1) t = 0;
    fclose(f);
    return (uint64_t)t;
}

static uint64_t read_urandom(void) {
    uint64_t v = 0;
    FILE *f = fopen("/dev/urandom", "rb");
    if (f) {
        if (fread(&v, sizeof v, 1, f) != 1) v = 0;
        fclose(f);
    }
    return v;
}

long long randInt(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    uint64_t x = read_urandom()
               ^ read_cpu_temp()
               ^ ((uint64_t)ts.tv_nsec << 20)
               ^ (uint64_t)ts.tv_sec;

    return (long long)(x & 0x7FFFFFFFFFFFFFFF);
}
#endif
